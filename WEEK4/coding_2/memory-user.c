#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

int main(int argc, char *argv[]){
  int size = atoi(argv[1]) * 1024 * 1024;
  int* arr = malloc(size);
  int elements = size / sizeof(int);
  printf("PID: %ld\n", (long)getpid());

  while (true) {
    for (int i = 0; i < elements; i++) {
      arr[i] = i;
    }
  }
}
