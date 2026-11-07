#include <stdio.h>
#include <sys/time.h>

int main()
{
    struct timeval t0, t1;
    long min = 1L << 60;

    gettimeofday(&t0, NULL);
    for (int i = 0; i < 1000000; i++)
    {
        gettimeofday(&t1, NULL);
        long d = (t1.tv_sec - t0.tv_sec) * 1000000L + (t1.tv_usec - t0.tv_usec);
        if (d > 0 && d < min)
        {
            min = d;
        }
        t0 = t1;
    }

    printf("smallest interval: %ld microseconds\n", min);
    return 0;
}
