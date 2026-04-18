int printf(const char *fmt, ...);

int add_many(int a, int b, int c, int d, int e, int f, int g, int h) {
  return a + b + c + d + e + f + g + h;
}

int main() {
  int result = add_many(1, 2, 3, 4, 5, 6, 7, 8);
  printf("result: %d\n", result);
  return 0;
}
