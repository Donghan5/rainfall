# Way to level1

## Core
`main` compares `atoi(argv[1])` against `0x1a7` (=423) with `cmp`. On a match it drops
to level1 privileges and runs a shell via `execv`. Match one argument and you win.

## Decisive disass (full: `disas main`)
```
<+20>:  call atoi
<+25>:  cmp  $0x1a7,%eax      ; 0x1a7 = 423, the only value the branch needs
<+30>:  jne  main+152         ; mismatch -> fwrite error, exit
   ...                        ; on pass: setresuid/gid set level1 privileges
<+145>: call execv            ; spawn shell with those privileges
```

## Exploit
- Value: `argv[1] = 423` (source: `cmp $0x1a7` @ main+25)
- Delivery: pass it as the argument, then read the pass in the spawned shell
```bash
./level0 423
$ cat /home/user/level1/.pass
```

## Diff from previous
First level. The shared habit used everywhere after — "find the cmp/call points with
`disas`" — starts here.

## Result
`1fe8a524fa4bec01ca4ea2a869af2a02260d4a7d5fe7e7c24d8617e6dca12d3a`
