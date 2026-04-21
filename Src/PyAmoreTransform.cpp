// Hazno - 2026

module;

#include <cstdint>

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
        Hook_ImportModule.WithTarget(Platform::FindExport(handle, "import_module"));
        Hook_SetOption.WithTarget(Platform::FindExport(handle, "set_option"));

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

    }

    uint64 ImportModule(char* modName, void* fileName)
    {

    }

    uint64 SetOption(int opt, void* value)
    {

    }

}
