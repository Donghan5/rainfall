#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

// ------------------- Function Prototypes --------------------

int32_t p(int32_t * str2, char * str);
int32_t pp(int32_t * str);

// ------------------------ Functions -------------------------

// Address range: 0x80484b4 - 0x804851e
int32_t p(int32_t * str2, char * str) {
    // 0x80484b4
    puts(str);
    int32_t buf; // bp-4108, 0x80484b4
    read(0, &buf, 0x1000);
    *strchr((char *)&buf, 10) = 0;
    return (int32_t)strncpy((char *)str2, (char *)&buf, 20);
}

// Address range: 0x804851e - 0x80485a4
// Demangled:     operator++
int32_t pp(int32_t * str) {
    int32_t v1 = (int32_t)str;
    int32_t str2; // bp-52, 0x804851e
    p(&str2, " - ");
    int32_t str3; // bp-32, 0x804851e
    p(&str3, " - ");
    strcpy((char *)str, (char *)&str2);
    int32_t v2 = -1; // 0x8048579
    int32_t v3 = v1; // 0x8048579
    int32_t v4 = 0; // 0x8048579
    while (v2 != 0) {
        int32_t v5 = v3;
        v2--;
        bool v6; // 0x804851e
        v3 = v5 + (v6 ? -1 : 1);
        v4 = v2;
        if (*(char *)v5 == 0) {
            // break -> 
            break;
        }
        v4 = 0;
    }
    *(int16_t *)(v1 - 2 - v4) = 32;
    return (int32_t)strcat((char *)str, (char *)&str3);
}

// Address range: 0x80485a4 - 0x80485cc
int main(int argc, char ** argv) {
    // 0x80485a4
    int32_t str; // bp-58, 0x80485a4
    pp(&str);
    puts((char *)&str);
    return 0;
}