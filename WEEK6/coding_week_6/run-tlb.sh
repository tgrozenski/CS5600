#!/bin/bash
gcc -O0 -o tlb tlb.c
for p in 1 2 4 8 16 32 64 128 256 512 1024 2048 4096; do
    ./tlb $p 100000
done
