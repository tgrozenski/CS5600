1. I was able to access the man pages for `free` with the following

```bash
man free
```

2. I ran with the human readable arg `-h` because I like human readable terminal output.

```bash
free -h
```

I got the following output:

```bash
ubuntu@cs5600:~$ free -h
               total        used        free      shared  buff/cache   available
Mem:           7.7Gi       406Mi       7.3Gi       4.9Mi       242Mi       7.4Gi
Swap:             0B          0B          0B
```

This matches my intuition more or less. There are some interesting things like the amount available being larger than the amount free. This implies that memory can be available but not necessarily free. The difference between total vs free is only .4 Gi or 406 Mi which is a remarkably small when we consider the fact that the average PC has between 8-16 GB or ram.

3. I wrote the program memory-user.c.

4. Before running memory-user.c I made sure to take a snapshot of memory.

```bash
ubuntu@cs5600:~$ free -h
               total        used        free      shared  buff/cache   available
Mem:           7.7Gi       493Mi       7.1Gi       4.9Mi       463Mi       7.3Gi
Swap:             0B          0B          0B
```

I then ran to have the program allocate 100 Mi.

```bash
ubuntu@cs5600:~$ free -h
               total        used        free      shared  buff/cache   available
Mem:           7.7Gi       535Mi       7.0Gi       4.9Mi       463Mi       7.2Gi
Swap:             0B          0B          0B
```

I noticed that the math does not add up. Before there were 493 mb used, I know my program allocated 100 Mb. So there is 42 extra bytes that are coming from somewhere. I know that from the textbook chapter the code needs to go into memory, we are allocating from the heap with malloc and some stack is also used for the variables I used. I also know that C has a runtime and maybe the syscalls it makes need to be loaded into memory as well. This all means that more memory may be used than the program code itself directly uses.

5. I read the man pages on  `pmap`
```bash
man pmap
```

6. I chose process 43 on my VM with the name `sshd` because I recognized it from the list of processes as I skimmed through the list created by `ps auxw`.

```bash
root         901  0.0  0.1  14740 10476 ?        Ss   20:37   0:00 sshd: ubuntu [priv]
```

7. I see a large block of virtual addresses and their mappings as the final column.

```bash
ubuntu@cs5600:~/coding_2$ sudo pmap -X 901
901:   sshd: ubuntu [priv]
         Address Perm   Offset Device Inode  Size   Rss  Pss Pss_Dirty Referenced Anonymous KSM LazyFree ShmemPmdMapped FilePmdMapped Shared_Hugetlb Private_Hugetlb Swap SwapPss Locked THPeligible Mapping
    57c87d675000 r--p 00000000  08:01 22939    48    48   12         0         48         0   0        0              0             0              0               0    0       0      0           0 sshd
    57c87d681000 r-xp 0000c000  08:01 22939   580   576   83         0        576         0   0        0              0             0              0               0    0       0      0           0 sshd
    57c87d712000 r--p 0009d000  08:01 22939   252   244   48         0        244         0   0        0              0             0              0               0    0       0      0           0 sshd
    57c87d751000 r--p 000dc000  08:01 22939    16    16    8         8         16        16   0        0              0             0              0               0    0       0      0           0 sshd
    57c87d755000 rw-p 000e0000  08:01 22939     4     4    4         4          4         4   0        0              0             0              0               0    0       0      0           0 sshd
    57c87d756000 rw-p 00000000  00:00     0     8     8    6         6          8         8   0        0              0             0              0               0    0       0      0           0
    57c8832d4000 rw-p 00000000  00:00     0   664   536  418       418        536       536   0        0              0             0              0               0    0       0      0           0 [heap]
    797e77493000 r--p 00000000  08:01  5417    32    32    4         0         32         0   0        0              0             0              0               0    0       0      0           0 libnss_systemd.so.2
    797e7749b000 r-xp 00008000  08:01  5417   252   248   25         0        248         0   0        0              0             0              0               0    0       0      0           0 libnss_systemd.so.2
# ect ...
```

I noticed that each memory address has permisions on whether or not it can read, write, or execute. This is likely a security measure the OS needs to take. The requested vs actual shows that OS is trying to only allocate what is needed to conserve memory. There are many more memory mappings than our simple heap, stack, code model (though these are all present). I saw many more such as libraries as .so files, anonymous mappings denoted by blank entries, and mappings like vvar, vdso, and vsyscall which look like they have to do with kernel syscalls. It is clear that the address space has a lot more elements than simply code, stack, and heap addresses.

8. I added a line to my program to output the pid and ran both programs:

