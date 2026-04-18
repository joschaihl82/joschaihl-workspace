/* * Amalgamated Header File
 * Sources: string.h, file.h, type.h, tokenize.h, header.h, parse.h, preprocess.h, error.h, codegen.h
 */

// ==========================================
// From string.h
// ==========================================

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

// ==========================================
// From file.h
// ==========================================

char *read_file(char *path);
char *get_dir(char *path);

// ==========================================
// From type.h
// ==========================================

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

// ==========================================
// From tokenize.h
// ==========================================

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

// ==========================================
// From header.h
// ==========================================

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

// ==========================================
// From parse.h
// ==========================================

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

// ==========================================
// From preprocess.h
// ==========================================

typedef struct Macro Macro;
struct Macro
{
	String *ident;
	Token *replace;
};

Token *preprocess(Token *tok);

// ==========================================
// From error.h
// ==========================================

void cerror(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);
void error_at_token(Token *tok, char *fmt, ...);
void error_at_here(char *fmt, ...);
void assert(bool flag);

// ==========================================
// From codegen.h
// ==========================================

void gen_function(External *ext);
void gen_stmt(Node *node);


#ifdef __STDC__
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#else
void *calloc();
#endif

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

#ifdef __STDC__
#include <stdlib.h>
#else
void *calloc();
#endif


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

#ifdef __STDC__
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#else
typedef __builtin_va_list va_list;
extern void *stderr;
#endif

void cerror(char *fmt, ...)
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
#ifdef __STDC__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#else
typedef void FILE;
typedef int size_t;
void *calloc();
FILE *fopen();
#endif

char *read_file(char *path)
{
	FILE *fp = fopen(path, "r");
	if (!fp)
		cerror("cannot open %s", path /*, strcerror(errno) */);

	if (fseek(fp, 0, /*SEEK_END*/ 2) == -1)
		cerror("%s: fseek", path /*, strcerror(errno) */);
	size_t size = ftell(fp);
	if (fseek(fp, 0, /*SEEK_SET*/ 0) == -1)
		cerror("%s: fseek", path /*, strcerror(errno)*/);

	char *buf = calloc(1, size + 2);
	fread(buf, size, 1, fp);

	if (size == 0 || buf[size - 1] != '\n')
		buf[size++] = '\n';

	buf[size] = '\0';
	fclose(fp);
	return buf;
}

char *get_dir(char *path)
{
	int i;
	for (i = strlen(path) - 1; i >= 0; i--)
	{
		if (path[i] == '/')
			break;
	}

	char *dir;
	if (i == -1)
	{
		dir = calloc(1, 2);
		strncpy(dir, ".", 1);
		return dir;
	}
	dir = calloc(1, i + 1);
	strncpy(dir, path, i);

	return dir;
}
#ifdef __STDC__
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#else
void *calloc();
int memcmp();
char *strstr();
char *strchr();
#endif


Token *token;

Token *new_token(TokenKind kind, Token *cur, char *str, int len)
{
	Token *tok = calloc(1, sizeof(Token));
	tok->kind = kind;
	tok->str = new_string(str, len);
	cur->next = tok;
	return tok;
}

bool is_alnum(char c)
{
	return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || ('0' <= c && c <= '9') || (c == '_');
}

bool startswith(char *p, char *q)
{
	return memcmp(p, q, strlen(q)) == 0;
}

