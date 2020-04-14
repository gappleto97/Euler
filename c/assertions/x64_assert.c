#include "../include/macros.h"
// this file should trigger compilation errors on non-x86 targets

#ifdef DOXYGEN
namespace c::assertions::x64_assert {
#endif

int main(int argc, char const *argv[])  {
    #if CL_COMPILER
        __asm {
            aaa;
        }
    #else
        __asm__ (
            "aaa;"
        );
    #endif
}

#ifdef DOXYGEN
}
#endif
