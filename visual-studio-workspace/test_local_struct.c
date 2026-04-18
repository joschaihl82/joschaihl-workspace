int printf(const char *fmt, ...);

struct S {
  int a;
  int b;
};

int main() {
  struct S s = { 1, 2 };
  printf("s.a=%d s.b=%d\n", s.a, s.b);
  if (s.a != 1) return 1;
  if (s.b != 2) return 2;
  return 0;
}