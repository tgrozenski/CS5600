#include <stdio.h>
#include <unistd.h>

int main() {
    pid_t rc = fork();

    if (rc == 0) {
        printf("hello\n");
    } else if (rc > 0) {
        sleep(1);
        printf("goodbye\n");
    }

}
