#include <stdio.h>
#include <sys/time.h>

#include <unistd.h>
#include <fcntl.h>

int main() {
    struct timeval start, end;
    int fd = open("/dev/null", O_RDONLY);
    long iters = 1000000;
    gettimeofday(&start, NULL);
    for (long i = 0; i < iters; i++) {
        read(fd, NULL, 0);
    }
    gettimeofday(&end, NULL);
    long usec = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);
    printf("Syscall average: %f usec\n", (float)usec/iters);

    close(fd);
}
