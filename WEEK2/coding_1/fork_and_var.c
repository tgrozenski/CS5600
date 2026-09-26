#include <stdio.h>
#include <unistd.h>

int main() {
    int x = 100;
    pid_t rc = fork();

    if (rc == 0) {
        x = 200;
        printf("child x %d\n", x);
    } else if (rc > 0) {
        x = 300;
        printf("parent x %d", x);
    }

}