```bash
ubuntu@cs5600:~/coding_2$ ./a.out 100
PID: 2661

ubuntu@cs5600:~/coding_2$ pmap -X 2661
2661:   ./a.out 100
         Address Perm   Offset Device  Inode   Size    Rss    Pss Pss_Dirty Referenced Anonymous KSM LazyFree ShmemPmdMapped FilePmdMapped Shared_Hugetlb Private_Hugetlb Swap SwapPss Locked THPeligible Mapping
    5a2128467000 r--p 00000000  08:01 276163      4      4      4         0          4         0   0        0              0             0              0               0    0       0      0           0 a.out
    5a2128468000 r-xp 00001000  08:01 276163      4      4      4         0          4         0   0        0              0             0              0               0    0       0      0           0 a.out
    5a2128469000 r--p 00002000  08:01 276163      4      4      4         0          4         0   0        0              0             0              0               0    0       0      0           0 a.out
    5a212846a000 r--p 00002000  08:01 276163      4      4      4         4          4         4   0        0              0             0              0               0    0       0      0           0 a.out
    5a212846b000 rw-p 00003000  08:01 276163      4      4      4         4          4         4   0        0              0             0              0               0    0       0      0           0 a.out
    5a21536ca000 rw-p 00000000  00:00      0    132      4      4         4          4         4   0        0              0             0              0               0    0       0      0           0 [heap]
    7e56145ff000 rw-p 00000000  00:00      0 102404 102404 102404    102404     102404    102404   0        0              0             0              0               0    0       0      0           0
    7e561aa00000 r--p 00000000  08:01   6368    160    160      5         0        160         0   0        0              0             0              0               0    0       0      0           0 libc.so.6
    7e561aa28000 r-xp 00028000  08:01   6368   1572    992     31         0        992         0   0        0              0             0              0               0    0       0      0           0 libc.so.6
    7e561abb1000 r--p 001b1000  08:01   6368    316    188      6         0        188         0   0        0              0             0              0               0    0       0      0           0 libc.so.6
    7e561ac00000 r--p 001ff000  08:01   6368     16     16     16        16         16        16   0        0              0             0              0               0    0       0      0           0 libc.so.6
    7e561ac04000 rw-p 00203000  08:01   6368      8      8      8         8          8         8   0        0              0             0              0               0    0       0      0           0 libc.so.6
    7e561ac06000 rw-p 00000000  00:00      0     52     16     16        16         16        16   0        0              0             0              0               0    0       0      0           0
    7e561add8000 rw-p 00000000  00:00      0     12      8      8         8          8         8   0        0              0             0              0               0    0       0      0           0
    7e561ade1000 rw-p 00000000  00:00      0      8      4      4         4          4         4   0        0              0             0              0               0    0       0      0           0
    7e561ade3000 r--p 00000000  08:01   6365      4      4      0         0          4         0   0        0              0             0              0               0    0       0      0           0 ld-linux-x86-64.so.2
    7e561ade4000 r-xp 00001000  08:01   6365    172    172      5         0        172         0   0        0              0             0              0               0    0       0      0           0 ld-linux-x86-64.so.2
    7e561ae0f000 r--p 0002c000  08:01   6365     40     40      1         0         40         0   0        0              0             0              0               0    0       0      0           0 ld-linux-x86-64.so.2
    7e561ae19000 r--p 00036000  08:01   6365      8      8      8         8          8         8   0        0              0             0              0               0    0       0      0           0 ld-linux-x86-64.so.2
    7e561ae1b000 rw-p 00038000  08:01   6365      8      8      8         8          8         8   0        0              0             0              0               0    0       0      0           0 ld-linux-x86-64.so.2
    7fff318f4000 rw-p 00000000  00:00      0    132     12     12        12         12        12   0        0              0             0              0               0    0       0      0           0 [stack]
    7fff3197b000 r--p 00000000  00:00      0     16      0      0         0          0         0   0        0              0             0              0               0    0       0      0           0 [vvar]
    7fff3197f000 r-xp 00000000  00:00      0      8      4      0         0          4         0   0        0              0             0              0               0    0       0      0           0 [vdso]
ffffffffff600000 --xp 00000000  00:00      0      4      0      0         0          0         0   0        0              0             0              0               0    0       0      0           0 [vsyscall]
                                             ====== ====== ====== ========= ========== ========= === ======== ============== ============= ============== =============== ==== ======= ====== ===========
                                             105092 104068 102556    102496     104068    102496   0        0              0             0              0               0    0       0      0           0 KB
```

I recognize the a.out mapping as the source code that I have compiled with gcc, then I see the heap 102404 is anonymous memory. I would have expected it to go above in the line that says heap, but I can see that the heap is size 132. Perhaps since this did not fit the OS allocated as anonymous memory instead. It also makes sense that we see these libc.so files, I did use a few header files such as stdio, stdbool, ect. The linux so files might have to do with gcc compiling the code for the particular architecture my VM has. We see again the linux specific mappings are used for this program as well.

