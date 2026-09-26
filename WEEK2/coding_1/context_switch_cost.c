#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sched.h>
#include <sys/wait.h>

int main() {
    int p1[2], p2[2];
    struct timeval start, end;
    long iters = 100000;
    char c = 'x';
    cpu_set_t set;
    CPU_ZERO(&set);
    CPU_SET(0, &set);
    sched_setaffinity(getpid(), sizeof(set), &set);
    pipe(p1);
    pipe(p2);

    if (fork() == 0) {
        for (long i = 0; i < iters; i++) {
            read(p1[0], &c, 1);
            write(p2[1], &c, 1);
        }
        exit(0);
    }

    gettimeofday(&start, NULL);
    for (long i = 0; i < iters; i++) {
        write(p1[1], &c, 1);
        read(p2[0], &c, 1);
    }
    gettimeofday(&end, NULL);
    wait(NULL);
    long usec = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec-start.tv_usec);
    printf("Context switch average: %f usec\n", ((float)usec/iters)/2.0);
}
