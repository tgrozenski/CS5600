#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>

typedef struct
{
    int value;
    pthread_mutex_t lock;
} counter_t;

counter_t counter;
int iters;

void *worker(void *a)
{
    for (int i = 0; i < iters; i++)
    {
        pthread_mutex_lock(&counter.lock);
        counter.value++;
        pthread_mutex_unlock(&counter.lock);
    }
    return NULL;
}

int main(int argc, char *argv[])
{
    int nthreads = atoi(argv[1]);
    iters = atoi(argv[2]);

    pthread_mutex_init(&counter.lock, NULL);
    printf("CPUs available: %ld\n", sysconf(_SC_NPROCESSORS_ONLN));

    pthread_t *t = malloc(nthreads * sizeof(pthread_t));
    struct timeval a, b;

    gettimeofday(&a, NULL);
    for (int i = 0; i < nthreads; i++)
    {
        pthread_create(&t[i], NULL, worker, (void *)(long)(i + 1));
    }
    for (int i = 0; i < nthreads; i++)
    {
        pthread_join(t[i], NULL);
    }
    gettimeofday(&b, NULL);

    double secs = (b.tv_sec - a.tv_sec) + (b.tv_usec - a.tv_usec) / 1000000.0;
    printf("value: %d\n", counter.value);
    printf("time: %f seconds\n", secs);
    return 0;
}