Token *tokenize(char *p, bool eof)
{
	Token head;
	head.next = NULL;
	Token *cur = &head;

	while (*p)
	{
		if (*p == '\n') {
			cur = new_token(TK_NEWLINE, cur, p++, 1);
			continue;
		}
		if (isspace(*p))
		{
			p++;
			continue;
		}

		if (startswith(p, "#"))
		{
			char *q = p;
			while (!isspace(*q))
				q++;
			cur = new_token(TK_PREPROCESSOR, cur, p, q - p);
			p = q;
			continue;
		}

		if (startswith(p, "==") || startswith(p, "!=") || startswith(p, "<=") || startswith(p, ">="))
		{
			cur = new_token(TK_RESERVED, cur, p, 2);
			p += 2;
			continue;
		}
		if (startswith(p, "||") || startswith(p, "&&"))
		{
			cur = new_token(TK_RESERVED, cur, p, 2);
			p += 2;
			continue;
		}

		if (startswith(p, "++") || startswith(p, "--"))
		{
			cur = new_token(TK_RESERVED, cur, p, 2);
			p += 2;
			continue;
		}

		if (startswith(p, "+=") || startswith(p, "-=") || startswith(p, "*=") || startswith(p, "/="))
		{
			cur = new_token(TK_RESERVED, cur, p, 2);
			p += 2;
			continue;
		}

		if (startswith(p, "//"))
		{
			p += 2;
			while (*p != '\n')
				p++;
			continue;
		}

		if (strncmp(p, "/*", 2) == 0)
		{
			char *q = strstr(p + 2, "*/");
			if (!q)
				error_at(p, "tokenize failed: \"*/\" not found");
			p = q + 2;
			continue;
		}

		if (strncmp(p, "->", 2) == 0)
		{
			cur = new_token(TK_RESERVED, cur, p, 2);
			p += 2;
			continue;
		}

		if (strncmp(p, "...", 3) == 0)
		{
			cur = new_token(TK_RESERVED, cur, p, 3);
			p += 3;
			continue;
		}

		if (strchr("+-*/()<>:;={},&[].!", *p))
		{
			cur = new_token(TK_RESERVED, cur, p++, 1);
			continue;
		}

		if (*p == '"')
		{
			cur = new_token(TK_STRING, cur, ++p, 0);
			for (; *(p - 1) == '\\' || (*p != '"'); p++)
				cur->str->len++;
			p++;
			continue;
		}

		if (*p == '\'')
		{
			cur = new_token(TK_CHAR_CONST, cur, ++p, 0);
			for (; (*(p - 2) != '\\' && *(p - 1) == '\\') || *p != '\''; p++)
				cur->str->len++;
			p++;
			continue;
		}

		if (isdigit(*p))
		{
			char *newp;
			int val = strtol(p, &newp, 10);
			int len = newp - p;
			p = newp;
			cur = new_token(TK_NUM, cur, p, len);
			cur->val = val;
			continue;
		}

		if (strncmp(p, "return", 6) == 0 && !is_alnum(p[6]))
		{
			cur = new_token(TK_RETURN, cur, p, 6);
			p += 6;
			continue;
		}

		if (strncmp(p, "if", 2) == 0 && !is_alnum(p[2]))
		{
			cur = new_token(TK_IF, cur, p, 2);
			p += 2;
			continue;
		}

		if (strncmp(p, "else", 4) == 0 && !is_alnum(p[4]))
		{
			cur = new_token(TK_ELSE, cur, p, 4);
			p += 4;
			continue;
		}

		if (strncmp(p, "while", 5) == 0 && !is_alnum(p[5]))
		{
			cur = new_token(TK_WHILE, cur, p, 5);
			p += 5;
			continue;
		}

		if (strncmp(p, "for", 3) == 0 && !is_alnum(p[3]))
		{
			cur = new_token(TK_FOR, cur, p, 3);
			p += 3;
			continue;
		}

		if (strncmp(p, "int", 3) == 0 && !is_alnum(p[3]))
		{
			cur = new_token(TK_INT, cur, p, 3);
			p += 3;
			continue;
		}

		if (strncmp(p, "sizeof", 6) == 0 && !is_alnum(p[6]))
		{
			cur = new_token(TK_SIZEOF, cur, p, 6);
			p += 6;
			continue;
		}

		if (strncmp(p, "char", 4) == 0 && !is_alnum(p[4]))
		{
			cur = new_token(TK_CHAR, cur, p, 4);
			p += 4;
			continue;
		}

		if (strncmp(p, "enum", 4) == 0 && !is_alnum(p[4]))
		{
			cur = new_token(TK_ENUM, cur, p, 4);
			p += 4;
			continue;
		}

		if (strncmp(p, "struct", 6) == 0 && !is_alnum(p[6]))
		{
			cur = new_token(TK_STRUCT, cur, p, 6);
			p += 6;
			continue;
		}

		if (strncmp(p, "union", 5) == 0 && !is_alnum(p[5]))
		{
			cur = new_token(TK_UNION, cur, p, 5);
			p += 5;
			continue;
		}

		if (strncmp(p, "typedef", 7) == 0 && !is_alnum(p[7]))
		{
			cur = new_token(TK_TYPEDEF, cur, p, 7);
			p += 7;
			continue;
		}

		if (strncmp(p, "switch", 6) == 0 && !is_alnum(p[6]))
		{
			cur = new_token(TK_SWITCH, cur, p, 6);
			p += 6;
			continue;
		}

		if (strncmp(p, "case", 4) == 0 && !is_alnum(p[4]))
		{
			cur = new_token(TK_CASE, cur, p, 4);
			p += 4;
			continue;
		}
		if (strncmp(p, "default", 7) == 0 && !is_alnum(p[7]))
		{
			cur = new_token(TK_DEFAULT, cur, p, 7);
			p += 7;
			continue;
		}

		if (strncmp(p, "break", 5) == 0 && !is_alnum(p[5]))
		{
			cur = new_token(TK_BREAK, cur, p, 5);
			p += 5;
			continue;
		}

		if (strncmp(p, "continue", 8) == 0 && !is_alnum(p[8]))
		{
			cur = new_token(TK_CONTINUE, cur, p, 8);
			p += 8;
			continue;
		}

		if (strncmp(p, "void", 4) == 0 && !is_alnum(p[4]))
		{
			cur = new_token(TK_VOID, cur, p, 4);
			p += 4;
			continue;
		}

		if (strncmp(p, "__builtin_va_list", 17) == 0 && !is_alnum(p[17]))
		{
			cur = new_token(TK_BUILTIN_VA_LIST, cur, p, 17);
			p += 17;
			continue;
		}

		if (strncmp(p, "_Bool", 5) == 0 && !is_alnum(p[5]))
		{
			cur = new_token(TK_BOOL, cur, p, 5);
			p += 5;
			continue;
		}

		if (strncmp(p, "extern", 6) == 0 && !is_alnum(p[6]))
		{
			cur = new_token(TK_EXTERN, cur, p, 6);
			p += 6;
			continue;
		}

		if (('a' <= *p && *p <= 'z') || ('A' <= *p && *p <= 'Z') || (*p == '_'))
		{
			char *q;
			for (q = p; ('a' <= *q && *q <= 'z') || ('A' <= *q && *q <= 'Z') || ('0' <= *q && *q <= '9') || (*q == '_'); q++)
				q = q;

			cur = new_token(TK_IDENT, cur, p, q - p);
			p = q;
			continue;
		}

		error_at(p, "tokenize failed");
	}

	if (eof)
		new_token(TK_EOF, cur, p, 1);
	return head.next;
}
#ifdef __STDC__
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#else
void *calloc();
char *strncpy();
int snprintf();
#endif

