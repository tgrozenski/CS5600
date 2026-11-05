# Week 8 — Concurrency: An Introduction (threads-intro)

---

## Question 1

```bash
python3 x86.py -t 1 -p loop.s -i 100 -R dx -c
```

```
ARG seed 0
ARG numthreads 1
ARG program loop.s
ARG interrupt frequency 100
ARG interrupt randomness False
ARG argv 
ARG load address 1000
ARG memsize 128
ARG memtrace 
ARG regtrace dx
ARG cctrace False
ARG printstats False
ARG verbose False

   dx          Thread 0         
    0   
   -1   1000 sub  $1,%dx
   -1   1001 test $0,%dx
   -1   1002 jgte .top
   -1   1003 halt
```

%dx starts at 0 (no -a given). The loop body runs once: sub $1,%dx is -1, then test $0,%dx + jgte .top sees -1 ≥ 0 is false, so it falls through to halt. So dx is 0, then -1. I ran with the -c flag and it validated my conclusion.

---

## Question 2

```bash
python3 x86.py -p loop.s -t 2 -i 100 -a dx=3,dx=3 -R dx -c
```

```
ARG seed 0
ARG numthreads 2
ARG program loop.s
ARG interrupt frequency 100
ARG interrupt randomness False
ARG argv dx=3,dx=3
ARG load address 1000
ARG memsize 128
ARG memtrace 
ARG regtrace dx
ARG cctrace False
ARG printstats False
ARG verbose False

   dx          Thread 0                Thread 1         
    3   
    2   1000 sub  $1,%dx
    2   1001 test $0,%dx
    2   1002 jgte .top
    1   1000 sub  $1,%dx
    1   1001 test $0,%dx
    1   1002 jgte .top
    0   1000 sub  $1,%dx
    0   1001 test $0,%dx
    0   1002 jgte .top
   -1   1000 sub  $1,%dx
   -1   1001 test $0,%dx
   -1   1002 jgte .top
   -1   1003 halt
    3   ----- Halt;Switch -----  ----- Halt;Switch -----  
    2                            1000 sub  $1,%dx
    2                            1001 test $0,%dx
    2                            1002 jgte .top
    1                            1000 sub  $1,%dx
    1                            1001 test $0,%dx
    1                            1002 jgte .top
    0                            1000 sub  $1,%dx
    0                            1001 test $0,%dx
    0                            1002 jgte .top
   -1                            1000 sub  $1,%dx
   -1                            1001 test $0,%dx
   -1                            1002 jgte .top
   -1                            1003 halt
```

We know that each thread has it's own private version of %dx and both start at 3. When we use -i 100 all 13 instructions in the loop fit inside one quantum so there each thread runs to completion without switching. Multiple threads change nothing in this case, there is no race or shared state touched.

---

## Question 3

```bash
python3 x86.py -p loop.s -t 2 -i 3 -r -R dx -a dx=3,dx=3 -c
python3 x86.py -p loop.s -t 2 -i 3 -r -R dx -a dx=3,dx=3 -s 1 -c
python3 x86.py -p loop.s -t 2 -i 3 -r -R dx -a dx=3,dx=3 -s 2 -c
```

`-s 0`:

