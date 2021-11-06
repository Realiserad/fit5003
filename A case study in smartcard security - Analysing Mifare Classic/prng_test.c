#include <stdio.h>
#include <stdint.h>
#include <string.h>
#define SWAPENDIAN(x)\
	(x = (x >> 8 & 0xff00ff) | (x & 0xff00ff) << 8, x = x >> 16 | x << 16)
void print_bits(size_t const size, void const * const ptr) {
    unsigned char *b = (unsigned char*) ptr;
    unsigned char byte;
    int i, j;
    for (i = size-1; i >= 0; i--) {
        for (j = 7; j >= 0; j--) {
            byte = (b[i] >> j) & 1;
            printf("%u  ", byte);
        }
        printf("  ");
    }
    puts("");
}
/* PRNG implementation from crapto1 library */
uint32_t prng_successor(uint32_t x, uint32_t n) {
    SWAPENDIAN(x);
    while(n--) x = x >> 1 | (x >> 16 ^ x >> 18 ^ x >> 19 ^ x >> 21) << 31;
    return SWAPENDIAN(x);
}
int main() {
    int i;
    uint32_t x;
    char buffer[33];
    buffer[32] = '\0';
    //x = 0xAAAAAAAA;
    x = 0xff00ff;
    printf(HEADER);
    print_bits(sizeof(uint32_t), &x);
    /* Compute suc32(x) and print intermediary states */
    for (i = 0; i < 32; i++)  {
        x = prng_successor(x, 1);
        print_bits(sizeof(uint32_t), &x);
    }
}