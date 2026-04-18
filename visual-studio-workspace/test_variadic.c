int printf(const char *fmt, ...);

int my_func(int x, ...) {
  return x;
}

int main() {
  int result = my_func(42, 1, 2, 3);
  printf("result: %d\n", result);
  return 0;
}
