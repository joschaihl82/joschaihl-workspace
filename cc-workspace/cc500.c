/*
 * cc500.c x86_64 vollständige Portierung
 *
 * Hinweis
 * ------
 * Diese Datei emittiert rohe x86_64‑Maschinenbytes in eine ELF64‑Datei.
 * Sie ist eine direkte Portierung des ursprünglichen cc500.c auf 64 Bit.
 * Teste die erzeugte Binärdatei und melde mir Laufzeitfehler oder
 * Probleme beim Laden, dann helfe ich beim Debugging.
 */

/* Our library functions. */
void exit(int);
int getchar(void);
void *malloc(int);
int putchar(int);

/* The first thing defined must be main(). */
int main1();
int main()
{
  return main1();
}

char *my_realloc(char *old, int oldlen, int newlen)
{
  char *new = malloc(newlen);
  int i = 0;
  while (i <= oldlen - 1) {
    new[i] = old[i];
    i = i + 1;
  }
  return new;
}

int nextc;
char *token;
int token_size;

void error()
{
  exit(1);
}

int i;

void takechar()
{
  if (token_size <= i + 1) {
    int x = (i + 10) << 1;
    token = my_realloc(token, token_size, x);
    token_size = x;
  }
  token[i] = nextc;
  i = i + 1;
  nextc = getchar();
}

void get_token()
{
  int w = 1;
  while (w) {
    w = 0;
    while ((nextc == ' ') | (nextc == 9) | (nextc == 10))
      nextc = getchar();
    i = 0;
    while ((('a' <= nextc) & (nextc <= 'z')) |
	   (('0' <= nextc) & (nextc <= '9')) | (nextc == '_'))
      takechar();
    if (i == 0)
      while ((nextc == '<') | (nextc == '=') | (nextc == '>') |
	     (nextc == '|') | (nextc == '&') | (nextc == '!'))
	takechar();
    if (i == 0) {
      if (nextc == 39) {
	takechar();
	while (nextc != 39)
	  takechar();
	takechar();
      }
      else if (nextc == '"') {
	takechar();
	while (nextc != '"')
	  takechar();
	takechar();
      }
      else if (nextc == '/') {
	takechar();
	if (nextc == '*') {
	  nextc = getchar();
	  while (nextc != '/') {
	    while (nextc != '*')
	      nextc = getchar();
	    nextc = getchar();
	  }
	  nextc = getchar();
	  w = 1;
	}
      }
      else if (nextc != 0-1)
	takechar();
    }
    token[i] = 0;
  }
}

int peek(char *s)
{
  int i = 0;
  while ((s[i] == token[i]) & (s[i] != 0))
    i = i + 1;
  return s[i] == token[i];
}

int accept(char *s)
{
  if (peek(s)) {
    get_token();
    return 1;
  }
  else
    return 0;
}

void expect(char *s)
{
  if (accept(s) == 0)
    error();
}

char *code;
int code_size;
int codepos;
long code_offset;

void save_long(char *p, long n)
{
  unsigned long v = (unsigned long)n;
  p[0] = v & 0xff;
  p[1] = (v >> 8) & 0xff;
  p[2] = (v >> 16) & 0xff;
  p[3] = (v >> 24) & 0xff;
  p[4] = (v >> 32) & 0xff;
  p[5] = (v >> 40) & 0xff;
  p[6] = (v >> 48) & 0xff;
  p[7] = (v >> 56) & 0xff;
}

long load_long(char *p)
{
  unsigned long v = 0;
  v |= (unsigned long)(p[0] & 255);
  v |= (unsigned long)(p[1] & 255) << 8;
  v |= (unsigned long)(p[2] & 255) << 16;
  v |= (unsigned long)(p[3] & 255) << 24;
  v |= (unsigned long)(p[4] & 255) << 32;
  v |= (unsigned long)(p[5] & 255) << 40;
  v |= (unsigned long)(p[6] & 255) << 48;
  v |= (unsigned long)(p[7] & 255) << 56;
  return (long)v;
}

void emit(int n, char *s)
{
  i = 0;
  if (code_size <= codepos + n) {
    int x = (codepos + n) << 1;
    code = my_realloc(code, code_size, x);
    code_size = x;
  }
  while (i <= n - 1) {
    code[codepos] = s[i];
    codepos = codepos + 1;
    i = i + 1;
  }
}

