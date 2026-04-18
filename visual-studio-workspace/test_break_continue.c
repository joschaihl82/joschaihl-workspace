int printf(const char *fmt, ...);

int main() {
  int i;
  int sum = 0;

  // Test break in while
  i = 0;
  while (1) {
    if (i == 5) break;
    i++;
  }
  if (i != 5) return 1;

  // Test continue in while
  i = 0;
  sum = 0;
  while (i < 10) {
    i++;
    if (i % 2 == 0) continue;
    sum += i;
  }
  if (sum != 25) return 2;

  // Test break in for
  sum = 0;
  for (i = 0; i < 10; i++) {
    if (i == 5) break;
    sum += i;
  }
  if (sum != 10) return 3;

  // Test continue in for
  sum = 0;
  for (i = 0; i < 10; i++) {
    if (i % 2 == 0) continue;
    sum += i;
  }
  if (sum != 25) return 4;

  // Test nested loops
  sum = 0;
  int j;
  for (i = 0; i < 5; i++) {
    for (j = 0; j < 5; j++) {
      if (j == 2) break;
      sum++;
    }
  }
  // Inner loop runs for j=0, j=1. Breaks at j=2.
  // So it adds 2 to sum, 5 times.
  // Total 10.
  if (sum != 10) return 6;

  printf("OK\n");
  return 0;
}