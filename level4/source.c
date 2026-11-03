#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// ------------------------ Structures ------------------------

struct _IO_FILE {
    int32_t e0;
};

// ------------------- Function Prototypes --------------------

int32_t n(void);
int32_t p(int32_t * format);

// --------------------- Global Variables ---------------------

struct _IO_FILE * g1 = NULL; // 0x8049804
int32_t g2 = 0; // 0x8049810

// ------------------------ Functions -------------------------

// Address range: 0x8048444 - 0x8048457
int32_t p(int32_t * format) {
    // 0x8048444
    return printf((char *)format);
}

// Address range: 0x8048457 - 0x80484a7
// Demangled:     __int128
int32_t n(void) {
    // 0x8048457
    int32_t str; // bp-524, 0x8048457
    fgets((char *)&str, 512, g1);
    p(&str);
    int32_t system_rc = g2; // 0x8048497
    if (g2 == 0x1025544) {
        // 0x8048499
        system_rc = system("/bin/cat /home/user/level5/.pass");
    }
    // 0x80484a5
    return system_rc;
}

// Address range: 0x80484a7 - 0x80484b4
int main(int argc, char ** argv) {
    // 0x80484a7
    return n();
}
