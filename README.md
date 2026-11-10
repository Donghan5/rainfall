# Rainfall

Rainfall is a 42 binary exploitation and reverse engineering project. It consists of progressively challenging programs in a Linux virtual machine. Each level requires analyzing a binary, identifying a vulnerability or flawed check, and using it to obtain access to the next account.

This repository documents the solutions for `level0` through `level9` and `bonus0` through `bonus3`. The write-ups explain the relevant disassembly, how offsets and target addresses are derived, and how the final inputs work.

> **Assignment version:** This project follows the assignment version from **before the November 1 revision**. It was evaluated using the **previous assignment format**, rather than the revised format introduced on November 1.

## Level guide

| Level | Main topic |
| --- | --- |
| [level0](level0/readme.md) | Reverse engineering an argument check |
| [level1](level1/readme.md) | Stack buffer overflow and return-to-function |
| [level2](level2/readme.md) | Heap shellcode and a return-address check bypass |
| [level3](level3/readme.md) | Format-string writes with `%n` |
| [level4](level4/readme.md) | Positional format-string writes |
| [level5](level5/readme.md) | GOT overwrite using `%hn` |
| [level6](level6/readme.md) | Heap overflow and function-pointer overwrite |
| [level7](level7/readme.md) | Heap pointer corruption and GOT overwrite |
| [level8](level8/readme.md) | Heap allocation layout and a forged authentication flag |
| [level9](level9/readme.md) | C++ object overflow and virtual-call hijacking |
| [bonus0](bonus0/readme.md) | Missing NUL termination and stack overflow |
| [bonus1](bonus1/readme.md) | Signed integer checks and copy-size overflow |
| [bonus2](bonus2/readme.md) | Language-dependent overflow and environment shellcode |
| [bonus3](bonus3/readme.md) | Unchecked string truncation and comparison bypass |

## Reading and reproducing the solutions

Start with `level0` and follow the levels in order, then continue with the bonus levels. Each write-up describes the vulnerability, the important instructions, and the payload or command used to advance. Many also compare the technique with the previous level.

The examples target the original Rainfall environment and its 32-bit x86 binaries. Analysis uses tools such as GDB and ltrace; many payload commands use Python 2 syntax. Addresses, heap layouts, and stack offsets can depend on the execution environment, so verify them in the target VM before reproducing a solution.

The repository contains write-ups and a small address helper in [bonus2/Ressources/getaddr.c](bonus2/Ressources/getaddr.c). The Rainfall VM and challenge binaries must be obtained separately; there is no application build or installation step for this repository.
