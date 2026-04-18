int printf(const char *fmt, ...);
int snprintf(char *str, int size, const char *fmt, ...);

int my_variadic_func(int x, int y, ...) {
  return x + y;
}

int main() {
  int result = my_variadic_func(10, 20, 30, 40);
  printf("result: %d\n", result);
  return 0;
}
