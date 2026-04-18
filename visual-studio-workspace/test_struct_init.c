struct S { int a; char b; int c; };
struct S s = { 1, 'x', 3 };
int main() { return s.a + s.b + s.c; }
