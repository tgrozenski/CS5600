#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main() {
    int fd = open("test_text.txt", O_CREAT | O_WRONLY | O_TRUNC, 0644);
    pid_t rc = fork();

    if (rc == 0) {
        write(fd, "child\n", 6);
    } else if (rc > 0) {
        write(fd, "parent\n", 7);
    }

}
