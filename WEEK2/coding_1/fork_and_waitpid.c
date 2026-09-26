#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t rc = fork();

    if (rc == 0) {
        printf("child running\n");
    } else if (rc > 0) {
        pid_t wc = waitpid(rc, NULL, 0);
        printf("parent waitpid returns: %d\n", wc);
    }

}
