#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// ------------------------ Structures ------------------------

struct _IO_FILE {
    int32_t e0;
};

// ------------------- Function Prototypes --------------------

int32_t p(int32_t a1);

// --------------------- Global Variables ---------------------

struct _IO_FILE * g1 = NULL; // 0x8049860

// ------------------------ Functions -------------------------

// Address range: 0x80484d4 - 0x804853f
int32_t p(int32_t a1) {
    // 0x80484d4
    fflush(g1);
    int32_t str; // bp-80, 0x80484d4
    gets((char *)&str);
    if ((a1 & -0x50000000) != -0x50000000) {
        // 0x8048527
        puts((char *)&str);
        return (int32_t)strdup((char *)&str);
    }
    // 0x8048507
    printf("(%p)\n", (int32_t *)a1);
    _exit(1);
    // UNREACHABLE
}

// Address range: 0x804853f - 0x804854c
int main(int argc, char ** argv) {
    // 0x804853f
    int32_t v1; // 0x804853f
    return p(v1);
}
