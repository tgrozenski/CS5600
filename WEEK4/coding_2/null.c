#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  int * x = malloc(sizeof(int));
  x = NULL;
  fprintf(stderr, "%i\n", *x);
}
