1.

I ran the following commands as instructed

```bash
./paging-linear-translate.py -P 1k -a 1m -p 512m -v -n 0
  ...
  [    1023]  0x00000000

./paging-linear-translate.py -P 1k -a 2m -p 512m -v -n 0
  [    2047]  0x00000000

./paging-linear-translate.py -P 1k -a 4m -p 512m -v -n 0
  [    4095]  0x8002e298
```

It is clear to me that it scales linearly with the address space size. I intuitively expected a 2x increase between 1m to 2m then between 2m to 4m but I saw it was 2n - 1, 1023*2 - 1 = 2047, 2047*2 - 1 = 4095. This is because (don't know)

2. I think that the page table size should shrink as page size grows. We know that the number of entries in the page tables are `address_space / page_size` so a greater page size would mean less entries in the page table.

```bash
./paging-linear-translate.py -P 1k -a 1m -p 512m -v -n 0

  [    1023]  0x00000000

./paging-linear-translate.py -P 2k -a 1m -p 512m -v -n 0
  [     511]  0x00000000

./paging-linear-translate.py -P 4k -a 1m -p 512m -v -n 0
  [     255]  0x8001fb27
```

3.

```bash
ubuntu@cs5600:~/ostep-homework/vm-paging$ ./paging-linear-translate.py -P 1k -a 16k -p 32k -v -u 0
ARG seed 0
ARG address space size 16k
ARG phys mem size 32k
ARG page size 1k
ARG verbose True
ARG addresses -1


The format of the page table is simple:
The high-order (left-most) bit is the VALID bit.
  If the bit is 1, the rest of the entry is the PFN.
  If the bit is 0, the page is not valid.
Use verbose mode (-v) if you want to print the VPN # by
each entry of the page table.

Page Table (from entry 0 down to the max size)
  [       0]  0x00000000
  [       1]  0x00000000
  [       2]  0x00000000
  [       3]  0x00000000
  [       4]  0x00000000
  [       5]  0x00000000
  [       6]  0x00000000
  [       7]  0x00000000
  [       8]  0x00000000
  [       9]  0x00000000
  [      10]  0x00000000
  [      11]  0x00000000
  [      12]  0x00000000
  [      13]  0x00000000
  [      14]  0x00000000
  [      15]  0x00000000

Virtual Address Trace
  VA 0x00003a39 (decimal:    14905) --> PA or invalid address?
  VA 0x00003ee5 (decimal:    16101) --> PA or invalid address?
  VA 0x000033da (decimal:    13274) --> PA or invalid address?
  VA 0x000039bd (decimal:    14781) --> PA or invalid address?
  VA 0x000013d9 (decimal:     5081) --> PA or invalid address?

For each virtual address, write down the physical address it translates to
OR write down that it is an out-of-bounds address (e.g., segfault).

```

For VA 0x00003a39 we know that the address space is 16KB so the VA is 14 bits long since 16KB = 2^14. The page size is 1KB = 2^10 so the offset needs is 10 bytes. So 14 - 10 = 4 bits for the VA. All entries in the Page Table start with 0x0 which means all are unset. We can assume all of the remaining values are invalid since all page table entries are unset because of the `-u` flag setting 0% of table entries.

With `-u 25` we get the following:

```bash
Page Table (from entry 0 down to the max size)
  [       0]  0x80000018
  [       1]  0x00000000
  [       2]  0x00000000
  [       3]  0x00000000
  [       4]  0x00000000
  [       5]  0x80000009
  [       6]  0x00000000
  [       7]  0x00000000
  [       8]  0x80000010
  [       9]  0x00000000
  [      10]  0x80000013
  [      11]  0x00000000
  [      12]  0x8000001f
  [      13]  0x8000001c
  [      14]  0x00000000
  [      15]  0x00000000

Virtual Address Trace
  VA 0x00003986 (decimal:    14726) --> PA or invalid address?
  VA 0x00002bc6 (decimal:    11206) --> PA or invalid address?
  VA 0x00001e37 (decimal:     7735) --> PA or invalid address?
  VA 0x00000671 (decimal:     1649) --> PA or invalid address?
  VA 0x00001bc9 (decimal:     7113) --> PA or invalid address?
```

For 0x00003986 we still have the VA will be the first 4 bits and the offset the last 10 bits. 11100110000110 is the address in binary, first 4 bits are 1110 and the last 10 are 0110000110. 1110 in decimal is 14, page table at index 14 starts with 0x0 so it is unset, invalid.

0x00002bc6 -> 10101111000110, 1010 we get index 10 which is valid. Index 10 is 0x8 not 0x0 so this is a valid entry. This is where the OS would do PA = (PFN << 10) | offset, but we can alternatively do PA = PFN × page size + offset. PA = 19 × 1024 + 966 = 19456 + 966 = 20422 = 0x4fc6

...

```bash
./paging-linear-translate.py -P 1k -a 16k -p 32k -v -u 50
Page Table (from entry 0 down to the max size)
  [       0]  0x80000018
  [       1]  0x00000000
  [       2]  0x00000000
  [       3]  0x8000000c
  [       4]  0x80000009
  [       5]  0x00000000
  [       6]  0x8000001d
  [       7]  0x80000013
  [       8]  0x00000000
  [       9]  0x8000001f
  [      10]  0x8000001c
  [      11]  0x00000000
  [      12]  0x8000000f
  [      13]  0x00000000
  [      14]  0x00000000
  [      15]  0x80000008

Virtual Address Trace
  VA 0x00003385 (decimal:    13189) --> PA or invalid address?
  VA 0x0000231d (decimal:     8989) --> PA or invalid address?
  VA 0x000000e6 (decimal:      230) --> PA or invalid address?
  VA 0x00002e0f (decimal:    11791) --> PA or invalid address?
  VA 0x00001986 (decimal:     6534) --> PA or invalid address?

For each virtual address, write down the physical address it translates to
OR write down that it is an out-of-bounds address (e.g., segfault).
```

Once run with `-u 100` we see all entries in the page table are valid/set.

```bash
ubuntu@cs5600:~/ostep-homework/vm-paging$ ./paging-linear-translate.py -P 1k -a 16k -p 32k -v -u 100
ARG seed 0
ARG address space size 16k
ARG phys mem size 32k
ARG page size 1k
ARG verbose True
ARG addresses -1


The format of the page table is simple:
The high-order (left-most) bit is the VALID bit.
  If the bit is 1, the rest of the entry is the PFN.
  If the bit is 0, the page is not valid.
Use verbose mode (-v) if you want to print the VPN
each entry of the page table.

Page Table (from entry 0 down to the max size)
  [       0]  0x80000018
  [       1]  0x80000008
  [       2]  0x8000000c
  [       3]  0x80000009
  [       4]  0x80000012
  [       5]  0x80000010
  [       6]  0x8000001f
  [       7]  0x8000001c
  [       8]  0x80000017
  [       9]  0x80000015
  [      10]  0x80000003
  [      11]  0x80000013
  [      12]  0x8000001e
  [      13]  0x8000001b
  [      14]  0x80000019
  [      15]  0x80000000

Virtual Address Trace
  VA 0x00002e0f (decimal:    11791) --> PA or invalid address?
  VA 0x00001986 (decimal:     6534) --> PA or invalid address?
  VA 0x000034ca (decimal:    13514) --> PA or invalid address?
  VA 0x00002ac3 (decimal:    10947) --> PA or invalid address?
  VA 0x00000012 (decimal:       18) --> PA or invalid address?

For each virtual address, write down the physical address it translates to
OR write down that it is an out-of-bounds address (e.g., segfault).
```

