#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ------------------- Function Prototypes --------------------

int32_t greetuser(void);

// --------------------- Global Variables ---------------------

int32_t g1 = 0; // 0x8049988

// ------------------------ Functions -------------------------

// Address range: 0x8048484 - 0x8048529
int32_t greetuser(void) {
    // 0x8048484
    int32_t str2; // 0x8048484
    char * str; // bp-76, 0x8048484
    switch (g1) {
        case 1: {
            // 0x80484ba
            str = (char *)-0x3c8986b8;
            // 0x804850a
            strcat((char *)&str, (char *)&str2);
            return puts((char *)&str);
        }
        case 2: {
            // 0x80484e9
            str = (char *)0x64656f47;
            // 0x804850a
            strcat((char *)&str, (char *)&str2);
            return puts((char *)&str);
        }
    }
    if (g1 == 0) {
        // 0x804849d
        str = (char *)0x6c6c6548;
    }
    // 0x804850a
    strcat((char *)&str, (char *)&str2);
    return puts((char *)&str);
}

// Address range: 0x8048529 - 0x8048638
int main(int argc, char ** argv) {
    // 0x8048529
    if (argc != 3) {
        // 0x8048630
        return 1;
    }
    // 0x8048548
    int32_t str; // bp-96, 0x8048529
    __asm_rep_stosd_memset((char *)&str, 0, 19);
    int32_t v1 = (int32_t)argv; // 0x804855c
    char * v2 = (char *)&str; // bp-176, 0x8048574
    strncpy((char *)&str, (char *)*(int32_t *)(v1 + 4), 40);
    int32_t str2; // bp-56, 0x8048529
    strncpy((char *)&str2, (char *)*(int32_t *)(v1 + 8), 32);
    v2 = "LANG";
    char * env_val = getenv("LANG"); // 0x80485a6
    if (env_val == NULL) {
        // 0x8048618
        __asm_rep_movsd_memcpy((char *)&v2, (char *)&str, 19);
        // 0x8048630
        return greetuser();
    }
    // 0x80485bc
    v2 = env_val;
    if (memcmp((int32_t *)env_val, (int32_t *)"fi", 2) == 0) {
        // 0x80485df
        g1 = 1;
        // 0x8048618
        __asm_rep_movsd_memcpy((char *)&v2, (char *)&str, 19);
        // 0x8048630
        return greetuser();
    }
    // 0x80485eb
    v2 = env_val;
    if (memcmp((int32_t *)env_val, (int32_t *)"nl", 2) == 0) {
        // 0x804860e
        g1 = 2;
    }
    // 0x8048618
    __asm_rep_movsd_memcpy((char *)&v2, (char *)&str, 19);
    // 0x8048630
    return greetuser();
}