```
   dx          Thread 0                Thread 1         
    3   
    2   1000 sub  $1,%dx
    2   1001 test $0,%dx
    2   1002 jgte .top
    3   ------ Interrupt ------  ------ Interrupt ------  
    2                            1000 sub  $1,%dx
    2                            1001 test $0,%dx
    2                            1002 jgte .top
    2   ------ Interrupt ------  ------ Interrupt ------  
    1   1000 sub  $1,%dx
    1   1001 test $0,%dx
    2   ------ Interrupt ------  ------ Interrupt ------  
    1                            1000 sub  $1,%dx
    1   ------ Interrupt ------  ------ Interrupt ------  
    1   1002 jgte .top
    0   1000 sub  $1,%dx
    1   ------ Interrupt ------  ------ Interrupt ------  
    1                            1001 test $0,%dx
    1                            1002 jgte .top
    0   ------ Interrupt ------  ------ Interrupt ------  
    0   1001 test $0,%dx
    0   1002 jgte .top
   -1   1000 sub  $1,%dx
    1   ------ Interrupt ------  ------ Interrupt ------  
    0                            1000 sub  $1,%dx
   -1   ------ Interrupt ------  ------ Interrupt ------  
   -1   1001 test $0,%dx
   -1   1002 jgte .top
    0   ------ Interrupt ------  ------ Interrupt ------  
    0                            1001 test $0,%dx
    0                            1002 jgte .top
   -1   ------ Interrupt ------  ------ Interrupt ------  
   -1   1003 halt
    0   ----- Halt;Switch -----  ----- Halt;Switch -----  
   -1                            1000 sub  $1,%dx
   -1                            1001 test $0,%dx
   -1   ------ Interrupt ------  ------ Interrupt ------  
   -1                            1002 jgte .top
   -1                            1003 halt
```

`-s 1`:

```
   dx          Thread 0                Thread 1         
    3   
    2   1000 sub  $1,%dx
    3   ------ Interrupt ------  ------ Interrupt ------  
    2                            1000 sub  $1,%dx
    2                            1001 test $0,%dx
    2                            1002 jgte .top
    2   ------ Interrupt ------  ------ Interrupt ------  
    2   1001 test $0,%dx
    2   1002 jgte .top
    1   1000 sub  $1,%dx
    2   ------ Interrupt ------  ------ Interrupt ------  
    1                            1000 sub  $1,%dx
    1   ------ Interrupt ------  ------ Interrupt ------  
    1   1001 test $0,%dx
    1   1002 jgte .top
    1   ------ Interrupt ------  ------ Interrupt ------  
    1                            1001 test $0,%dx
    1                            1002 jgte .top
    1   ------ Interrupt ------  ------ Interrupt ------  
    0   1000 sub  $1,%dx
    0   1001 test $0,%dx
    1   ------ Interrupt ------  ------ Interrupt ------  
    0                            1000 sub  $1,%dx
    0                            1001 test $0,%dx
    0                            1002 jgte .top
    0   ------ Interrupt ------  ------ Interrupt ------  
    0   1002 jgte .top
    0   ------ Interrupt ------  ------ Interrupt ------  
   -1                            1000 sub  $1,%dx
    0   ------ Interrupt ------  ------ Interrupt ------  
   -1   1000 sub  $1,%dx
   -1   1001 test $0,%dx
   -1   1002 jgte .top
   -1   ------ Interrupt ------  ------ Interrupt ------  
   -1                            1001 test $0,%dx
   -1                            1002 jgte .top
   -1   ------ Interrupt ------  ------ Interrupt ------  
   -1   1003 halt
   -1   ----- Halt;Switch -----  ----- Halt;Switch -----  
   -1                            1003 halt
```

I noticed that with random interrupts the threads did interleave, but the outcome was still identical. Each thread still decrimented its own %dx from 3 to 1 halts. Interrupt frequency and seed changed when each thread ran but not the final register values since there is still no shared data.

---

## Question 4

```bash
python3 x86.py -p looping-race-nolock.s -t 1 -M 2000 -c
```

```
ARG seed 0
ARG numthreads 1
ARG program looping-race-nolock.s
ARG interrupt frequency 50
ARG interrupt randomness False
ARG argv 
ARG load address 1000
ARG memsize 128
ARG memtrace 2000
ARG regtrace 
ARG cctrace False
ARG printstats False
ARG verbose False

 2000          Thread 0         
    0   
    0   1000 mov 2000, %ax
    0   1001 add $1, %ax
    1   1002 mov %ax, 2000
    1   1003 sub  $1, %bx
    1   1004 test $0, %bx
    1   1005 jgt .top
    1   1006 halt
```

Value started at 0. The default %bx is 0 so the loop runs once doing read 0, add, then store giving value = 1. Then sub makes bx = 1 so jgt fails and halts. Value goes from 0 to 1 and then stays at 1.

