// Hazno - 2026

module;

#include <Python.h>
#include <code.h>
#include <frameobject.h>
#include <opcode.h>

#define CO_OBFUSCATED_WRAP 0x40000000
#define CO_OBFUSCATED 0x8000000
#define HASJABS(op) ((op) == JUMP_ABSOLUTE || \
    (op) == POP_JUMP_IF_FALSE || \
    (op) == POP_JUMP_IF_TRUE || \
    (op) == JUMP_IF_FALSE_OR_POP || \
    (op) == JUMP_IF_TRUE_OR_POP)

module Code;
import Core;
import PyAmoreTransform;

namespace Code
{
    PyCodeObject* NewPyCodeObject(const PyCodeObject* code)
    {
        auto curSize = PyTuple_Size(code->co_consts);
        auto tuple = PyTuple_New(curSize);
        for (auto i = 0; i < curSize; i++) {
            PyObject* item = PyTuple_GetItem(code->co_consts, i);
            Py_INCREF(item);
            PyTuple_SetItem(tuple, i, item);
        }

        auto* newCode = PyCode_New(code->co_argcount,
                        code->co_kwonlyargcount,
                        code->co_nlocals,
                        code->co_stacksize,
                        code->co_flags,
                        //duplicate code object to avoid modifying the original one (which may be shared across multiple frames)
                        PyBytes_FromStringAndSize(PyBytes_AS_STRING(code->co_code), PyBytes_GET_SIZE(code->co_code)),
                        //and consts
                        tuple,
                        code->co_names,
                        code->co_varnames,
                        code->co_freevars,
                        code->co_cellvars,
                        code->co_filename,
                        code->co_name,
                        code->co_firstlineno,
                        code->co_lnotab);

        Py_DECREF(tuple);
        return newCode;
    }

    void TrimCode(PyCodeObject* code, const uint32 start, const uint32 end)
    {
        auto* bytecode = reinterpret_cast<uint8*>(PyBytes_AS_STRING(code->co_code));
        Py_ssize_t len = PyBytes_GET_SIZE(code->co_code);

        bytecode += start;
        len -= start;
        len -= end;

        if (start > 0) {
            for (auto i = 0; i < len; i += 2) {
                uint8_t opcode = bytecode[i];
                uint32_t arg = 0;

                while (opcode == EXTENDED_ARG) {
                    arg = (arg << 8) | bytecode[i + 1];
                    i += 2;
                    opcode = bytecode[i];
                }

                arg = (arg << 8) | bytecode[i + 1];

                if (HASJABS(opcode)) {
                    const uint32_t new_arg = arg - start;
                    bytecode[i + 1] = static_cast<uint8_t>(new_arg);
                }
            }
        }

        code->co_code = PyBytes_FromStringAndSize(reinterpret_cast<char*>(bytecode), len);
    }

    PyCodeObject* RecursiveDecrypt(PyCodeObject* code)
    {
        auto newCode = NewPyCodeObject(code);
        const auto constsCount = PyTuple_Size(newCode->co_consts);
        for (auto i = 0; i < constsCount; i++) {
            if (const auto c = PyTuple_GetItem(newCode->co_consts, i); PyCode_Check(c)) {
                const auto res = RecursiveDecrypt(reinterpret_cast<PyCodeObject*>(c));
                PyTuple_SetItem(newCode->co_consts, i, reinterpret_cast<PyObject*>(res));
            }
        }

        if (!IsEncrypted(code)) {
            return newCode;
        }

        char* codeBytes;
        ssize_t codeLen;
        if (PyBytes_AsStringAndSize(newCode->co_code, &codeBytes, &codeLen) == -1) {
            Log("[RecursiveDecrypt] Failed to get code bytes");
            return newCode;
        }

        auto frame = PyEval_GetFrame();
        auto origCode = frame->f_code;
        frame->f_code = newCode;

        if (newCode->co_flags & CO_OBFUSCATED_WRAP) {
            Log("[RecursiveDecrypt] Decrypting wrapped code object: {} ({}:{})", PyStr(newCode->co_name), PyStr(newCode->co_filename), newCode->co_firstlineno);
            PyAmoreTransform::Builtins::Hook_ArmorEnter.Call(nullptr, nullptr);
            TrimCode(newCode, 16, 16);

        } else if (newCode->co_flags & CO_OBFUSCATED) {
            Log("[RecursiveDecrypt] Decrypting jmp code object: {} ({}:{})", PyStr(newCode->co_name), PyStr(newCode->co_filename), newCode->co_firstlineno);
            PyAmoreTransform::Builtins::Hook_Armor.Call(nullptr, nullptr);
            TrimCode(newCode, 0, 10);
        }

        frame->f_code = origCode;
        return newCode;
    }

    bool IsEncrypted(const PyCodeObject* code)
    {
        return code->co_flags & CO_OBFUSCATED || code->co_flags & CO_OBFUSCATED_WRAP;
    }
}
