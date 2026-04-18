// Use standard headers instead of custom prototypes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdint.h>

typedef _Bool bool;
#define true 1
#define false 0

typedef struct String String;
struct String {
  char *ptr;
  int len;
};

String *new_string(char *ptr, int len);
bool str_equals(String *s1, String *s2);
bool str_chr_equals(String *s1, char *s2);

typedef struct Token Token;

typedef enum {
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
  TK_SHORT,
  TK_LONG,
  TK_UNSIGNED,
  TK_CONST,
  TK_ELLIPSIS,
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

struct Token {
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

typedef enum {
  INT,
  SHORT,
  LONG,
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

struct StructField {
  StructField *next;
  String *name;
  Type *type;
  int offset;
};

struct StructType {
  StructType *next;
  String *name;
  StructField *fields;
  int alignment;
  int size;
  bool is_union;
};

struct Type {
  TypeKind ty;
  Type *ptr_to;
  int array_size;
  StructType *struct_type;
  int is_unsigned;
  int is_const;
};

Type *new_type(TypeKind ty, Type *ptr_to);
Type *new_struct_type(String *name, bool is_union);
void add_field(StructType *type, Type *ty, String *name);
int sizeof_type(Type *type);

// initializer helpers
void store_int_to_bytes(long val, char *dest, int sz) {
  for (int b = 0; b < sz; b++)
    dest[b] = (char)((val >> (8 * b)) & 0xff);
}

void parse_initializer(Type *type, char *out, int out_size);

typedef enum {
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
  ND_COND,         // ? :
  ND_ADDR,         // *
  ND_DEREF,        // &
  ND_BITAND,       // & (bitwise)
  ND_BITOR,        // |
  ND_BITXOR,       // ^
  ND_SHL,          // <<
  ND_SHR,          // >>
  ND_BNOT,         // ~ (bitwise not)
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

typedef enum { DIR_INCLUDE } Directive;

typedef struct Node Node;

int eval_const_expr(Node *n, long *out);

struct Node {
  NodeKind kind;
  Node *lhs;
  Node *rhs;
  int val;      // ND_NUM
  int offset;   // ND_LVAR
  Type *type;   // expr
  String *name; // ND_CALL
};

typedef struct StringLiteral StringLiteral;
struct StringLiteral {
  StringLiteral *next;
  String *str;
  int offset;
};

extern char *user_input;
extern char *dir_name;
extern char *filename;
extern int current_stack_size;
extern int arg_count;

typedef enum {
  EXT_FUNC,
  EXT_FUNCDECL,
  EXT_GVAR,
  EXT_ENUM,
  EXT_STRUCT,
  EXT_TYPEDEF,
} ExternalKind;

typedef struct External External;
struct External {
  ExternalKind kind;
  String *name;
  Node *code;
  int *offsets;  // dynamically allocated for all parameters
  int num_params;  // total number of parameters
  int size;
  StringLiteral *literals;
  int stack_size;
  bool is_variadic;
  bool is_extern;
  // initializer bytes for globals
  char *init_bytes;
  int has_init;
};

bool at_eof();
External *external();
typedef struct Macro Macro;

Token *preprocess(Token *tok);
Type *new_type(TypeKind ty, Type *ptr_to) {
  Type *type = calloc(1, sizeof(Type));
  type->ty = ty;
  type->ptr_to = ptr_to;
  type->is_unsigned = 0;

  return type;
}

Type *new_struct_type(String *name, bool is_union) {
  Type *ty = new_type(STRUCT, NULL);
  ty->struct_type = calloc(1, sizeof(StructType));
  ty->struct_type->name = name;
  ty->struct_type->is_union = is_union;
  return ty;
}

int align(int n, int al) { return (n + al - 1) / al * al; }

void add_field(StructType *type, Type *ty, String *name) {
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

  if (type->is_union) {
    if (sizeof_type(ty) > type->size)
      type->size = sizeof_type(ty);
  } else {
    if (type->fields) {
      field->offset =
          align(type->fields->offset + sizeof_type(type->fields->type), al);
      type->size =
          align(field->offset + sizeof_type(field->type), type->alignment);
    } else {
      field->offset = 0;
      type->size = align(sizeof_type(field->type), type->alignment);
    }
  }

  field->next = type->fields;
  type->fields = field;
}

int sizeof_type(Type *type) {
  switch (type->ty) {
  case INT:
    return 4;
  case SHORT:
    return 2;
  case LONG:
    return 8;
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

char *user_input;
char *filename;
char *dir_name;
int current_stack_size;
int arg_count;

String *new_string(char *ptr, int len) {
  String *substr = calloc(1, sizeof(String));
  substr->ptr = ptr;
  substr->len = len;
  return substr;
}

bool str_equals(String *s1, String *s2) {
  return s1->len == s2->len && !memcmp(s1->ptr, s2->ptr, s1->len);
}

bool str_chr_equals(String *s1, char *s2) {
  return s1->len == strlen(s2) && !memcmp(s1->ptr, s2, s1->len);
}

// --- Neue Strukturen für Linked Lists ---

typedef struct FileList FileList;
struct FileList {
  char *filename;
  FileList *next;
};

// Macro Struktur erweitert um next-Pointer
struct Macro {
  String *ident;
  Token *replace;
  Macro *next; // Verkettung
};

// Globale Listen-Anker (statt Arrays)
FileList *once_files_list;
Macro *macros_list;

// --- Hilfsfunktionen für Linked Lists ---

Macro *find_macro(String *str) {
  for (Macro *m = macros_list; m; m = m->next) {
    if (str_equals(str, m->ident))
      return m;
  }
  return NULL;
}

Token *remove_newline(Token *tok) {
  Token *before = NULL;
  Token *head = tok;
  for (Token *t = head; t; t = t->next) {
    if (t->kind == TK_NEWLINE) {
      if (before)
        before->next = t->next;
      else
        head = t->next;
    } else {
      before = t;
    }
  }
  return head;
}

// --- Hauptfunktion Preprocess ---

Token *preprocess(Token *tok) {
  Token *before = NULL;
  Token *start = NULL;

  for (Token *t = tok; t; t = t->next) {
    if (t->kind == TK_PREPROCESSOR) {

      // 1. #include Logik
      if (startswith(t->str->ptr, "#include")) {
        char *curfile = filename;
        t = t->next;
        assert(t->kind == TK_STRING);

        char *inc_name = strndup(t->str->ptr, t->str->len);
        char *path = calloc(1, 512);
        snprintf(path, 512, "%s/%s", dir_name, inc_name);

        char *header = read_file(path);
        Token *header_token = tokenize(header, false);
        header_token = preprocess(header_token);

        filename = curfile;

        if (header_token) {
          Token *header_token_end = header_token;
          while (header_token_end->next)
            header_token_end = header_token_end->next;

          if (before)
            before->next = header_token;
          else
            start = header_token;
          before = header_token_end;
        }
        continue;
      }

      // 2. #pragma once Logik (jetzt mit FileList)
      if (startswith(t->str->ptr, "#pragma")) {
        t = t->next;
        if (str_chr_equals(t->str, "once")) {
          for (FileList *fl = once_files_list; fl; fl = fl->next) {
            if (strcmp(fl->filename, filename) == 0)
              return NULL; // Datei bereits inkludiert
          }
          // Neue Datei zur Liste hinzufügen
          FileList *new_f = calloc(1, sizeof(FileList));
          new_f->filename = filename;
          new_f->next = once_files_list;
          once_files_list = new_f;
        }
        continue;
      }

      // 3. #define Logik (jetzt mit Macro-Verkettung)
      if (startswith(t->str->ptr, "#define")) {
        t = t->next;
        assert(t->kind == TK_IDENT);

        Macro *m = calloc(1, sizeof(Macro));
        m->ident = t->str;

        t = t->next;
        m->replace = t;

        while (t->next && t->next->kind != TK_NEWLINE)
          t = t->next;

        Token *last = t;
        t = t->next;
        last->next = NULL;

        // Am Kopf der Liste einfügen (O(1))
        m->next = macros_list;
        macros_list = m;
        continue;
      }

      // (Hier können weitere Direktiven wie #ifdef folgen...)

    } else {
      // Identifier Handling (Makro-Expansion)
      if (t->kind == TK_IDENT) {
        Macro *m = find_macro(t->str);
        if (!m) {
          if (before)
            before->next = t;
          else
            start = t;
          before = t;
          continue;
        }

        Token *repl = m->replace;
        while (repl) {
          Token *newt = calloc(1, sizeof(Token));
          memcpy(newt, repl, sizeof(Token));
          newt->next = NULL;
          if (before)
            before->next = newt;
          else
            start = newt;
          before = newt;
          repl = repl->next;
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

  return remove_newline(start);
}

typedef struct LVar LVar;
typedef struct GVar GVar;
typedef struct EnumVal EnumVal;
typedef struct ConsumeTypeRes ConsumeTypeRes;
typedef struct TypeDef TypeDef;
typedef struct Enum Enum;
typedef struct Func Func;

struct LVar {
  LVar *next;
  String *name;
  int offset;
  Type *type;
};

struct GVar {
  GVar *next;
  String *name;
  Type *type;
};

struct EnumVal {
  EnumVal *next;
  String *name;
  int val;
};

struct ConsumeTypeRes {
  Type *type;
  Token *tok;
};

struct TypeDef {
  TypeDef *next;
  String *name;
  Type *type;
};

struct Func {
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

LVar *find_lvar(Token *tok) {
  for (LVar *var = locals; var; var = var->next)
    if (str_equals(var->name, tok->str))
      return var;

  return NULL;
}

LVar *new_lvar(String *name, Type *type) {
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

EnumVal *find_enum_val(Token *tok) {
  for (EnumVal *val = enumVals; val; val = val->next)
    if (str_equals(val->name, tok->str))
      return val;

  return NULL;
}

EnumVal *new_enum_val(String *name, int val) {
  EnumVal *enumVal = calloc(1, sizeof(EnumVal));
  enumVal->next = enumVals;
  enumVal->name = name;
  enumVal->val = val;

  enumVals = enumVal;
  return enumVal;
}

StructType *find_struct(Token *tok) {
  for (StructType *type = structs; type; type = type->next)
    if (str_equals(type->name, tok->str))
      return type;

  return NULL;
}

StructField *find_struct_field(Token *tok, StructType *type) {
  for (StructField *field = type->fields; field; field = field->next)
    if (str_equals(field->name, tok->str))
      return field;

  return NULL;
}

void new_gvar(String *name, Type *type) {
  GVar *gvar = calloc(1, sizeof(GVar));
  gvar->next = globals;
  gvar->name = name;
  gvar->type = type;

  globals = gvar;
}

GVar *find_gvar(Token *tok) {
  for (GVar *var = globals; var; var = var->next)
    if (str_equals(var->name, tok->str))
      return var;

  return NULL;
}

TypeDef *find_typedef(Token *tok) {
  for (TypeDef *type = typedefs; type; type = type->next)
    if (str_equals(type->name, tok->str))
      return type;

  return NULL;
}

TypeDef *new_typedef(String *name, Type *type) {
  TypeDef *typedef_ = calloc(1, sizeof(TypeDef));
  typedef_->next = typedefs;
  typedef_->name = name;
  typedef_->type = type;

  typedefs = typedef_;
  return typedef_;
}

Func *find_func(Token *tok) {
  for (Func *func = funcs; func; func = func->next)
    if (str_equals(func->name, tok->str))
      return func;

  return NULL;
}

Func *new_func(String *name, Type *type) {
  Func *func = calloc(1, sizeof(Func));
  func->next = funcs;
  func->name = name;
  func->type = type;

  funcs = func;
  return func;
}

bool consume(char *op) {
  if (token->kind != TK_RESERVED || !str_chr_equals(token->str, op))
    return false;

  token = token->next;
  return true;
}

Token *consume_kind(TokenKind kind) {
  if (token->kind == kind) {
    Token *tok = token;
    token = token->next;
    return tok;
  }
  return NULL;
}

Token *consume_ident() {
  if (token->kind != TK_IDENT)
    return NULL;

  Token *res = token;
  token = token->next;
  return res;
}

bool check_kind(TokenKind kind) { return token->kind == kind; }

void go_to(Token *tok) { token = tok; }

void expect(char *op) {
  if (token->kind != TK_RESERVED || !str_chr_equals(token->str, op))
    error_at_here("token mismatch: expected %s", op);

  token = token->next;
}

void expect_kind(TokenKind kind) {
  if (token->kind != kind)
    error_at_here("token mismatch");

  token = token->next;
}

int expect_number() {
  if (token->kind != TK_NUM)
    error_at_here("token mismatch: expected number");

  int val = token->val;
  token = token->next;
  return val;
}

bool at_eof() { return token->kind == TK_EOF; }

ConsumeTypeRes *consume_type();
Type *consume_type_name() {
  bool is_unsigned = false;
  bool is_const = false;
  if (consume_kind(TK_CONST))
    is_const = true;
  if (consume_kind(TK_UNSIGNED))
    is_unsigned = true;

  if (consume_kind(TK_INT)) {
    Type *t = new_type(INT, NULL);
    t->is_unsigned = is_unsigned;
    t->is_const = is_const;
    return t;
  }
  if (consume_kind(TK_SHORT)) {
    Type *t = new_type(SHORT, NULL);
    t->is_unsigned = is_unsigned;
    t->is_const = is_const;
    return t;
  }
  if (consume_kind(TK_LONG)) {
    Type *t = new_type(LONG, NULL);
    t->is_unsigned = is_unsigned;
    t->is_const = is_const;
    return t;
  }
  if (consume_kind(TK_CHAR)) {
    Type *t = new_type(CHAR, NULL);
    t->is_unsigned = is_unsigned;
    t->is_const = is_const;
    return t;
  }
  if (consume_kind(TK_VOID))
    return new_type(VOID, NULL);
  if (consume_kind(TK_BOOL))
    return new_type(BOOL, NULL);
  if (consume_kind(TK_BUILTIN_VA_LIST)) {
    Type *type = new_type(ARRAY, new_type(VA_LIST_TAG, NULL));
    type->array_size = 1;
    return type;
  }
  if (consume_kind(TK_ENUM)) {
    consume_kind(TK_IDENT);
    if (consume("{")) {
      int i = 0;
      while (!consume("}")) {
        Token *name = consume_ident();
        if (!name)
          error_at_here("expected enum name");

        new_enum_val(name->str, i++);

        consume(",");
      }
    }
    {
      Type *t = new_type(INT, NULL);
      t->is_unsigned = is_unsigned;
      t->is_const = is_const;
      return t;
    }
  }

  if (check_kind(TK_STRUCT) || check_kind(TK_UNION)) {
    bool is_union = false;
    if (consume_kind(TK_UNION))
      is_union = true;
    else
      consume_kind(TK_STRUCT);

    Token *id = consume_kind(TK_IDENT);

    if (consume("{")) {
      String *name = NULL;
      if (id)
        name = id->str;
      Type *type = new_struct_type(name, is_union);

      while (!consume("}")) {
        ConsumeTypeRes *res = consume_type();
        if (!res)
          error_at_here("expected type");

        add_field(type->struct_type, res->type, res->tok->str);
        expect(";");
      }

      if (id) {
        StructType *s = find_struct(id);
        if (s) {
          if (s->fields)
            error_at_here("struct %.*s is already defined", id->str->len,
                          id->str->ptr);
          else {
            s->fields = type->struct_type->fields;
            s->is_union = type->struct_type->is_union;
            s->alignment = type->struct_type->alignment;
            s->size = type->struct_type->size;
          }
        } else {
          type->struct_type->next = structs;
          structs = type->struct_type;
        }
      }

      type->is_unsigned = is_unsigned;
      return type;
    } else {
      if (!id)
        error_at_here("expected struct name");
      StructType *type = find_struct(id);
      Type *ty;
      if (!type) {
        // forward declaration
        ty = new_struct_type(id->str, is_union);
        ty->struct_type->next = structs;
        structs = ty->struct_type;
      } else {
        ty = new_type(STRUCT, NULL);
        ty->struct_type = type;
      }
      ty->is_unsigned = is_unsigned;
      return ty;
    }
  }

  Token *tok = consume_kind(TK_IDENT);
  if (tok) {
    TypeDef *tdef = find_typedef(tok);
    if (tdef) {
      if (is_unsigned) {
        Type *copy = calloc(1, sizeof(Type));
        *copy = *(tdef->type);
        copy->is_unsigned = 1;
        return copy;
      }
      return tdef->type;
    }
    go_to(tok);
  }

  return NULL;
}

Node *expr();
Type *consume_noident_type();
ConsumeTypeRes *expect_nested_type(Type *type) {
  int ptr = 0;
  ConsumeTypeRes *res;

  if (consume("(")) {
    while (consume("*"))
      ptr++;
    res = expect_nested_type(type);
    expect(")");
  } else {
    Token *tok = consume_ident();
    if (!tok)
      error_at_here("expected identifier");

    res = calloc(1, sizeof(ConsumeTypeRes));
    res->type = type;
    res->tok = tok;
  }
  if (consume("(")) {
    if (!check_kind(TK_ELLIPSIS) && !check_kind(TK_VOID)) {
      consume_noident_type();
      consume_ident();
    }
    while (consume(",")) {
      if (consume_kind(TK_ELLIPSIS))
        break;
      Type *param_type = consume_noident_type();
      if (param_type)
        consume_ident();
    }

    expect(")");

    res->type = new_type(FUNC, type);
  }
  for (int i = 0; i < ptr; i++)
    res->type = new_type(PTR, res->type);

  while (consume("[")) {
    Node *size = expr();
    if (size->kind != ND_NUM)
      error_at_here("expected constant expression");
    expect("]");
    res->type = new_type(ARRAY, type);
    res->type->array_size = size->val;
  }

  return res;
}

ConsumeTypeRes *consume_type() {
  Type *type = consume_type_name();
  if (!type)
    return NULL;
  while (consume("*"))
    type = new_type(PTR, type);
  ConsumeTypeRes *res = expect_nested_type(type);
  return res;
}

Type *expect_noident_type(Type *type) {
  while (consume("*"))
    type = new_type(PTR, type);

  if (consume("(")) {
    type = expect_noident_type(type);
    expect(")");
  }

  if (consume("(")) {
    if (!check_kind(TK_ELLIPSIS)) {
      consume_noident_type();
      while (consume(",")) {
        if (consume_kind(TK_ELLIPSIS))
          break;
        consume_noident_type();
      }
    } else {
      consume_kind(TK_ELLIPSIS);
    }
    expect(")");
    type = new_type(FUNC, NULL);
  }

  while (consume("[")) {
    Node *size = expr();
    if (size->kind != ND_NUM)
      error_at_here("expected constant expression");
    expect("]");
    type = new_type(ARRAY, type);
    type->array_size = size->val;
  }

  return type;
}

Type *consume_noident_type() {
  Type *type = consume_type_name();
  if (!type)
    return NULL;

  return expect_noident_type(type);
}

void next() { token = token->next; }

Node *new_node(NodeKind kind, Node *lhs, Node *rhs) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  node->lhs = lhs;
  node->rhs = rhs;

  switch (kind) {
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
    case ND_BITAND:
    case ND_BITOR:
    case ND_BITXOR:
    case ND_SHL:
    case ND_SHR:
    case ND_BNOT:
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

Node *new_typed_node(NodeKind kind, Node *lhs, Node *rhs, Type *type) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  node->lhs = lhs;
  node->rhs = rhs;
  node->type = type;
  return node;
}

Node *new_node_num(int val) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_NUM;
  node->val = val;
  Type *type = calloc(1, sizeof(Type));
  type->ty = INT;
  node->type = type;
  return node;
}

Node *new_node_char(char val) {
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
Node *conditional();
Node *logical();
Node *bitwise_and();
Node *bitwise_xor();
Node *bitwise_or();
Node *equality();
Node *relational();
Node *shift();
Node *add();
Node *mul();
Node *unary();
Node *postfix();
Node *primary();

External *ext;
int literal_count;

External *external() {
  locals = NULL;
  if (!globals)
    new_gvar(new_string("NULL", 4), new_type(PTR, new_type(VOID, NULL)));

  External *external = calloc(1, sizeof(External));
  ext = external;
  int i = 0;

  if (consume_kind(TK_EXTERN))
    ext->is_extern = true;

  if (check_kind(TK_ENUM)) {
    ext->kind = EXT_ENUM;

    consume_type_name();
    expect(";");
    return external;
  }

  if (check_kind(TK_STRUCT) || check_kind(TK_UNION)) {
    ext->kind = EXT_STRUCT;
    Type *ty = consume_type_name();
    if (!ty)
      error_at_here("expected struct or union");
    expect(";");
    return external;
  }

  if (consume_kind(TK_TYPEDEF)) {
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
  if (res->type->ty == FUNC) {
    external->kind = EXT_FUNC;

    if (!find_func(res->tok))
      new_func(res->tok->str, res->type->ptr_to);

    go_to(res->tok);
    next();
    expect("(");

    Token *cur = token;
    bool no_args = false;
    if (consume_kind(TK_VOID)) {
      if (consume(")"))
        no_args = true;
      else
        go_to(cur);
    }
    if (!consume(")") && !no_args) {
      for (;;) {
        if (consume_kind(TK_ELLIPSIS)) {
          external->is_variadic = true;
          break;
        }
        ConsumeTypeRes *res = consume_type();
        if (!res)
          error_at_here("failed to parse argument");

        LVar *lvar = find_lvar(res->tok);
        if (!lvar)
          lvar = new_lvar(res->tok->str, res->type);

        // Allocate more space if needed (grow by 8 parameters at a time)
        if (i >= external->num_params) {
          int new_capacity = ((i / 8) + 1) * 8;
          int *new_offsets = calloc(new_capacity, sizeof(int));
          if (external->offsets) {
            for (int j = 0; j < external->num_params; j++)
              new_offsets[j] = external->offsets[j];
          }
          external->offsets = new_offsets;
          external->num_params = new_capacity;
        }

        external->offsets[i++] = lvar->offset;

        if (!consume(","))
          break;
      }

      expect(")");
    }

    if (consume(";")) {
      external->kind = EXT_FUNCDECL;
      return external;
    }

    external->code = stmt();
    if (external->code->kind != ND_BLOCK)
      error_at_here("expected block");
  } else {
    external->kind = EXT_GVAR;

    new_gvar(res->tok->str, res->type);
    external->size = sizeof_type(res->type);

    // Optional initializer for global variable
    if (consume("=")) {
      // allocate init bytes and zero
      external->init_bytes = calloc(1, external->size);
      external->has_init = 1;

      // String literal initializer
      Token *strtok = consume_kind(TK_STRING);
      if (strtok) {
        int copy = strtok->str->len;
        if (copy > external->size) copy = external->size;
        memcpy(external->init_bytes, strtok->str->ptr, copy);
      } else if (consume("{")) {
        // brace initializer: fill sequentially
        int idx = 0;
        while (!consume("}")) {
          if (idx >= external->size) {
            // consume but ignore extra
            expr();
          } else {
            Node *n = expr();
            long val;
            if (!eval_const_expr(n, &val))
              error_at_here("initializer must be constant");
            // store little-endian
            for (int b = 0; b < sizeof(int) && idx < external->size; b++) {
              external->init_bytes[idx++] = (char)((val >> (8*b)) & 0xff);
            }
          }
          consume(",");
        }
      } else {
        // single expression initializer
        Node *n = expr();
        long val;
        if (!eval_const_expr(n, &val))
          error_at_here("initializer must be constant");
        // store into bytes according to size
        int sz = external->size;
        if (sz > 8) sz = 8;
        for (int b = 0; b < sz; b++)
          external->init_bytes[b] = (char)((val >> (8*b)) & 0xff);
      }

      fprintf(stderr, "AFTER_INIT token='%.*s' kind=%d\n", token->str->len, token->str->ptr, token->kind);
      fflush(stderr);
      expect(";");
    } else {
      expect(";");
    }
  }
  if (locals)
    external->stack_size = (locals->offset / 8) * 8 + 8;

  return external;
}

Node *stmt() {
  Node *node;

  if (consume_kind(TK_CASE)) {
    Node *e = expr();
    if (e->kind != ND_NUM)
      error_at_here("expected constant expression");

    expect(":");
    node = new_node(ND_CASE, e, NULL);
  } else if (consume_kind(TK_DEFAULT)) {
    expect(":");
    node = new_node(ND_DEFAULT, NULL, NULL);
  } else if (consume_kind(TK_IF)) {
    expect("(");
    Node *lhs = expr();
    expect(")");
    Node *rhs = stmt();

    if (consume_kind(TK_ELSE))
      node = new_node(ND_IFELSE, lhs, new_node(ND_UNNAMED, rhs, stmt()));
    else
      node = new_node(ND_IF, lhs, rhs);
  } else if (consume_kind(TK_SWITCH)) {
    expect("(");
    Node *lhs = expr();
    expect(")");
    Node *rhs = stmt();

    node = new_node(ND_SWITCH, lhs, rhs);
  } else if (consume_kind(TK_WHILE)) {
    expect("(");
    Node *lhs = expr();
    expect(")");
    node = new_node(ND_WHILE, lhs, stmt());
  } else if (consume_kind(TK_FOR)) {
    Node *a = NULL;
    Node *b = NULL;
    Node *c = NULL;
    Node *d = NULL;
    expect("(");
    if (!consume(";")) {
      a = expr();
      expect(";");
    }
    if (!consume(";")) {
      b = expr();
      expect(";");
    }
    if (!consume(")")) {
      c = expr();
      expect(")");
    }
    d = stmt();

    Node *lhs = new_node(ND_UNNAMED, a, b);
    Node *rhs = new_node(ND_UNNAMED, c, d);

    node = new_node(ND_FOR, lhs, rhs);
  } else if (consume("{")) {
    node = new_node(ND_BLOCK, NULL, NULL);

    for (Node *last = node; !consume("}"); last = last->rhs)
      last->rhs = new_node(ND_UNNAMED, stmt(), NULL);
  } else if (consume_kind(TK_RETURN)) {
    if (consume(";"))
      node = new_node(ND_RETURN, NULL, NULL);
    else {
      node = new_node(ND_RETURN, expr(), NULL);
      expect(";");
    }
  } else if (consume_kind(TK_BREAK)) {
    node = new_node(ND_BREAK, NULL, NULL);
    expect(";");
  } else if (consume_kind(TK_CONTINUE)) {
    node = new_node(ND_CONTINUE, NULL, NULL);
    expect(";");
  } else {
    node = expr();
    expect(";");
  }

  return node;
}

Node *expr() { return assign(); }

Node *assign() {
  Node *node = conditional();
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
  else if (consume("&="))
    node = new_node(ND_ASSIGN, node, new_node(ND_BITAND, node, assign()));
  else if (consume("|="))
    node = new_node(ND_ASSIGN, node, new_node(ND_BITOR, node, assign()));
  else if (consume("^="))
    node = new_node(ND_ASSIGN, node, new_node(ND_BITXOR, node, assign()));
  else if (consume("<<="))
    node = new_node(ND_ASSIGN, node, new_node(ND_SHL, node, assign()));
  else if (consume(">>="))
    node = new_node(ND_ASSIGN, node, new_node(ND_SHR, node, assign()));

  return node;
}

Node *conditional() {
  Node *node = logical();
  
  if (consume("?")) {
    Node *cond_node = calloc(1, sizeof(Node));
    cond_node->kind = ND_COND;
    cond_node->lhs = node;  // condition
    cond_node->rhs = calloc(1, sizeof(Node));
    cond_node->rhs->lhs = expr();  // true expression
    expect(":");
    cond_node->rhs->rhs = conditional();  // false expression
    node = cond_node;
  }
  
  return node;
}

Node *bitwise_and() {
  Node *node = equality();
  for (;;) {
    if (consume("&"))
      node = new_node(ND_BITAND, node, equality());
    else
      return node;
  }
}

Node *bitwise_xor() {
  Node *node = bitwise_and();
  for (;;) {
    if (consume("^"))
      node = new_node(ND_BITXOR, node, bitwise_and());
    else
      return node;
  }
}

Node *bitwise_or() {
  Node *node = bitwise_xor();
  for (;;) {
    if (consume("|"))
      node = new_node(ND_BITOR, node, bitwise_xor());
    else
      return node;
  }
}

Node *logical() {
  Node *node = bitwise_or();

  for (;;) {
    if (consume("&&"))
      node = new_node(ND_AND, node, bitwise_or());
    else if (consume("||"))
      node = new_node(ND_OR, node, bitwise_or());
    else
      return node;
  }
}

Node *equality() {
  Node *node = relational();

  for (;;) {
    if (consume("=="))
      node = new_node(ND_EQ, node, relational());
    else if (consume("!="))
      node = new_node(ND_NE, node, relational());
    else
      return node;
  }
}

Node *relational() {
  Node *node = shift();

  for (;;) {
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

Node *add() {
  Node *node = mul();

  for (;;) {
    if ((node->type->ty == PTR || node->type->ty == ARRAY) &&
        node->type->ptr_to->ty != VOID) {
      int size = sizeof_type(node->type->ptr_to);

      if (consume("+")) {
        if (node->type->ty == ARRAY)
          node = new_typed_node(ND_ADDR, node, NULL,
                                new_type(PTR, node->type->ptr_to));
        node =
            new_node(ND_ADD, node, new_node(ND_MUL, mul(), new_node_num(size)));
      } else if (consume("-")) {
        if (node->type->ty == ARRAY)
          node = new_typed_node(ND_ADDR, node, NULL,
                                new_type(PTR, node->type->ptr_to));
        node =
            new_node(ND_SUB, node, new_node(ND_MUL, mul(), new_node_num(size)));
      }
      return node;
    }

    if (consume("+")) {
      Node *rhs = mul();
      if (node->kind == ND_NUM && rhs->kind == ND_NUM)
        node->val += rhs->val;
      else
        node = new_node(ND_ADD, node, rhs);
    } else if (consume("-")) {
      Node *rhs = mul();
      if (node->kind == ND_NUM && rhs->kind == ND_NUM)
        node->val -= rhs->val;
      else
        node = new_node(ND_SUB, node, rhs);
    } else
      return node;
  }
}

Node *shift() {
  Node *node = add();
  for (;;) {
    if (consume("<<"))
      node = new_node(ND_SHL, node, add());
    else if (consume(">>"))
      node = new_node(ND_SHR, node, add());
    else
      return node;
  }
}

Node *mul() {
  Node *node = unary();

  for (;;) {
    if (consume("*")) {
      Node *rhs = unary();
      if (node->kind == ND_NUM && rhs->kind == ND_NUM)
        node->val *= rhs->val;
      else
        node = new_node(ND_MUL, node, rhs);
    } else if (consume("/")) {
      Node *rhs = unary();
      if (node->kind == ND_NUM && rhs->kind == ND_NUM)
        node->val /= rhs->val;
      else
        node = new_node(ND_DIV, node, rhs);
    } else
      return node;
  }
}

Node *unary() {
  if (consume("+"))
    return postfix();
  else if (consume("-")) {
    Node *rhs = postfix();
    if (rhs->kind == ND_NUM) {
      rhs->val = -rhs->val;
      return rhs;
    } else
      return new_node(ND_SUB, new_node_num(0), rhs);
  } else if (consume("&"))
    return new_node(ND_ADDR, postfix(), NULL);
  else if (consume("*")) {
    Node *l = primary();
    if (l->type->ty != PTR && l->type->ty != ARRAY)
      error_at_here("dereference failed: not a pointer");

    if (l->type->ty == ARRAY)
      l = new_node(ND_ADDR, l, NULL);

    return new_node(ND_DEREF, l, NULL);
  } else if (consume_kind(TK_SIZEOF)) {
    Token *cur = token;
    if (consume("(")) {
      Type *type = consume_noident_type();
      if (type) {
        expect(")");
        return new_node_num(sizeof_type(type));
      }
      go_to(cur);
    }

    Node *n = unary();
    return new_node_num(sizeof_type(n->type));
  } else if (consume("!"))
    return new_node(ND_EQ, postfix(), new_node_num(0));
  else if (consume("~")) {
    Node *n = postfix();
    return new_node(ND_BNOT, n, NULL);
  }
  else if (consume("++")) {
    Node *n = postfix();
    return new_node(ND_ASSIGN, n, new_node(ND_ADD, n, new_node_num(1)));
  } else if (consume("--")) {
    Node *n = postfix();
    return new_node(ND_ASSIGN, n, new_node(ND_SUB, n, new_node_num(1)));
  }

  return postfix();
}

Node *postfix() {
  Node *node = primary();

  for (;;) {
    if (consume("[")) {
      Node *subscript = expr();
      int size = sizeof_type(node->type->ptr_to);
      if (node->type->ty == ARRAY)
        node = new_typed_node(ND_ADDR, node, NULL,
                              new_type(PTR, node->type->ptr_to));
      node = new_node(ND_DEREF,
                      new_node(ND_ADD, node,
                               new_node(ND_MUL, subscript, new_node_num(size))),
                      NULL);
      expect("]");
      continue;
    }
    if (consume(".")) {
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
    if (consume("->")) {
      Token *tok = consume_ident();
      if (!tok)
        error_at_token(tok, "expected identifier after '->'");

      if (node->type->ty != PTR)
        error_at_token(tok, "expected pointer type");

      if (node->type->ptr_to->ty != STRUCT)
        error_at_token(tok, "expected struct type");

      StructField *field =
          find_struct_field(tok, node->type->ptr_to->struct_type);
      if (!field)
        error_at_here("no such field");

      node = new_node(ND_ADD, node, new_node_num(field->offset));
      node->type = new_type(PTR, field->type);
      node = new_node(ND_DEREF, node, NULL);
      continue;
    }
    if (consume("++")) {
      // TODO: pointer
      node = new_node(ND_POST_INCR, node, new_node_num(1));
      continue;
    }
    if (consume("--")) {
      node = new_node(ND_POST_DECR, node, new_node_num(1));
      continue;
    }

    break;
  }
  return node;
}

Node *primary() {
  if (consume("(")) {
    Node *node = expr();
    expect(")");
    return node;
  }

  Token *tok = consume_kind(TK_STRING);
  if (tok) {
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
  if (res) {
    LVar *lvar = find_lvar(res->tok);
    if (!lvar)
      lvar = new_lvar(res->tok->str, res->type);

    Node *node = new_typed_node(ND_LVAR, NULL, NULL, lvar->type);
    node->offset = lvar->offset;

    if (consume("=")) {
      if (consume("{")) {
        if (lvar->type->ty == ARRAY)
          node = handle_local_array_init(node, lvar);
        else if (lvar->type->ty == STRUCT)
          node = handle_local_struct_init(node, lvar);
        else
          error_at_token(tok, "type mismatch");
      } else
        node = new_node(ND_ASSIGN, node, assign());
    }

    return node;
  }

  tok = consume_ident();
  if (tok) {
    Node *node;

    if (consume("(")) {
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
      for (;;) {
        last->rhs = new_node(ND_UNNAMED, expr(), NULL);
        last = last->rhs;

        if (consume(")"))
          break;
        else
          expect(",");
      }
    } else {
      LVar *lvar = find_lvar(tok);
      if (lvar) {
        node = new_typed_node(ND_LVAR, NULL, NULL, lvar->type);
        node->offset = lvar->offset;
      } else {
        EnumVal *val = find_enum_val(tok);
        if (val)
          node = new_node_num(val->val);
        else {
          GVar *gvar = find_gvar(tok);
          if (gvar) {
            node = new_typed_node(ND_GVAR, NULL, NULL, gvar->type);
            node->name = tok->str;
          } else
            error_at_token(tok, "%.*s is not defined", tok->str->len,
                           tok->str->ptr);
        }
      }
    }

    return node;
  }

  tok = consume_kind(TK_CHAR_CONST);
  if (tok) {
    char c = *(tok->str->ptr);
    if (c == '\\') {
      switch (*(tok->str->ptr + 1)) {
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

Token *token;

Token *new_token(TokenKind kind, Token *cur, char *str, int len) {
  Token *tok = calloc(1, sizeof(Token));
  tok->kind = kind;
  tok->str = new_string(str, len);
  cur->next = tok;
  return tok;
}

bool is_alnum(char c) {
  return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') ||
         ('0' <= c && c <= '9') || (c == '_');
}

bool startswith(char *p, char *q) { return memcmp(p, q, strlen(q)) == 0; }

char *skip_comments(char *p) {
  if (startswith(p, "//")) {
    p += 2;
    while (*p != '\n')
      p++;
    return p;
  }
  if (strncmp(p, "/*", 2) == 0) {
    char *q = strstr(p + 2, "*/");
    if (!q)
      error_at(p, "tokenize failed: \"*/\" not found");
    return q + 2;
  }
  return p;
}

Token *tokenize(char *p, bool eof) {
  Token head;
  head.next = NULL;
  Token *cur = &head;

  while (*p) {
    if (*p == '\n') {
      cur = new_token(TK_NEWLINE, cur, p++, 1);
      continue;
    }
    if (isspace(*p)) {
      p++;
      continue;
    }

    if (startswith(p, "#")) {
      char *q = p;
      while (!isspace(*q))
        q++;
      cur = new_token(TK_PREPROCESSOR, cur, p, q - p);
      p = q;
      continue;
    }

    if (startswith(p, "==") || startswith(p, "!=") || startswith(p, "<=") ||
        startswith(p, ">=")) {
      cur = new_token(TK_RESERVED, cur, p, 2);
      p += 2;
      continue;
    }
    if (startswith(p, "||") || startswith(p, "&&")) {
      cur = new_token(TK_RESERVED, cur, p, 2);
      p += 2;
      continue;
    }

    if (startswith(p, "++") || startswith(p, "--")) {
      cur = new_token(TK_RESERVED, cur, p, 2);
      p += 2;
      continue;
    }

    if (startswith(p, "+=") || startswith(p, "-=") || startswith(p, "*=") ||
        startswith(p, "/=")) {
      cur = new_token(TK_RESERVED, cur, p, 2);
      p += 2;
      continue;
    }
    if (startswith(p, "&=") || startswith(p, "|=") || startswith(p, "^=")) {
      cur = new_token(TK_RESERVED, cur, p, 2);
      p += 2;
      continue;
    }
    if (startswith(p, "<<=") || startswith(p, ">>=")) {
      cur = new_token(TK_RESERVED, cur, p, 3);
      p += 3;
      continue;
    }
    if (startswith(p, "<<") || startswith(p, ">>")) {
      cur = new_token(TK_RESERVED, cur, p, 2);
      p += 2;
      continue;
    }

    if (startswith(p, "//") || strncmp(p, "/*", 2) == 0) {
      p = skip_comments(p);
      continue;
    }

    if (strncmp(p, "->", 2) == 0) {
      cur = new_token(TK_RESERVED, cur, p, 2);
      p += 2;
      continue;
    }

    if (strncmp(p, "...", 3) == 0) {
      cur = new_token(TK_ELLIPSIS, cur, p, 3);
      p += 3;
      continue;
    }

    if (strchr("+-*/()<>:;={},&[].!|^~", *p)) {
      cur = new_token(TK_RESERVED, cur, p++, 1);
      continue;
    }

    if (*p == '"') {
      cur = new_token(TK_STRING, cur, ++p, 0);
      for (; *(p - 1) == '\\' || (*p != '"'); p++)
        cur->str->len++;
      p++;
      continue;
    }

    if (*p == '\'') {
      cur = new_token(TK_CHAR_CONST, cur, ++p, 0);
      for (; (*(p - 2) != '\\' && *(p - 1) == '\\') || *p != '\''; p++)
        cur->str->len++;
      p++;
      continue;
    }

    if (isdigit(*p)) {
      char *newp;
      int base = 10;
      if (p[0] == '0' && (p[1] == 'x' || p[1] == 'X'))
        base = 0; // let strtol detect 0x (use base 0)
      long val = strtol(p, &newp, base);
      int len = newp - p;
      cur = new_token(TK_NUM, cur, p, len);
      cur->val = (int)val;
      p = newp;
      continue;
    }

    if (strncmp(p, "return", 6) == 0 && !is_alnum(p[6])) {
      cur = new_token(TK_RETURN, cur, p, 6);
      p += 6;
      continue;
    }

    if (strncmp(p, "if", 2) == 0 && !is_alnum(p[2])) {
      cur = new_token(TK_IF, cur, p, 2);
      p += 2;
      continue;
    }

    if (strncmp(p, "else", 4) == 0 && !is_alnum(p[4])) {
      cur = new_token(TK_ELSE, cur, p, 4);
      p += 4;
      continue;
    }

    if (strncmp(p, "while", 5) == 0 && !is_alnum(p[5])) {
      cur = new_token(TK_WHILE, cur, p, 5);
      p += 5;
      continue;
    }

    if (strncmp(p, "for", 3) == 0 && !is_alnum(p[3])) {
      cur = new_token(TK_FOR, cur, p, 3);
      p += 3;
      continue;
    }

    if (strncmp(p, "int", 3) == 0 && !is_alnum(p[3])) {
      cur = new_token(TK_INT, cur, p, 3);
      p += 3;
      continue;
    }

    if (strncmp(p, "unsigned", 8) == 0 && !is_alnum(p[8])) {
      cur = new_token(TK_UNSIGNED, cur, p, 8);
      p += 8;
      continue;
    }

    if (strncmp(p, "short", 5) == 0 && !is_alnum(p[5])) {
      cur = new_token(TK_SHORT, cur, p, 5);
      p += 5;
      continue;
    }

    if (strncmp(p, "long", 4) == 0 && !is_alnum(p[4])) {
      cur = new_token(TK_LONG, cur, p, 4);
      p += 4;
      continue;
    }

    if (strncmp(p, "const", 5) == 0 && !is_alnum(p[5])) {
      cur = new_token(TK_CONST, cur, p, 5);
      p += 5;
      continue;
    }

    if (strncmp(p, "sizeof", 6) == 0 && !is_alnum(p[6])) {
      cur = new_token(TK_SIZEOF, cur, p, 6);
      p += 6;
      continue;
    }

    if (strncmp(p, "char", 4) == 0 && !is_alnum(p[4])) {
      cur = new_token(TK_CHAR, cur, p, 4);
      p += 4;
      continue;
    }

    if (strncmp(p, "enum", 4) == 0 && !is_alnum(p[4])) {
      cur = new_token(TK_ENUM, cur, p, 4);
      p += 4;
      continue;
    }

    if (strncmp(p, "struct", 6) == 0 && !is_alnum(p[6])) {
      cur = new_token(TK_STRUCT, cur, p, 6);
      p += 6;
      continue;
    }

    if (strncmp(p, "union", 5) == 0 && !is_alnum(p[5])) {
      cur = new_token(TK_UNION, cur, p, 5);
      p += 5;
      continue;
    }

    if (strncmp(p, "typedef", 7) == 0 && !is_alnum(p[7])) {
      cur = new_token(TK_TYPEDEF, cur, p, 7);
      p += 7;
      continue;
    }

    if (strncmp(p, "switch", 6) == 0 && !is_alnum(p[6])) {
      cur = new_token(TK_SWITCH, cur, p, 6);
      p += 6;
      continue;
    }

    if (strncmp(p, "case", 4) == 0 && !is_alnum(p[4])) {
      cur = new_token(TK_CASE, cur, p, 4);
      p += 4;
      continue;
    }
    if (strncmp(p, "default", 7) == 0 && !is_alnum(p[7])) {
      cur = new_token(TK_DEFAULT, cur, p, 7);
      p += 7;
      continue;
    }

    if (strncmp(p, "break", 5) == 0 && !is_alnum(p[5])) {
      cur = new_token(TK_BREAK, cur, p, 5);
      p += 5;
      continue;
    }

    if (strncmp(p, "continue", 8) == 0 && !is_alnum(p[8])) {
      cur = new_token(TK_CONTINUE, cur, p, 8);
      p += 8;
      continue;
    }

    if (strncmp(p, "void", 4) == 0 && !is_alnum(p[4])) {
      cur = new_token(TK_VOID, cur, p, 4);
      p += 4;
      continue;
    }

    if (strncmp(p, "__builtin_va_list", 17) == 0 && !is_alnum(p[17])) {
      cur = new_token(TK_BUILTIN_VA_LIST, cur, p, 17);
      p += 17;
      continue;
    }

    if (strncmp(p, "_Bool", 5) == 0 && !is_alnum(p[5])) {
      cur = new_token(TK_BOOL, cur, p, 5);
      p += 5;
      continue;
    }

    if (strncmp(p, "extern", 6) == 0 && !is_alnum(p[6])) {
      cur = new_token(TK_EXTERN, cur, p, 6);
      p += 6;
      continue;
    }

    if (('a' <= *p && *p <= 'z') || ('A' <= *p && *p <= 'Z') || (*p == '_')) {
      char *q;
      for (q = p; ('a' <= *q && *q <= 'z') || ('A' <= *q && *q <= 'Z') ||
                  ('0' <= *q && *q <= '9') || (*q == '_');
           q++)
        ;

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

int label_count;
int switch_count;
int max_switch_count;
int break_count;
int max_break_count;
int continue_count;
int max_continue_count;

void gen(Node *node);

// Load value from address in %rax to %rax
void load(Type *type) {
  int sz = sizeof_type(type);
  if (sz >= 8) {
    printf("  movq (%%rax), %%rax\n");
  } else if (sz == 4) {
    if (type->is_unsigned)
      printf("  movl (%%rax), %%eax\n");
    else
      printf("  movslq (%%rax), %%rax\n");
  } else if (sz == 2) {
    if (type->is_unsigned)
      printf("  movzwl (%%rax), %%eax\n");
    else
      printf("  movswl (%%rax), %%eax\n");
  } else if (sz == 1) {
    if (type->is_unsigned)
      printf("  movzbl (%%rax), %%eax\n");
    else
      printf("  movsbq (%%rax), %%rax\n");
  }
}

// Store value in %r11 to address in %rax
void store(Type *type) {
  int sz = sizeof_type(type);
  if (sz == 8) printf("  movq %%r11, (%%rax)\n");
  else if (sz == 4) printf("  movl %%r11d, (%%rax)\n");
  else if (sz == 2) printf("  movw %%r11w, (%%rax)\n");
  else if (sz == 1) printf("  movb %%r11b, (%%rax)\n");
}

// Generate value of expression into %rax (no push). This avoids push/pop
// for many binary ops by using registers (%rax, %rcx, %rdi).
void gen_rvalue(Node *node) {
  int a, b;
  if (!node) return;
  // Simple temp register allocator for two temps: %%r10 and %%r11
  static int temp_used_r10 = 0;
  static int temp_used_r11 = 0;
  char *alloc_temp() {
    if (!temp_used_r10) {
      temp_used_r10 = 1;
      return "%r10";
    }
    if (!temp_used_r11) {
      temp_used_r11 = 1;
      return "%r11";
    }
    // fallback to %r11 if both busy (rare)
    return "%r11";
  }


  void free_temp(char *r) {
    if (r == NULL) return;
    if (strcmp(r, "%r10") == 0)
      temp_used_r10 = 0;
    else if (strcmp(r, "%r11") == 0)
      temp_used_r11 = 0;
  }
  // Helper to evaluate constant expressions at compile time.
  int eval_const(Node *n, long *out) {
    if (!n) return 0;
    long lv, rv;
    switch (n->kind) {
    case ND_NUM:
      *out = n->val;
      return 1;
    case ND_BNOT:
      if (eval_const(n->lhs, &lv)) { *out = ~lv; return 1; }
      return 0;
    case ND_ADD: if (eval_const(n->lhs,&lv) && eval_const(n->rhs,&rv)) { *out = lv+rv; return 1; } return 0;
    case ND_SUB: if (eval_const(n->lhs,&lv) && eval_const(n->rhs,&rv)) { *out = lv-rv; return 1; } return 0;
    case ND_MUL: if (eval_const(n->lhs,&lv) && eval_const(n->rhs,&rv)) { *out = lv*rv; return 1; } return 0;
    case ND_DIV: if (eval_const(n->lhs,&lv) && eval_const(n->rhs,&rv) && rv!=0) { *out = lv/rv; return 1; } return 0;
    case ND_SHL: if (eval_const(n->lhs,&lv) && eval_const(n->rhs,&rv)) { *out = lv<<rv; return 1; } return 0;
    case ND_SHR: if (eval_const(n->lhs,&lv) && eval_const(n->rhs,&rv)) { *out = lv>>rv; return 1; } return 0;
    case ND_BITAND: if (eval_const(n->lhs,&lv) && eval_const(n->rhs,&rv)) { *out = lv&rv; return 1; } return 0;
    case ND_BITOR: if (eval_const(n->lhs,&lv) && eval_const(n->rhs,&rv)) { *out = lv|rv; return 1; } return 0;
    case ND_BITXOR: if (eval_const(n->lhs,&lv) && eval_const(n->rhs,&rv)) { *out = lv^rv; return 1; } return 0;
    default: return 0;
    }
  }

  switch (node->kind) {
  case ND_NUM:
    printf("  movq $%d, %%rax\n", node->val);
    return;
  case ND_BNOT:
    gen_rvalue(node->lhs);
    printf("  notq %%rax\n");
    return;
  case ND_LVAR:
    // compute address into %rax then load
    printf("  movq %%rbp, %%rax\n");
    printf("  subq $%d, %%rax\n", node->offset);
    load(node->type);
    return;
  case ND_GVAR:
    printf("  leaq %.*s(%%rip), %%rax\n", node->name->len, node->name->ptr);
    load(node->type);
    return;
  case ND_STRING:
    printf("  leaq .LC%d(%%rip), %%rax\n", node->offset);
    return;
  case ND_SHL:
  case ND_SHR:
  case ND_ADD:
  case ND_SUB:
  case ND_MUL:
  case ND_DIV:
  case ND_BITAND:
  case ND_BITOR:
  case ND_BITXOR: {
    // Constant folding: if both sides are numbers, compute at compile time
    if (node->lhs && node->rhs && node->lhs->kind == ND_NUM && node->rhs->kind == ND_NUM) {
      long lv = node->lhs->val;
      long rv = node->rhs->val;
      long res = 0;
      switch (node->kind) {
      case ND_SHL: res = lv << rv; break;
      case ND_SHR: res = lv >> rv; break;
      case ND_ADD: res = lv + rv; break;
      case ND_SUB: res = lv - rv; break;
      case ND_MUL: res = lv * rv; break;
      case ND_DIV: res = lv / rv; break;
      case ND_BITAND: res = lv & rv; break;
      case ND_BITOR: res = lv | rv; break;
      case ND_BITXOR: res = lv ^ rv; break;
      default: break;
      }
      printf("  movq $%ld, %%rax\n", res);
      return;
    }

    // General case: compute rhs into %rax, save into a temp reg, compute lhs into %rax; operate with temp
    gen_rvalue(node->rhs);
    char *temp = alloc_temp();
    printf("  movq %%rax, %s\n", temp);
    gen_rvalue(node->lhs);

    switch (node->kind) {
    case ND_ADD: printf("  addq %s, %%rax\n", temp); break;
    case ND_SUB: printf("  subq %s, %%rax\n", temp); break;
    case ND_MUL: printf("  imulq %s, %%rax\n", temp); break;
    case ND_DIV: printf("  movq %s, %%rdi\n", temp); printf("  cqto\n"); printf("  idivq %%rdi\n"); break;
    case ND_BITAND: printf("  andq %s, %%rax\n", temp); break;
    case ND_BITOR: printf("  orq %s, %%rax\n", temp); break;
    case ND_BITXOR: printf("  xorq %s, %%rax\n", temp); break;
    case ND_SHL: printf("  movq %s, %%rcx\n", temp); printf("  salq %%cl, %%rax\n"); break;
    case ND_SHR:
      printf("  movq %s, %%rcx\n", temp);
      if (node->type && node->type->is_unsigned)
        printf("  shrq %%cl, %%rax\n");
      else
        printf("  sarq %%cl, %%rax\n");
      break;
    default: break;
    }

    free_temp(temp);
    return;
  }
  default:
    // Fallback: use stack-based generator
    gen(node);
    printf("  popq %%rax\n");
    return;
  }
}

void gen_string_literal(StringLiteral *literals) {
  for (StringLiteral *l = literals; l; l = l->next) {
    printf(".data\n");
    printf(".LC%d:\n", l->offset);
    printf("  .string \"%.*s\"\n", l->str->len, l->str->ptr);
  }
}

// Evaluate constant expression tree. Returns 1 if constant, fills out.
int eval_const_expr(Node *n, long *out) {
  if (!n) return 0;
  long lv, rv;
  switch (n->kind) {
  case ND_NUM:
    *out = n->val;
    return 1;
  case ND_BNOT:
    if (eval_const_expr(n->lhs, &lv)) { *out = ~lv; return 1; }
    return 0;
  case ND_ADD: if (eval_const_expr(n->lhs,&lv) && eval_const_expr(n->rhs,&rv)) { *out = lv+rv; return 1; } return 0;
  case ND_SUB: if (eval_const_expr(n->lhs,&lv) && eval_const_expr(n->rhs,&rv)) { *out = lv-rv; return 1; } return 0;
  case ND_MUL: if (eval_const_expr(n->lhs,&lv) && eval_const_expr(n->rhs,&rv)) { *out = lv*rv; return 1; } return 0;
  case ND_DIV: if (eval_const_expr(n->lhs,&lv) && eval_const_expr(n->rhs,&rv) && rv!=0) { *out = lv/rv; return 1; } return 0;
  case ND_SHL: if (eval_const_expr(n->lhs,&lv) && eval_const_expr(n->rhs,&rv)) { *out = lv<<rv; return 1; } return 0;
  case ND_SHR: if (eval_const_expr(n->lhs,&lv) && eval_const_expr(n->rhs,&rv)) { *out = lv>>rv; return 1; } return 0;
  case ND_BITAND: if (eval_const_expr(n->lhs,&lv) && eval_const_expr(n->rhs,&rv)) { *out = lv&rv; return 1; } return 0;
  case ND_BITOR: if (eval_const_expr(n->lhs,&lv) && eval_const_expr(n->rhs,&rv)) { *out = lv|rv; return 1; } return 0;
  case ND_BITXOR: if (eval_const_expr(n->lhs,&lv) && eval_const_expr(n->rhs,&rv)) { *out = lv^rv; return 1; } return 0;
  default: return 0;
  }
}

// Recursive initializer parser. It consumes tokens and writes into `out` up to out_size.
void parse_initializer(Type *type, char *out, int out_size) {
  // out is pre-zeroed by caller
  if (!type) return;
  fprintf(stderr, "PARSE_INIT ENTER type=%d token='%.*s'\n", type->ty, token->str->len, token->str->ptr);
  fflush(stderr);

  // String literal into char array
  if (type->ty == ARRAY && type->ptr_to && type->ptr_to->ty == CHAR && token->kind == TK_STRING) {
    Token *t = consume_kind(TK_STRING);
    int copy = t->str->len;
    if (copy > out_size) copy = out_size;
    memcpy(out, t->str->ptr, copy);
    return;
  }

  if (consume("{")) {
    if (type->ty == ARRAY) {
      int elem_size = sizeof_type(type->ptr_to);
      for (int i = 0; i < type->array_size; i++) {
        if (consume("}")) break;
        parse_initializer(type->ptr_to, out + i * elem_size, elem_size);
        consume(",");
      }
      // consume trailing '}' if not already
      if (!check_kind(TK_RESERVED) || !str_chr_equals(token->str, "}"))
        ;
      else
        expect("}");
      return;
    } else if (type->ty == STRUCT) {
      // collect fields into array to iterate in declaration order
      int nf = 0;
      for (StructField *ff = type->struct_type->fields; ff; ff = ff->next)
        nf++;
      StructField **arr = calloc(nf, sizeof(StructField *));
      int i = 0;
      for (StructField *ff = type->struct_type->fields; ff; ff = ff->next)
        arr[i++] = ff;
      // fields are stored in reverse order, so iterate from end
      for (int idx = nf - 1; idx >= 0; idx--) {
        if (consume("}")) break;
        parse_initializer(arr[idx]->type, out + arr[idx]->offset, sizeof_type(arr[idx]->type));
        consume(",");
      }
      if (!check_kind(TK_RESERVED) || !str_chr_equals(token->str, "}"))
        expect("}");
      free(arr);
      return;
    } else {
      // For scalar types inside braces
      int idx = 0;
      while (!consume("}")) {
        if (idx >= out_size) {
          expr();
        } else {
          Node *n = expr();
          long val;
          if (!eval_const_expr(n, &val))
            error_at_here("initializer must be constant");
          int sz = out_size;
          if (sz > 8) sz = 8;
          store_int_to_bytes(val, out + idx, sz);
          idx += sz;
        }
        consume(",");
      }
      return;
    }
  }

  // Not a brace. Expect expression or string literal handled earlier.
  if (token->kind == TK_STRING && type->ty == ARRAY && type->ptr_to && type->ptr_to->ty == CHAR) {
    Token *t = consume_kind(TK_STRING);
    int copy = t->str->len;
    if (copy > out_size) copy = out_size;
    memcpy(out, t->str->ptr, copy);
    return;
  }

  // single expression
  Node *n = expr();
  long val;
  if (!eval_const_expr(n, &val))
    error_at_here("initializer must be constant");
  int sz = out_size;
  if (sz > 8) sz = 8;
  store_int_to_bytes(val, out, sz);
}

void gen_lval(Node *node) {
  if (node->kind == ND_DEREF)
    gen(node->lhs);
  else if (node->kind == ND_LVAR) {
    printf("  movq %%rbp, %%rax\n");
    printf("  subq $%d, %%rax\n", node->offset);
    printf("  pushq %%rax\n");
  } else if (node->kind == ND_GVAR) {
    printf("  leaq %.*s(%%rip), %%rax\n", node->name->len, node->name->ptr);
    printf("  pushq %%rax\n");
  } else if (node->kind == ND_STRING) {
    printf("  leaq .LC%d(%%rip), %%rax\n", node->offset);
    printf("  pushq %%rax\n");
  } else
    error("left value of assignment must be variable: found %d\n", node->kind);
}

// Compute the address of an lvalue into %rax (no push)
void gen_addr(Node *node) {
  if (node->kind == ND_DEREF) {
    gen_rvalue(node->lhs); // result in %rax
    return;
  }
  if (node->kind == ND_LVAR) {
    printf("  movq %%rbp, %%rax\n");
    printf("  subq $%d, %%rax\n", node->offset);
    return;
  }
  if (node->kind == ND_GVAR) {
    printf("  leaq %.*s(%%rip), %%rax\n", node->name->len, node->name->ptr);
    return;
  }
  if (node->kind == ND_STRING) {
    printf("  leaq .LC%d(%%rip), %%rax\n", node->offset);
    return;
  }
  error("left value of assignment must be variable: found %d\n", node->kind);
}

void gen_if(Node *node) {
  int l = label_count++;
  gen(node->lhs);
  printf("  popq %%rax\n");
  printf("  cmpq $0, %%rax\n");
  printf("  je .Lend%d\n", l);
  gen_stmt(node->rhs);
  printf(".Lend%d:\n", l);
}

void gen_ifelse(Node *node) {
  int l = label_count++;
  gen(node->lhs);
  printf("  popq %%rax\n");
  printf("  cmpq $0, %%rax\n");
  printf("  je .Lelse%d\n", l);
  gen_stmt(node->rhs->lhs);
  printf("  jmp .Lend%d\n", l);
  printf(".Lelse%d:\n", l);
  gen_stmt(node->rhs->rhs);
  printf(".Lend%d:\n", l);
}

void gen_while(Node *node) {
  int l = label_count++;
  int i = continue_count;
  continue_count = ++max_continue_count;
  int j = break_count;
  int b = break_count = ++max_break_count;

  printf(".Lbegin%d:\n", l);
  printf(".Lcontinue%d:\n", continue_count);
  gen(node->lhs);
  printf("  popq %%rax\n");
  printf("  cmpq $0, %%rax\n");
  printf("  je .Lend%d\n", l);
  gen_stmt(node->rhs);
  printf("  jmp .Lbegin%d\n", l);
  printf(".Lend%d:\n", l);
  printf(".Lbreak%d:\n", b);
  continue_count = i;
  break_count = j;
}

void gen_for(Node *node) {
  int l = label_count++;
  int i = continue_count;
  int j = continue_count = ++max_continue_count;
  int k = break_count;
  int b = break_count = ++max_break_count;

  if (node->lhs->lhs) gen_stmt(node->lhs->lhs);
  printf(".Lbegin%d:\n", l);
  if (node->lhs->rhs) {
    gen(node->lhs->rhs);
    printf("  popq %%rax\n");
    printf("  cmpq $0, %%rax\n");
    printf("  je .Lend%d\n", l);
  }
  gen_stmt(node->rhs->rhs);
  printf(".Lcontinue%d:\n", j);
  if (node->rhs->lhs) gen_stmt(node->rhs->lhs);
  printf("  jmp .Lbegin%d\n", l);
  printf(".Lend%d:\n", l);
  printf(".Lbreak%d:\n", b);
  continue_count = i;
  break_count = k;
}

void gen_switch(Node *node) {
  assert(node->rhs->kind == ND_BLOCK);
  gen(node->lhs);
  printf("  popq %%rax\n");

  int i = break_count;
  int b = break_count = ++max_break_count;

  int j = switch_count;
  switch_count = ++max_switch_count;

  for (Node *n = node->rhs; n; n = n->rhs) {
    if (n->lhs && n->lhs->kind == ND_CASE) {
      printf("  cmpq $%d, %%rax\n", n->lhs->lhs->val);
      printf("  je .Lcase%d_%d\n", switch_count, n->lhs->lhs->val);
    }
    if (n->lhs && n->lhs->kind == ND_DEFAULT)
      printf("  jmp .Ldefault%d\n", switch_count);
  }

  gen_stmt(node->rhs);
  printf(".Lbreak%d:\n", b);
  break_count = i;
  switch_count = j;
}

void gen_return(Node *node) {
  if (node->lhs)
    gen(node->lhs);
  else
    printf("  pushq $0\n");
  printf("  popq %%rax\n");
  printf("  movq %%rbp, %%rsp\n");
  printf("  popq %%rbp\n");
  printf("  ret\n");
}

void gen_stmt(Node *node) {
  Node *n;
  int l;
  int i;
  int b;
  int j;
  switch (node->kind) {
  case ND_BLOCK:
    for (n = node->rhs; n; n = n->rhs)
      gen_stmt(n->lhs);
    return;
  case ND_IF:
    gen_if(node);
    return;
  case ND_IFELSE:
    gen_ifelse(node);
    return;
  case ND_SWITCH:
    gen_switch(node);
    return;
  case ND_CASE:
    assert(node->lhs->kind == ND_NUM);
    printf(".Lcase%d_%d:\n", switch_count, node->lhs->val);
    return;
  case ND_DEFAULT:
    printf(".Ldefault%d:\n", switch_count);
    return;
  case ND_WHILE:
    gen_while(node);
    return;
  case ND_FOR:
    gen_for(node);
    return;
  case ND_RETURN:
    gen_return(node);
    return;
  case ND_BREAK:
    printf("  jmp .Lbreak%d\n", break_count);
    return;
  case ND_CONTINUE:
    printf("  jmp .Lcontinue%d\n", continue_count);
    return;
  default:
    gen(node);
    printf("  popq %%rax\n");
  }
}

void gen_function(External *ext) {
  char *regs1[6] = {"%dil", "%sil", "%dl", "%cl", "%r8b", "%r9b"};
  char *regs4[6] = {"%edi", "%esi", "%edx", "%ecx", "%r8d", "%r9d"};
  char *regs2[6] = {"%di", "%si", "%dx", "%cx", "%r8w", "%r9w"};
  char *regs8[6] = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};

  gen_string_literal(ext->literals);

  printf(".globl %.*s\n", ext->name->len, ext->name->ptr);
  printf(".text\n");
  printf("%.*s:\n", ext->name->len, ext->name->ptr);

  printf("  pushq %%rbp\n");
  printf("  movq %%rsp, %%rbp\n");
  printf("  subq $%d, %%rsp\n", ext->stack_size);

  if (ext->is_variadic)
    for (int i = 5; i >= 0; i--)
      printf("  pushq %s\n", regs8[i]);

  int i = 0;
  int ri = 0;
  while (i < 6 && ext->offsets && ext->offsets[i]) {
    int size;
    if (i == 0) {
      size = ext->offsets[i];
    } else {
      size = ext->offsets[i] - ext->offsets[i - 1];
    }
    int offset = 0;

    for (; size != offset; ri++) {
      if (size - offset >= 8) {
        printf("  movq %s, -%d(%%rbp)\n", regs8[ri], ext->offsets[i] - offset);
        offset += 8;
      } else if (size - offset == 4) {
        printf("  movl %s, -%d(%%rbp)\n", regs4[ri], ext->offsets[i] - offset);
        offset += 4;
      } else if (size - offset == 2) {
        printf("  movw %s, -%d(%%rbp)\n", regs2[ri], ext->offsets[i] - offset);
        offset += 2;
      } else if (size - offset == 1) {
        printf("  movb %s, -%d(%%rbp)\n", regs1[ri], ext->offsets[i] - offset);
        offset += 1;
      } else
        error("invalid size");
    }
    i++;
  }

  arg_count = i;
  current_stack_size = ext->stack_size;

  gen_stmt(ext->code);

  printf("  movq %%rbp, %%rsp\n");
  printf("  popq %%rbp\n");
  printf("  ret\n");
}

void gen_assign(Node *node) {
  // Compute RHS into %rax and save it in %r11
  gen_rvalue(node->rhs);
  printf("  movq %%rax, %%r11\n");
  // Compute LHS address into %rax
  gen_addr(node->lhs);

  int total = sizeof_type(node->lhs->type);
  if (total == 1 || total == 2 || total == 4 || total == 8) {
    store(node->lhs->type);
    // Reload value into rax for result of assignment expression
    if (total == 8) printf("  movq %%r11, %%rax\n");
    else if (total == 4) {
      if (node->lhs->type->is_unsigned) printf("  movl %%r11d, %%eax\n");
      else printf("  movl %%r11d, %%eax\n  movslq %%eax, %%rax\n");
    } else if (total == 2) {
      if (node->lhs->type->is_unsigned) printf("  movzwl %%r11w, %%eax\n");
      else printf("  movswl %%r11w, %%eax\n  movslq %%eax, %%rax\n");
    } else if (total == 1) {
      if (node->lhs->type->is_unsigned) printf("  movzbl %%r11b, %%eax\n");
      else printf("  movsbq %%r11b, %%rax\n");
    }
    printf("  pushq %%rax\n");
    return;
  }

  // Fallback for aggregates: keep existing stack-based logic
  gen_lval(node->lhs);
  gen(node->rhs);
  printf("  popq %%rdi\n");
  printf("  popq %%rax\n");

  {
    int tmp = total;
    int chunks[32];
    int nc = 0;
    while (tmp > 0) {
      if (tmp >= 8) {
        chunks[nc++] = 8;
        tmp -= 8;
      } else if (tmp == 4) {
        chunks[nc++] = 4;
        tmp -= 4;
      } else if (tmp == 2) {
        chunks[nc++] = 2;
        tmp -= 2;
      } else {
        chunks[nc++] = 1;
        tmp -= 1;
      }
    }

    printf("  movq %d(%%rsp), %%rax\n", 8 * nc);

    int off = total;
    for (int i = nc - 1; i >= 0; i--) {
      int sz = chunks[i];
      off -= sz;
      printf("  popq %%r10\n");
      if (sz == 8)
        printf("  movq %%r10, %d(%%rax)\n", off);
      else if (sz == 4)
        printf("  movl %%r10d, %d(%%rax)\n", off);
      else if (sz == 2)
        printf("  movw %%r10w, %d(%%rax)\n", off);
      else
        printf("  movb %%r10b, %d(%%rax)\n", off);
    }

    printf("  popq %%rax\n");
    printf("  pushq $0\n");
    return;
  }
}

void gen_call(Node *node) {
  Node *n = node->rhs;
  int i = 0;

  while (n) {
    gen(n->lhs);
    if (sizeof_type(n->lhs->type) > 32)
      error("not implemented: too big object");
    i += (sizeof_type(n->lhs->type) + 7) / 8;
    n = n->rhs;
  }
  // Parameters 1-6 go in registers, 7+ go on the stack
  // No error for too many arguments - they're just passed on stack

  char *regs[6] = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};

  // Pop arguments in reverse order; first 6 go to registers
  for (int j = (i < 6 ? i : 6) - 1; j >= 0; j--)
    printf("  popq %s\n", regs[j]);
  
  // Arguments 7+ remain on the stack (already in correct position)

  // Align stack to 16 bytes before call. i is total 8-byte words pushed.
  {
    int bytes = i * 8;
    int adj = (16 - (bytes % 16)) % 16;
    if (adj)
      printf("  subq $%d, %%rsp\n", adj);
    printf("  movq $0, %%rax\n");
    printf("  call %.*s\n", node->lhs->name->len, node->lhs->name->ptr);
    if (adj)
      printf("  addq $%d, %%rsp\n", adj);
  }

  if (node->type->ty == VOID) {
    printf("  pushq $0\n");
    return;
  }

  switch (sizeof_type(node->type)) {
  case 1:
    if (node->type->is_unsigned) {
      printf("  andq $0xff, %%rax\n");
      printf("  pushq %%rax\n");
    } else {
      printf("  movsbq %%al, %%rax\n");
      printf("  pushq %%rax\n");
    }
    break;
  case 4:
    if (node->type->is_unsigned) {
      printf("  andq $0xffffffff, %%rax\n");
      printf("  pushq %%rax\n");
    } else {
      printf("  movslq %%eax, %%rax\n");
      printf("  pushq %%rax\n");
    }
    break;
  case 2:
    if (node->type->is_unsigned) {
      printf("  andq $0xffff, %%rax\n");
      printf("  pushq %%rax\n");
    } else {
      printf("  movswq %%ax, %%rax\n");
      printf("  pushq %%rax\n");
    }
    break;
  case 8:
    printf("  pushq %%rax\n");
    break;
  default:
    error("not implemented: return value");
  }
}

void gen(Node *node) {
  int l;
  int i;
  Node *n;

  switch (node->kind) {
  case ND_NUM:
    printf("  pushq $%d\n", node->val);
    return;
  case ND_LVAR:
  case ND_GVAR:
    gen_lval(node);
    printf("  popq %%rax\n");
    int size = sizeof_type(node->type);
    for (;;) {
      if (size >= 8) {
        printf("  movq %d(%%rax), %%r10\n", sizeof_type(node->type) - size);
        printf("  pushq %%r10\n");
        size -= 8;
      } else if (size == 4) {
        if (node->type->is_unsigned) {
          printf("  movl (%%rax), %%eax\n");
          printf("  pushq %%rax\n");
        } else {
          printf("  movslq (%%rax), %%rax\n");
          printf("  pushq %%rax\n");
        }
        size -= 4;
      } else if (size == 2) {
        if (node->type->is_unsigned) {
          printf("  movzwl (%%rax), %%eax\n");
          printf("  pushq %%rax\n");
        } else {
          printf("  movswl (%%rax), %%eax\n");
          printf("  movslq %%eax, %%rax\n");
          printf("  pushq %%rax\n");
        }
        size -= 2;
      } else if (size == 1) {
        if (node->type->is_unsigned) {
          printf("  movzbl (%%rax), %%eax\n");
          printf("  pushq %%rax\n");
        } else {
          printf("  movsbq (%%rax), %%rax\n");
          printf("  pushq %%rax\n");
        }
        size -= 1;
      } else
        error("not implemented: size %d", size);

      if (size == 0)
        break;
    }

    return;
  case ND_STRING:
    gen_lval(node);
    return;
  case ND_ASSIGN:
    gen_assign(node);
    return;
  case ND_ASSIGN_ARRAY:
    for (n = node->rhs; n; n = n->rhs) {
      gen(n->lhs);
      printf("  popq %%rax\n");
    }
    printf("  pushq $0\n");
    return;
  case ND_POST_INCR:
    gen_lval(node->lhs);
    printf(" popq %%rax\n");

    switch (sizeof_type(node->lhs->type)) {
    case 8:
      printf("  movq (%%rax), %%rdi\n");
      printf("  pushq %%rdi\n");
      printf("  addq $1, %%rdi\n");
      printf("  movq %%rdi, (%%rax)\n");
      break;
    case 4:
      printf("  movl (%%rax), %%edi\n");
      printf("  pushq %%rdi\n");
      printf("  addl $1, %%edi\n");
      printf("  movl %%edi, (%%rax)\n");
      break;
    case 2:
      printf("  movw (%%rax), %%di\n");
      printf("  pushq %%rdi\n");
      printf("  addw $1, %%di\n");
      printf("  movw %%di, (%%rax)\n");
      break;
    case 1:
      printf("  movb (%%rax), %%dil\n");
      printf("  pushq %%rdi\n");
      printf("  addb $1, %%dil\n");
      printf("  movb %%dil, (%%rax)\n");
      break;
    }
    return;
  case ND_POST_DECR:
    gen_lval(node->lhs);
    printf(" popq %%rax\n");

    switch (sizeof_type(node->lhs->type)) {
    case 8:
      printf("  movq (%%rax), %%rdi\n");
      printf("  pushq %%rdi\n");
      printf("  subq $1, %%rdi\n");
      printf("  movq %%rdi, (%%rax)\n");
      break;
    case 4:
      printf("  movl (%%rax), %%edi\n");
      printf("  pushq %%rdi\n");
      printf("  subl $1, %%edi\n");
      printf("  movl %%edi, (%%rax)\n");
      break;
    case 2:
      printf("  movw (%%rax), %%di\n");
      printf("  pushq %%rdi\n");
      printf("  subw $1, %%di\n");
      printf("  movw %%di, (%%rax)\n");
      break;
    case 1:
      printf("  movb (%%rax), %%dil\n");
      printf("  pushq %%rdi\n");
      printf("  subb $1, %%dil\n");
      printf("  movb %%dil, (%%rax)\n");
      break;
    }
    return;

  case ND_CALL:
    gen_call(node);
    return;
  case ND_VA_START:
    n = node->rhs->lhs;
    assert(n->kind == ND_LVAR);

    printf("  movl $%d, %%eax\n", arg_count * 8);
    printf("  movl %%eax, -%d(%%rbp)\n", n->offset);
    printf("  movl $48, %%eax\n");
    printf("  movl %%eax, -%d(%%rbp)\n", n->offset - 4);
    printf("  leaq 16(%%rbp), %%rax\n");
    printf("  movq %%rax, -%d(%%rbp)\n", n->offset - 8);
    printf("  leaq -%d(%%rbp), %%rax\n", current_stack_size + 48);
    printf("  movq %%rax, -%d(%%rbp)\n", n->offset - 16);

    printf("  pushq $0\n");
    return;
  case ND_ADDR:
    gen_lval(node->lhs);
    return;
  case ND_DEREF:
    gen(node->lhs);
    printf("  popq %%rax\n");
    {
      int total = sizeof_type(node->type);
      if (total > 8 || node->type->ty == STRUCT || node->type->ty == ARRAY) {
        int off = 0;
        while (off < total) {
          int rem = total - off;
          if (rem >= 8) {
            printf("  movq %d(%%rax), %%r10\n", off);
            printf("  pushq %%r10\n");
            off += 8;
          } else if (rem >= 4) {
            printf("  movl %d(%%rax), %%r10d\n", off);
            printf("  pushq %%r10\n");
            off += 4;
          } else if (rem >= 2) {
            printf("  movzwl %d(%%rax), %%r10d\n", off);
            printf("  pushq %%r10\n");
            off += 2;
          } else {
            printf("  movzbl %d(%%rax), %%r10d\n", off);
            printf("  pushq %%r10\n");
            off += 1;
          }
        }
        return;
      }

      int sz = total;
      if (sz == 8) {
        printf("  movq (%%rax), %%rax\n");
        printf("  pushq %%rax\n");
        return;
      }
      if (sz == 4) {
        if (node->type->is_unsigned)
          printf("  movl (%%rax), %%eax\n");
        else
          printf("  movslq (%%rax), %%rax\n");
        printf("  pushq %%rax\n");
        return;
      }
      if (sz == 2) {
        if (node->type->is_unsigned)
          printf("  movzwl (%%rax), %%eax\n");
        else {
          printf("  movswl (%%rax), %%eax\n");
          printf("  movslq %%eax, %%rax\n");
        }
        printf("  pushq %%rax\n");
        return;
      }
      if (sz == 1) {
        if (node->type->is_unsigned)
          printf("  movzbl (%%rax), %%eax\n");
        else
          printf("  movsbq (%%rax), %%rax\n");
        printf("  pushq %%rax\n");
        return;
      }

      error("unexpected type");
    }
  case ND_AND:
    if (node->lhs && node->rhs && node->lhs->kind == ND_NUM && node->rhs->kind == ND_NUM) {
      int res = (node->lhs->val != 0 && node->rhs->val != 0);
      printf("  pushq $%d\n", res);
      return;
    }

    l = label_count++;
    gen(node->lhs);
    printf("  popq %%rax\n");
    printf("  cmpq $0, %%rax\n");
    printf("  je .Lfalse%d\n", l);
    gen(node->rhs);
    printf("  popq %%rax\n");
    printf("  cmpq $0, %%rax\n");
    printf("  je .Lfalse%d\n", l);
    printf("  pushq $1\n");
    printf("  jmp .Lend%d\n", l);
    printf(".Lfalse%d:\n", l);
    printf("  pushq $0\n");
    printf(".Lend%d:\n", l);
    return;
  case ND_OR:
    if (node->lhs && node->rhs && node->lhs->kind == ND_NUM && node->rhs->kind == ND_NUM) {
      int res = (node->lhs->val != 0 || node->rhs->val != 0);
      printf("  pushq $%d\n", res);
      return;
    }

    l = label_count++;
    gen(node->lhs);
    printf("  popq %%rax\n");
    printf("  cmpq $0, %%rax\n");
    printf("  jne .Ltrue%d\n", l);
    gen(node->rhs);
    printf("  popq %%rax\n");
    printf("  cmpq $0, %%rax\n");
    printf("  jne .Ltrue%d\n", l);
    printf("  pushq $0\n");
    printf("  jmp .Lend%d\n", l);
    printf(".Ltrue%d:\n", l);
    printf("  pushq $1\n");
    printf(".Lend%d:\n", l);
    return;
  case ND_COND: {
    int l = label_count++;
    gen(node->lhs);
    printf("  popq %%rax\n");
    printf("  cmpq $0, %%rax\n");
    printf("  je .Lelse%d\n", l);
    gen(node->rhs->lhs);
    printf("  jmp .Lend%d\n", l);
    printf(".Lelse%d:\n", l);
    gen(node->rhs->rhs);
    printf(".Lend%d:\n", l);
    return;
  }
  case ND_BNOT:
  case ND_BITAND:
  case ND_BITOR:
  case ND_BITXOR:
  case ND_SHL:
  case ND_SHR:
  case ND_ADD:
  case ND_SUB:
  case ND_MUL:
  case ND_DIV:
    gen_rvalue(node);
    printf("  pushq %%rax\n");
    return;
  default:
    gen(node->lhs);
    gen(node->rhs);

    printf("  popq %%rdi\n");
    printf("  popq %%rax\n");

    switch (node->kind) {
    case ND_ADD:
      printf("  addq %%rdi, %%rax\n");
      break;
    case ND_SUB:
      printf("  subq %%rdi, %%rax\n");
      break;
    case ND_MUL:
      printf("  imulq %%rdi, %%rax\n");
      break;
    case ND_DIV:
      printf("  cqto\n");
      printf("  idivq %%rdi\n");
      break;
    case ND_EQ:
      printf("  cmpq %%rdi, %%rax\n");
      printf("  sete %%al\n");
      printf("  movzbq %%al, %%rax\n");
      break;
    case ND_NE:
      printf("  cmpq %%rdi, %%rax\n");
      printf("  setne %%al\n");
      printf("  movzbq %%al, %%rax\n");
      break;
    case ND_LT:
      printf("  cmpq %%rdi, %%rax\n");
      printf("  setl %%al\n");
      printf("  movzbq %%al, %%rax\n");
      break;
    case ND_LE:
      printf("  cmpq %%rdi, %%rax\n");
      printf("  setle %%al\n");
      printf("  movzbq %%al, %%rax\n");
      break;
    default:
      error("unreachable: %d", node->kind);
      return;
    }

    printf("  pushq %%rax\n");
  }
}


void error(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  va_end(ap);
  fprintf(stderr, "\n");
  exit(1);
}

void _error_at(char *loc, char *fmt, va_list ap) {
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

  int pos = loc - line + indent;
  fprintf(stderr, "%*s", pos, "");
  fprintf(stderr, "^ ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

void error_at(char *loc, char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  _error_at(loc, fmt, ap);
  va_end(ap);
}

void error_at_token(Token *tok, char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  _error_at(tok->str->ptr, fmt, ap);
  va_end(ap);
}

void error_at_here(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  _error_at(token->str->ptr, fmt, ap);
  va_end(ap);
}

void assert(bool flag) {
  if (!flag)
    error_at_token(token, "assertion failed");
}

char *read_file(char *path) {
  FILE *fp = fopen(path, "r");
  if (!fp)
    error("cannot open %s", path);

  if (fseek(fp, 0, 2) == -1)
    error("%s: fseek", path);
  size_t size = ftell(fp);
  if (fseek(fp, 0, 0) == -1)
    error("%s: fseek", path);

  char *buf = calloc(1, size + 2);
  fread(buf, size, 1, fp);

  if (size == 0 || buf[size - 1] != '\n')
    buf[size++] = '\n';

  buf[size] = '\0';
  fclose(fp);
  return buf;
}

char *get_dir(char *path) {
  int i;
  for (i = strlen(path) - 1; i >= 0; i--) {
    if (path[i] == '/')
      break;
  }

  char *dir;
  if (i == -1) {
    dir = calloc(1, 2);
    strncpy(dir, ".", 1);
    return dir;
  }
  dir = calloc(1, i + 1);
  strncpy(dir, path, i);

  return dir;
}

int main(int argc, char **argv) {
  if (argc != 2) {
    error("invalid argument");
    return 1;
  }

  filename = argv[1];
  dir_name = get_dir(filename);

  user_input = read_file(filename);
  token = tokenize(user_input, true);
  token = preprocess(token);

  printf(".data\n");
  printf("NULL:\n");
  printf("  .zero 8\n");

  while (!at_eof()) {
    External *ext = external();

    switch (ext->kind) {
    case EXT_FUNC:
      gen_function(ext);
      break;
    case EXT_GVAR:
      if (!ext->is_extern) {
        printf(".globl %.*s\n", ext->name->len, ext->name->ptr);
        printf(".data\n");
        printf("%.*s:\n", ext->name->len, ext->name->ptr);
          if (ext->has_init && ext->init_bytes) {
            int i = 0;
            while (i < ext->size) {
              int rem = ext->size - i;
              if (rem >= 8 && (i % 8) == 0) {
                unsigned long long v = 0;
                for (int b = 0; b < 8; b++) v |= ((unsigned long long)(unsigned char)ext->init_bytes[i + b]) << (8*b);
                printf("  .quad %llu\n", v);
                i += 8;
              } else if (rem >= 4 && (i % 4) == 0) {
                unsigned int v = 0;
                for (int b = 0; b < 4; b++) v |= ((unsigned int)(unsigned char)ext->init_bytes[i + b]) << (8*b);
                printf("  .long %u\n", v);
                i += 4;
              } else if (rem >= 2 && (i % 2) == 0) {
                unsigned int v = 0;
                for (int b = 0; b < 2; b++) v |= ((unsigned int)(unsigned char)ext->init_bytes[i + b]) << (8*b);
                printf("  .word %u\n", v);
                i += 2;
              } else {
                unsigned int v = (unsigned char)ext->init_bytes[i];
                printf("  .byte %u\n", v);
                i += 1;
              }
            }
          } else {
            printf("  .zero %d\n", ext->size);
          }
      }
      break;
    default:
      break;
    }
  }

  // mark no-exec-stack to silence some linkers' warnings
  printf(".section .note.GNU-stack,\"\",@progbits\n");

  return 0;
}