---

## Question 5

```bash
python3 x86.py -p looping-race-nolock.s -t 2 -a bx=3 -M 2000 -c
```

```
ARG seed 0
ARG numthreads 2
ARG program looping-race-nolock.s
ARG interrupt frequency 50
ARG interrupt randomness False
ARG argv bx=3
ARG load address 1000
ARG memsize 128
ARG memtrace 2000
ARG regtrace 
ARG cctrace False
ARG printstats False
ARG verbose False

 2000          Thread 0                Thread 1         
    0   
    0   1000 mov 2000, %ax
    0   1001 add $1, %ax
    1   1002 mov %ax, 2000
    1   1003 sub  $1, %bx
    1   1004 test $0, %bx
    1   1005 jgt .top
    1   1000 mov 2000, %ax
    1   1001 add $1, %ax
    2   1002 mov %ax, 2000
    2   1003 sub  $1, %bx
    2   1004 test $0, %bx
    2   1005 jgt .top
    2   1000 mov 2000, %ax
    2   1001 add $1, %ax
    3   1002 mov %ax, 2000
    3   1003 sub  $1, %bx
    3   1004 test $0, %bx
    3   1005 jgt .top
    3   1006 halt
    3   ----- Halt;Switch -----  ----- Halt;Switch -----  
    3                            1000 mov 2000, %ax
    3                            1001 add $1, %ax
    4                            1002 mov %ax, 2000
    4                            1003 sub  $1, %bx
    4                            1004 test $0, %bx
    4                            1005 jgt .top
    4                            1000 mov 2000, %ax
    4                            1001 add $1, %ax
    5                            1002 mov %ax, 2000
    5                            1003 sub  $1, %bx
    5                            1004 test $0, %bx
    5                            1005 jgt .top
    5                            1000 mov 2000, %ax
    5                            1001 add $1, %ax
    6                            1002 mov %ax, 2000
    6                            1003 sub  $1, %bx
    6                            1004 test $0, %bx
    6                            1005 jgt .top
    6                            1006 halt
```

-a bx=3 is applied to all threads, so both loop 3 times. Each iteration is doing add $1 to value. Thread 0 runs it's 3 first then thread 0 runs it's three so the final value is 6.

---

## Question 6

```bash
python3 x86.py -p looping-race-nolock.s -t 2 -M 2000 -i 4 -r -s 0 -c
python3 x86.py -p looping-race-nolock.s -t 2 -M 2000 -i 4 -r -s 1 -c
python3 x86.py -p looping-race-nolock.s -t 2 -M 2000 -i 4 -r -s 2 -c
```

`-s 0` (final value 2000 = 2):

```
 2000          Thread 0                Thread 1         
    0   
    0   1000 mov 2000, %ax
    0   1001 add $1, %ax
    1   1002 mov %ax, 2000
    1   1003 sub  $1, %bx
    1   ------ Interrupt ------  ------ Interrupt ------  
    1                            1000 mov 2000, %ax
    1                            1001 add $1, %ax
    2                            1002 mov %ax, 2000
    2                            1003 sub  $1, %bx
    2   ------ Interrupt ------  ------ Interrupt ------  
    2   1004 test $0, %bx
    2   1005 jgt .top
    2   ------ Interrupt ------  ------ Interrupt ------  
    2                            1004 test $0, %bx
    2                            1005 jgt .top
    2   ------ Interrupt ------  ------ Interrupt ------  
    2   1006 halt
    2   ----- Halt;Switch -----  ----- Halt;Switch -----  
    2                            1006 halt
```

`-s 1` (final value 2000 = 1 — a lost update):

