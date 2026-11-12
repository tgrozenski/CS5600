#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "common_threads.h"

//
// Here, you have to write (almost) ALL the code. Oh no!
// How can you show that a thread does not starve
// when attempting to acquire this mutex you build?
//

typedef struct __ns_mutex_t {
    sem_t mutex;
    sem_t gate;
    int locked;
    int waiting;
} ns_mutex_t;

void ns_mutex_init(ns_mutex_t *m) {
    m->locked = 0;
    m->waiting = 0;
    Sem_init(&m->mutex, 1);
    Sem_init(&m->gate, 0);
}

void ns_mutex_acquire(ns_mutex_t *m) {
    Sem_wait(&m->mutex);
    if (m->locked) {
        m->waiting++;
        Sem_post(&m->mutex);
        Sem_wait(&m->gate);
    } else {
        m->locked = 1;
        Sem_post(&m->mutex);
    }
}

void ns_mutex_release(ns_mutex_t *m) {
    Sem_wait(&m->mutex);
    if (m->waiting > 0) {
        m->waiting--;
        Sem_post(&m->gate);
    } else {
        m->locked = 0;
    }
    Sem_post(&m->mutex);
}


ns_mutex_t m;
int counts[10];

#define ITERS 100000

void *worker(void *arg) {
    int id = *(int *) arg;
    int i;
    for (i = 0; i < ITERS; i++) {
        ns_mutex_acquire(&m);
        counts[id]++;
        ns_mutex_release(&m);
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    assert(argc == 2);
    int num_threads = atoi(argv[1]);
    assert(num_threads > 0 && num_threads <= 10);

    printf("parent: begin\n");

    ns_mutex_init(&m);

    pthread_t p[num_threads];
    int ids[num_threads];

    int i;
    for (i = 0; i < num_threads; i++) {
        ids[i] = i;
        Pthread_create(&p[i], NULL, worker, &ids[i]);
    }

    for (i = 0; i < num_threads; i++)
        Pthread_join(p[i], NULL);

    int total = 0;
    for (i = 0; i < num_threads; i++) {
        printf("thread %d: %d\n", i, counts[i]);
        total += counts[i];
    }
    printf("total: %d\n", total);

    printf("parent: end\n");
    return 0;
}
