int printf(const char *fmt, ...);

int add_ten(int a, int b, int c, int d, int e, int f, int g, int h, int i, int j) {
  return a + b + c + d + e + f + g + h + i + j;
}

int main() {
  int result = add_ten(1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
  printf("result: %d\n", result);
  return 0;
}
