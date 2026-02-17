#ifdef EIGHTEST_RUN_MODULE
#include <Eightest/Core.hpp>

#if defined(_WIN32)
#   include <windows.h>
#   define EIGHTEST_LOAD_MODULE() LoadLibraryA(EIGHTEST_RUN_MODULE ".dll")
#elif defined(__linux__)
#   include <dlfcn.h>
#   define EIGHTEST_LOAD_MODULE() dlopen(EIGHTEST_RUN_MODULE ".so", RTLD_NOW)
#elif defined(__APPLE__)
#   include <dlfcn.h>
#   define EIGHTEST_LOAD_MODULE() dlopen(EIGHTEST_RUN_MODULE ".dylib", RTLD_NOW)
#endif // if

int main()
{
    EIGHTEST_LOAD_MODULE();
    eightest::global()->execute_all();
    return !eightest::global()->stat();
}
#endif // EIGHTEST_RUN_MODULE
