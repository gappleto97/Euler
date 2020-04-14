#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include "../include/macros.h"

#if (CL_COMPILER || defined(__MINGW32__) || defined(_WIN32) || defined(WIN32))
    #include <windows.h> // for QueryPerformanceCounter()
#endif

intmax_t nz_rand();
inline intmax_t nz_rand()   {
    intmax_t ret = rand();
    while (!ret)
        ret = rand();
    return ret;
}

unsigned long long perf_time();
inline unsigned long long perf_time()   {
    #if (CL_COMPILER || defined(__MINGW32__))
        LARGE_INTEGER t;
        QueryPerformanceCounter(&t);  // returns microseconds
        return t.QuadPart * 1000;
    #else
        struct timespec t;
        #if defined(CLOCK_PROCESS_CPUTIME_ID)
            clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t);
        #elif defined(CLOCK_MONOTONIC)
            clock_gettime(CLOCK_MONOTONIC, &t);
        #else
            clock_gettime(CLOCK_REALTIME, &t);
        #endif
        return t.tv_nsec + t.tv_sec * 1000000000;
    #endif
}
