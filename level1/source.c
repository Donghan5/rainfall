#include <stdio.h>
#include <unistd.h>

// 08048444: void run()
void run()
{
	fwrite(&g_v8048570, 0x01, 0x13, stdout);
	system("/bin/sh");
}

int main(int argc, char ** argv) 
{
    // 0x8048480
    int32_t str; // bp-80, 0x8048480
    return (int32_t)gets((char *)&str);
}
