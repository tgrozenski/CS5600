# Chapter 15

1.

```bash
ubuntu@cs5600:~/ostep-homework/vm-mechanism$ ./relocation.py -s 1 -c
ARG seed 1
ARG address space size 1k
ARG phys mem size 16k

Base-and-Bounds register information:

  Base   : 0x0000363c (decimal 13884)
  Limit  : 290

Virtual Address Trace
  VA  0: 0x0000030e (decimal:  782) --> SEGMENTATION VIOLATION
  VA  1: 0x00000105 (decimal:  261) --> VALID: 0x00003741 (decimal: 14145)
  VA  2: 0x000001fb (decimal:  507) --> SEGMENTATION VIOLATION
  VA  3: 0x000001cc (decimal:  460) --> SEGMENTATION VIOLATION
  VA  4: 0x0000029b (decimal:  667) --> SEGMENTATION VIOLATION
```

For one only VA 1 is valid, to calculate the translation that occurs we will have to do VA - Base to get the true hardware address. In decimal this is 14145 - 13884 = 261

```bash
ubuntu@cs5600:~/ostep-homework/vm-mechanism$ ./relocation.py -s 2 -c
ARG seed 2
ARG address space size 1k
ARG phys mem size 16k

Base-and-Bounds register information:

  Base   : 0x00003ca9 (decimal 15529)
  Limit  : 500

Virtual Address Trace
  VA  0: 0x00000039 (decimal:   57) --> VALID: 0x00003ce2 (decimal: 15586)
  VA  1: 0x00000056 (decimal:   86) --> VALID: 0x00003cff (decimal: 15615)
  VA  2: 0x00000357 (decimal:  855) --> SEGMENTATION VIOLATION
  VA  3: 0x000002f1 (decimal:  753) --> SEGMENTATION VIOLATION
  VA  4: 0x000002ad (decimal:  685) --> SEGMENTATION VIOLATION
```

0 -> 15586 - 15529 = 57
1 -> 15615 - 15529 = 86

```bash
ubuntu@cs5600:~/ostep-homework/vm-mechanism$ ./relocation.py -s 3 -c

ARG seed 3
ARG address space size 1k
ARG phys mem size 16k

Base-and-Bounds register information:

  Base   : 0x000022d4 (decimal 8916)
  Limit  : 316

Virtual Address Trace
  VA  0: 0x0000017a (decimal:  378) --> SEGMENTATION VIOLATION
  VA  1: 0x0000026a (decimal:  618) --> SEGMENTATION VIOLATION
  VA  2: 0x00000280 (decimal:  640) --> SEGMENTATION VIOLATION
  VA  3: 0x00000043 (decimal:   67) --> VALID: 0x00002317 (decimal: 8983)
  VA  4: 0x0000000d (decimal:   13) --> VALID: 0x000022e1 (decimal: 8929)
```

3 -> 8983 - 8916 = 67
4 -> 8929 - 8916 = 13

2.

```bash
ubuntu@cs5600:~/ostep-homework/vm-mechanism$ ./relocation.py -s 0 -n 10 -c -l 930

ARG seed 0
ARG address space size 1k
ARG phys mem size 16k

Base-and-Bounds register information:

  Base   : 0x0000360b (decimal 13835)
  Limit  : 930

Virtual Address Trace
  VA  0: 0x00000308 (decimal:  776) --> VALID: 0x00003913 (decimal: 14611)
  VA  1: 0x000001ae (decimal:  430) --> VALID: 0x000037b9 (decimal: 14265)
  VA  2: 0x00000109 (decimal:  265) --> VALID: 0x00003714 (decimal: 14100)
  VA  3: 0x0000020b (decimal:  523) --> VALID: 0x00003816 (decimal: 14358)
  VA  4: 0x0000019e (decimal:  414) --> VALID: 0x000037a9 (decimal: 14249)
  VA  5: 0x00000322 (decimal:  802) --> VALID: 0x0000392d (decimal: 14637)
  VA  6: 0x00000136 (decimal:  310) --> VALID: 0x00003741 (decimal: 14145)
  VA  7: 0x000001e8 (decimal:  488) --> VALID: 0x000037f3 (decimal: 14323)
  VA  8: 0x00000255 (decimal:  597) --> VALID: 0x00003860 (decimal: 14432)
  VA  9: 0x000003a1 (decimal:  929) --> VALID: 0x000039ac (decimal: 14764)
```

I noticed when running it a lot that bigger numbers for the bounds produced better results. When searching for the smallest number that would have all addresses valid I found you have to set the bounds register to the largest address generated + 1. So the base doesn't actually matter here, as long as the addresses generated are within the bounds we set there is no segmentation violation.

3. I saw that when I run with a very large number I get an error that says:

```bash
Error: address space does not fit into physical memory with those base/bounds values.
Base + Limit: 10000000   Psize: 16384
```

With that I found that 16284 (have to factor in the + 100 for the limit) will fit in memory. 16285 will not as it is over the base + limit Psize. This checks out because 16k is 16384 bytes.