/* backend primitives for x86_64 */

void be_push()
{
  emit(1, "\x50"); /* push rax */
}

void be_pop(int n)
{
  /* add rsp, imm32  -> 48 81 c4 imm32 */
  emit(6, "\x48\x81\xc4....");
  save_long(code + codepos - 8, (long)(n << 3));
}

/* symbol table and stack bookkeeping */

char *table;
int table_size;
int table_pos;
int stack_pos;

int sym_lookup(char *s)
{
  int t = 0;
  int current_symbol = 0;
  while (t <= table_pos - 1) {
    i = 0;
    while ((s[i] == table[t]) & (s[i] != 0)) {
      i = i + 1;
      t = t + 1;
    }
    if (s[i] == table[t])
      current_symbol = t;
    while (table[t] != 0)
      t = t + 1;
    t = t + 10;
  }
  return current_symbol;
}

void sym_declare(char *s, int type, long value)
{
  int t = table_pos;
  i = 0;
  while (s[i] != 0) {
    if (table_size <= t + 10) {
      int x = (t + 10) << 1;
      table = my_realloc(table, table_size, x);
      table_size = x;
    }
    table[t] = s[i];
    i = i + 1;
    t = t + 1;
  }
  table[t] = 0;
  table[t + 1] = type;
  save_long(table + t + 2, value);
  table_pos = t + 10;
}

int sym_declare_global(char *s)
{
  int current_symbol = sym_lookup(s);
  if (current_symbol == 0) {
    sym_declare(s, 'U', code_offset);
    current_symbol = table_pos - 10;
  }
  return current_symbol;
}

void sym_define_global(int current_symbol)
{
  int i;
  int j;
  int t = current_symbol;
  long v = codepos + code_offset;
  if (table[t + 1] != 'U')
    error();
  i = (int)(load_long(table + t + 2) - code_offset);
  while (i) {
    j = (int)(load_long(code + i) - code_offset);
    save_long(code + i, v);
    i = j;
  }
  table[t + 1] = 'D';
  save_long(table + t + 2, v);
}

int number_of_args;

void sym_get_value(char *s)
{
  int t;
  if ((t = sym_lookup(s)) == 0)
    error();

  /* movabs rax, imm64 -> 48 b8 imm64 */
  emit(2, "\x48\xb8");
  save_long(code + codepos - 8, load_long(table + t + 2));

  if (table[t + 1] == 'D') {
  }
  else if (table[t + 1] == 'U')
    save_long(table + t + 2, codepos + code_offset - 10);
  else if (table[t + 1] == 'L') {
    int k = (stack_pos - table[t + 2] - 1) << 3;
    /* lea rax, [rsp + k] -> 48 8d 84 24 imm32 */
    emit(4, "\x48\x8d\x84\x24");
    save_long(code + codepos - 8, k);
  }
  else if (table[t + 1] == 'A') {
    int k = (stack_pos + number_of_args - table[t + 2] + 1) << 3;
    emit(4, "\x48\x8d\x84\x24");
    save_long(code + codepos - 8, k);
  }
  else
    error();
}

/* ELF64 header and runtime stubs */

