// Hazno - 2026

module;

#include <cstdint>

export module PyAmoreTransform;
import Core;

export namespace PyAmoreTransform
{
    void Initialize(void* handle);

    void* GetHandle();
    uintptr_t GetBase();

    uint64 InitModule(int major, int minor, void* pyApiHandle);
    inline Hook Hook_InitModule(&InitModule);

    uint64 ImportModule(char* modName, void* fileName);
    inline Hook Hook_ImportModule(&InitModule);

    uint64 SetOption(int opt, void* value);
    inline Hook Hook_SetOption(&SetOption);
}
