#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  int * x = malloc(sizeof(int));
  *x = 1;
  printf("The number is: %i\n", *x);
}
