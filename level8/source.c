#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ------------------------ Structures ------------------------

struct _IO_FILE {
    int32_t e0;
};

// --------------------- Global Variables ---------------------

struct _IO_FILE * g1 = NULL; // 0x8049a80
struct _IO_FILE * g2 = NULL; // 0x8049aa0
char * g3; // 0x8049aac
int32_t g4 = 0; // 0x8049ab0

// ------------------------ Functions -------------------------

// Address range: 0x8048564 - 0x8048739
int main(int argc, char ** argv) {
    // 0x8048564
    printf("%p, %p \n", g3, (int32_t *)g4);
    int32_t str; // bp-144, 0x8048564
    if (fgets((char *)&str, 128, g1) == NULL) {
        // 0x804872c
        return 0;
    }
    int32_t v1 = &str; // 0x80485cb
    bool v2; // 0x8048564
    int32_t v3 = v2 ? -1 : 1;
    int32_t v4 = 5; // 0x80485cf
    int32_t v5 = v1;
    int32_t v6 = (int32_t)"auth ";
    unsigned char v7 = *(char *)v5; // 0x80485cf
    char v8 = *(char *)v6; // 0x80485cf
    char v9 = v8; // 0x80485cf
    bool v10 = false; // 0x80485cf
    int32_t v11; // 0x8048564
    int32_t v12; // 0x8048564
    while (v7 == v8) {
        v4--;
        v11 = v6 + v3;
        v12 = v5 + v3;
        v9 = v7;
        v10 = true;
        if (v4 == 0) {
            // break -> 
            break;
        }
        v5 = v12;
        v6 = v11;
        v7 = *(char *)v5;
        v8 = *(char *)v6;
        v9 = v8;
        v10 = false;
    }
    unsigned char v13 = v9;
    int32_t v14; // 0x8048564
    int32_t v15; // 0x8048564
    int32_t v16; // 0x8048564
    int32_t v17; // 0x8048564
    int32_t str2; // bp-139, 0x8048564
    int32_t * mem; // 0x80485eb
    int32_t v18; // 0x804861c
    if ((v7 >= v13 && !v10) == v7 < v13) {
        // 0x80485e4
        mem = malloc(4);
        g3 = (char *)mem;
        *mem = 0;
        v16 = &str2;
        v14 = -1;
        v15 = 0;
        while (v14 != 0) {
            v17 = v16;
            v18 = v14 - 1;
            v16 = v17 + v3;
            v15 = v18;
            if (*(char *)v17 == 0) {
                // break -> 
                break;
            }
            v14 = v18;
            v15 = 0;
        }
        if (-v15 < 33) {
            // 0x804862a
            strcpy(g3, (char *)&str2);
        }
    }
    int32_t v19 = 5; // 0x8048656
    int32_t v20 = v1;
    int32_t v21 = (int32_t)"reset";
    unsigned char v22 = *(char *)v20; // 0x8048656
    char v23 = *(char *)v21; // 0x8048656
    char v24 = v23; // 0x8048656
    bool v25 = false; // 0x8048656
    int32_t v26; // 0x8048564
    int32_t v27; // 0x8048564
    while (v22 == v23) {
        v19--;
        v26 = v21 + v3;
        v27 = v20 + v3;
        v24 = v22;
        v25 = true;
        if (v19 == 0) {
            // break -> 
            break;
        }
        v20 = v27;
        v21 = v26;
        v22 = *(char *)v20;
        v23 = *(char *)v21;
        v24 = v23;
        v25 = false;
    }
    unsigned char v28 = v24;
    if ((v22 >= v28 && !v25) == v22 < v28) {
        // 0x804866b
        free((int32_t *)g3);
    }
    int32_t v29 = 6; // 0x804868c
    int32_t v30 = v1;
    int32_t v31 = (int32_t)"service";
    unsigned char v32 = *(char *)v30; // 0x804868c
    char v33 = *(char *)v31; // 0x804868c
    char v34 = v33; // 0x804868c
    bool v35 = false; // 0x804868c
    int32_t v36; // 0x8048564
    int32_t v37; // 0x8048564
    while (v32 == v33) {
        v29--;
        v36 = v31 + v3;
        v37 = v30 + v3;
        v34 = v32;
        v35 = true;
        if (v29 == 0) {
            // break -> 
            break;
        }
        v30 = v37;
        v31 = v36;
        v32 = *(char *)v30;
        v33 = *(char *)v31;
        v34 = v33;
        v35 = false;
    }
    unsigned char v38 = v34;
    int32_t str3; // bp-137, 0x8048564
    if ((v32 >= v38 && !v35) == v32 < v38) {
        // 0x80486a1
        g4 = (int32_t)strdup((char *)&str3);
    }
    int32_t v39 = 5; // 0x80486c9
    int32_t v40 = v1;
    int32_t v41 = (int32_t)"login";
    unsigned char v42 = *(char *)v40; // 0x80486c9
    char v43 = *(char *)v41; // 0x80486c9
    char v44 = v43; // 0x80486c9
    bool v45 = false; // 0x80486c9
    int32_t v46; // 0x8048564
    int32_t v47; // 0x8048564
    while (v42 == v43) {
        v39--;
        v46 = v41 + v3;
        v47 = v40 + v3;
        v44 = v42;
        v45 = true;
        if (v39 == 0) {
            // break -> 
            break;
        }
        v40 = v47;
        v41 = v46;
        v42 = *(char *)v40;
        v43 = *(char *)v41;
        v44 = v43;
        v45 = false;
    }
    unsigned char v48 = v44;
    if ((v42 >= v48 && !v45) == v42 < v48) {
        // 0x80486e2
        if (*(int32_t *)((int32_t)g3 + 32) == 0) {
            // 0x80486ff
            fwrite((int32_t *)"Password:\n", 1, 10, g2);
        } else {
            // 0x80486ee
            system("/bin/sh");
        }
    }
    // 0x8048574
    printf("%p, %p \n", g3, (int32_t *)g4);
    while (fgets((char *)&str, 128, g1) != NULL) {
        // 0x80485bb
        v4 = 5;
        v5 = v1;
        v6 = (int32_t)"auth ";
        v7 = *(char *)v5;
        v8 = *(char *)v6;
        v9 = v8;
        v10 = false;
        while (v7 == v8) {
            v4--;
            v11 = v6 + v3;
            v12 = v5 + v3;
            v9 = v7;
            v10 = true;
            if (v4 == 0) {
                // break -> 
                break;
            }
            v5 = v12;
            v6 = v11;
            v7 = *(char *)v5;
            v8 = *(char *)v6;
            v9 = v8;
            v10 = false;
        }
        v13 = v9;
        if ((v7 >= v13 && !v10) == v7 < v13) {
            // 0x80485e4
            mem = malloc(4);
            g3 = (char *)mem;
            *mem = 0;
            v16 = &str2;
            v14 = -1;
            v15 = 0;
            while (v14 != 0) {
                v17 = v16;
                v18 = v14 - 1;
                v16 = v17 + v3;
                v15 = v18;
                if (*(char *)v17 == 0) {
                    // break -> 
                    break;
                }
                v14 = v18;
                v15 = 0;
            }
            if (-v15 < 33) {
                // 0x804862a
                strcpy(g3, (char *)&str2);
            }
        }
        // 0x8048642
        v19 = 5;
        v20 = v1;
        v21 = (int32_t)"reset";
        v22 = *(char *)v20;
        v23 = *(char *)v21;
        v24 = v23;
        v25 = false;
        while (v22 == v23) {
            v19--;
            v26 = v21 + v3;
            v27 = v20 + v3;
            v24 = v22;
            v25 = true;
            if (v19 == 0) {
                // break -> 
                break;
            }
            v20 = v27;
            v21 = v26;
            v22 = *(char *)v20;
            v23 = *(char *)v21;
            v24 = v23;
            v25 = false;
        }
        v28 = v24;
        if ((v22 >= v28 && !v25) == v22 < v28) {
            // 0x804866b
            free((int32_t *)g3);
        }
        // 0x8048678
        v29 = 6;
        v30 = v1;
        v31 = (int32_t)"service";
        v32 = *(char *)v30;
        v33 = *(char *)v31;
        v34 = v33;
        v35 = false;
        while (v32 == v33) {
            v29--;
            v36 = v31 + v3;
            v37 = v30 + v3;
            v34 = v32;
            v35 = true;
            if (v29 == 0) {
                // break -> 
                break;
            }
            v30 = v37;
            v31 = v36;
            v32 = *(char *)v30;
            v33 = *(char *)v31;
            v34 = v33;
            v35 = false;
        }
        v38 = v34;
        if ((v32 >= v38 && !v35) == v32 < v38) {
            // 0x80486a1
            g4 = (int32_t)strdup((char *)&str3);
        }
        // 0x80486b5
        v39 = 5;
        v40 = v1;
        v41 = (int32_t)"login";
        v42 = *(char *)v40;
        v43 = *(char *)v41;
        v44 = v43;
        v45 = false;
        while (v42 == v43) {
            v39--;
            v46 = v41 + v3;
            v47 = v40 + v3;
            v44 = v42;
            v45 = true;
            if (v39 == 0) {
                // break -> 
                break;
            }
            v40 = v47;
            v41 = v46;
            v42 = *(char *)v40;
            v43 = *(char *)v41;
            v44 = v43;
            v45 = false;
        }
        v48 = v44;
        if ((v42 >= v48 && !v45) == v42 < v48) {
            // 0x80486e2
            if (*(int32_t *)((int32_t)g3 + 32) == 0) {
                // 0x80486ff
                fwrite((int32_t *)"Password:\n", 1, 10, g2);
            } else {
                // 0x80486ee
                system("/bin/sh");
            }
        }
        // 0x8048574
        printf("%p, %p \n", g3, (int32_t *)g4);
    }
    // 0x804872c
    return 0;
}