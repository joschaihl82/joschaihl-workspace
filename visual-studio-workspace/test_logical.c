int g;
int inc() { g++; return g; }

int main() {
  g = 0;
  int a = 0;
  if (0 && inc()) a = 1;      // inc not called
  if (1 || inc()) a += 2;     // short-circuit, inc not called
  if (1 && inc()) a += 4;     // inc called
  if (0 || inc()) a += 8;     // inc called
  return a + g;               // expected 16
}
