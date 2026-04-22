// Hazno - 2026

module;

#include <cstdint>
#include <Python.h>
#include <ceval.h>
#include <marshal.h>
#include <frameobject.h>
#include <filesystem>

module PyAmoreTransform;
import Core;
import Platform;
import Code;

namespace PyAmoreTransform::Builtins
{
    uint64 Armor(PyObject* self, PyObject* args)
    {
        Log("[Armor] Called");
        auto res = Hook_Armor.Call(self, args);
        Log("[Armor] Returned: {}", res);
        return res;
    }

    uint64 ArmorEnter(PyObject* self, PyObject* args)
    {
        auto frame = PyEval_GetFrame();
        Log("[ArmorEnter] Called on: {} ({}:{})", PyStr(frame->f_code->co_name), PyStr(frame->f_code->co_filename), frame->f_lineno);

        if (!PyStr(frame->f_code->co_name).contains("<module>")) {
            auto res = Hook_ArmorEnter.Call(self, args);
            Log("[ArmorEnter] Returned: {}", res);
            return res;
        }

        Log("[ArmorEnter] Detected module-level code.");
        auto codeRes = Code::RecursiveDecrypt(frame->f_code);
        auto mod = PyImport_ImportModule("importlib._bootstrap_external");
        auto func = PyObject_GetAttrString(mod, "_code_to_timestamp_pyc");
        auto result = PyObject_CallFunction(func, "O",
            codeRes
        );

        PyObject* globals = frame->f_globals;
        PyObject* nameObj = PyDict_GetItemString(globals, "__name__");
        std::string filename = PyUnicode_AsUTF8(nameObj);
        if (!filename.ends_with(".pyc")) {
            filename += ".pyc";
        }
        auto outPath = std::filesystem::current_path() / "PyAmore" / filename;
        if (!std::filesystem::exists(outPath.parent_path())) {
            std::filesystem::create_directories(outPath.parent_path());
        }

        std::ofstream out(outPath, std::ios::out | std::ios::binary);
        if (!out) {
            Log("[ArmorEnter] Failed to open output file: {}", outPath.c_str());
            return Hook_ArmorEnter.Call(self, args);
        }

        auto buf = PyByteArray_AsString(result);
        auto len = PyByteArray_Size(result);
        if (len == -1) {
            Log("[ArmorEnter] Failed to convert result to bytes");
            return Hook_ArmorEnter.Call(self, args);
        }

        out.write(buf, len);
        out.flush();
        out.close();

        Log("[ArmorEnter] Written decrypted code to {}", outPath.c_str());
        auto res = Hook_ArmorEnter.Call(self, args);
        Log("[ArmorEnter] Returned: {}", res);
        return res;
    }

    uint64 ArmorExit(PyObject* self, PyObject* args)
    {
        auto frame = PyEval_GetFrame();
        Log("[ArmorExit] Called on: {} ({}:{})", PyStr(frame->f_code->co_name), PyStr(frame->f_code->co_filename), frame->f_lineno);
        auto res = Hook_ArmorExit.Call(self, args);
        Log("[ArmorExit] Returned: {}", res);
        return res;
    }

    uint64 WrapArmor(PyObject* self, PyObject* args)
    {
        Log("[WrapArmor] Called");
        auto res = Hook_WrapArmor.Call(self, args);
        Log("[WrapArmor] Returned: {}", res);
        return res;
    }

    uint64 PyArmor(PyObject* self, PyObject* args)
    {
        char* fileName;
        char* filePath;
        PyObject* encryptedData;
        int obfuscationMode;
        if (!PyArg_ParseTuple(args, "ssO|i", &fileName, &filePath, &encryptedData, &obfuscationMode)) {
            Log("[PyArmor] Called");
            Log("[PyArmor] Failed to parse arguments");
            return Hook_PyArmor.Call(self, args);
        }

        char* buf;
        Py_ssize_t len;
        if (PyBytes_AsStringAndSize(encryptedData, &buf, &len) == -1) {
            Log("[PyArmor] Called");
            Log("[PyArmor] FLOP. Couldn't convert encrypted data to str...?");
            return Hook_PyArmor.Call(self, args);
        }

        Log("[PyArmor] Called on {} ({}) with obfuscation_mode={}", fileName, filePath, obfuscationMode);
        Log("[PyArmor] {} was built in Python {}.{} with advanced_mode=", fileName,
            *reinterpret_cast<uint8*>(buf + 9), *reinterpret_cast<uint8*>(buf + 10), *reinterpret_cast<uint32*>(buf + 20));
        Log("[PyArmor] Ciphertext is located @+{} with len {}", *reinterpret_cast<uint32*>(buf + 28), *reinterpret_cast<uint32*>(buf + 32));
        auto res = Hook_PyArmor.Call(self, args);
        Log("[PyArmor] Returned: {}", res);
        return res;
    }
}
