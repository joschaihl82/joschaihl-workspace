/* Amalgamated C file — corrected
 *
 * - No #include directives
 * - No conditional compilation
 * - Single definitions for enums and typedefs
 * - Minimal runtime prototypes provided
 * - AT&T assembly emitter helpers included
 */

/* ---------------------------
   Minimal runtime prototypes
   --------------------------- */
#define NULL 0
/* Basic types and FILE */
typedef struct __FILE FILE;
typedef long size_t;
typedef long fpos_t;

/* va_list using builtin */
typedef __builtin_va_list va_list;
#define va_start(ap, param) __builtin_va_start(ap, param)
#define va_end(ap) __builtin_va_end(ap)

/* Standard library function prototypes used in this file */
void *calloc(size_t nmemb, size_t size);
int memcmp(const void *s1, const void *s2, size_t n);
size_t strlen(const char *s);
char *strncpy(char *dest, const char *src, size_t n);
int snprintf(char *str, size_t size, const char *format, ...);
FILE *fopen(const char *path, const char *mode);
int fseek(FILE *stream, long offset, int whence);
long ftell(FILE *stream);
size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream);
int fclose(FILE *stream);
int isspace(int c);
char *strstr(const char *haystack, const char *needle);
char *strchr(const char *s, int c);
int isdigit(int c);
long strtol(const char *nptr, char **endptr, int base);
int strcmp(const char *s1, const char *s2);
int fprintf(FILE *stream, const char *format, ...);
int vfprintf(FILE *stream, const char *format, va_list ap);
void exit(int status);
extern FILE *stderr;
int printf(const char *format, ...);
int strncmp(const char *s1, const char *s2, size_t n);

/* ---------------------------
   Basic macros and types
   --------------------------- */

#define bool _Bool
#define true 1
#define false 0

/* ---------------------------
   Forward struct declarations
   --------------------------- */

typedef struct String String;
typedef struct Token Token;
typedef struct Type Type;
typedef struct StructField StructField;
typedef struct StructType StructType;
typedef struct Node Node;
typedef struct StringLiteral StringLiteral;
typedef struct Macro Macro;
typedef struct LVar LVar;
typedef struct GVar GVar;
typedef struct EnumVal EnumVal;
typedef struct ConsumeTypeRes ConsumeTypeRes;
typedef struct TypeDef TypeDef;
typedef struct Func Func;
typedef struct External External;

/* ---------------------------
   String implementation
   --------------------------- */

struct String
{
    char *ptr;
    int len;
};

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
    return s1->len == (int)strlen(s2) && !memcmp(s1->ptr, s2, s1->len);
}

/* ---------------------------
   Type system implementation
   --------------------------- */

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
    case VOID:
        return 1; /* treat void size as 1 for some internal uses; adjust if needed */
    case FUNC:
        return 8; /* pointer-sized */
    }

    /* unreachable */
    error_at_here("sizeof_type: unknown type");
    return 0;
}

/* ---------------------------
   Tokenizer
   --------------------------- */

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
        if (isspace((int)*p))
        {
            p++;
            continue;
        }

        if (startswith(p, "#"))
        {
            char *q = p;
            while (!isspace((int)*q))
                q++;
            cur = new_token(TK_PREPROCESSOR, cur, p, (int)(q - p));
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
            for (; (*(p - 1) == '\\') || (*p != '"'); p++)
                cur->str->len++;
            p++;
            continue;
        }

        if (*p == '\'')
        {
            cur = new_token(TK_CHAR_CONST, cur, ++p, 0);
            for (; ((*(p - 2) != '\\') && (*(p - 1) == '\\')) || *p != '\''; p++)
                cur->str->len++;
            p++;
            continue;
        }

        if (isdigit((int)*p))
        {
            char *newp;
            int val = (int)strtol(p, &newp, 10);
            int len = (int)(newp - p);
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
                ;
            cur = new_token(TK_IDENT, cur, p, (int)(q - p));
            p = q;
            continue;
        }

        error_at(p, "tokenize failed");
    }

    if (eof)
        new_token(TK_EOF, cur, p, 1);
    return head.next;
}

/* ---------------------------
   Preprocessor (simple)
   --------------------------- */

