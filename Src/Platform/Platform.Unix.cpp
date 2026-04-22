// Hazno - 2026

module;

#include <cstring>
#include <dlfcn.h>
#include <cassert>

module Platform;
import PyAmoreTransform;
import Core;

typedef void* (*dlopen_t)(const char*, int);
static dlopen_t RealDlOpen = nullptr;

typedef void* (*dlsym_t)(void*, const char*);
static dlsym_t RealDlSym = nullptr;

extern "C" void *dlsym(void *handle, const char *name)
{
    if (!RealDlSym) {
        RealDlSym = reinterpret_cast<dlsym_t>(dlvsym(RTLD_NEXT, "dlsym", "GLIBC_2.2.5"));
    }

    if (handle != PyAmoreTransform::GetHandle()) {
        return RealDlSym(handle, name);
    }

    if (strcmp(name, "init_module") == 0) {
        assert(PyAmoreTransform::Hook_InitModule.GetTarget() != nullptr);
        return PyAmoreTransform::Hook_InitModule.GetTrampoline();
    }

    if (strcmp(name, "init_runtime") == 0) {
        assert(PyAmoreTransform::Hook_InitRuntime.GetTarget() != nullptr);
        return PyAmoreTransform::Hook_InitRuntime.GetTrampoline();
    }

    if (strcmp(name, "set_option") == 0) {
        assert(PyAmoreTransform::Hook_SetOption.GetTarget() != nullptr);
        return PyAmoreTransform::Hook_SetOption.GetTrampoline();
    }

    if (strcmp(name, "import_module") == 0) {
        assert(PyAmoreTransform::Hook_ImportModule.GetTarget() != nullptr);
        return PyAmoreTransform::Hook_ImportModule.GetTrampoline();
    }

    void* ret = RealDlSym(handle, name);
    Log("DlSym request on PT for: {} -> {}", name, ret);
    return ret;

    /*if (handle == PyAmoreTransform::GetHandle()) {
        if (strcmp(name, "init_module") == 0 && init_mod_orig != nullptr) {
            log("dlsym request: %s -> %s\n", name, "my_init_module");
            return (void*)&my_init_module;
        }

        if (strcmp(name, "set_option") == 0 && set_option_orig != nullptr) {
            log("dlsym request: %s -> %s\n", name, "my_set_option");
            return (void*)&my_set_option;
        }
    }*/
}

extern "C" void* dlopen(const char* filename, int flags)
{
    if (!RealDlSym) {
        RealDlSym = reinterpret_cast<dlsym_t>(dlvsym(RTLD_NEXT, "dlsym", "GLIBC_2.2.5"));
    }

    if (!RealDlOpen) {
        RealDlOpen = reinterpret_cast<dlopen_t>(RealDlSym(RTLD_NEXT, "dlopen"));
    }

    void* handle = RealDlOpen(filename, flags);
    if (!handle) {
        return nullptr;
    }

    if (!filename || !strstr(filename, "pytransform.so")) {
        return handle;
    }

    PyAmoreTransform::Initialize(handle);

    return handle;
}


namespace Platform
{
    void* FindExport(void* handle, const char* name)
    {
        return RealDlSym(handle, name);
    }
}