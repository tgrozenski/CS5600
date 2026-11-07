#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

typedef struct
{
    int global;
    pthread_mutex_t glock;
    int *local;
    pthread_mutex_t *llock;
    int threshold;
} counter_t;

counter_t c;
int iters;

void *worker(void *a)
{
    int cpu = (int)(long)a;

    for (int i = 0; i < iters; i++)
    {
        pthread_mutex_lock(&c.llock[cpu]);
        c.local[cpu]++;
        if (c.local[cpu] >= c.threshold)
        {
            pthread_mutex_lock(&c.glock);
            c.global += c.local[cpu];
            pthread_mutex_unlock(&c.glock);
            c.local[cpu] = 0;
        }
        pthread_mutex_unlock(&c.llock[cpu]);
    }
    return NULL;
}

int main(int argc, char *argv[])
{
    int nthreads = atoi(argv[1]);
    iters = atoi(argv[2]);
    c.threshold = atoi(argv[3]);

    pthread_mutex_init(&c.glock, NULL);
    c.local = calloc(nthreads, sizeof(int));
    c.llock = malloc(nthreads * sizeof(pthread_mutex_t));
    for (int i = 0; i < nthreads; i++)
    {
        pthread_mutex_init(&c.llock[i], NULL);
    }

    pthread_t *t = malloc(nthreads * sizeof(pthread_t));
    struct timeval a, b;

    gettimeofday(&a, NULL);
    for (int i = 0; i < nthreads; i++)
    {
        pthread_create(&t[i], NULL, worker, (void *)(long)i);
    }
    for (int i = 0; i < nthreads; i++)
    {
        pthread_join(t[i], NULL);
    }
    gettimeofday(&b, NULL);

    double secs = (b.tv_sec - a.tv_sec) + (b.tv_usec - a.tv_usec) / 1000000.0;
    printf("time: %f seconds (threads=%d threshold=%d)\n", secs, nthreads, c.threshold);
    return 0;
}
