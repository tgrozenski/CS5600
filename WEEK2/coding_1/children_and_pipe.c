#include <stdio.h>
#include <unistd.h>

int main() {
    int p[2];
    pipe(p);

    if (fork() == 0) {
        dup2(p[1], STDOUT_FILENO);
        close(p[0]);
        close(p[1]);
        printf("child 1 data\n");
    } else if (fork() == 0) {
        dup2(p[0], STDIN_FILENO);
        close(p[0]);
        close(p[1]);
        char buf[64];
        read(STDIN_FILENO, buf, 64);
        printf("child 2 read: %s", buf);
    }

}
