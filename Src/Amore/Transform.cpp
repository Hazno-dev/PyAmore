// Hazno - 2026

module;

#include <cstdint>
#include <Python.h>
#include <ceval.h>

module PyAmoreTransform;
import Core;
import Platform;

namespace PyAmoreTransform
{
    static void* modHandle;
    static uintptr_t modBase;

    void Initialize(void* handle)
    {
        modHandle = handle;
        modBase = *static_cast<uintptr_t*>(modHandle);

        Log("PyTransform Module Addr: {}", modHandle);
        Log("PyTransform Module Base: {}", reinterpret_cast<void*>(modBase));

        Hook_InitModule.WithTarget(Platform::FindExport(handle, "init_module"));
        Hook_InitRuntime.WithTarget(Platform::FindExport(handle, "init_runtime"));
        Hook_SetOption.WithTarget(Platform::FindExport(handle, "set_option"));
        Hook_ImportModule.WithTarget(Platform::FindExport(handle, "import_module"));

        //Enable PyTransform.Log (Kinda useless but w/e)
        Hook_SetOption.Call(3, reinterpret_cast<void*>(1));
    }

    void* GetHandle()
    {
        return modHandle;
    }

    uintptr_t GetBase()
    {
        return modBase;
    }

    uint64 InitModule(int major, int minor, void* pyApiHandle)
    {
        Log("[InitModule] Initializing on Python{}.{} with API@{}", major, minor, pyApiHandle);
        auto res = Hook_InitModule.Call(major, minor, pyApiHandle);
        Log("[InitModule] Returned: {}", res);
        return res;
    }

    uint64 InitRuntime(int32 sysTrace, int32 sysProfile, int32 threadTrace, int32 threadProfile)
    {
        Log("[InitRuntime] Initializing runtime with sysTrace={}, sysProfile={}, threadTrace={}, threadProfile={}", sysTrace, sysProfile, threadTrace, threadProfile);
        auto res = Hook_InitRuntime.Call(sysTrace, sysProfile, threadTrace, threadProfile);
        auto builtins = PyEval_GetBuiltins();
        //LogPy("[InitRuntime] Post-Builtins", builtins);

        Builtins::Hook_Armor
            .WithTarget(reinterpret_cast<PyCFunctionObject*>(PyDict_GetItemString(builtins, "__armor__")))
            .Enable("__armor__");

        Builtins::Hook_ArmorEnter
            .WithTarget(reinterpret_cast<PyCFunctionObject*>(PyDict_GetItemString(builtins, "__armor_enter__")))
            .Enable("__armor_enter__");

        Builtins::Hook_ArmorExit
            .WithTarget(reinterpret_cast<PyCFunctionObject*>(PyDict_GetItemString(builtins, "__armor_exit__")))
            .Enable("__armor_exit__");

        Builtins::Hook_WrapArmor
            .WithTarget(reinterpret_cast<PyCFunctionObject*>(PyDict_GetItemString(builtins, "__wraparmor__")))
            .Enable("__wraparmor__");

        Builtins::Hook_PyArmor
            .WithTarget(reinterpret_cast<PyCFunctionObject*>(PyDict_GetItemString(builtins, "__pyarmor__")))
            .Enable("__pyarmor__");

        //PyEval_GetFrame()

        Log("[InitRuntime] Returned: {}", res);
        return res;
    }

    uint64 SetOption(int opt, void* value)
    {
        Log("[SetOption] Setting option {} to value {}", opt, value);
        auto res = Hook_SetOption.Call(opt, value);
        Log("[SetOption] Returned: {}", res);
        return res;
    }

    uint64 ImportModule(char* modName, char* fileName)
    {
        Log("[ImportModule] Importing module: {} from file: {}", modName, fileName);
        auto res = Hook_ImportModule.Call(modName, fileName);
        Log("[ImportModule] Returned: {}", res);
        return res;
    }
}
