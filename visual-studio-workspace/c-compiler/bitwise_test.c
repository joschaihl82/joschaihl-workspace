int main(){
  int a=1;
  int b=2;
  int c = a & b;
  c |= 0x10;
  c ^= ~a;
  c <<= 2;
  c >>= 1;
  return c;
}