```
 2000          Thread 0                Thread 1         
    0   
    0   1000 mov 2000, %ax
    0   ------ Interrupt ------  ------ Interrupt ------  
    0                            1000 mov 2000, %ax
    0                            1001 add $1, %ax
    1                            1002 mov %ax, 2000
    1                            1003 sub  $1, %bx
    1   ------ Interrupt ------  ------ Interrupt ------  
    1   1001 add $1, %ax
    1   1002 mov %ax, 2000
    1   1003 sub  $1, %bx
    1   1004 test $0, %bx
    1   ------ Interrupt ------  ------ Interrupt ------  
    1                            1004 test $0, %bx
    1                            1005 jgt .top
    1   ------ Interrupt ------  ------ Interrupt ------  
    1   1005 jgt .top
    1   1006 halt
    1   ----- Halt;Switch -----  ----- Halt;Switch -----  
    1   ------ Interrupt ------  ------ Interrupt ------  
    1                            1006 halt
```

`-s 2` (final value 2000 = 2):

```
 2000          Thread 0                Thread 1         
    0   
    0   1000 mov 2000, %ax
    0   1001 add $1, %ax
    1   1002 mov %ax, 2000
    1   1003 sub  $1, %bx
    1   ------ Interrupt ------  ------ Interrupt ------  
    1                            1000 mov 2000, %ax
    1                            1001 add $1, %ax
    2                            1002 mov %ax, 2000
    2                            1003 sub  $1, %bx
    2   ------ Interrupt ------  ------ Interrupt ------  
    2   1004 test $0, %bx
    2   ------ Interrupt ------  ------ Interrupt ------  
    2                            1004 test $0, %bx
    2   ------ Interrupt ------  ------ Interrupt ------  
    2   1005 jgt .top
    2   1006 halt
    2   ----- Halt;Switch -----  ----- Halt;Switch -----  
    2                            1005 jgt .top
    2                            1006 halt
```

The critical section is the three instructions mov 2000,%ax / add $1,%ax / mov %ax,2000 . With seeds 0 and 2 the interrupts land between whole critical sections, so final = 2. Seed 1 interrupts inside the section, so both threads read the old value 0 and both write 1 → lost update, final = 1. Timing matters here as an interrupt must not split that 3 instruction read-modify-write.

---

## Question 7

```bash
python3 x86.py -p looping-race-nolock.s -a bx=1 -t 2 -M 2000 -i 1 -c
python3 x86.py -p looping-race-nolock.s -a bx=1 -t 2 -M 2000 -i 2 -c
python3 x86.py -p looping-race-nolock.s -a bx=1 -t 2 -M 2000 -i 3 -c
```

`-i 1` (final value = 1):

```
 2000          Thread 0                Thread 1         
    0   
    0   1000 mov 2000, %ax
    0   ------ Interrupt ------  ------ Interrupt ------  
    0                            1000 mov 2000, %ax
    0   ------ Interrupt ------  ------ Interrupt ------  
    0   1001 add $1, %ax
    0   ------ Interrupt ------  ------ Interrupt ------  
    0                            1001 add $1, %ax
    0   ------ Interrupt ------  ------ Interrupt ------  
    1   1002 mov %ax, 2000
    1   ------ Interrupt ------  ------ Interrupt ------  
    1                            1002 mov %ax, 2000
    1   ------ Interrupt ------  ------ Interrupt ------  
    1   1003 sub  $1, %bx
    1   ------ Interrupt ------  ------ Interrupt ------  
    1                            1003 sub  $1, %bx
    1   ------ Interrupt ------  ------ Interrupt ------  
    1   1004 test $0, %bx
    1   ------ Interrupt ------  ------ Interrupt ------  
    1                            1004 test $0, %bx
    1   ------ Interrupt ------  ------ Interrupt ------  
    1   1005 jgt .top
    1   ------ Interrupt ------  ------ Interrupt ------  
    1                            1005 jgt .top
    1   ------ Interrupt ------  ------ Interrupt ------  
    1   1006 halt
    1   ----- Halt;Switch -----  ----- Halt;Switch -----  
    1   ------ Interrupt ------  ------ Interrupt ------  
    1                            1006 halt
```

`-i 2` (final value = 1):

