/*
Project Euler Problem 76

I ended up having to do this iteratively, which I'm not super happy with. I feel like there is almost certainly a
closed-form solution to this, but I was unable to figure it out.

Revision 1:

Have counts store values instead of numeral counts to shave a few seconds off

Revision 2:

Manually expand the sum loop in the preprocessor to try and get TCC output to be faster. Shaved a ~1/3 of runtime in
both CL and GCC in my initial testing.

Revision 3:

After testing on non-Windows systems, I found that Revision 2 royally borked it up. I reverted this, then applied an
optimization I found earlier today. The number of solutions to a + 2b + n = 100, where a, b, n in [0, 100] is
(100 - n) / 2 + 1. This brought runtime on TCC from ~3min -> ~1min and clang from ~6s -> ~2s.

Revision 4:

Repeat an earlier optimization for the 2s case, so now it tries to keep the 2s value as close to the missing piece as
possible, cutting out a lot of useless loops. Runtime is approximately halved on TCC.

Problem:

It is possible to write five as a sum in exactly six different ways:

4 + 1
3 + 2
3 + 1 + 1
2 + 2 + 1
2 + 1 + 1 + 1
1 + 1 + 1 + 1 + 1

How many different ways can one hundred be written as a sum of at least two
positive integers?
*/
#include <stdio.h>
#include "include/macros.h"

#ifdef DOXYGEN
namespace c::p0076 {
#endif


int main(int argc, char const *argv[])  {
    unsigned int answer = 0;
    unsigned char idx, i, sum = 100, counts[101] = {0, 0, 100, 0};
    while (!counts[100]) {
        counts[2] += 2;
        if (sum >= 100) {
            answer += (100 + counts[2] - sum) / 2;
            idx = 2;
            do {
                counts[idx] = 0;
                ++idx;
                counts[idx] += idx;
                sum = counts[2];
                for (i = 3; i < 100; ++i) {
                    sum += counts[i];
                }
            } while (sum > 100);
            counts[2] = 100 - sum - (sum % 2);
        }
        sum = counts[2];
        for (i = 3; i < 100; ++i) {
            sum += counts[i];
        }
    }
    printf("%u", answer);
    return 0;
}

#ifdef DOXYGEN
}
#endif
