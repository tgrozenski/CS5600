# Chapter 26

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
python3 x86.py -p wait-for-me.s -a ax=0,ax=1 -R ax -M 2000 -i 1000 -c \
    | awk 'BEGIN{c=0} /1002 mov/{c++} END{print "thread-0 spin iterations:", c}'
```

```
thread-0 spin iterations: 334
```

```bash
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

# Chapter 28

---

## Question 1

The flag.s file creates a lock using a single memory variable flag which can be set to 0 for free and 1 to be held. to aquire: mov flag,%ax; test $0,%ax; jne .acquire; mov $1,flag. Critical section increments count. To Release: mov $0,flag. The acquire is not atomic since the process is load, test, then store, so it can be interrupted between the test and the set.

---

## Question 2

```bash
python3 x86.py -p flag.s -M flag,count -R ax,bx -c
```

```
 flag count      ax    bx          Thread 0                Thread 1         
    0     0       0     0   
    0     0       0     0   1000 mov  flag, %ax
    1     0       0     0   1003 mov  $1, flag
    ...
    0     2       2    -1                            1011 halt
```

With the defaults (-t 2, -i 50, no -a) each thread loops once so the flag ends at 0 indicating the lock is released and the count ends at 2. It only works out because -i 50 is bigger than the whole program so the threads never end up interleaving.

---

## Question 3

```bash
python3 x86.py -p flag.s -t 2 -a bx=2,bx=2 -M flag,count -i 100 -c
```

```
 flag count          Thread 0                Thread 1
    0     0   
    0     0   1000 mov  flag, %ax
    ...
    0     4                            1011 halt
```

With these args each thread loops twice and count ends at 4. Things still worked because no interrupts happened mid run, the lock itself was never really tested in this case.

---

## Question 4

```bash
python3 x86.py -p flag.s -t 2 -a bx=2,bx=2 -M flag,count -i 1 -c
```

```
 flag count          Thread 0                Thread 1         
    0     0   
    0     0   1000 mov  flag, %ax
    1     0   1003 mov  $1, flag
    1     0                            1003 mov  $1, flag
    ...
    0     2                            1011 halt
```

```bash
for i in $(seq 1 24); do
  v=$(python3 x86.py -p flag.s -t 2 -a bx=2,bx=2 -M count -i $i -c \
      | awk '/1011 halt/{v=$1} END{print v}')
  echo "-i $i -> count = $v"
done
```

```
-i 1  -> count = 2
-i 2  -> count = 2
-i 3  -> count = 3
-i 4  -> count = 3
-i 5  -> count = 4
-i 6  -> count = 3
-i 7  -> count = 3
-i 8  -> count = 4
-i 9  -> count = 4
-i 10 -> count = 4
...
-i 24 -> count = 4
```

With these args the lock fails: -i 1,2 gets count 2; -i 3,4 gets count 3; -i 6,7 gets count 3. We get 4, the correct count, only at -i 5 and -i ≥ 8. I noticed that bad outcomes happen when an interrupt happens inside the 4-instruction acquire (mov flag, test, jne, mov $1,flag), letting both threads see flag is 0 and both set it to 1.

---

## Question 5

```bash
cat test-and-set.s
```

```
.var mutex
.var count

.main
.top

.acquire
mov  $1, %ax
xchg %ax, mutex
test $0, %ax
jne  .acquire

mov  count, %ax
add  $1, %ax
mov  %ax, count

mov  $0, mutex

sub  $1, %bx
test $0, %bx
jgt .top

halt
```

test-and-set.s builds the lock with the atomic xchg. To aquire it does the folloing: mov $1,%ax; xchg %ax,mutex; test $0,%ax; jne .acquire — xchg atomically swaps and returns the old value. To release it does mov $0,mutex.

---

## Question 6

```bash
python3 x86.py -p test-and-set.s -t 2 -a bx=2,bx=2 -M mutex,count -i 3 -c
```

```
mutex count          Thread 0                Thread 1         
    0     0   
    0     0   1000 mov  $1, %ax
    1     0   1001 xchg %ax, mutex
    ...
    0     4                            1011 halt
```

```bash
python3 x86.py -p test-and-set.s -t 2 -a bx=2,bx=2 -M mutex,count -i 3 -c -S 2>&1 | grep "STATS"
```

```
STATS:: Instructions    66
```