char *once_file[100];
Macro *macros[100];
int mi;

Macro *find_macro(String *str)
{
	for (int i = 0; i < mi; i++)
		if (str_equals(str, macros[i]->ident))
			return macros[i];
	return NULL;
}

Token *remove_newline(Token *tok)
{
	Token *before = NULL;
	for (Token *t = tok; t; t = t->next)
	{
		if (t->kind == TK_NEWLINE)
		{
			if (before)
				before->next = t->next;
			else
				tok = t->next;
		}
		else
			before = t;
	}
	return tok;
}

Token *preprocess(Token *tok)
{
	Token *before = NULL;
	Token *start = NULL;
	for (Token *t = tok; t; t = t->next)
	{
		if (t->kind == TK_PREPROCESSOR)
		{
			if (startswith(t->str->ptr, "#include"))
			{
				char *curfile = filename;
				filename = calloc(1, 100);
				char *path = calloc(1, 200);

				t = t->next;
				assert(t->kind == TK_STRING);

				strncpy(filename, t->str->ptr, t->str->len);

				snprintf(path, 200, "%s/%s", dir_name, filename);

				char *header = read_file(path);
				Token *header_token = tokenize(header, 0); // TODO: use `false`
				header_token = preprocess(header_token);
				filename = curfile;
				if (header_token)
				{
					Token *header_token_end = header_token;
					while (header_token_end->next)
						header_token_end = header_token_end->next;

					if (before)
						before->next = header_token;
					else
						start = header_token;

					before = header_token_end;
				}
			}

			if (startswith(t->str->ptr, "#ifdef"))
			{
				while (!(startswith(t->str->ptr, "#else") || startswith(t->str->ptr, "#endif")))
					t = t->next;

				if (startswith(t->str->ptr, "#else"))
				{
					t = t->next;

					if (before)
						before->next = t;
					else
						start = t;

					while (!startswith(t->str->ptr, "#endif"))
					{
						before = t;
						t = t->next;
					}
				}
				else
				{
					assert(startswith(t->str->ptr, "#endif"));
				}
			}

			if (startswith(t->str->ptr, "#pragma"))
			{
				t = t->next;
				if (str_chr_equals(t->str, "once"))
				{
					int i = 0;
					while (once_file[i])
					{
						if (strcmp(once_file[i], filename) == 0)
							return NULL;
						i++;
					}
					once_file[i] = filename;
				}
			}

			if (startswith(t->str->ptr, "#define"))
			{
				t = t->next;
				assert(t->kind == TK_IDENT);

				Macro *m = calloc(1, sizeof(Macro));
				m->ident = t->str;

				t = t->next;
				m->replace = t;

				while (t->next->kind != TK_NEWLINE)
					t = t->next;

				Token *last = t;
				t = t->next;
				last->next = NULL;

				macros[mi++] = m;
			}
		}
		else
		{
			if (t->kind == TK_IDENT)
			{
				Macro *m = find_macro(t->str);
				if (!m)
				{
					if (before)
						before->next = t;
					else
						start = t;
					before = t;
					continue;
				}

				Token *replace = m->replace;
				while (replace)
				{
					Token *newt = calloc(1, sizeof(Token));
					newt->kind = replace->kind;
					newt->val = replace->val;
					newt->str = t->str;
					if (before)
						before->next = newt;
					else
						start = t;
					before = newt;
					replace = replace->next;
				}
				continue;
			}

			if (before)
				before->next = t;
			else
				start = t;
			before = t;
		}
	}

	start = remove_newline(start);

	return start;
}
#ifdef __STDC__
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#else
void *calloc();
#endif
typedef struct LVar LVar;
typedef struct GVar GVar;
typedef struct EnumVal EnumVal;
typedef struct ConsumeTypeRes ConsumeTypeRes;
typedef struct TypeDef TypeDef;
typedef struct Enum Enum;
typedef struct Func Func;

struct LVar
{
  LVar *next;
  String *name;
  int offset;
  Type *type;
};

struct GVar
{
  GVar *next;
  String *name;
  Type *type;
};

struct EnumVal
{
  EnumVal *next;
  String *name;
  int val;
};

struct ConsumeTypeRes
{
  Type *type;
  Token *tok;
};

struct TypeDef
{
  TypeDef *next;
  String *name;
  Type *type;
};

struct Func
{
  Func *next;
  String *name;
  Type *type;
};

LVar *locals;
GVar *globals;
EnumVal *enumVals;
StructType *structs;
TypeDef *typedefs;
Func *funcs;

LVar *find_lvar(Token *tok)
{
  for (LVar *var = locals; var; var = var->next)
    if (str_equals(var->name, tok->str))
      return var;

  return NULL;
}

LVar *new_lvar(String *name, Type *type)
{
  LVar *lvar = calloc(1, sizeof(LVar));
  lvar->next = locals;
  lvar->name = name;
  lvar->type = type;

  if (locals)
    lvar->offset = locals->offset + sizeof_type(type);
  else
    lvar->offset = sizeof_type(type);

  locals = lvar;
  return lvar;
}

EnumVal *find_enum_val(Token *tok)
{
  for (EnumVal *val = enumVals; val; val = val->next)
    if (str_equals(val->name, tok->str))
      return val;

  return NULL;
}

EnumVal *new_enum_val(String *name, int val)
{
  EnumVal *enumVal = calloc(1, sizeof(EnumVal));
  enumVal->next = enumVals;
  enumVal->name = name;
  enumVal->val = val;

  enumVals = enumVal;
  return enumVal;
}