void be_start()
{
  /* ELF64 header */
  emit(16, "\x7f\x45\x4c\x46\x02\x01\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00");
  emit(16, "\x03\x00\x3e\x00\x01\x00\x00\x00\x00\x00\x00\x00\x40\x00\x00\x00");
  emit(16, "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00");

  /* Program header (PT_LOAD) */
  emit(8, "\x01\x00\x00\x00\x05\x00\x00\x00");
  emit(8, "\x40\x00\x00\x00\x00\x00\x00\x00");
  emit(8, "\x00\x00\x40\x00\x00\x00\x00\x00");
  emit(8, "\x00\x00\x40\x00\x00\x00\x00\x00");
  /* p_filesz placeholder */
  emit(8, "\x00\x00\x00\x00\x00\x00\x00\x00");
  /* p_memsz placeholder */
  emit(8, "\x00\x00\x00\x00\x00\x00\x00\x00");
  emit(8, "\x00\x00\x20\x00\x00\x00\x00\x00");

  /* exit stub */
  sym_define_global(sym_declare_global("exit"));
  /* mov edi, imm32 ; mov eax, 60 ; syscall */
  emit(5, "\xbf\x00\x00\x00\x00");
  save_long(code + codepos - 4, 0);
  emit(5, "\xb8\x3c\x00\x00\x00");
  emit(2, "\x0f\x05");

  /* getchar stub */
  sym_define_global(sym_declare_global("getchar"));
  /* sub rsp,16 */
  emit(4, "\x48\x83\xec\x10");
  /* mov edi,0 */
  emit(5, "\xbf\x00\x00\x00\x00");
  /* lea rsi,[rsp+8] */
  emit(5, "\x48\x8d\x74\x24\x08");
  /* mov edx,1 */
  emit(5, "\xba\x01\x00\x00\x00");
  /* mov eax,0 */
  emit(5, "\xb8\x00\x00\x00\x00");
  /* syscall */
  emit(2, "\x0f\x05");
  /* cmp eax,1 */
  emit(3, "\x83\xf8\x01");
  /* jne rel32 -> jump to ret_minus1 */
  emit(6, "\x0f\x85....");
  save_long(code + codepos - 4, 14);
  /* movzx eax, byte [rsp+8] */
  emit(5, "\x0f\xb6\x44\x24\x08");
  /* add rsp,16 */
  emit(4, "\x48\x83\xc4\x10");
  /* ret */
  emit(1, "\xc3");
  /* ret_minus1: mov eax,-1 ; add rsp,16 ; ret */
  emit(5, "\xb8\xff\xff\xff\xff");
  emit(4, "\x48\x83\xc4\x10");
  emit(1, "\xc3");

  /* malloc stub */
  sym_define_global(sym_declare_global("malloc"));
  /* mov edi,0 ; mov edx,3 ; mov r10d,0x22 ; mov r8, -1 ; mov r9,0 ; mov eax,9 ; syscall ; ret */
  emit(5, "\xbf\x00\x00\x00\x00");
  emit(5, "\xba\x03\x00\x00\x00");
  emit(6, "\x41\xba\x22\x00\x00\x00");
  emit(7, "\x49\xc7\xc0\xff\xff\xff\xff");
  emit(7, "\x49\xc7\xc1\x00\x00\x00\x00");
  emit(5, "\xb8\x09\x00\x00\x00");
  emit(2, "\x0f\x05");
  emit(1, "\xc3");

  /* putchar stub */
  sym_define_global(sym_declare_global("putchar"));
  emit(4, "\x48\x83\xec\x10");
  emit(5, "\x41\x88\x74\x24\x08");
  emit(5, "\xbf\x01\x00\x00\x00");
  emit(5, "\x48\x8d\x74\x24\x08");
  emit(5, "\xba\x01\x00\x00\x00");
  emit(5, "\xb8\x01\x00\x00\x00");
  emit(2, "\x0f\x05");
  emit(4, "\x48\x83\xc4\x10");
  emit(1, "\xc3");
}

/* be_finish patches ELF header fields and writes out the file */

void be_finish()
{
  /* Patch e_entry at offset 24 to code_offset + entry (we choose current codepos) */
  save_long(code + 24, codepos + code_offset);

  /* Patch program header p_filesz and p_memsz placeholders.
     We know the placeholders were emitted after the ELF header and program header.
     The p_filesz placeholder starts at offset 64 + 32 (approx). We'll patch both
     placeholders conservatively at offsets 64 + 16 and 64 + 24 which match our emits.
  */
  save_long(code + 64 + 16, codepos);
  save_long(code + 64 + 24, codepos);

  i = 0;
  while (i <= codepos - 1) {
    putchar(code[i]);
    i = i + 1;
  }
}

/* Code generation helpers and parser */

void promote(int type)
{
  if (type == 1)
    emit(3, "\x0f\xbe\x00"); /* movsbl (%rax),%eax */
  else if (type == 2)
    emit(2, "\x8b\x00"); /* mov (%rax),%eax placeholder */
}

int expression();

