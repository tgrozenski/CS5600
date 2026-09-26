#include <stdio.h>
#include <unistd.h>

int main() {
    pid_t rc = fork();
    if (rc == 0) {
        char *myargs[2];
        myargs[0] = "ls";
        myargs[1] = NULL;
        execvp(myargs[0], myargs);
    }

}
