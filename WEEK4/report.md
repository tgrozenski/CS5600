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