StructType *find_struct(Token *tok)
{
  for (StructType *type = structs; type; type = type->next)
    if (str_equals(type->name, tok->str))
      return type;

  return NULL;
}

StructField *find_struct_field(Token *tok, StructType *type)
{
  for (StructField *field = type->fields; field; field = field->next)
    if (str_equals(field->name, tok->str))
      return field;

  return NULL;
}

void new_gvar(String *name, Type *type)
{
  GVar *gvar = calloc(1, sizeof(GVar));
  gvar->next = globals;
  gvar->name = name;
  gvar->type = type;

  globals = gvar;
}

GVar *find_gvar(Token *tok)
{
  for (GVar *var = globals; var; var = var->next)
    if (str_equals(var->name, tok->str))
      return var;

  return NULL;
}

TypeDef *find_typedef(Token *tok)
{
  for (TypeDef *type = typedefs; type; type = type->next)
    if (str_equals(type->name, tok->str))
      return type;

  return NULL;
}

TypeDef *new_typedef(String *name, Type *type)
{
  TypeDef *typedef_ = calloc(1, sizeof(TypeDef));
  typedef_->next = typedefs;
  typedef_->name = name;
  typedef_->type = type;

  typedefs = typedef_;
  return typedef_;
}

Func *find_func(Token *tok)
{
  for (Func *func = funcs; func; func = func->next)
    if (str_equals(func->name, tok->str))
      return func;

  return NULL;
}

Func *new_func(String *name, Type *type)
{
  Func *func = calloc(1, sizeof(Func));
  func->next = funcs;
  func->name = name;
  func->type = type;

  funcs = func;
  return func;
}

bool consume(char *op)
{
  if (token->kind != TK_RESERVED || !str_chr_equals(token->str, op))
    return false;

  token = token->next;
  return true;
}

Token *consume_kind(TokenKind kind)
{
  if (token->kind == kind)
  {
    Token *tok = token;
    token = token->next;
    return tok;
  }
  return NULL;
}

Token *consume_ident()
{
  if (token->kind != TK_IDENT)
    return NULL;

  Token *res = token;
  token = token->next;
  return res;
}

bool check_kind(TokenKind kind)
{
  return token->kind == kind;
}

void go_to(Token *tok)
{
  token = tok;
}

void expect(char *op)
{
  if (token->kind != TK_RESERVED || !str_chr_equals(token->str, op))
    error_at_here("token mismatch: expected %s", op);

  token = token->next;
}

void expect_kind(TokenKind kind)
{
  if (token->kind != kind)
    error_at_here("token mismatch");

  token = token->next;
}

int expect_number()
{
  if (token->kind != TK_NUM)
    error_at_here("token mismatch: expected number");

  int val = token->val;
  token = token->next;
  return val;
}

bool at_eof()
{
  return token->kind == TK_EOF;
}

ConsumeTypeRes *consume_type();
Type *consume_type_name()
{
  if (consume_kind(TK_INT))
    return new_type(INT, NULL);
  if (consume_kind(TK_CHAR))
    return new_type(CHAR, NULL);
  if (consume_kind(TK_VOID))
    return new_type(VOID, NULL);
  if (consume_kind(TK_BOOL))
    return new_type(BOOL, NULL);
  if (consume_kind(TK_BUILTIN_VA_LIST))
  {
    Type *type = new_type(ARRAY, new_type(VA_LIST_TAG, NULL));
    type->array_size = 1;
    return type;
  }
  if (consume_kind(TK_ENUM))
  {
    consume_kind(TK_IDENT);
    if (consume("{"))
    {
      int i = 0;
      while (!consume("}"))
      {
        Token *name = consume_ident();
        if (!name)
          error_at_here("expected enum name");

        new_enum_val(name->str, i++);

        consume(",");
      }
    }
    return new_type(INT, NULL);
  }

  if (check_kind(TK_STRUCT) || check_kind(TK_UNION))
  {
    bool is_union = false;
    if (consume_kind(TK_UNION))
      is_union = true;
    else
      consume_kind(TK_STRUCT);

    Token *id = consume_kind(TK_IDENT);

    if (consume("{"))
    {
      String *name = NULL;
      if (id)
        name = id->str;
      Type *type = new_struct_type(name, is_union);

      while (!consume("}"))
      {
        ConsumeTypeRes *res = consume_type();
        if (!res)
          error_at_here("expected type");

        add_field(type->struct_type, res->type, res->tok->str);
        expect(";");
      }

      if (id)
      {
        StructType *s = find_struct(id);
        if (s)
        {
          if (s->fields)
            error_at_here("struct %.*s is already defined", id->str->len, id->str->ptr);
          else
          {
            s->fields = type->struct_type->fields;
            s->is_union = type->struct_type->is_union;
            s->alignment = type->struct_type->alignment;
            s->size = type->struct_type->size;
          }
        }
        else
        {
          type->struct_type->next = structs;
          structs = type->struct_type;
        }
      }

      return type;
    }
    else
    {
      if (!id)
        error_at_here("expected struct name");
      StructType *type = find_struct(id);
      Type *ty;
      if (!type)
      {
        // forward declaration
        ty = new_struct_type(id->str, is_union);
        ty->struct_type->next = structs;
        structs = ty->struct_type;
      }
      else
      {
        ty = new_type(STRUCT, NULL);
        ty->struct_type = type;
      }
      return ty;
    }
  }

  Token *tok = consume_kind(TK_IDENT);
  if (tok)
  {
    TypeDef *tdef = find_typedef(tok);
    if (tdef)
      return tdef->type;
    go_to(tok);
  }

  return NULL;
}

