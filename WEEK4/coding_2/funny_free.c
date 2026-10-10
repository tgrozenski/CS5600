#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  int * x = malloc(sizeof(int) * 100);
  free(x + 49);
  printf("The number is: %i\n", x[1]);
}
