#include "stack.h"

void stack_print(int *stack, int size) {
  int i = 0;
  fprintf(stderr, "\t[");
  for (i = 0; i < size; i++) {
    fprintf(stderr, "%d, ", stack[i]);
  }
  fprintf(stderr, "]");
}