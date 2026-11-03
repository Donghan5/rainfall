#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// ------------------------ Structures ------------------------

struct _IO_FILE {
    int32_t e0;
};

// ------------------- Function Prototypes --------------------

int32_t v(void);

// --------------------- Global Variables ---------------------

struct _IO_FILE * g1 = NULL; // 0x8049860
struct _IO_FILE * g2 = NULL; // 0x8049880
int32_t g3 = 0; // 0x804988c

// ------------------------ Functions -------------------------

// Address range: 0x80484a4 - 0x804851a
// Demangled:     void
int32_t v(void) {
    // 0x80484a4
    int32_t str; // bp-524, 0x80484a4
    fgets((char *)&str, 512, g1);
    printf((char *)&str);
    int32_t system_rc = g3; // 0x80484e2
    if (g3 == 64) {
        // 0x80484e4
        fwrite((int32_t *)"Wait what?!\n", 1, 12, g2);
        system_rc = system("/bin/sh");
    }
    // 0x8048518
    return system_rc;
}

// Address range: 0x804851a - 0x8048527
int main(int argc, char ** argv) {
    // 0x804851a
    return v();
}