#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../include/bcd.h"

typedef enum {ADD, SUB, MUL, DIV, MOD, POW, END} Operation;

intmax_t nz_rand();
inline intmax_t nz_rand()   {
    intmax_t ret = rand();
    while (!ret)
        ret = rand();
    return ret;
}

int main(int argc, char const *argv[])  {
    const unsigned int seed = (unsigned int) time(NULL);
    printf("%d\n", seed);
    srand(seed);
    BCD_int tmp, arr[] = {new_BCD_int1(nz_rand()), new_BCD_int1(-nz_rand()), new_BCD_int2(nz_rand(), rand() % 2)},
    pows[] = {new_BCD_int1(2), new_BCD_int1(3), new_BCD_int1(5), new_BCD_int1(7), new_BCD_int1(11)};
    for (unsigned char a = 0; a < 13; a++)   {
        for (unsigned char b = 0; b < 3; b++)    {
            imul_bcd_cuint(&(arr[b]), nz_rand());
        }
        for (Operation op = ADD; op < END; op++)    {
            for (unsigned char i = 0; i < 3; i++)    {
                for (unsigned char j = 0; j < ((op == POW) ? 5 : 3); j++)   {
                    printf("(");
                    print_bcd(arr[i]);
                    if (op == POW)  {
                        tmp = pow_bcd(arr[i], pows[j]);
                        printf(") ** (");
                        print_bcd(pows[j]);
                    }
                    else    {
                        if (op == ADD)  {
                            tmp = add_bcd(arr[i], arr[j]);
                            printf(") + (");
                        }
                        else if (op == SUB) {
                            tmp = sub_bcd(arr[i], arr[j]);
                            printf(") - (");
                        }
                        else if (op == MUL) {
                            tmp = mul_bcd(arr[i], arr[j]);
                            printf(") * (");
                        }
                        else if (op == DIV) {
                            tmp = div_bcd(arr[i], arr[j]);
                            printf(") // (");
                        }
                        else if (op == MOD) {
                            tmp = mod_bcd(arr[i], arr[j]);
                            printf(") %% (");
                        }
                        print_bcd(arr[j]);
                    }
                    printf(") == (");
                    print_bcd(tmp);
                    printf(")\n");
                    free_BCD_int(&tmp);
                }
            }
        }
    }
    for (unsigned char i = 0; i < 3; i++)   {
        free_BCD_int(&(arr[i]));
    }
    for (unsigned char i = 0; i < 5; i++)   {
        free_BCD_int(&(pows[i]));
    }
    return 0;
}
