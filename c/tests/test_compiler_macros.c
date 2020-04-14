#include <stdio.h>
#include "../include/macros.h"

#ifdef DOXYGEN
namespace c::tests::test_compiler_macros {
#endif

int main(int argc, char const *argv[]) {
    printf(
        "%d %d %d %d %d %d %d %d",
        CL_COMPILER,
        CLANG_COMPILER,
        GCC_COMPILER,
        INTEL_COMPILER,
        AMD_COMPILER,
        X86_COMPILER,
        X64_COMPILER,
        ARM_COMPILER
    );
    return 0;
}

#ifdef DOXYGEN
}
#endif
