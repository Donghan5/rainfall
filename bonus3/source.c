#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// ------------------------ Structures ------------------------

struct _IO_FILE {
    int32_t e0;
};

// ------------------------ Functions -------------------------

// Address range: 0x80484f4 - 0x804861c
int main(int argc, char ** argv) {
    struct _IO_FILE * v1 = (struct _IO_FILE *)"/home/user/end/.pass"; // bp-176, 0x8048510
    struct _IO_FILE * file = fopen("/home/user/end/.pass", "r"); // 0x8048513
    int32_t data; // bp-152, 0x80484f4
    __asm_rep_stosd_memset((char *)&data, 0, 33);
    if (argc == 2 != file != NULL) {
        // 0x8048615
        return -1;
    }
    // 0x804854d
    v1 = (struct _IO_FILE *)&data;
    fread(&data, 1, 66, file);
    int32_t * str2 = (int32_t *)((int32_t)argv + 4); // 0x804857f
    int32_t str = *str2; // 0x804857f
    v1 = (struct _IO_FILE *)str;
    *(char *)((int32_t)&v1 + 24 + atoi((char *)str)) = 0;
    int32_t data2; // bp-86, 0x80484f4
    v1 = (struct _IO_FILE *)&data2;
    fread(&data2, 1, 65, file);
    *(int32_t *)&v1 = (int32_t)file;
    fclose(file);
    v1 = (struct _IO_FILE *)&data;
    if (strcmp((char *)&data, (char *)*str2) != 0) {
        // 0x8048601
        v1 = (struct _IO_FILE *)&data2;
        puts((char *)&data2);
    } else {
        // 0x80485e3
        v1 = (struct _IO_FILE *)"/bin/sh";
        execl("/bin/sh", "sh");
    }
    // 0x8048615
    return 0;
}