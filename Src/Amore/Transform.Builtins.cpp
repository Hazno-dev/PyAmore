// Hazno - 2026

module;

#include <cstdint>
#include <Python.h>
#include <ceval.h>
#include <frameobject.h>

module PyAmoreTransform;
import Core;
import Platform;

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
        Log("[ArmorEnter] Called on frame: {} ({}:{})", PyStr(frame->f_code->co_name), PyStr(frame->f_code->co_filename), frame->f_lineno);

        auto res = Hook_ArmorEnter.Call(self, args);
        Log("[ArmorEnter] Returned: {}", res);
        return res;
    }

    uint64 ArmorExit(PyObject* self, PyObject* args)
    {
        Log("[ArmorExit] Called");
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
        char* fileNameMaybe;
        char* filePathMaybe;
        PyObject* encryptedData;
        int obfuscationMode;
        if (!PyArg_ParseTuple(args, "ssO|i", &fileNameMaybe, &filePathMaybe, &encryptedData, &obfuscationMode)) {
            Log("[PyArmor] Called");
            Log("[PyArmor] Failed to parse arguments");
            return Hook_PyArmor.Call(self, args);
        }

        Log("[PyArmor] Called on {} ({}) with obfuscation mode {}", fileNameMaybe, filePathMaybe, obfuscationMode);
        auto res = Hook_PyArmor.Call(self, args);
        Log("[PyArmor] Returned: {}", res);
        return res;
    }
}
