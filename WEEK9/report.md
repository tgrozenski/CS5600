# Week 9 Ch 31 - Semaphores

## 1. fork-join.c

I filled in the semaphore so the parent waits for the child to finish. I initialized the semaphore to 0 in the parent, had the child post it after printing child, and had the parent wait on it before printing parent: end. This makes the parent always print after the child.

## 2. rendezvous.c

I filled in the two semaphores so both children print their "before" message before either prints "after". Each child posts its own semaphore and then waits on the other one. both must arrive before either can move on.

## 3. barrier.c

I filled in the barrier so every thread prints "before" before any thread prints "after". I added a few thing such as a mutex semaphore, a turnstile semaphore, and a counter. Each thread locks the mutex and increments the counter. the last one to arrive opens the turnstile, then every thread passes through the turnstile.

## 4. reader-writer.c

I filled in the reader-writer lock so many readers can read at once but a writer runs alone. I used one semaphore to protect the reader count and another to guard the write. The first reader locks out writers and the last reader lets them back in. Writers may starve here.

## 5. reader-writer-nostarve.c

I filled in the same reader-writer lock but added a semaphore called a turnstile so that a writer waiting blocks new readers from entering. This lets the writer eventually get in instead of starving.

## 6. mutex-nostarve.c

I created a mutex from semaphores that will not starve. The Threads arriving while it is locked line up on a gate semaphore, then the lock is handed to the next waiter in order. I also needed to write a worker and main function to run threads that acquire the lock 100000 times and count how often they get it. We can see that each thread gets it the same number of times.

# Week 9 Ch 32 - Concurrency Bugs

## 1.

```bash
./vector-deadlock -n 2 -l 1 -v
```

```
->add(0, 1)
<-add(0, 1)
              ->add(0, 1)
              <-add(0, 1)
```

the `->add(0, 1)` means a thread is about to do v[0] += v[1], and `<-add(0, 1)` means it finished. The indented lines are thread 1, the unindented lines are thread 0. Both threads do the same add(0, 1), so the values never actually change. The order of the two threads does actually changes. Over 30 runs the first line was thread 0 in 28 runs and thread 1 in 2 runs, so the output ordering isn't deterministic.

## 2.

`-d` makes odd threads reverse the order, so thread 0 does add(0,1) and thread 1 does add(1,0). Each grabs a different lock first, which causes a deadlock. Deadlocks out of 10 runs per loop count:

```bash
for l in 100 1000 10000 100000; do
  timeout 2 ./vector-deadlock -d -n 2 -l $l >/dev/null 2>&1
done
```

```
-l 100       0 / 10 deadlocked
-l 1000      5 / 10 deadlocked
-l 10000    10 / 10 deadlocked
-l 100000   10 / 10 deadlocked
```

I observed deadlock did not always occur. With few loops one thread usually finishes first, but more loops means more chances to collide, so at 10000+ it is basically guaranteed to deadlock.

## 3.

```bash
./vector-deadlock -d -n $n -l 100000
```

```
-n 1    0 / 5 deadlocked
-n 2    5 / 5 deadlocked
-n 3    5 / 5 deadlocked
-n 4    5 / 5 deadlocked
-n 5    5 / 5 deadlocked
-n 10   5 / 5 deadlocked
```

Only -n 1 guarantees no deadlock. A single thread cannot deadlock with itself.

## 4. vector-global-order.c

It avoids deadlock by grabbing locks in a fixed order, lower address first. This prevents a circular wait. With this two threads can't hold a low lock while waiting for a high one in opposite orders. There is a special case when the source and dest are equal where it locks only once since a normal mutex cannot be locked twice by the same thread

## 5. Timing global-order

```bash
./vector-global-order -t -n 2 -l 100000 -d
./vector-global-order -t -n 2 -l 200000 -d
./vector-global-order -t -n 2 -l 1000000 -d
./vector-global-order -t -n 4 -l 100000 -d
```

```
n=2 l=100000   0.02s
n=2 l=200000   0.05s
n=2 l=1000000  0.23s
n=4 l=100000   0.12s
```

Time grows linearly with loops, and goes up when more threads fight over the same two vectors.

## 6. The -p flag

```bash
./vector-global-order -t -n 2 -l 100000 -d
./vector-global-order -t -n 2 -l 100000 -d -p
./vector-global-order -t -n 4 -l 100000 -d
./vector-global-order -t -n 4 -l 100000 -d -p
```

```
n=2  no -p  0.03s     with -p  0.01s
n=4  no -p  0.13s     with -p  0.03s
```

With -p each thread works on its own vectors, so there is no lock contention and it runs in parallel cleanly. This is much faster.

## 7. vector-try-wait.c

```bash
./vector-try-wait -t -n 2 -l 100000 -d
./vector-try-wait -t -n 4 -l 100000 -d
./vector-try-wait -t -n 8 -l 100000 -d
./vector-try-wait -t -n 16 -l 100000 -d
```

```
n=2   Retries: 945431     0.19s
n=4   Retries: 2641315    0.76s
n=8   Retries: 12209975   4.50s
n=16  Retries: 51914164   41.22s
```

It is much slower than global-order (0.02s for n=2) and retries explode as threads increase. The first pthread_mutex_trylock() is not really needed, it is just a busy. The second trylock on the source is the important one. It releases the destination and retries instead of holding and waiting

## 8. vector-avoid-hold-and-wait.c

```bash
./vector-avoid-hold-and-wait -t -n 2 -l 100000 -d
./vector-avoid-hold-and-wait -t -n 4 -l 100000 -d
./vector-avoid-hold-and-wait -t -n 2 -l 100000 -d -p
./vector-avoid-hold-and-wait -t -n 4 -l 100000 -d -p
```

```
n=2  no -p  0.09s     with -p  0.06s
n=4  no -p  0.43s     with -p  0.11s
```

The main problem is the single global lock around all lock acquisition. It stops deadlock but serializes everything. Even threads on different vectors must take the same global lock first hurting performance.

## 9. vector-nolock.c semantics

```bash
./vector-nolock -t -n 2 -l 100000 -d
```

It does not give the same semantics. It uses an atomic fetch with add per element, so only a single element add is atomic. The whole 100 element vector add is not atomic. Another thread can see a half-updated vector. It also never locks the source, so the source can change mid-add.

## 10. vector-nolock.c performance

```bash
./vector-nolock -t -n 2 -l 100000 -d
./vector-nolock -t -n 4 -l 100000 -d
./vector-nolock -t -n 2 -l 100000 -d -p
./vector-nolock -t -n 4 -l 100000 -d -p
```

```
n=2  no -p  0.52s     with -p  0.10s
n=4  no -p  1.12s     with -p  0.10s
```

Without -p it is the slowest of all because the atomic fetch with add causes cache line bouncing when many threads hit the same vectors. With -p it is fast because each thread touches its own vectors with no contention.
