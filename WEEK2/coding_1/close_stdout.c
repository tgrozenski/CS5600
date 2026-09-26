#include <stdio.h>
#include <unistd.h>

int main() {
    pid_t rc = fork();

    if (rc == 0) {
        close(STDOUT_FILENO);
        printf("child trying to print\n");
    }

}
