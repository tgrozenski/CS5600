# Week 9 — Semaphores

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
