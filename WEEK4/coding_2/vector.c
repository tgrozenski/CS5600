#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  int * vec;
  int vec_len = 0;

  while (1) {
    int x;
    scanf("%d", &x);

    if (vec_len == 0) {
      vec = malloc(sizeof(int));
      vec[0] = x;
    } else {
      vec = realloc(vec, sizeof(int) * (vec_len + 1));
      vec[vec_len] = x;
    }

    vec_len++;
    printf("vec len = %d\n", vec_len);
    print_vec(vec, vec_len);
  }
}

void print_vec(int * vec, int len) {
  for (int i = 0; i < len; i++) {
    printf("%d, ", vec[i]);
  }
  printf("\n");
}