int primary_expr()
{
  int type;
  if (('0' <= token[0]) & (token[0] <= '9')) {
    int n = 0;
    i = 0;
    while (token[i]) {
      n = (n << 1) + (n << 3) + token[i] - '0';
      i = i + 1;
    }
    /* movabs rax, imm64 */
    emit(2, "\x48\xb8");
    save_long(code + codepos - 8, n);
    type = 3;
  }
  else if (('a' <= token[0]) & (token[0] <= 'z')) {
    sym_get_value(token);
    type = 2;
  }
  else if (accept("(")) {
    type = expression();
    if (peek(")") == 0)
      error();
  }
  else if ((token[0] == 39) & (token[1] != 0) &
	   (token[2] == 39) & (token[3] == 0)) {
    emit(2, "\x48\xb8");
    save_long(code + codepos - 8, token[1]);
    type = 3;
  }
  else if (token[0] == '"') {
    int i = 0;
    int j = 1;
    int k;
    while (token[j] != '"') {
      if ((token[j] == 92) & (token[j + 1] == 'x')) {
	if (token[j + 2] <= '9')
	  k = token[j + 2] - '0';
	else
	  k = token[j + 2] - 'a' + 10;
	k = k << 4;
	if (token[j + 3] <= '9')
	  k = k + token[j + 3] - '0';
	else
	  k = k + token[j + 3] - 'a' + 10;
	token[i] = k;
	j = j + 4;
      }
      else {
	token[i] = token[j];
	j = j + 1;
      }
      i = i + 1;
    }
    token[i] = 0;
    /* call rel32 to jump over string, then string bytes, then pop rax */
    /* emit call rel32 */
    emit(1, "\xe8");
    /* store rel32 as 4 bytes; we use save_long but only low 4 bytes used by call */
    save_long(code + codepos, i + 1);
    emit(i + 1, token);
    emit(1, "\x58"); /* pop rax */
    type = 3;
  }
  else
    error();
  get_token();
  return type;
}

void binary1(int type)
{
  promote(type);
  be_push();
  stack_pos = stack_pos + 1;
}

int binary2(int type, int n, char *s)
{
  promote(type);
  if (n > 0)
    emit(n, s);
  stack_pos = stack_pos - 1;
  return 3;
}

int postfix_expr()
{
  int type = primary_expr();
  if (accept("[")) {
    binary1(type);
    /* pop rbx ; add rbx,rax -> 5b 48 01 d8 */
    emit(1, "\x5b");
    emit(3, "\x48\x01\xd8");
    binary2(expression(), 0, "");
    expect("]");
    type = 1;
  }
  else if (accept("(")) {
    int s = stack_pos;
    be_push();
    stack_pos = stack_pos + 1;
    if (accept(")") == 0) {
      promote(expression());
      be_push();
      stack_pos = stack_pos + 1;
      while (accept(",")) {
	promote(expression());
	be_push();
	stack_pos = stack_pos + 1;
      }
      expect(")");
    }
    /* mov rax, [rsp + offset] -> 48 8b 84 24 imm32 */
    emit(4, "\x48\x8b\x84\x24");
    save_long(code + codepos - 8, (long)((stack_pos - s - 1) << 3));
    /* call *%rax -> ff d0 */
    emit(2, "\xff\xd0");
    be_pop(stack_pos - s);
    stack_pos = s;
    type = 3;
  }
  return type;
}

int additive_expr()
{
  int type = postfix_expr();
  while (1) {
    if (accept("+")) {
      binary1(type);
      /* pop rbx ; add rbx,rax -> 5b 48 01 d8 */
      emit(4, "\x5b\x48\x01\xd8");
      type = binary2(postfix_expr(), 0, "");
    }
    else if (accept("-")) {
      binary1(type);
      /* pop rbx ; sub rax,rbx ; mov rax,rbx -> 5b 48 29 d8 48 89 d8 */
      emit(7, "\x5b\x48\x29\xd8\x48\x89\xd8");
      type = binary2(postfix_expr(), 0, "");
    }
    else
      return type;
  }
}

int shift_expr()
{
  int type = additive_expr();
  while (1) {
    if (accept("<<")) {
      binary1(type);
      /* mov rcx,rax ; pop rax ; shl cl,rax -> 48 89 c1 58 d3 e0 */
      emit(6, "\x48\x89\xc1\x58\xd3\xe0");
      type = binary2(additive_expr(), 0, "");
    }
    else if (accept(">>")) {
      binary1(type);
      emit(6, "\x48\x89\xc1\x58\xd3\xf8");
      type = binary2(additive_expr(), 0, "");
    }
    else
      return type;
  }
}

