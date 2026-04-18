/*
 ============================================================================
 Name        : cc3.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#define bool _Bool
#define true 1
#define false 0

typedef struct String String;
struct String
{
	char *ptr;
	int len;
};

String *new_string(char *ptr, int len);
bool str_equals(String *s1, String *s2);
bool str_chr_equals(String *s1, char *s2);


typedef struct Token Token;

typedef enum
{
	TK_RESERVED,
	TK_NEWLINE,
	TK_IDENT,
	TK_NUM,
	TK_RETURN,
	TK_IF,
	TK_ELSE,
	TK_WHILE,
	TK_FOR,
	TK_INT,
	TK_CHAR,
	TK_ENUM,
	TK_STRUCT,
	TK_UNION,
	TK_EOF,
	TK_SIZEOF,
	TK_STRING,
	TK_CHAR_CONST,
	TK_PREPROCESSOR,
	TK_TYPEDEF,
	TK_SWITCH,
	TK_CASE,
	TK_DEFAULT,
	TK_BREAK,
	TK_CONTINUE,
	TK_VOID,
	TK_BOOL,
	TK_BUILTIN_VA_LIST,
	TK_EXTERN,
} TokenKind;

struct Token
{
	TokenKind kind;
	Token *next;
	int val;
	String *str;
};

extern Token *token;
bool startswith(char *p, char *q);
Token *tokenize(char *p, bool eof);

void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);
void error_at_token(Token *tok, char *fmt, ...);
void error_at_here(char *fmt, ...);
void assert(bool flag);

char *read_file(char *path);
char *get_dir(char *path);


typedef enum
{
	INT,
	PTR,
	ARRAY,
	CHAR,
	STRUCT,
	VOID,
	FUNC,
	BOOL,
	VA_LIST_TAG
} TypeKind;

typedef struct Type Type;
typedef struct StructField StructField;
typedef struct StructType StructType;

struct StructField
{
	StructField *next;
	String *name;
	Type *type;
	int offset;
};

struct StructType
{
	StructType *next;
	String *name;
	StructField *fields;
	int alignment;
	int size;
	bool is_union;
};

struct Type
{
	TypeKind ty;
	Type *ptr_to;
	int array_size;
	StructType *struct_type;
};

Type *new_type(TypeKind ty, Type *ptr_to);
Type *new_struct_type(String *name, bool is_union);
void add_field(StructType *type, Type *ty, String *name);
int sizeof_type(Type *type);


typedef enum
{
  ND_ADD,          // +
  ND_SUB,          // -
  ND_MUL,          // *
  ND_DIV,          // /
  ND_ASSIGN,       // =
  ND_ASSIGN_ARRAY, // = { ... }
  ND_EQ,           // ==
  ND_NE,           // !=
  ND_LT,           // <
  ND_LE,           // <=
  ND_AND,          // &&
  ND_OR,           // ||
  ND_ADDR,         // *
  ND_DEREF,        // &
  ND_POST_INCR,    // a++
  ND_POST_DECR,    // a--
  ND_RETURN,       // return
  ND_IF,           // if
  ND_IFELSE,       // if ... else ...
  ND_SWITCH,       // switch
  ND_CASE,         // case
  ND_DEFAULT,      // default
  ND_BREAK,        // break
  ND_CONTINUE,     // continue
  ND_WHILE,        // while
  ND_FOR,          // for
  ND_BLOCK,        // { ... }
  ND_LVAR,         // Local Variable
  ND_GVAR,         // Global Variable
  ND_CALL,         // Function Call
  ND_NUM,          // Integer
  ND_STRING,       // String literals
  ND_VA_START,     // va_start()
  ND_UNNAMED
} NodeKind;

typedef enum
{
  DIR_INCLUDE
} Directive;

typedef struct Node Node;

struct Node
{
  NodeKind kind;
  Node *lhs;
  Node *rhs;
  int val;      // ND_NUM
  int offset;   // ND_LVAR
  Type *type;   // expr
  String *name; // ND_CALL
};

typedef struct StringLiteral StringLiteral;
struct StringLiteral
{
  StringLiteral *next;
  String *str;
  int offset;
};

extern char *user_input;
extern char *dir_name;
extern char *filename;
extern int current_stack_size;
extern int arg_count;

typedef struct Macro Macro;
struct Macro
{
	String *ident;
	Token *replace;
};

Token *preprocess(Token *tok);

typedef enum
{
	EXT_FUNC,
	EXT_FUNCDECL,
	EXT_GVAR,
	EXT_ENUM,
	EXT_STRUCT,
	EXT_TYPEDEF,
} ExternalKind;

typedef struct External External;
struct External
{
	ExternalKind kind;
	String *name;
	Node *code;
	int offsets[6];
	int size;
	StringLiteral *literals;
	int stack_size;
	bool is_variadic;
	bool is_extern;
};

bool at_eof();
External *external();

void gen_function(External *ext);
void gen_stmt(Node *node);

String *new_string(char *ptr, int len)
{
	String *substr = calloc(1, sizeof(String));
	substr->ptr = ptr;
	substr->len = len;
	return substr;
}

bool str_equals(String *s1, String *s2)
{
	return s1->len == s2->len && !memcmp(s1->ptr, s2->ptr, s1->len);
}

bool str_chr_equals(String *s1, char *s2)
{
	return s1->len == strlen(s2) && !memcmp(s1->ptr, s2, s1->len);
}


void error(char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);

	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
	exit(1);
}

void _error_at(char *loc, char *fmt, va_list ap)
{
	char *line = loc;
	while (user_input < line && line[-1] != '\n')
		line--;

	char *end = loc;
	while (*end != '\n')
		end++;

	int line_num = 1;
	for (char *p = user_input; p < line; p++)
		if (*p == '\n')
			line_num++;

	int indent = fprintf(stderr, "%s:%d: ", filename, line_num);
	fprintf(stderr, "%.*s\n", end - line, line);

	int pos = loc - line + indent;
	fprintf(stderr, "%*s", pos, "");
	fprintf(stderr, "^ ");
	vfprintf(stderr, fmt, &(ap[0]));
	fprintf(stderr, "\n");
	exit(1);
}

void error_at(char *loc, char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);

	_error_at(loc, fmt, ap);
}

void error_at_token(Token *tok, char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);

	_error_at(tok->str->ptr, fmt, ap);
}

void error_at_here(char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);

	_error_at(token->str->ptr, fmt, ap);
}

void assert(bool flag)
{
	if (!flag)
		error_at_token(token, "assertion failed");
}

Type *new_type(TypeKind ty, Type *ptr_to)
{
	Type *type = calloc(1, sizeof(Type));
	type->ty = ty;
	type->ptr_to = ptr_to;

	return type;
}

Type *new_struct_type(String *name, bool is_union)
{
	Type *ty = new_type(STRUCT, NULL);
	ty->struct_type = calloc(1, sizeof(StructType));
	ty->struct_type->name = name;
	ty->struct_type->is_union = is_union;
	return ty;
}

int align(int n, int al)
{
	return (n + al - 1) / al * al;
}

void add_field(StructType *type, Type *ty, String *name)
{
	StructField *field = calloc(1, sizeof(StructField));
	field->type = ty;
	field->name = name;

	int al;
	if (ty->ty == ARRAY)
		al = sizeof_type(ty->ptr_to);
	else
		al = sizeof_type(ty);

	if (al > type->alignment)
		type->alignment = al;

	if (type->is_union)
	{
		if (sizeof_type(ty) > type->size)
			type->size = sizeof_type(ty);
	}
	else
	{
		if (type->fields)
		{
			field->offset = align(type->fields->offset + sizeof_type(type->fields->type), al);
			type->size = align(field->offset + sizeof_type(field->type), type->alignment);
		}
		else
		{
			field->offset = 0;
			type->size = align(sizeof_type(field->type), type->alignment);
		}
	}

	field->next = type->fields;
	type->fields = field;
}

int sizeof_type(Type *type)
{
	switch (type->ty)
	{
	case INT:
		return 4;
	case PTR:
		return 8;
	case ARRAY:
		return sizeof_type(type->ptr_to) * type->array_size;
	case CHAR:
		return 1;
	case STRUCT:
		return type->struct_type->size;
	case BOOL:
		return 1;
	case VA_LIST_TAG:
		return 24;
	}

	error_at_here("sizeof_type: unknown type");
}

int label_count;
int switch_count;
int max_switch_count;
int break_count;
int max_break_count;
int continue_count;
int max_continue_count;

void gen(Node *node);

void gen_string_literal(StringLiteral *literals)
{
  for (StringLiteral *l = literals; l; l = l->next)
  {
    printf(".data\n");
    printf(".LC%d:\n", l->offset);
    printf("  .string \"%.*s\"\n", l->str->len, l->str->ptr);
  }
}

void gen_lval(Node *node)
{
  if (node->kind == ND_DEREF)
    gen(node->lhs);
  else if (node->kind == ND_LVAR)
  {
    printf("  mov rax, rbp\n");
    printf("  sub rax, %d\n", node->offset);
    printf("  push rax\n");
  }
  else if (node->kind == ND_GVAR)
  {
    printf("  lea rax, %.*s[rip]\n", node->name->len, node->name->ptr);
    printf("  push rax\n");
  }
  else if (node->kind == ND_STRING)
  {
    printf("  lea rax, .LC%d[rip]\n", node->offset);
    printf("  push rax\n");
  }
  else
    error("left value of assignment must be variable: found %d\n", node->kind);
}

void gen_stmt(Node *node)
{
  Node *n;
  int l;
  switch (node->kind)
  {
  case ND_BLOCK:
    for (n = node->rhs; n; n = n->rhs)
      gen_stmt(n->lhs);
    return;
  case ND_IF:
    l = label_count++;
    gen(node->lhs);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  je .Lend%d\n", l);
    gen_stmt(node->rhs);
    printf(".Lend%d:\n", l);
    return;
  case ND_IFELSE:
    l = label_count++;
    gen(node->lhs);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  je .Lelse%d\n", l);
    gen_stmt(node->rhs->lhs);
    printf("  jmp .Lend%d\n", l);
    printf(".Lelse%d:\n", l);
    gen_stmt(node->rhs->rhs);
    printf(".Lend%d:\n", l);
    return;
  case ND_SWITCH:
    assert(node->rhs->kind == ND_BLOCK);
    gen(node->lhs);
    printf("  pop rax\n");

    int i = break_count;
    int b = break_count = ++max_break_count;

    int j = switch_count;
    switch_count = ++max_switch_count;

    for (n = node->rhs; n; n = n->rhs)
    {
      if (n->lhs && n->lhs->kind == ND_CASE)
      {
        printf("  cmp rax, %d\n", n->lhs->lhs->val);
        printf("  je .Lcase%d_%d\n", switch_count, n->lhs->lhs->val);
      }
      if (n->lhs && n->lhs->kind == ND_DEFAULT)
        printf("  jmp .Ldefault%d\n", switch_count);
    }

    gen_stmt(node->rhs);
    printf(".Lbreak%d:\n", b);
    break_count = i;
    switch_count = j;
    return;
  case ND_CASE:
    assert(node->lhs->kind == ND_NUM);
    printf(".Lcase%d_%d:\n", switch_count, node->lhs->val);
    return;
  case ND_DEFAULT:
    printf(".Ldefault%d:\n", switch_count);
    return;
  case ND_WHILE:
    l = label_count++;

    i = continue_count;
    continue_count = ++max_continue_count;

    j = break_count;
    b = break_count = ++max_break_count;

    printf(".Lbegin%d:\n", l);
    printf(".Lcontinue%d:\n", continue_count);
    gen(node->lhs);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  je .Lend%d\n", l);
    gen_stmt(node->rhs);
    printf("  jmp .Lbegin%d\n", l);
    printf(".Lend%d:\n", l);
    printf(".Lbreak%d:\n", b);
    continue_count = i;
    break_count = j;
    return;
  case ND_FOR:
    l = label_count++;

    i = continue_count;
    j = continue_count = ++max_continue_count;

    int k = break_count;
    b = break_count = ++max_break_count;

    if (node->lhs->lhs)
      gen_stmt(node->lhs->lhs);

    printf(".Lbegin%d:\n", l);
    if (node->lhs->rhs)
    {
      gen(node->lhs->rhs);
      printf("  pop rax\n");
      printf("  cmp rax, 0\n");
      printf("  je .Lend%d\n", l);
    }
    gen_stmt(node->rhs->rhs);
    printf(".Lcontinue%d:\n", j);
    if (node->rhs->lhs)
      gen_stmt(node->rhs->lhs);

    printf("  jmp .Lbegin%d\n", l);
    printf(".Lend%d:\n", l);
    printf(".Lbreak%d:\n", b);
    continue_count = i;
    break_count = k;
    return;
  case ND_RETURN:
    if (node->lhs)
      gen(node->lhs);
    else
      printf("  push 0\n");
    printf("  pop rax\n");
    printf("  mov rsp, rbp\n");
    printf("  pop rbp\n");
    printf("  ret\n");
    return;
  case ND_BREAK:
    printf("  jmp .Lbreak%d\n", break_count);
    return;
  case ND_CONTINUE:
    printf("  jmp .Lcontinue%d\n", continue_count);
    return;
  default:
    gen(node);
    printf("  pop rax\n");
  }
}

void gen_function(External *ext)
{
  char *regs1[6] = {"dil", "sil", "dl", "cl", "r8b", "r9b"};
  char *regs4[6] = {"edi", "esi", "edx", "ecx", "r8d", "r9d"};
  char *regs8[6] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};

  gen_string_literal(ext->literals);

  printf(".globl %.*s\n", ext->name->len, ext->name->ptr);
  printf(".text\n");
  printf("%.*s:\n", ext->name->len, ext->name->ptr);

  printf("  push rbp\n");
  printf("  mov rbp, rsp\n");
  printf("  sub rsp, %d\n", ext->stack_size);

  if (ext->is_variadic)
    for (int i = 5; i >= 0; i--)
      printf("  push %s\n", regs8[i]);

  int i = 0;
  int ri = 0;
  for (; i < 6 && ext->offsets[i]; i++)
  {
    int size = 0;
    int offset = 0;
    if (i == 0)
      size = ext->offsets[i];
    else
      size = ext->offsets[i] - ext->offsets[i - 1];

    for (; size != offset; ri++)
    {
      if (size - offset >= 8)
      {
        printf("  mov [rbp - %d], %s\n", ext->offsets[i] - offset, regs8[ri]);
        offset += 8;
      }
      else if (size - offset == 4)
      {
        printf("  mov [rbp - %d], %s\n", ext->offsets[i] - offset, regs4[ri]);
        offset += 4;
      }
      else if (size - offset == 1)
      {
        printf("  mov [rbp - %d], %s\n", ext->offsets[i] - offset, regs1[ri]);
        offset += 1;
      }
      else
        error("invalid size");
    }
  }

  arg_count = i;
  current_stack_size = ext->stack_size;

  gen_stmt(ext->code);

  printf("  mov rsp, rbp\n");
  printf("  pop rbp\n");
  printf("  ret\n");
}

void gen(Node *node)
{
  int l;
  int i;
  Node *n;

  switch (node->kind)
  {
  case ND_NUM:
    printf("  push %d\n", node->val);
    return;
  case ND_LVAR:
  case ND_GVAR:
    gen_lval(node);
    printf("  pop rax\n");
    int size = sizeof_type(node->type);
    for (;;)
    {
      if (size >= 8)
      {
        printf("  mov r10, [rax + %d]\n", sizeof_type(node->type) - size);
        printf("  push r10\n");
        size -= 8;
      }
      else if (size == 4)
      {
        printf("  movsxd rax, [rax]\n");
        printf("  push rax\n");
        size -= 4;
      }
      else if (size == 1)
      {
        printf("  movsx rax, BYTE PTR [rax]\n");
        printf("  push rax\n");
        size -= 1;
      }
      else
        error("not implemented: size %d", size);

      if (size == 0)
        break;
    }

    return;
  case ND_STRING:
    gen_lval(node);
    return;
  case ND_ASSIGN:
    gen_lval(node->lhs);
    gen(node->rhs);

    printf("  pop rdi\n");
    printf("  pop rax\n");

    switch (sizeof_type(node->lhs->type))
    {
    case 4:
      printf("  mov DWORD PTR [rax], edi\n");
      break;
    case 1:
      printf("  mov BYTE PTR [rax], dil\n");
      break;
    case 8:
      printf("  mov [rax], rdi\n");
      break;
    default:
      error("failed to assign");
    }

    printf("  push rdi\n");
    return;
  case ND_ASSIGN_ARRAY:
    for (n = node->rhs; n; n = n->rhs)
    {
      gen(n->lhs);
      printf("  pop rax\n");
    }
    printf("  push 0\n");
    return;
  case ND_POST_INCR:
    gen_lval(node->lhs);
    printf(" pop rax\n");

    switch (sizeof_type(node->lhs->type))
    {
    case 8:
      printf("  mov rdi, [rax]\n");
      printf("  push rdi\n");
      printf("  add rdi, 1\n");
      printf("  mov [rax], rdi\n");
      break;
    case 4:
      printf("  mov edi, [rax]\n");
      printf("  push rdi\n");
      printf("  add edi, 1\n");
      printf("  mov [rax], edi\n");
      break;
    case 1:
      printf("  mov dil, [rax]\n");
      printf("  push rdi\n");
      printf("  add dil, 1\n");
      printf("  mov [rax], dil\n");
      break;
    }
    return;
  case ND_POST_DECR:
    gen_lval(node->lhs);
    printf(" pop rax\n");

    switch (sizeof_type(node->lhs->type))
    {
    case 8:
      printf("  mov rdi, [rax]\n");
      printf("  push rdi\n");
      printf("  sub rdi, 1\n");
      printf("  mov [rax], rdi\n");
      break;
    case 4:
      printf("  mov edi, [rax]\n");
      printf("  push rdi\n");
      printf("  sub edi, 1\n");
      printf("  mov [rax], edi\n");
      break;
    case 1:
      printf("  mov dil, [rax]\n");
      printf("  push rdi\n");
      printf("  sub dil, 1\n");
      printf("  mov [rax], dil\n");
      break;
    }
    return;

  case ND_CALL:
    n = node->rhs;
    i = 0;

    while (n)
    {
      gen(n->lhs);
      if (sizeof_type(n->lhs->type) > 32)
        error("not implemented: too big object");
      i += (sizeof_type(n->lhs->type) + 7) / 8;
      n = n->rhs;
    }
    if (i > 6)
      error_at(node->lhs->name->ptr, "too many arguments");

    char *regs[6] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};

    for (int j = i - 1; j >= 0; j--)
      printf("  pop %s\n", regs[j]);

    printf("  mov r10, rsp\n");
    printf("  and rsp, 0xfffffffffffffff0\n");
    printf("  push r10\n");
    printf("  push 0\n");
    printf("  mov rax, 0\n");

    printf("  call %.*s\n", node->lhs->name->len, node->lhs->name->ptr);

    printf("  pop rdi\n");
    printf("  pop rdi\n");

    printf("  mov rsp, rdi\n");

    if (node->type->ty == VOID)
    {
      printf("  push 0\n");
      return;
    }

    switch (sizeof_type(node->type))
    {
    case 1:
      printf("  movsx rax, al\n");
      printf("  push rax\n");
      break;
    case 4:
      printf("  movsx rax, eax\n");
      printf("  push rax\n");
      break;
    case 8:
      printf("  push rax\n");
      break;
    default:
      error("not implemented: return value");
    }

    return;
  case ND_VA_START:
    n = node->rhs->lhs;
    assert(n->kind == ND_LVAR);

    printf("  mov eax, %d\n", arg_count * 8);
    printf("  mov [rbp - %d], eax\n", n->offset);
    printf("  mov eax, 48\n");
    printf("  mov [rbp - %d], eax\n", n->offset - 4);
    printf("  lea rax, [rbp + 16]\n");
    printf("  mov [rbp - %d], rax\n", n->offset - 8);
    printf("  lea rax, [rbp - %d]\n", current_stack_size + 48);
    printf("  mov [rbp - %d], rax\n", n->offset - 16);

    printf("  push 0\n");
    return;
  case ND_ADDR:
    gen_lval(node->lhs);
    return;
  case ND_DEREF:
    gen(node->lhs);
    printf("  pop rax\n");
    switch (sizeof_type(node->type))
    {
    case 4:
      printf("  mov eax, [rax]\n");
      break;
    case 8:
      printf("  mov rax, [rax]\n");
      break;
    case 1:
      printf("  movsx rax, BYTE PTR [rax]\n");
      break;
    default:
      error("unexpected type");
    }

    printf("  push rax\n");
    return;
  case ND_AND:
    l = label_count++;
    gen(node->lhs);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  je .Lfalse%d\n", l);
    gen(node->rhs);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  je .Lfalse%d\n", l);
    printf("  push 1\n");
    printf("  jmp .Lend%d\n", l);
    printf(".Lfalse%d:\n", l);
    printf("  push 0\n");
    printf(".Lend%d:\n", l);
    return;
  case ND_OR:
    l = label_count++;
    gen(node->lhs);
    printf("  pop rax\n");
    printf("  cmp rax, 1\n");
    printf("  je .Ltrue%d\n", l);
    gen(node->rhs);
    printf("  pop rax\n");
    printf("  cmp rax, 1\n");
    printf("  je .Ltrue%d\n", l);
    printf("  push 0\n");
    printf("  jmp .Lend%d\n", l);
    printf(".Ltrue%d:\n", l);
    printf("  push 1\n");
    printf(".Lend%d:\n", l);
    return;
  default:
    gen(node->lhs);
    gen(node->rhs);

    printf("  pop rdi\n");
    printf("  pop rax\n");

    switch (node->kind)
    {
    case ND_ADD:
      printf("  add rax, rdi\n");
      break;
    case ND_SUB:
      printf("  sub rax, rdi\n");
      break;
    case ND_MUL:
      printf("  imul rax, rdi\n");
      break;
    case ND_DIV:
      printf("  cqo\n");
      printf("  idiv rdi\n");
      break;
    case ND_EQ:
      printf("  cmp rax, rdi\n");
      printf("  sete al\n");
      printf("  movzb rax, al\n");
      break;
    case ND_NE:
      printf("  cmp rax, rdi\n");
      printf("  setne al\n");
      printf("  movzb rax, al\n");
      break;
    case ND_LT:
      printf("  cmp rax, rdi\n");
      printf("  setl al\n");
      printf("  movzb rax, al\n");
      break;
    case ND_LE:
      printf("  cmp rax, rdi\n");
      printf("  setle al\n");
      printf("  movzb rax, al\n");
      break;
    default:
      error("unreachable");
      return;
    }

    printf("  push rax\n");
  }
}