Node *expr();
Type *consume_noident_type();
ConsumeTypeRes *expect_nested_type(Type *type)
{
  int ptr = 0;
  ConsumeTypeRes *res;

  if (consume("("))
  {
    while (consume("*"))
      ptr++;
    res = expect_nested_type(type);
    expect(")");
  }
  else
  {
    Token *tok = consume_ident();
    if (!tok)
      error_at_here("expected identifier");

    res = calloc(1, sizeof(ConsumeTypeRes));
    res->type = type;
    res->tok = tok;
  }
  if (consume("("))
  {
    consume_noident_type();
    consume_ident();
    while (consume(","))
    {
      if (consume("..."))
        break;
      assert(consume_noident_type() != NULL);
      consume_ident();
    }

    expect(")");

    res->type = new_type(FUNC, type);
  }
  for (int i = 0; i < ptr; i++)
    res->type = new_type(PTR, res->type);

  while (consume("["))
  {
    Node *size = expr();
    if (size->kind != ND_NUM)
      error_at_here("expected constant expression");
    expect("]");
    res->type = new_type(ARRAY, type);
    res->type->array_size = size->val;
  }

  return res;
}

ConsumeTypeRes *consume_type()
{
  Type *type = consume_type_name();
  if (!type)
    return NULL;
  while (consume("*"))
    type = new_type(PTR, type);
  ConsumeTypeRes *res = expect_nested_type(type);
  return res;
}

Type *expect_noident_type(Type *type)
{
  while (consume("*"))
    type = new_type(PTR, type);

  if (consume("("))
  {
    type = expect_noident_type(type);
    expect(")");
  }

  if (consume("("))
  {
    consume_noident_type();
    while (consume(","))
      consume_noident_type();
    expect(")");
    type = new_type(FUNC, NULL);
  }

  while (consume("["))
  {
    Node *size = expr();
    if (size->kind != ND_NUM)
      error_at_here("expected constant expression");
    expect("]");
    type = new_type(ARRAY, type);
    type->array_size = size->val;
  }

  return type;
}

Type *consume_noident_type()
{
  Type *type = consume_type_name();
  if (!type)
    return NULL;

  return expect_noident_type(type);
}

void next()
{
  token = token->next;
}

Node *new_node(NodeKind kind, Node *lhs, Node *rhs)
{
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  node->lhs = lhs;
  node->rhs = rhs;

  switch (kind)
  {
  case ND_ADD:
  case ND_SUB:
  case ND_MUL:
  case ND_DIV:
  case ND_ASSIGN:
    assert(lhs->type != NULL);
    node->type = lhs->type;
    break;
  case ND_EQ:
  case ND_NE:
  case ND_LT:
  case ND_LE:
  case ND_AND:
  case ND_OR:
    node->type = new_type(INT, NULL);
    break;
  case ND_ADDR:
    assert(!rhs);
    node->type = new_type(PTR, lhs->type);
    break;
  case ND_DEREF:
    assert(lhs->type->ty == PTR);
    assert(rhs == NULL);
    node->type = lhs->type->ptr_to;
    break;
  case ND_STRING:
    node->type = new_type(PTR, new_type(CHAR, NULL));
    break;
  case ND_CALL:
    node->type = new_type(INT, NULL);
    break;
  case ND_POST_INCR:
  case ND_POST_DECR:
    assert(lhs->type != NULL);
    assert(rhs->kind == ND_NUM);
    node->type = lhs->type;
    break;
  case ND_LVAR:
  case ND_GVAR:
    error_at_here("internal error");
    break;
  default:
    break;
  }

  return node;
}

Node *new_typed_node(NodeKind kind, Node *lhs, Node *rhs, Type *type)
{
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  node->lhs = lhs;
  node->rhs = rhs;
  node->type = type;
  return node;
}

Node *new_node_num(int val)
{
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_NUM;
  node->val = val;
  Type *type = calloc(1, sizeof(Type));
  type->ty = INT;
  node->type = type;
  return node;
}

Node *new_node_char(char val)
{
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_NUM;
  node->val = val;
  Type *type = calloc(1, sizeof(Type));
  type->ty = CHAR;
  node->type = type;
  return node;
}

Node *stmt();
Node *expr();
Node *assign();
Node *logical();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *postfix();
Node *primary();

External *ext;
int literal_count;

