// Hazno - 2026

module;

#include <dlfcn.h>
#include <cstdio>
#include <cstring>
#include <sys/mman.h>
#include <cassert>
#include <sys/types.h>
#include <sstream>
#include <cstdio>
#include <execinfo.h>
#include <Python.h>
#include <format>
#include <object.h>
#include <iomanip>
#include <filesystem>
#include <printf.h>
#include <fstream>

export module Core:Log;

inline std::ofstream& GetLog()
{
    static std::ofstream log(std::filesystem::current_path() / "PyAmore.log", std::ios::out | std::ios::trunc);
    return log;
}

export {

    template <typename... Args>
    void Log(std::format_string<Args...> fmt, Args&&... args) {
        GetLog() << std::format(fmt, std::forward<Args>(args)...) << std::endl << std::flush;
    }

    void Log(const std::string_view str) {
        GetLog() << str << std::endl << std::flush;
    }

    inline void LogPy(const char* name, PyObject *obj) {
        PyObject* repr = PyObject_Repr(obj);
        PyObject* str = PyUnicode_AsEncodedString(repr, "utf-8", "~E~");
        const char *bytes = PyBytes_AS_STRING(str);

        Log("{}: {}", name, bytes);

        Py_XDECREF(repr);
        Py_XDECREF(str);
    }

    inline void Backtrace()
    {
        void* buffer[64];
        const auto nPtrs = backtrace(buffer, 64);
        const auto strings = backtrace_symbols(buffer, nPtrs);

        for(auto i = 1; i < nPtrs - 2; ++i) {
            Log(strings[i]);
        }

        free(strings);
    }

    template<typename Iter>
    std::string HexStr(Iter begin, Iter end)
    {
        std::ostringstream output;
        output << std::hex << std::setfill('0') << std::uppercase;
        while(begin != end) {
            output << std::setw(2) << static_cast<unsigned>(*begin++);
            if (begin != end) {
                output << ":";
            }
        }
        return output.str();
    }

    std::string PyStr(PyObject* obj)
    {
        PyObject* repr = PyObject_Repr(obj);
        PyObject* str = PyUnicode_AsEncodedString(repr, "utf-8", "~E~");
        return PyBytes_AS_STRING(str);
    }
}