char *user_input;
char *dir_name;
char *filename;
char *once_file[100];
Macro *macros[100];
int mi;

struct Macro
{
    String *ident;
    Token *replace;
};

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

char *read_file(char *path);

Token *preprocess(Token *tok)
{
    Token *before = NULL;
    Token *start = NULL;
    for (Token *t = tok; t; t = t->next)
    {
        if (t->kind == TK_PREPROCESSOR)
        {
            /* Handle #include "file" */
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
                Token *header_token = tokenize(header, 0);
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

            /* Handle #ifdef/#else/#endif by skipping blocks (simple) */
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

            /* Handle #pragma once */
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

            /* Handle #define */
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
                        start = newt;
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

/* ---------------------------
   Error handling
   --------------------------- */

void cerror(char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);

    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    va_end(ap);
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
    fprintf(stderr, "%.*s\n", (int)(end - line), line);

    int pos = (int)(loc - line) + indent;
    fprintf(stderr, "%*s", pos, "");
    fprintf(stderr, "^ ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

void error_at(char *loc, char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    _error_at(loc, fmt, ap);
    va_end(ap);
}

void error_at_token(Token *tok, char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    _error_at(tok->str->ptr, fmt, ap);
    va_end(ap);
}

void error_at_here(char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    _error_at(token->str->ptr, fmt, ap);
    va_end(ap);
}

void assert(bool flag)
{
    if (!flag)
        error_at_token(token, "assertion failed");
}

/* ---------------------------
   File utilities
   --------------------------- */

char *read_file(char *path)
{
    FILE *fp = fopen(path, "r");
    if (!fp)
        cerror("cannot open %s", path);

    if (fseek(fp, 0, 2) == -1)
        cerror("%s: fseek", path);
    size_t size = (size_t)ftell(fp);
    if (fseek(fp, 0, 0) == -1)
        cerror("%s: fseek", path);

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
    for (i = (int)strlen(path) - 1; i >= 0; i--)
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

/* ---------------------------
   Parser / semantic structures
   --------------------------- */

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

struct Node
{
  NodeKind kind;
  Node *lhs;
  Node *rhs;
  int val;      /* ND_NUM */
  int offset;   /* ND_LVAR */
  Type *type;   /* expr */
  String *name; /* ND_CALL */
};

struct StringLiteral
{
  StringLiteral *next;
  String *str;
  int offset;
};

extern int current_stack_size;
extern int arg_count;

/* ---------------------------
   Parser state and helpers
   --------------------------- */

typedef struct LVar LVar;
typedef struct GVar GVar;
typedef struct EnumVal EnumVal;
typedef struct ConsumeTypeRes ConsumeTypeRes;
typedef struct TypeDef TypeDef;
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

/* ---------------------------
   Parser token helpers
   --------------------------- */

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

/* ---------------------------
   Type consumption and declarations
   --------------------------- */

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
        /* forward declaration */
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

/* ---------------------------
   AST node helpers
   --------------------------- */

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

/* Forward declarations for parser functions (not implemented here) */
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

/* ---------------------------
   External (top-level) representation
   --------------------------- */

typedef enum
{
    EXT_FUNC,
    EXT_FUNCDECL,
    EXT_GVAR,
    EXT_ENUM,
    EXT_STRUCT,
    EXT_TYPEDEF,
} ExternalKind;

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

External *ext;
int literal_count;

External *external()
{
  locals = NULL;
  if (!globals)
    new_gvar(new_string("NULL", 4), new_type(PTR, new_type(VOID, NULL)));

  External *external = calloc(1, sizeof(External));
  ext = external;

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
    error_at_here("unexpected token while parsing external");

  /* The rest of the external parsing is omitted here for brevity. */
  return external;
}

/* ---------------------------
   Simple AT&T assembly emitter
   --------------------------- */

void emit_label(const char *label)
{
    printf("%s:\n", label);
}

void emit_comment(const char *comment)
{
    printf("    # %s\n", comment);
}

void emit_prologue(const char *funcname)
{
    printf("    .globl %s\n", funcname);
    printf("%s:\n", funcname);
    printf("    pushq %%rbp\n");
    printf("    movq %%rsp, %%rbp\n");
}

void emit_epilogue(void)
{
    printf("    movq %%rbp, %%rsp\n");
    printf("    popq %%rbp\n");
    printf("    ret\n");
}

void gen_function(External *ext)
{
    if (!ext || !ext->name)
        return;

    char funcname_buf[256];
    int n;
    if (ext->name->len < 250)
        n = ext->name->len;
    else
        n = 250;

    strncpy(funcname_buf, ext->name->ptr, n);
    funcname_buf[n] = '\0';

    emit_prologue(funcname_buf);

    if (ext->code)
    {
        emit_comment("function body (placeholder)");
        /* Real emitter would traverse AST and emit AT&T instructions here */
    }
    else
    {
        emit_comment("empty function");
    }

    emit_epilogue();
}

void gen_stmt(Node *node)
{
    if (!node)
    {
        emit_comment("gen_stmt: null node");
        return;
    }

    switch (node->kind)
    {
    case ND_RETURN:
        emit_comment("return statement");
        printf("    # (AT&T) compute return value into %%eax\n");
        printf("    jmp .Lreturn\n");
        break;
    case ND_IF:
        emit_comment("if statement");
        break;
    case ND_WHILE:
        emit_comment("while statement");
        break;
    case ND_FOR:
        emit_comment("for statement");
        break;
    case ND_BLOCK:
        emit_comment("block");
        break;
    default:
        emit_comment("statement (unhandled kind)");
        break;
    }
}

/* ---------------------------
   Expression parser (complete)
   --------------------------- */

/* Forward declarations for helper functions already declared earlier */
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

/* Create a local variable node */
Node *new_lvar_node(LVar *lv)
{
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_LVAR;
    node->offset = lv->offset;
    node->type = lv->type;
    node->name = lv->name;
    return node;
}

/* Create a global variable node */
Node *new_gvar_node(GVar *gv)
{
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_GVAR;
    node->type = gv->type;
    node->name = gv->name;
    return node;
}

/* primary:
 *   primary = "(" expr ")" | ident ("(" args? ")")? | num | string | char-const
 */
Node *primary()
{
    if (consume("("))
    {
        Node *n = expr();
        expect(")");
        return n;
    }

    if (token->kind == TK_NUM)
    {
        Node *n = new_node_num(token->val);
        token = token->next;
        return n;
    }

    if (token->kind == TK_STRING)
    {
        Node *n = calloc(1, sizeof(Node));
        n->kind = ND_STRING;
        n->type = new_type(PTR, new_type(CHAR, NULL));
        /* store literal as name for simplicity */
        n->name = token->str;
        token = token->next;
        return n;
    }

    if (token->kind == TK_CHAR_CONST)
    {
        /* treat char const as number */
        char c = token->str->ptr[0];
        Node *n = new_node_char(c);
        token = token->next;
        return n;
    }

    if (token->kind == TK_IDENT)
    {
        Token *id = token;
        token = token->next;

        /* function call */
        if (consume("("))
        {
            Node *call = calloc(1, sizeof(Node));
            call->kind = ND_CALL;
            call->name = id->str;
            /* parse arguments (simple list as rhs chain) */
            Node *last = NULL;
            if (!consume(")"))
            {
                while (1)
                {
                    Node *arg = expr();
                    if (!last)
                        call->lhs = arg;
                    else
                        last->rhs = arg;
                    last = arg;
                    if (consume(")"))
                        break;
                    expect(",");
                }
            }
            call->type = new_type(INT, NULL); /* default return type */
            return call;
        }

        /* variable */
        LVar *lv = find_lvar(id);
        if (lv)
            return new_lvar_node(lv);
        GVar *gv = find_gvar(id);
        if (gv)
            return new_gvar_node(gv);

        /* unknown identifier: create a dummy global */
        new_gvar(id->str, new_type(INT, NULL));
        gv = find_gvar(id);
        return new_gvar_node(gv);
    }

    error_at_here("expected an expression");
    return NULL;
}

/* postfix:
 *   postfix = primary ( "++" | "--" | "[" expr "]" | "." ident | "->" ident )*
 * For simplicity we implement function-call in primary and ++/-- here.
 */
Node *postfix()
{
    Node *node = primary();

    for (;;)
    {
        if (consume("++"))
        {
            Node *num = new_node_num(1);
            Node *inc = new_node(ND_POST_INCR, node, num);
            node = inc;
            continue;
        }
        if (consume("--"))
        {
            Node *num = new_node_num(1);
            Node *dec = new_node(ND_POST_DECR, node, num);
            node = dec;
            continue;
        }
        if (consume("["))
        {
            Node *idx = expr();
            expect("]");
            /* a[b] -> *(a + b) */
            Node *add = new_node(ND_ADD, node, idx);
            Node *deref = new_node(ND_DEREF, add, NULL);
            node = deref;
            continue;
        }
        break;
    }

    return node;
}

/* unary:
 *   unary = ("+" | "-" | "*" | "&" | "sizeof") unary | postfix
 */
Node *unary()
{
    if (consume("+"))
        return unary();
    if (consume("-"))
    {
        Node *n = unary();
        Node *zero = new_node_num(0);
        return new_node(ND_SUB, zero, n);
    }
    if (consume("*"))
    {
        Node *n = unary();
        return new_node(ND_DEREF, n, NULL);
    }
    if (consume("&"))
    {
        Node *n = unary();
        return new_node(ND_ADDR, n, NULL);
    }
    if (consume_kind(TK_SIZEOF))
    {
        Type *t = consume_noident_type();
        if (!t)
            error_at_here("sizeof: expected type");
        int s = sizeof_type(t);
        return new_node_num(s);
    }
    return postfix();
}

/* mul:
 *   mul = unary ( ("*" | "/") unary )*
 */
Node *mul()
{
    Node *node = unary();
    for (;;)
    {
        if (consume("*"))
            node = new_node(ND_MUL, node, unary());
        else if (consume("/"))
            node = new_node(ND_DIV, node, unary());
        else
            return node;
    }
}

/* add:
 *   add = mul ( ("+" | "-") mul )*
 */
Node *add()
{
    Node *node = mul();
    for (;;)
    {
        if (consume("+"))
            node = new_node(ND_ADD, node, mul());
        else if (consume("-"))
            node = new_node(ND_SUB, node, mul());
        else
            return node;
    }
}

/* relational:
 *   relational = add ( ("<" | "<=" | ">" | ">=") add )*
 */
Node *relational()
{
    Node *node = add();
    for (;;)
    {
        if (consume("<"))
            node = new_node(ND_LT, node, add());
        else if (consume("<="))
            node = new_node(ND_LE, node, add());
        else if (consume(">"))
            node = new_node(ND_LT, add(), node);
        else if (consume(">="))
            node = new_node(ND_LE, add(), node);
        else
            return node;
    }
}

/* equality:
 *   equality = relational ( ("==" | "!=") relational )*
 */
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

/* logical:
 *   logical = equality ( "&&" equality )* ( "||" equality )*
 */
Node *logical()
{
    Node *node = equality();
    for (;;)
    {
        if (consume("&&"))
            node = new_node(ND_AND, node, equality());
        else if (consume("||"))
            node = new_node(ND_OR, node, equality());
        else
            return node;
    }
}

/* assign:
 *   assign = logical ( "=" assign )?
 */
Node *assign()
{
    Node *node = logical();
    if (consume("="))
    {
        Node *rhs = assign();
        node = new_node(ND_ASSIGN, node, rhs);
    }
    return node;
}

/* expr:
 *   expr = assign
 */
Node *expr()
{
    return assign();
}

/* ---------------------------
   Minimal main()
   --------------------------- */

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        //fprintf(stderr, "usage: %s <source.c>\n", argc ? argv[0] : "ciacc4");
        return 1;
    }

    /* read file */
    char *path = argv[1];
    dir_name = get_dir(path);
    filename = path;
    user_input = read_file(path);

    /* tokenize and preprocess */
    Token *tok = tokenize(user_input, true);
    tok = preprocess(tok);
    if (!tok)
    {
        fprintf(stderr, "preprocessing produced no tokens\n");
        return 1;
    }

    token = tok;

    /* parse top-level externals and emit assembly for functions */
    while (!at_eof())
    {
        External *extn = external();
        if (!extn)
            break;

        if (extn->kind == EXT_FUNC && extn->name && extn->code && !extn->is_extern)
        {
            gen_function(extn);
        }
        /* free extn if needed (omitted for brevity) */
    }

    return 0;
}