4. I tried many different values for -a and -p and the rules held. The bound check is still virtual address < the limit & base + limit needs to be less than the physical memory size. The translation stays the same even as the size scaled up.

5. I used `matplotlib` to create the following chart. 

```python
import random
import bisect
import matplotlib.pyplot as plt

ASIZE = 1024
NUM_ADDRS = 1000
SEEDS = range(10)

limits = list(range(0, ASIZE + 1))
all_fracs = []

for seed in SEEDS:
    random.seed(seed)
    vas = sorted(int(ASIZE * random.random()) for _ in range(NUM_ADDRS))
    fracs = [bisect.bisect_left(vas, lim) / NUM_ADDRS for lim in limits]
    all_fracs.append(fracs)
    plt.plot(limits, fracs, alpha=0.35, linewidth=1)

avg = [sum(col) / len(col) for col in zip(*all_fracs)]
plt.plot(limits, avg, color="black", linewidth=2, label="average across seeds")
plt.plot(limits, [l / ASIZE for l in limits], "r--", label="limit / address space size")

plt.xlabel("Bounds register value (limit)")
plt.ylabel("Fraction of valid virtual addresses")
plt.title(f"Valid address fraction vs. bounds ({len(SEEDS)} seeds, {NUM_ADDRS} addrs each)")
plt.xlim(0, ASIZE)
plt.ylim(0, 1.05)
plt.grid(True, alpha=0.3)
plt.legend()
plt.show()
```

![Graph](./graph.png)

# Chapter 16

1. If we assume 2 segments as the output states segements are in 0 or 1 the first segment is between 0 - 63 with the second being between 64 and 128.

```bash
VA  0: 0x0000006c (decimal:  108) --> PA or segmentation violation?
VA  1: 0x00000061 (decimal:   97) --> PA or segmentation violation?
VA  2: 0x00000035 (decimal:   53) --> PA or segmentation violation?
VA  3: 0x00000021 (decimal:   33) --> PA or segmentation violation?
VA  4: 0x00000041 (decimal:   65) --> PA or segmentation violation?
```

0: The first address (108) would go in segment 1. Since segment 1 grows negatively we need to calculate negative offset, our physical address will be base plus the negative offset. Negative offset is 108 - 128 =  -20 (we can do this because segment 1 is always the top segment in this simulator, but really should be segment sizes VA - 64 - 64). Since negative 20 is below the limit we know that it is not a segmentation violation. Thus the physical address is 512 - 20 = 492.

1: 97 - 128 = -31, |-31| <= 20 -> False segmentation fault

2. For 2 this is within segment 0 so positive growth this is greater than to 20 so it is a segmentation fault.

3. 33 Also going to be greater than 20 so segmentation violation.

4. 65 - 128 = -63, |-63| <= 20 -> false, Segmentation violation

```bash
VA  0: 0x00000011 (decimal:   17) --> PA or segmentation violation?
VA  1: 0x0000006c (decimal:  108) --> PA or segmentation violation?
VA  2: 0x00000061 (decimal:   97) --> PA or segmentation violation?
VA  3: 0x00000020 (decimal:   32) --> PA or segmentation violation?
VA  4: 0x0000003f (decimal:   63) --> PA or segmentation violation?
```

```bash
VA  0: 0x0000007a (decimal:  122) --> PA or segmentation violation?
VA  1: 0x00000079 (decimal:  121) --> PA or segmentation violation?
VA  2: 0x00000007 (decimal:    7) --> PA or segmentation violation?
VA  3: 0x0000000a (decimal:   10) --> PA or segmentation violation?
VA  4: 0x0000006a (decimal:  106) --> PA or segmentation violation?
```

2. We know that with a limit of 20 we have 20 valid addresses on either side. Segment 0 is easy for positive growth, there are 20 valid addresses here, 0 - 19. Then on the upper side we have between 108 - 128. Everything below 108 is out of bounds so 20 - 108 will not work.

3. I did the following to get only two valid addresses on each side of the address space. The insight is to set the length to 2 for each and keep base 0 at 0 and base 1 at physical memories size. This ensures that 0 and 1 will be valid and 14 & 15 will be valid. Nothing in between can be with length 2.

```bash
ubuntu@cs5600:~/ostep-homework/vm-segmentation$ ./segmentation.py -a 16 -p 128 -A 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15 --b0 0 --l0 2 --b1 128 --l1 2 -c
```
```
```

4. Limit is the most important lever we can pull here to guarantee an exact percentage. Take the example:

num_valid = (l0 + l1) / address_size_space

To test this I ran the following:

```bash
total=0
for i in $(seq 1 100); do
    count=$(./segmentation.py -a 32 -p 128 --b0 0 --l0 15 --b1 128 --l1 14 -n 100 -s $i -c \
        | grep -o "SEGMENTATION VIOLATION" | wc -l)
    total=$((total + count))
done
echo "scale=2; $total / $NUM_RUNS" | bc
```

The result was 9.34 segmentation violations which means approximately 90% of addresses were valid. Because we are working with randomness we can't guarantee an outcome but we can get fairly close.

5. If you set -l0 and -l1 to 0 then there can be no valid addresses.