```bash
for i in 1 2 3 4 5 6 8 10; do
  v=$(python3 x86.py -p test-and-set.s -t 2 -a bx=2,bx=2 -M count -i $i -c \
      | awk '/1011 halt/{v=$1} END{print v}')
  echo "-i $i -> count = $v"
done
```

```
-i 1  -> count = 4
-i 2  -> count = 4
-i 3  -> count = 4
-i 4  -> count = 4
-i 5  -> count = 4
-i 6  -> count = 4
-i 8  -> count = 4
-i 10 -> count = 4
```

In my experiement test-and-set always produced the correct count. Mutual exclusion was maintained since xchg is atomic. I did observe that it can be inefficient. When the lock is held the waiting thread spins. I noticed around 20 instructions were wasted of the 66 total.

---

## Question 7

```bash
python3 x86.py -p test-and-set.s -t 2 -a bx=1,bx=1 -M mutex,count -P 00001111 -c
```

```
mutex count          Thread 0                Thread 1         
    0     0   
    0     0   1000 mov  $1, %ax
    1     0   1001 xchg %ax, mutex
    ...
    0     2                            1011 halt
```

Using -P 00001111 thread 0 gets the lock, thread 1 tries xchg, gets 1 back, and spins. The correct thing happens: thread 1 waits until thread 0 releases, then acquires; final count = 2 (good). It was also worth testing when both threads attempting the first xchg simultaneously and a lock already held with many contending threads.

---

## Question 8

```bash
cat peterson.s
```

```
.var flag   2

.var turn
.var count

.main
lea flag, %fx

mov %bx, %cx
neg %cx
add $1, %cx

.acquire
mov $1, 0(%fx,%bx,4)
mov %cx, turn

.spin1
mov 0(%fx,%cx,4), %ax
test $1, %ax
jne .fini

.spin2
mov turn, %ax
test %cx, %ax
je .spin1

.fini
mov count, %ax
add $1, %ax
mov %ax, count

.release
mov $0, 0(%fx,%bx,4)
mov %cx, turn
halt
```

This implements peterson's algorithm which uses a flag array a turn with functions as a tiebreaker. Requires bx = thread id, cx = 1-self. A thread spins only when the other has also set its flag and turn equals 1-self.

---

## Question 9

```bash
python3 x86.py -p peterson.s -t 2 -a bx=0,bx=1 -M 100,104,108,112 -c
python3 x86.py -p peterson.s -t 2 -a bx=0,bx=1 -M 100,104,108,112 -i 2 -c
```

```
  100   104   108   112          Thread 0                Thread 1
    0     0     0     0
    1     0     1     0   1005 mov %cx, turn
    ...
    0     0     0     2                            1017 halt
```

With these args count ends at 2 so mutual exclusion and progress both hold. Different values for -i do not impact correctness.

---

## Question 10

```bash
python3 x86.py -p peterson.s -t 2 -a bx=0,bx=1 -M 100,104,108,112 -P 01 -c
```

```
  100   104   108   112          Thread 0                Thread 1
    0     0     0     0
    1     1     0     0                            1004 mov $1, 0(%fx,%bx,4)
    ...
    0     0     0     2                            1017 halt
```

-P 01 is the most challenging schedule but the count still ends at 2 meaning mutual and exclusion and progress both hold. When both threads try to aquire the lock only one gets it and the other spins. The spinning thread eventually proceeds and passes its turn back so we avoid a deadlock.


---

## Question 11

```bash
python3 x86.py -p ticket.s -t 2 -a bx=2,bx=2 -M ticket,turn,count -c
```

```
ticket  turn count          Thread 0                Thread 1         
    0     0     0   
    0     0     0   1000 mov $1, %ax
    1     0     0   1001 fetchadd %ax, ticket
    ...
    4     4     4                            1013 halt
```

```bash
python3 x86.py -p ticket.s -t 1 -a bx=2000 -M count -S -c 2>&1 | grep "STATS:: Instructions"
python3 x86.py -p ticket.s -t 2 -a bx=1000,bx=1000 -M count -S -c 2>&1 | grep "STATS:: Instructions"
python3 x86.py -p ticket.s -t 2 -a bx=1000,bx=1000 -M count -c | awk '/1013 halt/{v=$1} END{print "final count =", v}'
```

```
STATS:: Instructions    26001
STATS:: Instructions    99463
final count = 2000
```

ticket.s matches the chapter's ticket lock. With bx=1000 each, count ends at the correct value 2000 and acquisition is FIFO. The threads do spin-wait a lot. 99,463 total instructions vs 26,001 for the single-thread baseline meaning around 73,000 instructions were wasted spinning