External *external()
{
  locals = NULL;
  if (!globals)
    new_gvar(new_string("NULL", 4), new_type(PTR, new_type(VOID, NULL)));

  External *external = calloc(1, sizeof(External));
  ext = external;
  int i = 0;

  if (consume_kind(TK_EXTERN))
    ext->is_extern = true;

  if (check_kind(TK_ENUM))
  {
    ext->kind = EXT_ENUM;

    consume_type_name();
    expect(";");
    return external;
  }

  if (check_kind(TK_STRUCT) || check_kind(TK_UNION))
  {
    ext->kind = EXT_STRUCT;
    Type *ty = consume_type_name();
    if (!ty)
      error_at_here("expected struct or union");
    expect(";");
    return external;
  }

  if (consume_kind(TK_TYPEDEF))
  {
    ext->kind = EXT_TYPEDEF;

    ConsumeTypeRes *res = consume_type();
    if (!res)
      error_at_here("expected type");

    new_typedef(res->tok->str, res->type);
    expect(";");
    return external;
  }

  ConsumeTypeRes *res = consume_type();
  if (!res)
    error_at_here("invalid type");

  external->name = res->tok->str;
  if (res->type->ty == FUNC)
  {
    external->kind = EXT_FUNC;

    if (!find_func(res->tok))
      new_func(res->tok->str, res->type->ptr_to);

    go_to(res->tok);
    next();
    expect("(");

    Token *cur = token;
    bool no_args = false;
    if (consume_kind(TK_VOID))
    {
      if (consume(")"))
        no_args = true;
      else
        go_to(cur);
    }
    if (!consume(")") && !no_args)
    {
      for (;;)
      {
        if (consume("..."))
        {
          external->is_variadic = true;
          break;
        }
        ConsumeTypeRes *res = consume_type();
        if (!res)
          error_at_here("failed to parse argument");

        LVar *lvar = find_lvar(res->tok);
        if (!lvar)
          lvar = new_lvar(res->tok->str, res->type);

        external->offsets[i++] = lvar->offset;

        if (!consume(","))
          break;
      }

      expect(")");
    }

    if (consume(";"))
    {
      external->kind = EXT_FUNCDECL;
      return external;
    }

    external->code = stmt();
    if (external->code->kind != ND_BLOCK)
      error_at_here("expected block");
  }
  else
  {
    external->kind = EXT_GVAR;

    new_gvar(res->tok->str, res->type);
    external->size = sizeof_type(res->type);

    expect(";");
  }
  if (locals)
    external->stack_size = (locals->offset / 8) * 8 + 8;

  return external;
}

Node *stmt()
{
  Node *node;

  if (consume_kind(TK_CASE))
  {
    Node *e = expr();
    if (e->kind != ND_NUM)
      error_at_here("expected constant expression");

    expect(":");
    node = new_node(ND_CASE, e, NULL);
  }
  else if (consume_kind(TK_DEFAULT))
  {
    expect(":");
    node = new_node(ND_DEFAULT, NULL, NULL);
  }
  else if (consume_kind(TK_IF))
  {
    expect("(");
    Node *lhs = expr();
    expect(")");
    Node *rhs = stmt();

    if (consume_kind(TK_ELSE))
      node = new_node(ND_IFELSE, lhs, new_node(ND_UNNAMED, rhs, stmt()));
    else
      node = new_node(ND_IF, lhs, rhs);
  }
  else if (consume_kind(TK_SWITCH))
  {
    expect("(");
    Node *lhs = expr();
    expect(")");
    Node *rhs = stmt();

    node = new_node(ND_SWITCH, lhs, rhs);
  }
  else if (consume_kind(TK_WHILE))
  {
    expect("(");
    Node *lhs = expr();
    expect(")");
    node = new_node(ND_WHILE, lhs, stmt());
  }
  else if (consume_kind(TK_FOR))
  {
    Node *a = NULL;
    Node *b = NULL;
    Node *c = NULL;
    Node *d = NULL;
    expect("(");
    if (!consume(";"))
    {
      a = expr();
      expect(";");
    }
    if (!consume(";"))
    {
      b = expr();
      expect(";");
    }
    if (!consume(")"))
    {
      c = expr();
      expect(")");
    }
    d = stmt();

    Node *lhs = new_node(ND_UNNAMED, a, b);
    Node *rhs = new_node(ND_UNNAMED, c, d);

    node = new_node(ND_FOR, lhs, rhs);
  }
  else if (consume("{"))
  {
    node = new_node(ND_BLOCK, NULL, NULL);

    for (Node *last = node; !consume("}"); last = last->rhs)
      last->rhs = new_node(ND_UNNAMED, stmt(), NULL);
  }
  else if (consume_kind(TK_RETURN))
  {
    if (consume(";"))
      node = new_node(ND_RETURN, NULL, NULL);
    else
    {
      node = new_node(ND_RETURN, expr(), NULL);
      expect(";");
    }
  }
  else if (consume_kind(TK_BREAK))
  {
    node = new_node(ND_BREAK, NULL, NULL);
    expect(";");
  }
  else if (consume_kind(TK_CONTINUE))
  {
    node = new_node(ND_CONTINUE, NULL, NULL);
    expect(";");
  }
  else
  {
    node = expr();
    expect(";");
  }

  return node;
}

Node *expr()
{
  return assign();
}

Node *assign()
{
  Node *node = logical();
  if (consume("="))
    node = new_node(ND_ASSIGN, node, assign());
  else if (consume("+="))
    node = new_node(ND_ASSIGN, node, new_node(ND_ADD, node, assign()));
  else if (consume("-="))
    node = new_node(ND_ASSIGN, node, new_node(ND_SUB, node, assign()));
  else if (consume("*="))
    node = new_node(ND_ASSIGN, node, new_node(ND_MUL, node, assign()));
  else if (consume("/="))
    node = new_node(ND_ASSIGN, node, new_node(ND_DIV, node, assign()));

  return node;
}

Node *logical()
{
  Node *node = equality();

  for (;;)
  {
    if (consume("&&"))
      node = new_node(ND_AND, node, logical());
    else if (consume("||"))
      node = new_node(ND_OR, node, logical());
    else
      return node;
  }
}

Node *equality()
{
  Node *node = relational();

  for (;;)
  {
    if (consume("=="))
      node = new_node(ND_EQ, node, relational());
    else if (consume("!="))
      node = new_node(ND_NE, node, relational());
    else
      return node;
  }
}

Node *relational()
{
  Node *node = add();

  for (;;)
  {
    if (consume("<="))
      node = new_node(ND_LE, node, add());
    else if (consume("<"))
      node = new_node(ND_LT, node, add());
    else if (consume(">="))
      node = new_node(ND_LE, add(), node);
    else if (consume(">"))
      node = new_node(ND_LT, add(), node);
    else
      return node;
  }
}

