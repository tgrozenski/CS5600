#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t rc = fork();

    if (rc == 0) {
        pid_t wc = wait(NULL);
        printf("Child wait returns: %d\n", wc);
    } else if (rc > 0) {
        pid_t wc = wait(NULL);

        printf("parent wait returns: %d\n", wc);
    }

}
