// Hazno - 2026

export module Platform;

#ifdef unix

extern "C" void* dlsym(void *handle, const char *name);
extern "C" void* dlopen(const char* filename, int flags);

#endif

export namespace Platform
{
    void* FindExport(void* handle, const char* name);
}