Node *add()
{
  Node *node = mul();

  for (;;)
  {
    if ((node->type->ty == PTR || node->type->ty == ARRAY) && node->type->ptr_to->ty != VOID)
    {
      int size = sizeof_type(node->type->ptr_to);

      if (consume("+"))
      {
        if (node->type->ty == ARRAY)
          node = new_typed_node(ND_ADDR, node, NULL, new_type(PTR, node->type->ptr_to));
        node = new_node(ND_ADD, node, new_node(ND_MUL, mul(), new_node_num(size)));
      }
      else if (consume("-"))
      {
        if (node->type->ty == ARRAY)
          node = new_typed_node(ND_ADDR, node, NULL, new_type(PTR, node->type->ptr_to));
        node = new_node(ND_SUB, node, new_node(ND_MUL, mul(), new_node_num(size)));
      }
      return node;
    }

    if (consume("+"))
    {
      Node *rhs = mul();
      if (node->kind == ND_NUM && rhs->kind == ND_NUM)
        node->val += rhs->val;
      else
        node = new_node(ND_ADD, node, rhs);
    }
    else if (consume("-"))
    {
      Node *rhs = mul();
      if (node->kind == ND_NUM && rhs->kind == ND_NUM)
        node->val -= rhs->val;
      else
        node = new_node(ND_SUB, node, rhs);
    }
    else
      return node;
  }
}

Node *mul()
{
  Node *node = unary();

  for (;;)
  {
    if (consume("*"))
    {
      Node *rhs = unary();
      if (node->kind == ND_NUM && rhs->kind == ND_NUM)
        node->val *= rhs->val;
      else
        node = new_node(ND_MUL, node, rhs);
    }
    else if (consume("/"))
    {
      Node *rhs = unary();
      if (node->kind == ND_NUM && rhs->kind == ND_NUM)
        node->val /= rhs->val;
      else
        node = new_node(ND_DIV, node, rhs);
    }
    else
      return node;
  }
}

Node *unary()
{
  if (consume("+"))
    return postfix();
  else if (consume("-"))
  {
    Node *rhs = postfix();
    if (rhs->kind == ND_NUM)
    {
      rhs->val = -rhs->val;
      return rhs;
    }
    else
      return new_node(ND_SUB, new_node_num(0), rhs);
  }
  else if (consume("&"))
    return new_node(ND_ADDR, postfix(), NULL);
  else if (consume("*"))
  {
    Node *l = primary();
    if (l->type->ty != PTR && l->type->ty != ARRAY)
      error_at_here("dereference failed: not a pointer");

    if (l->type->ty == ARRAY)
      l = new_node(ND_ADDR, l, NULL);

    return new_node(ND_DEREF, l, NULL);
  }
  else if (consume_kind(TK_SIZEOF))
  {
    Token *cur = token;
    if (consume("("))
    {
      Type *type = consume_noident_type();
      if (type)
      {
        expect(")");
        return new_node_num(sizeof_type(type));
      }
      go_to(cur);
    }

    Node *n = unary();
    return new_node_num(sizeof_type(n->type));
  }
  else if (consume("!"))
    return new_node(ND_EQ, postfix(), new_node_num(0));
  else if (consume("++"))
  {
    Node *n = postfix();
    return new_node(ND_ASSIGN, n, new_node(ND_ADD, n, new_node_num(1)));
  }
  else if (consume("--"))
  {
    Node *n = postfix();
    return new_node(ND_ASSIGN, n, new_node(ND_SUB, n, new_node_num(1)));
  }

  return postfix();
}

Node *postfix()
{
  Node *node = primary();

  for (;;)
  {
    if (consume("["))
    {
      Node *subscript = expr();
      int size = sizeof_type(node->type->ptr_to);
      if (node->type->ty == ARRAY)
        node = new_typed_node(ND_ADDR, node, NULL, new_type(PTR, node->type->ptr_to));
      node = new_node(ND_DEREF, new_node(ND_ADD, node, new_node(ND_MUL, subscript, new_node_num(size))), NULL);
      expect("]");
      continue;
    }
    if (consume("."))
    {
      Token *tok = consume_ident();
      if (!tok)
        error_at_token(tok, "expected identifier after '.'");

      if (node->type->ty != STRUCT)
        error_at_token(tok, "expected struct type");

      StructField *field = find_struct_field(tok, node->type->struct_type);
      if (!field)
        error_at_here("no such field");

      node = new_node(ND_ADDR, node, NULL);
      node = new_node(ND_ADD, node, new_node_num(field->offset));
      node->type = new_type(PTR, field->type);
      node = new_node(ND_DEREF, node, NULL);
      continue;
    }
    if (consume("->"))
    {
      Token *tok = consume_ident();
      if (!tok)
        error_at_token(tok, "expected identifier after '->'");

      if (node->type->ty != PTR)
        error_at_token(tok, "expected pointer type");

      if (node->type->ptr_to->ty != STRUCT)
        error_at_token(tok, "expected struct type");

      StructField *field = find_struct_field(tok, node->type->ptr_to->struct_type);
      if (!field)
        error_at_here("no such field");

      node = new_node(ND_ADD, node, new_node_num(field->offset));
      node->type = new_type(PTR, field->type);
      node = new_node(ND_DEREF, node, NULL);
      continue;
    }
    if (consume("++"))
    {
      // TODO: pointer
      node = new_node(ND_POST_INCR, node, new_node_num(1));
      continue;
    }
    if (consume("--"))
    {
      node = new_node(ND_POST_DECR, node, new_node_num(1));
      continue;
    }

    break;
  }
  return node;
}

