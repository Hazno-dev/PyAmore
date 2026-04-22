// Hazno - 2026

module;

#include <cstdint>
#include <Python.h>

export module PyAmoreTransform;
import Core;

namespace PyAmoreTransform
{
    export void Initialize(void* handle);

    export void* GetHandle();
    export uintptr_t GetBase();

    uint64 InitModule(int major, int minor, void* pyApiHandle);
    export inline Hook Hook_InitModule(&InitModule);

    uint64 InitRuntime(int32 sysTrace, int32 sysProfile, int32 threadTrace, int32 threadProfile);
    export inline Hook Hook_InitRuntime(&InitRuntime);

    uint64 SetOption(int opt, void* value);
    export inline Hook Hook_SetOption(&SetOption);

    uint64 ImportModule(char* modName, char* fileName);
    export inline Hook Hook_ImportModule(&ImportModule);

    namespace Builtins
    {
        uint64 Armor(PyObject* self, PyObject* args);
        export inline Hook Hook_Armor(&Armor);

        uint64 ArmorEnter(PyObject* self, PyObject* args);
        export inline Hook Hook_ArmorEnter(&ArmorEnter);

        uint64 ArmorExit(PyObject* self, PyObject* args);
        export inline Hook Hook_ArmorExit(&ArmorExit);

        uint64 WrapArmor(PyObject* self, PyObject* args);
        export inline Hook Hook_WrapArmor(&WrapArmor);

        uint64 PyArmor(PyObject* self, PyObject* args);
        export inline Hook Hook_PyArmor(&PyArmor);
    }
}