---

## Question 12

```bash
python3 x86.py -p ticket.s -t 3 -a bx=1000 -M count -S -c 2>&1 | grep -E "numthreads|STATS:: Instructions"
python3 x86.py -p ticket.s -t 4 -a bx=500  -M count -S -c 2>&1 | grep -E "numthreads|STATS:: Instructions"
```

```
ARG numthreads 3
STATS:: Instructions    149424
ARG numthreads 4
STATS:: Instructions    99291
```

In my experiment more threads meant more total spinning. Things remained correct and fair, but the spinning and waiting scales up.

---

## Question 13

```bash
python3 x86.py -p test-and-set.s -t 2 -a bx=1000,bx=1000 -M count -S -c 2>&1 | grep "STATS:: Instructions"
python3 x86.py -p yield.s -t 2 -a bx=1000,bx=1000 -M count -S -c 2>&1 | grep "STATS:: Instructions"
```

```
STATS:: Instructions    33954
STATS:: Instructions    23334
```

```bash
python3 x86.py -p yield.s -t 2 -a bx=2,bx=2 -M mutex,count -i 3 -c
```

```
mutex count          Thread 0                Thread 1         
    0     0   
    1     0   1001 xchg %ax, mutex
    1     0                            1004 yield
    ...
    0     4                            1013 halt
```

13. With bx=1000, test-and-set.s uses 33,954 instructions and yield.s uses only 23,334 which is 31% less. The happen when a thread finds the lock held. Instead of spinning for its whole time, yield.s yields and immediately hands the CPU to the other thread. Savings happen whenever the lock is requested and the holder is able to give it up.


---

## Question 14

```bash
python3 x86.py -p test-and-test-and-set.s -t 2 -a bx=2,bx=2 -M mutex,count -i 3 -c
```

```
mutex count          Thread 0                Thread 1
    0     0   
    0     0   1000 mov  mutex, %ax
    1     0   1004 xchg %ax, mutex
    ...
    0     4                            1014 halt
```

```bash
python3 x86.py -p test-and-set.s          -t 2 -a bx=1000,bx=1000 -M count -S -c 2>&1 | grep "STATS:: Instructions"
python3 x86.py -p test-and-test-and-set.s -t 2 -a bx=1000,bx=1000 -M count -S -c 2>&1 | grep "STATS:: Instructions"
python3 x86.py -p test-and-set.s          -t 2 -a bx=1000,bx=1000 -M count -c 2>&1 | grep -c xchg
python3 x86.py -p test-and-test-and-set.s -t 2 -a bx=1000,bx=1000 -M count -c 2>&1 | grep -c xchg
```

```
STATS:: Instructions    33954
STATS:: Instructions    42821
xchg count = 4988
xchg count = 2284
```

test-and-test-and-set.s adds mov mutex,%ax read before the atomic xchg. This spins and only attempts the expensive xchg when the load shows the lock might be free. In the simulator it runs more total instructions but executes far fewer atomic xchg operations. (2,284 vs 4,988). Instead of saving instruction count it's avoiding the expensive atomic instruction on real multiprocessors.

# Chapter 29

1. timer.c uses gettimeofday. It measures in microseconds. The smallest interval it can measure is 6 microseconds.

2. counter.c is a simple counter with one lock. The system has 4 CPUs. It got slower as threads grew, 0.010 seconds with 1 thread and 0.182 seconds with 8 threads. The 4 CPUs do not help because the one lock lets only one thread increment at a time.

3. sloppy.c is the approximate counter. With 4 threads it got faster as the threshold grew, 0.209 seconds at threshold 1 and 0.098 seconds at threshold 1024. This matches the chapter. A small threshold acts like one global lock. A bigger threshold touches the global counter less so it is faster.

4. list-hoh.c is the hand over hand list. It got slower as threads grew, 0.119 seconds with 1 thread and 2.339 seconds with 4 threads. It beats a single lock list only when the list is long and threads touch different nodes. On a short list the per node lock cost is not worth it.

5. btree.c is a B-tree with one lock. It got slower as threads grew, 0.117 seconds with 1 thread and 1.191 seconds with 8 threads.

6. btree-lock.c is the same B-tree with a lock per node. It was worse than the one lock version, 0.199 seconds with 1 thread and 11.881 seconds with 8 threads. The many locks add overhead and threads pile up at the root. Per node locking only helps on a larger tree with less contention.
