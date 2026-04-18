int printf(const char *fmt, ...);

int mixed_params(char a, int b, char c, int d, short e, int f, char g, int h, short i) {
  return a + b + c + d + e + f + g + h + i;
}

int main() {
  int result = mixed_params(1, 2, 3, 4, 5, 6, 7, 8, 9);
  printf("result: %d\n", result);
  return 0;
}