```
 2000          Thread 0                Thread 1         
    0   
    0   1000 mov 2000, %ax
    0   1001 add $1, %ax
    0   ------ Interrupt ------  ------ Interrupt ------  
    0                            1000 mov 2000, %ax
    0                            1001 add $1, %ax
    0   ------ Interrupt ------  ------ Interrupt ------  
    1   1002 mov %ax, 2000
    1   1003 sub  $1, %bx
    1   ------ Interrupt ------  ------ Interrupt ------  
    1                            1002 mov %ax, 2000
    1                            1003 sub  $1, %bx
    1   ------ Interrupt ------  ------ Interrupt ------  
    1   1004 test $0, %bx
    1   1005 jgt .top
    1   ------ Interrupt ------  ------ Interrupt ------  
    1                            1004 test $0, %bx
    1                            1005 jgt .top
    1   ------ Interrupt ------  ------ Interrupt ------  
    1   1006 halt
    1   ----- Halt;Switch -----  ----- Halt;Switch -----  
    1                            1006 halt
```

`-i 3` (final value = 2, correct):

```
 2000          Thread 0                Thread 1         
    0   
    0   1000 mov 2000, %ax
    0   1001 add $1, %ax
    1   1002 mov %ax, 2000
    1   ------ Interrupt ------  ------ Interrupt ------  
    1                            1000 mov 2000, %ax
    1                            1001 add $1, %ax
    2                            1002 mov %ax, 2000
    2   ------ Interrupt ------  ------ Interrupt ------  
    2   1003 sub  $1, %bx
    2   1004 test $0, %bx
    2   1005 jgt .top
    2   ------ Interrupt ------  ------ Interrupt ------  
    2                            1003 sub  $1, %bx
    2                            1004 test $0, %bx
    2                            1005 jgt .top
    2   ------ Interrupt ------  ------ Interrupt ------  
    2   1006 halt
    2   ----- Halt;Switch -----  ----- Halt;Switch -----  
    2                            1006 halt
```

With bx=1 the answer is 2. With -i 1 both read 0 before either writes, with -i 2 read+add complete but interrupt is before the write -> 1. With -i 3 the instruction set completes atomically.

---

## Question 8

```bash
for i in $(seq 1 24); do
  v=$(python3 x86.py -p looping-race-nolock.s -a bx=100 -t 2 -M 2000 -i $i -c \
      | awk '/halt/{v=$1} END{print v}')
  echo "-i $i -> final value = $v"
done
```

```
-i 1  -> final value = 100
-i 2  -> final value = 100
-i 3  -> final value = 200
-i 4  -> final value = 150
-i 5  -> final value = 160
-i 6  -> final value = 200
-i 7  -> final value = 157
-i 8  -> final value = 150
-i 9  -> final value = 200
-i 10 -> final value = 160
-i 11 -> final value = 164
-i 12 -> final value = 200
-i 13 -> final value = 160
-i 14 -> final value = 158
-i 15 -> final value = 200
-i 16 -> final value = 164
-i 17 -> final value = 166
-i 18 -> final value = 200
-i 19 -> final value = 163
-i 20 -> final value = 160
-i 21 -> final value = 200
-i 22 -> final value = 164
-i 23 -> final value = 168
-i 24 -> final value = 200
```

With bx=100 the only correct intervals are multiples of 3: -i 3, 6, 9, 12, 15, 18, 21, 24 → 200. Everything else loses updates. The surprising ones are the in between results such as -i 4 and -i 8 → 150, -i 5/-i 10/-i 13/-i 20 → 160, -i 7 → 157, -i 11 → 164, etc. These are surprising because the loss isn't an "everything lost" kind of situation. It turns out that the count of lost updates depends on how the fixed interval lines up with the 3-instruction section across many turns.

---

## Question 9

```bash
python3 x86.py -p wait-for-me.s -a ax=1,ax=0 -R ax -M 2000 -c
```

