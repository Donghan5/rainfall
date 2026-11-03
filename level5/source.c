#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// ------------------------ Structures ------------------------

struct _IO_FILE {
    int32_t e0;
};

// ------------------- Function Prototypes --------------------

int32_t n(void);

// --------------------- Global Variables ---------------------

struct _IO_FILE * g1 = NULL; // 0x8049848
int32_t g2;

// ------------------------ Functions -------------------------

// Address range: 0x80484c2 - 0x8048504
// Demangled:     __int128
int32_t n(void) {
    // 0x80484c2
    int32_t str; // bp-524, 0x80484c2
    fgets((char *)&str, 512, g1);
    printf((char *)&str);
    exit(1);
    return &g2;
}

void o()
{
	system("/bin/sh");
	_exit(0x01);
}

// Address range: 0x8048504 - 0x8048511
int main(int argc, char ** argv) {
    // 0x8048504
    n();
    // UNREACHABLE
}