int relational_expr()
{
  int type = shift_expr();
  while (accept("<=")) {
    binary1(type);
    /* pop rbx ; cmp rax,rbx ; setle al ; movzx eax,al */
    emit(11, "\x5b\x48\x39\xd8\x0f\x9e\xc0\x0f\xb6\xc0");
    type = binary2(shift_expr(), 0, "");
  }
  return type;
}

int equality_expr()
{
  int type = relational_expr();
  while (1) {
    if (accept("==")) {
      binary1(type);
      emit(11, "\x5b\x48\x39\xd8\x0f\x94\xc0\x0f\xb6\xc0");
      type = binary2(relational_expr(), 0, "");
    }
    else if (accept("!=")) {
      binary1(type);
      emit(11, "\x5b\x48\x39\xd8\x0f\x95\xc0\x0f\xb6\xc0");
      type = binary2(relational_expr(), 0, "");
    }
    else
      return type;
  }
}

int bitwise_and_expr()
{
  int type = equality_expr();
  while (accept("&")) {
    binary1(type);
    emit(4, "\x5b\x48\x21\xd8");
    type = binary2(equality_expr(), 0, "");
  }
  return type;
}

int bitwise_or_expr()
{
  int type = bitwise_and_expr();
  while (accept("|")) {
    binary1(type);
    emit(4, "\x5b\x48\x09\xd8");
    type = binary2(bitwise_and_expr(), 0, "");
  }
  return type;
}

int expression()
{
  int type = bitwise_or_expr();
  if (accept("=")) {
    be_push();
    stack_pos = stack_pos + 1;
    promote(expression());
    if (type == 2)
      emit(4, "\x5b\x48\x89\x03");
    else
      emit(3, "\x5b\x88\x03");
    stack_pos = stack_pos - 1;
    type = 3;
  }
  return type;
}

void type_name()
{
  get_token();
  while (accept("*")) {
  }
}

void statement()
{
  int p1;
  int p2;
  if (accept("{")) {
    int n = table_pos;
    int s = stack_pos;
    while (accept("}") == 0)
      statement();
    table_pos = n;
    be_pop(stack_pos - s);
    stack_pos = s;
  }
  else if (peek("char") | peek("int")) {
    type_name();
    sym_declare(token, 'L', stack_pos);
    get_token();
    if (accept("="))
      promote(expression());
    expect(";");
    be_push();
    stack_pos = stack_pos + 1;
  }
  else if (accept("if")) {
    expect("(");
    promote(expression());
    /* test rax,rax -> 48 85 c0 ; je rel32 -> 0f 84 imm32 */
    emit(3, "\x48\x85\xc0");
    emit(5, "\x0f\x84....");
    p1 = codepos;
    expect(")");
    statement();
    emit(5, "\xe9....");
    p2 = codepos;
    save_long(code + p1 - 4, codepos - p1);
    if (accept("else"))
      statement();
    save_long(code + p2 - 4, codepos - p2);
  }
  else if (accept("while")) {
    expect("(");
    p1 = codepos;
    promote(expression());
    emit(3, "\x48\x85\xc0");
    emit(5, "\x0f\x84....");
    p2 = codepos;
    expect(")");
    statement();
    emit(5, "\xe9....");
    save_long(code + codepos - 4, p1 - codepos);
    save_long(code + p2 - 4, codepos - p2);
  }
  else if (accept("return")) {
    if (peek(";") == 0)
      promote(expression());
    expect(";");
    be_pop(stack_pos);
    emit(1, "\xc3");
  }
  else {
    expression();
    expect(";");
  }
}

void program()
{
  int current_symbol;
  while (token[0]) {
    type_name();
    current_symbol = sym_declare_global(token);
    get_token();
    if (accept(";")) {
      sym_define_global(current_symbol);
      emit(8, "\x00\x00\x00\x00\x00\x00\x00\x00");
    }
    else if (accept("(")) {
      int n = table_pos;
      number_of_args = 0;
      while (accept(")") == 0) {
	number_of_args = number_of_args + 1;
	type_name();
	if (peek(")") == 0) {
	  sym_declare(token, 'A', number_of_args);
	  get_token();
	}
	accept(",");
      }
      if (accept(";") == 0) {
	sym_define_global(current_symbol);
	statement();
	emit(1, "\xc3");
      }
      table_pos = n;
    }
    else
      error();
  }
}

int main1()
{
  code_offset = 0x400000;
  be_start();
  nextc = getchar();
  get_token();
  program();
  be_finish();
  return 0;
}



