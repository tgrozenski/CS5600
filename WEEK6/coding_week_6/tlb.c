#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#ifdef __linux__
#include <sched.h>
#endif
#ifdef __APPLE__
#include <mach/mach.h>
#include <mach/thread_policy.h>
#endif

int main(int argc, char *argv[]) {
    int pages = atoi(argv[1]);
    int trials = atoi(argv[2]);
    int jump = 4096 / sizeof(int);
    int *a = malloc(pages * 4096);
    long sum = 0;
    struct timeval t1, t2;
    int i, j;

    for (i = 0; i < pages * jump; i += jump)
        a[i] = 0;

#ifdef __linux__
    cpu_set_t set;
    CPU_ZERO(&set);
    CPU_SET(0, &set);
    sched_setaffinity(0, sizeof(set), &set);
#endif
#ifdef __APPLE__
    thread_affinity_policy_data_t p = { 0 };
    thread_policy_set(mach_thread_self(), THREAD_AFFINITY_POLICY,
                      (thread_policy_t)&p, 1);
#endif

    gettimeofday(&t1, NULL);
    for (j = 0; j < trials; j++)
        for (i = 0; i < pages * jump; i += jump)
            sum += a[i];
    gettimeofday(&t2, NULL);

    printf("%d %d %.2f\n", pages, trials,
        (double)((t2.tv_sec - t1.tv_sec) * 1000000 +
                 (t2.tv_usec - t1.tv_usec)) * 1000.0 / ((double)pages * trials));
    return (int)sum;
}