```
ARG seed 0
ARG numthreads 2
ARG program wait-for-me.s
ARG interrupt frequency 50
ARG interrupt randomness False
ARG argv ax=1,ax=0
ARG load address 1000
ARG memsize 128
ARG memtrace 2000
ARG regtrace ax
ARG cctrace False
ARG printstats False
ARG verbose False

 2000      ax          Thread 0                Thread 1         
    0       1   
    0       1   1000 test $1, %ax
    0       1   1001 je .signaller
    1       1   1006 mov  $1, 2000
    1       1   1007 halt
    1       0   ----- Halt;Switch -----  ----- Halt;Switch -----  
    1       0                            1000 test $1, %ax
    1       0                            1001 je .signaller
    1       0                            1002 mov  2000, %cx
    1       0                            1003 test $1, %cx
    1       0                            1004 jne .waiter
    1       0                            1005 halt
```

---

## Question 10

```bash
python3 x86.py -p wait-for-me.s -a ax=0,ax=1 -R ax -M 2000 -c
```

```
ARG seed 0
ARG numthreads 2
ARG program wait-for-me.s
ARG interrupt frequency 50
ARG interrupt randomness False
ARG argv ax=0,ax=1
ARG load address 1000
ARG memsize 128
ARG memtrace 2000
ARG regtrace ax
ARG cctrace False
ARG printstats False
ARG verbose False

 2000      ax          Thread 0                Thread 1         
    0       0   
    0       0   1000 test $1, %ax
    0       0   1001 je .signaller
    0       0   1002 mov  2000, %cx
    0       0   1003 test $1, %cx
    0       0   1004 jne .waiter
    0       0   1002 mov  2000, %cx
    0       0   1003 test $1, %cx
    0       0   1004 jne .waiter
    0       0   1002 mov  2000, %cx
    0       0   1003 test $1, %cx
    0       0   1004 jne .waiter
    ... (repeats while memory[2000] == 0) ...
    0       1   ------ Interrupt ------  ------ Interrupt ------  
    0       1                            1000 test $1, %ax
    0       1                            1001 je .signaller
    1       1                            1006 mov  $1, 2000
    1       1                            1007 halt
    1       0   ----- Halt;Switch -----  ----- Halt;Switch -----  
    1       0   1002 mov  2000, %cx
    1       0   1003 test $1, %cx
    1       0   1004 jne .waiter
    1       0   1005 halt
```

```bash
# with -i 1000, count how many times thread 0 spins (executes the waiter's mov):
python3 x86.py -p wait-for-me.s -a ax=0,ax=1 -R ax -M 2000 -i 1000 -c \
    | awk 'BEGIN{c=0} /1002 mov/{c++} END{print "thread-0 spin iterations:", c}'
```

```
thread-0 spin iterations: 334
```

```bash
# with small random intervals (-i 5 -r -s 0):
python3 x86.py -p wait-for-me.s -a ax=0,ax=1 -R ax -M 2000 -i 5 -r -s 0 -c
```

```
 2000      ax          Thread 0                Thread 1         
    0       0   
    0       0   1000 test $1, %ax
    0       0   1001 je .signaller
    0       0   1002 mov  2000, %cx
    0       0   1003 test $1, %cx
    0       0   1004 jne .waiter
    0       1   ------ Interrupt ------  ------ Interrupt ------  
    0       1                            1000 test $1, %ax
    0       1                            1001 je .signaller
    1       1                            1006 mov  $1, 2000
    1       1                            1007 halt
    1       0   ----- Halt;Switch -----  ----- Halt;Switch -----  
    1       0   ------ Interrupt ------  ------ Interrupt ------  
    1       0   1002 mov  2000, %cx
    1       0   1003 test $1, %cx
    1       0   1004 jne .waiter
    1       0   ------ Interrupt ------  ------ Interrupt ------  
    1       0   1005 halt
```

Now thread 0 (ax=0) is the waiter and runs first, but thread 1 (the signaller) hasn't run yet meaning memory2000 stays 0 and thread 0 busy-waits, spinning on the load/test/jump loop. It only stops when the scheduler switches to thread 1, which sets value=1. With -i 1000, thread 0 spins around 334 times before the switch.  Random intervals vary the spin count (with -i 5 -r -s 0 it spins just once). This definitely not efficient. The waiter burns CPU cycles spinning instead of yielding/blocking like it should.
