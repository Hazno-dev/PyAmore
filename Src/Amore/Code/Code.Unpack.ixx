// Hazno - 2026

module;

#include <Python.h>
#include <code.h>

export module Code:Unpack;

export namespace Code
{
    PyCodeObject* RecursiveDecrypt(PyCodeObject* code);

    bool IsEncrypted(const PyCodeObject* code);
}
