#include <stdio.h>
#include <time.h>
#include "./test_utils.h"
#include "../include/bcd.h"

typedef enum {ADD, SUB, MUL, DIV, MOD, POW, LSHIFT, RSHIFT, END} Operation;
const char op_symbols[END][6] = {"+", "-", "*", "//", "%", "**", "*10**"};
const unsigned char counts[6] = {1, 2, 3, 4, 5, 6};

int main(int argc, char const *argv[])  {
    unsigned long long ns;
    #ifndef SEED_OVERRIDE
        const unsigned int seed = (unsigned int) time(NULL);
    #else
        const unsigned int seed = SEED_OVERRIDE;
    #endif
    printf("%d\n", seed);
    srand(seed);
    BCD_int tmp,
    pows[] = {new_BCD_int1(2), new_BCD_int1(3), new_BCD_int1(5), new_BCD_int1(7), new_BCD_int1(11), new_BCD_int1(13)},
    arr[] = {new_BCD_int1(nz_rand()), new_BCD_int1(-nz_rand()), new_BCD_int2(nz_rand(), rand() % 2),
             new_BCD_int1(nz_rand()), new_BCD_int1(-nz_rand()), new_BCD_int2(nz_rand(), rand() % 2)};
    for (unsigned char a = 0; a < 13; a++)  {
        for (unsigned char b = 0; b < 3; b++)   {
            imul_bcd_cuint(&(arr[b]), nz_rand());
        }
        for (Operation op = ADD; op < END; op++)    {
            for (unsigned char i = 0; i < 3; i++)   {
                for (unsigned char j = 0; j < 6; j++)   {
                    if (op == RSHIFT)   {
                        printf("int(str");
                    }
                    printf("(");
                    print_bcd(arr[i]);
                    if (op == RSHIFT)
                        printf(")[:-%i]", counts[j]);
                    else
                        printf(") %s (", op_symbols[op]);
                    if (op == POW)
                        print_bcd(pows[j]);
                    else if (op == LSHIFT)
                        printf("%i", counts[j]);
                    else if (op == RSHIFT)
                        ;
                    else
                        print_bcd(arr[j]);
                    ns = perf_time();
                    switch(op)  {
                    case ADD:
                        tmp = add_bcd(arr[i], arr[j]);
                        break;
                    case SUB:
                        tmp = sub_bcd(arr[i], arr[j]);
                        break;
                    case MUL:
                        tmp = mul_bcd(arr[i], arr[j]);
                        break;
                    case DIV:
                        tmp = div_bcd(arr[i], arr[j]);
                        break;
                    case MOD:
                        tmp = mod_bcd(arr[i], arr[j]);
                        break;
                    case POW:
                        tmp = pow_bcd(arr[i], pows[j]);
                        break;
                    case LSHIFT:
                        tmp = shift_bcd_left(arr[i], counts[j]);
                        break;
                    case RSHIFT:
                        tmp = shift_bcd_right(arr[i], counts[j]);
                        break;
                    default:
                        return -1;
                    }
                    ns = perf_time() - ns;
                    printf(") == (");
                    print_bcd(tmp);
                    printf(")  # in %llu_%03llu_%03llu ns\n", ns / 1000000, (ns / 1000) % 1000, ns % 1000);
                    free_BCD_int(&tmp);
                }
            }
        }
    }
    for (unsigned char i = 0; i < 6; i++)   {
        free_BCD_int(&(pows[i]));
        free_BCD_int(&(arr[i]));
    }
    return 0;
}
