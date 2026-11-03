#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// ------------------------ Functions -------------------------

// Address range: 0x8048424 - 0x80484a5
int main(int argc, char ** argv) {
    int32_t str_as_i = atoi((char *)*(int32_t *)(argc + 4)); // 0x8048438
    int32_t result = 1; // 0x8048446
    if (str_as_i < 10) {
        // 0x804844f
        int32_t v1; // bp-60, 0x8048424
        memcpy(&v1, (int32_t *)*(int32_t *)(argc + 8), 4 * str_as_i);
        result = 0;
    }
    // 0x80484a3
    return result;
}