Node *primary()
{
  if (consume("("))
  {
    Node *node = expr();
    expect(")");
    return node;
  }

  Token *tok = consume_kind(TK_STRING);
  if (tok)
  {
    StringLiteral *s = calloc(1, sizeof(StringLiteral));
    s->str = tok->str;
    s->next = ext->literals;
    ext->literals = s;
    Node *node = new_node(ND_STRING, NULL, NULL);
    node->offset = literal_count++;
    s->offset = node->offset;
    return node;
  }

  ConsumeTypeRes *res = consume_type();
  if (res)
  {
    LVar *lvar = find_lvar(res->tok);
    if (!lvar)
      lvar = new_lvar(res->tok->str, res->type);

    Node *node = new_typed_node(ND_LVAR, NULL, NULL, lvar->type);
    node->offset = lvar->offset;

    if (consume("="))
    {
      if (consume("{"))
      {
        if (lvar->type->ty != ARRAY)
          error_at_token(tok, "type mismatch");

        node = new_typed_node(ND_ASSIGN_ARRAY, node, NULL, lvar->type);
        Node *last = node;

        for (int i = 0; i < lvar->type->array_size; i++)
        {
          Node *ptr = new_typed_node(ND_ADD, new_node(ND_ADDR, node->lhs, NULL), new_node_num(i * sizeof_type(lvar->type->ptr_to)), new_type(PTR, lvar->type->ptr_to));
          Node *deref = new_node(ND_DEREF, ptr, NULL);

          last->rhs = new_node(ND_UNNAMED, new_node(ND_ASSIGN, deref, assign()), NULL);
          last = last->rhs;

          if (i != lvar->type->array_size - 1)
            expect(",");
        }
        expect("}");
      }
      else
        node = new_node(ND_ASSIGN, node, assign());
    }

    return node;
  }

  tok = consume_ident();
  if (tok)
  {
    Node *node;

    if (consume("("))
    {
      Node *func = calloc(1, sizeof(Node));
      func->name = tok->str;

      Type *type;
      if (find_func(tok))
        type = find_func(tok)->type;
      else
        type = new_type(INT, NULL);

      node = new_typed_node(ND_CALL, func, NULL, type);
      if (str_chr_equals(tok->str, "va_start"))
        node->kind = ND_VA_START;

      Node *last = node;
      if (consume(")"))
        return node;
      for (;;)
      {
        last->rhs = new_node(ND_UNNAMED, expr(), NULL);
        last = last->rhs;

        if (consume(")"))
          break;
        else
          expect(",");
      }
    }
    else
    {
      LVar *lvar = find_lvar(tok);
      if (lvar)
      {
        node = new_typed_node(ND_LVAR, NULL, NULL, lvar->type);
        node->offset = lvar->offset;
      }
      else
      {
        EnumVal *val = find_enum_val(tok);
        if (val)
          node = new_node_num(val->val);
        else
        {
          GVar *gvar = find_gvar(tok);
          if (gvar)
          {
            node = new_typed_node(ND_GVAR, NULL, NULL, gvar->type);
            node->name = tok->str;
          }
          else
            error_at_token(tok, "%.*s is not defined", tok->str->len, tok->str->ptr);
        }
      }
    }

    return node;
  }

  tok = consume_kind(TK_CHAR_CONST);
  if (tok)
  {
    char c = *(tok->str->ptr);
    if (c == '\\')
    {
      switch (*(tok->str->ptr + 1))
      {
      case '\\':
        c = '\\';
        break;
      case '\'':
        c = '\'';
        break;
      case 'n':
        c = '\n';
        break;
      case '0':
        c = '\0';
        break;
      default:
        error_at_token(tok, "unknown escape sequence");
      }
    }
    Node *node = new_node_char(c);
    return node;
  }

  return new_node_num(expect_number());
}
#ifdef __STDC__
#include <stdio.h>
#endif

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
    cerror("left value of assignment must be variable: found %d\n", node->kind);
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
        cerror("invalid size");
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
        cerror("not implemented: size %d", size);

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
      cerror("failed to assign");
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
        cerror("not implemented: too big object");
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
      cerror("not implemented: return value");
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
      cerror("unexpected type");
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
      cerror("unreachable");
      return;
    }

    printf("  push rax\n");
  }
}
#ifdef __STDC__
#include <stdio.h>
#include <stdlib.h>
#else
void *calloc();
#endif

char *user_input;
char *filename;
char *dir_name;
int current_stack_size;
int arg_count;

int main(int argc, char **argv)
{
  if (argc != 2)
  {
    cerror("invalid argument");
    return 1;
  }

  filename = argv[1];
  dir_name = get_dir(filename);

  user_input = read_file(filename);
  token = tokenize(user_input, true);
  token = preprocess(token);

  printf(".intel_syntax noprefix\n");
  printf(".data\n");
  printf("NULL:\n");
  printf("  .zero 8\n");

  while (!at_eof())
  {
    External *ext = external();

    switch (ext->kind)
    {
    case EXT_FUNC:
      gen_function(ext);
      break;
    case EXT_GVAR:
      if (!ext->is_extern)
      {
        printf(".globl %.*s\n", ext->name->len, ext->name->ptr);
        printf(".data\n");
        printf("%.*s:\n", ext->name->len, ext->name->ptr);
        printf("  .zero %d\n", ext->size);
      }
      break;
    default:
      break;
    }
  }

  return 0;
}