# Chapter 14

1. I wrote the program null.c where I allocated memory for a pointer, set it to null, then tried to access the value. I got a segmentation fault:

```bash
ubuntu@cs5600:~/coding_2$ ./a.out
Segmentation fault (core dumped)
```

2. I reran with gdb and got the following output:

```bash
Starting program: /home/ubuntu/coding_2/null
[Thread debugging using libthread_db enabled]
Using host libthread_db library "/lib/x86_64-linux-gnu/libthread_db.so.1".

Program received signal SIGSEGV, Segmentation fault.
0x0000555555555196 in main ()
```

It would appear to me that SIGSEGV is some sort of signal sent from the operating system to kill the program because it is breaking the rules. We have the function that caused this main. The hex number is definitely an address, maybe to the instruction that crashed the program.

3. Running valgrind I got the following:

```bash
ubuntu@cs5600:~/coding_2$ valgrind --leak-check=yes ./null
==3034== Memcheck, a memory error detector
==3034== Copyright (C) 2002-2024, and GNU GPL'd, by Julian Seward et al.
==3034== Using Valgrind-3.26.0 and LibVEX; rerun with -h for copyright info
==3034== Command: ./null
==3034==
==3034== Invalid read of size 4
==3034==    at 0x4001196: main (in /home/ubuntu/coding_2/null)
==3034==  Address 0x0 is not stack'd, malloc'd or (recently) free'd
==3034==
==3034==
==3034== Process terminating with default action of signal 11 (SIGSEGV)
==3034==  Access not within mapped region at address 0x0
==3034==    at 0x4001196: main (in /home/ubuntu/coding_2/null)
==3034==  If you believe this happened as a result of a stack
==3034==  overflow in your program's main thread (unlikely but
==3034==  possible), you can try to increase the size of the
==3034==  main thread stack using the --main-stacksize= flag.
==3034==  The main thread stack size used in this run was 8388608.
==3034==
==3034== HEAP SUMMARY:
==3034==     in use at exit: 4 bytes in 1 blocks
==3034==   total heap usage: 1 allocs, 0 frees, 4 bytes allocated
==3034==
==3034== 4 bytes in 1 blocks are definitely lost in loss record 1 of 1
==3034==    at 0x484B80F: malloc (vg_replace_malloc.c:447)
==3034==    by 0x4001185: main (in /home/ubuntu/coding_2/null)
==3034==
==3034== LEAK SUMMARY:
==3034==    definitely lost: 4 bytes in 1 blocks
==3034==    indirectly lost: 0 bytes in 0 blocks
==3034==      possibly lost: 0 bytes in 0 blocks
==3034==    still reachable: 0 bytes in 0 blocks
==3034==         suppressed: 0 bytes in 0 blocks
==3034==
==3034== For lists of detected and suppressed errors, rerun with: -s
==3034== ERROR SUMMARY: 2 errors from 2 contexts (suppressed: 0 from 0)
Segmentation fault (core dumped)
```

It clearly is telling me the program performed an invalid read of size 4. The 4 bytes must be the integer I malloc'd. It also tells me the memory address 0x4001196 and offending function main. It tells me the signal as gdb did and explained that the program tried to access 0x0 which the program is not allowed to access from. We know that NULL is mapped to 0 in c, so 0x0 must be referring to the fact that our program is trying to dereference null. This explains what happened (illegal read) and where it happened (the instruction address of compiled program).

4. I wrote no_free.c for this question. It just allocates memory for a number, assigns it, and dereferences it to access the value.

When I run gdb I get that the process exits normally.

```bash
(gdb) run
Starting program: /home/ubuntu/coding_2/a.out
[Thread debugging using libthread_db enabled]
Using host libthread_db library "/lib/x86_64-linux-gnu/libthread_db.so.1".
The number is: 1
[Inferior 1 (process 3264) exited normally]
```

Valgrind however does show that there is a memory leak.

```bash
==3271== 4 bytes in 1 blocks are definitely lost in loss record 1 of 1
==3271==    at 0x484B80F: malloc (vg_replace_malloc.c:447)
==3271==    by 0x4001185: main (in /home/ubuntu/coding_2/a.out)
```

This matches what I would expect. Valgrind is a tool to detect memory leaks like this one, gdb is more focused on debugging. Behavior like this is not technically harmful though it is bad practice.

5. I wrote the program named malloc_zero.c. When I compiled this program nothing bad seemed to happen it just printed the "done" message I put at the end. When I run valgrind we see that I still had an invalid write and the 400 bytes are being leaked. There is no mention of a SIGSEGV. So even though this program not correct and has a memory leak it did not trigger a segmentation fault. The two bugs are an invalid write (out of bounds) and the memory leak. This goes to show that the program would seem perfectly benign after compiling and running it with serious memory issues.
