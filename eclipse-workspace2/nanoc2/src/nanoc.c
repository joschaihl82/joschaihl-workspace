/*
 * nanoc.c
 *
 * Teaching C compiler front-end and x86-64 code generator that emits
 * AT&T-syntax assembly. This file is an extended, self-contained single-file
 * compiler front-end (lexer, parser, semantic analysis) and a code generator
 * that prints AT&T-style x86-64 assembly (GAS / clang/ gcc compatible).
 *
 * Notes:
 * - AT&T syntax differences applied: registers prefixed with '%', immediates with '$',
 *   memory operands like -8(%rbp), instruction operand order is src, dest.
 * - push/pop use pushq/popq with register operands (e.g., pushq %rax).
 * - movq $imm, %rax for immediates; leaq -8(%rbp), %rax for addresses.
 *
 * This is educational code and not production-grade.
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdint.h>
#include <assert.h>

/* ----------------------------------------------------------------------
 * CONFIGURATION
 * ---------------------------------------------------------------------- */

#define MAX_TOKEN_LEN 512
#define MAX_AST_CHILDREN 8
#define MAX_FUNCTIONS 512
#define MAX_LOCALS 8192
#define MAX_ARGS 8
#define MAX_STRING_LITERALS 1024
#define ALIGN_TO(n, a) (((n) + ((a)-1)) & ~((a)-1))

/* ----------------------------------------------------------------------
 * ERROR HANDLING
 * ---------------------------------------------------------------------- */

static char *current_input = NULL;

void error_at(char *loc, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int pos = (int)(loc - current_input);
    fprintf(stderr, "\n--- COMPILE ERROR ---\n");
    fprintf(stderr, "%s\n", current_input);
    fprintf(stderr, "%*s^ ", pos, "");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n---------------------\n");
    va_end(ap);
    exit(1);
}

/* ----------------------------------------------------------------------
 * LEXER
 * ---------------------------------------------------------------------- */

typedef enum {
    TOK_EOF = 0,
    // Keywords
    TOK_INT, TOK_CHAR, TOK_LONG, TOK_UNSIGNED, TOK_RETURN, TOK_IF, TOK_ELSE,
    TOK_WHILE, TOK_FOR, TOK_STRUCT, TOK_VOID, TOK_BREAK, TOK_CONTINUE,
    // Literals and identifiers
    TOK_IDENT, TOK_NUMBER, TOK_CHAR_LITERAL, TOK_STRING_LITERAL,
    // Operators and punctuation
    TOK_SEMI, TOK_COMMA,
    TOK_LPAREN, TOK_RPAREN, TOK_LBRACE, TOK_RBRACE, TOK_LBRACKET, TOK_RBRACKET,
    TOK_PLUS, TOK_MINUS, TOK_STAR, TOK_SLASH, TOK_EQ, TOK_NEQ, TOK_ASSIGN,
    TOK_LT, TOK_LE, TOK_GT, TOK_GE, TOK_AMP, TOK_AND, TOK_OR, TOK_NOT,
    TOK_ARROW, // -> (not used yet)
    TOK_UNKNOWN
} TokenType;

typedef struct Token Token;
struct Token {
    TokenType type;
    char *str;      // for identifiers and string literals
    long val;       // for numeric and char literals
    char *loc;      // pointer into original input (for error reporting)
    Token *next;    // used during initial lexing
};

static Token *tokens = NULL; // token array pointer
static int token_idx = 0;    // index into tokens array

// String literal table
typedef struct {
    int id;
    char *s;
} StringLit;
static StringLit string_table[MAX_STRING_LITERALS];
static int string_table_count = 0;

static Token *tokenize(const char *p_in) {
    // Build linked list of tokens first
    current_input = (char *)p_in;
    Token head = {0};
    Token *cur = &head;
    const char *p = p_in;

    while (*p) {
        // Skip whitespace
        if (isspace((unsigned char)*p)) { p++; continue; }

        // Comments
        if (p[0] == '/' && p[1] == '/') {
            p += 2;
            while (*p && *p != '\n') p++;
            continue;
        }
        if (p[0] == '/' && p[1] == '*') {
            p += 2;
            while (*p && !(p[0] == '*' && p[1] == '/')) p++;
            if (*p) p += 2;
            continue;
        }

        // Numbers (decimal only for now)
        if (isdigit((unsigned char)*p)) {
            cur = cur->next = calloc(1, sizeof(Token));
            cur->type = TOK_NUMBER;
            cur->loc = (char *)p;
            char *end;
            cur->val = strtol(p, &end, 10);
            p = end;
            continue;
        }

        // Identifiers and keywords
        if (isalpha((unsigned char)*p) || *p == '_') {
            const char *start = p;
            int len = 0;
            while (isalnum((unsigned char)p[len]) || p[len] == '_') len++;
            char *s = strndup(start, len);
            cur = cur->next = calloc(1, sizeof(Token));
            cur->loc = (char *)start;
            cur->str = s;
            // Check keywords
            if (!strcmp(s, "int")) cur->type = TOK_INT;
            else if (!strcmp(s, "char")) cur->type = TOK_CHAR;
            else if (!strcmp(s, "long")) cur->type = TOK_LONG;
            else if (!strcmp(s, "unsigned")) cur->type = TOK_UNSIGNED;
            else if (!strcmp(s, "return")) cur->type = TOK_RETURN;
            else if (!strcmp(s, "if")) cur->type = TOK_IF;
            else if (!strcmp(s, "else")) cur->type = TOK_ELSE;
            else if (!strcmp(s, "while")) cur->type = TOK_WHILE;
            else if (!strcmp(s, "for")) cur->type = TOK_FOR;
            else if (!strcmp(s, "struct")) cur->type = TOK_STRUCT;
            else if (!strcmp(s, "void")) cur->type = TOK_VOID;
            else if (!strcmp(s, "break")) cur->type = TOK_BREAK;
            else if (!strcmp(s, "continue")) cur->type = TOK_CONTINUE;
            else cur->type = TOK_IDENT;
            p += len;
            continue;
        }

        // Character literal
        if (*p == '\'') {
            cur = cur->next = calloc(1, sizeof(Token));
            cur->loc = (char *)p;
            p++;
            int c = 0;
            if (*p == '\\') {
                p++;
                if (*p == 'n') c = '\n';
                else if (*p == 't') c = '\t';
                else if (*p == '\\') c = '\\';
                else c = *p;
                p++;
            } else {
                c = *p++;
            }
            if (*p != '\'') error_at((char *)p, "Unterminated character literal");
            p++;
            cur->type = TOK_CHAR_LITERAL;
            cur->val = c;
            continue;
        }

        // String literal
        if (*p == '"') {
            cur = cur->next = calloc(1, sizeof(Token));
            cur->loc = (char *)p;
            p++;
            char buf[MAX_TOKEN_LEN];
            int i = 0;
            while (*p && *p != '"') {
                if (*p == '\\') {
                    p++;
                    if (*p == 'n') buf[i++] = '\n';
                    else if (*p == 't') buf[i++] = '\t';
                    else if (*p == '\\') buf[i++] = '\\';
                    else buf[i++] = *p;
                    p++;
                } else {
                    buf[i++] = *p++;
                }
                if (i >= MAX_TOKEN_LEN - 1) break;
            }
            if (*p != '"') error_at((char *)p, "Unterminated string literal");
            buf[i] = '\0';
            p++;
            cur->type = TOK_STRING_LITERAL;
            cur->str = strndup(buf, i);
            continue;
        }

        // Multi-char operators
        if (!strncmp(p, "==", 2)) { cur = cur->next = calloc(1, sizeof(Token)); cur->type = TOK_EQ; cur->loc = (char *)p; p += 2; continue; }
        if (!strncmp(p, "!=", 2)) { cur = cur->next = calloc(1, sizeof(Token)); cur->type = TOK_NEQ; cur->loc = (char *)p; p += 2; continue; }
        if (!strncmp(p, "<=", 2)) { cur = cur->next = calloc(1, sizeof(Token)); cur->type = TOK_LE; cur->loc = (char *)p; p += 2; continue; }
        if (!strncmp(p, ">=", 2)) { cur = cur->next = calloc(1, sizeof(Token)); cur->type = TOK_GE; cur->loc = (char *)p; p += 2; continue; }
        if (!strncmp(p, "&&", 2)) { cur = cur->next = calloc(1, sizeof(Token)); cur->type = TOK_AND; cur->loc = (char *)p; p += 2; continue; }
        if (!strncmp(p, "||", 2)) { cur = cur->next = calloc(1, sizeof(Token)); cur->type = TOK_OR; cur->loc = (char *)p; p += 2; continue; }
        if (!strncmp(p, "->", 2)) { cur = cur->next = calloc(1, sizeof(Token)); cur->type = TOK_ARROW; cur->loc = (char *)p; p += 2; continue; }

        // Single-char tokens
        cur = cur->next = calloc(1, sizeof(Token));
        cur->loc = (char *)p;
        switch (*p++) {
            case '+': cur->type = TOK_PLUS; break;
            case '-': cur->type = TOK_MINUS; break;
            case '*': cur->type = TOK_STAR; break;
            case '/': cur->type = TOK_SLASH; break;
            case '(': cur->type = TOK_LPAREN; break;
            case ')': cur->type = TOK_RPAREN; break;
            case '{': cur->type = TOK_LBRACE; break;
            case '}': cur->type = TOK_RBRACE; break;
            case '[': cur->type = TOK_LBRACKET; break;
            case ']': cur->type = TOK_RBRACKET; break;
            case ';': cur->type = TOK_SEMI; break;
            case ',': cur->type = TOK_COMMA; break;
            case '=': cur->type = TOK_ASSIGN; break;
            case '<': cur->type = TOK_LT; break;
            case '>': cur->type = TOK_GT; break;
            case '&': cur->type = TOK_AMP; break;
            case '!': cur->type = TOK_NOT; break;
            default:
                cur->type = TOK_UNKNOWN;
                error_at((char *)p - 1, "Unexpected character '%c'", *(p - 1));
        }
    }

    cur = cur->next = calloc(1, sizeof(Token));
    cur->type = TOK_EOF;
    cur->loc = (char *)p;

    // Convert linked list to array for random access
    int count = 0;
    for (Token *t = head.next; t; t = t->next) count++;
    Token *arr = calloc(count, sizeof(Token));
    Token *t = head.next;
    for (int i = 0; i < count; i++) {
        arr[i] = *t;
        // Keep pointers to strings; do not free t->str here
        t = t->next;
    }
    return arr;
}

/* Token helpers */
static Token *cur_token(void) { return &tokens[token_idx]; }
static Token *peek_token(int offset) { return &tokens[token_idx + offset]; }
static int at_eof(void) { return cur_token()->type == TOK_EOF; }

static Token *consume_token(TokenType ty) {
    if (cur_token()->type != ty) {
        error_at(cur_token()->loc, "Expected token %d but got %d", ty, cur_token()->type);
    }
    return &tokens[token_idx++];
}

static int consume_if(TokenType ty) {
    if (cur_token()->type == ty) { token_idx++; return 1; }
    return 0;
}

static Token *consume_ident(void) {
    if (cur_token()->type != TOK_IDENT) error_at(cur_token()->loc, "Expected identifier");
    return &tokens[token_idx++];
}

/* ----------------------------------------------------------------------
 * TYPES
 * ---------------------------------------------------------------------- */

typedef enum {
    TYPEK_INT,
    TYPEK_CHAR,
    TYPEK_LONG,
    TYPEK_VOID,
    TYPEK_ARRAY,
    TYPEK_PTR,
    TYPEK_STRUCT
} BaseTypeKind;

typedef struct Type {
    BaseTypeKind kind;
    int is_unsigned;
    int pointer_level;
    int array_size;      // for arrays
    struct Type *base;   // base type for pointers/arrays
} Type;

static Type *type_int(void) { Type *t = calloc(1, sizeof(Type)); t->kind = TYPEK_INT; return t; }
static Type *type_char(void) { Type *t = calloc(1, sizeof(Type)); t->kind = TYPEK_CHAR; return t; }
static Type *type_long(void) { Type *t = calloc(1, sizeof(Type)); t->kind = TYPEK_LONG; return t; }
static Type *type_void(void) { Type *t = calloc(1, sizeof(Type)); t->kind = TYPEK_VOID; return t; }
static Type *type_ptr(Type *base) { Type *t = calloc(1, sizeof(Type)); t->kind = TYPEK_PTR; t->pointer_level = base ? base->pointer_level + 1 : 1; t->base = base; return t; }
static Type *type_array(Type *base, int size) { Type *t = calloc(1, sizeof(Type)); t->kind = TYPEK_ARRAY; t->array_size = size; t->base = base; return t; }

/* Compute size of type in bytes (simplified) */
int get_type_size(Type *t) {
    if (!t) return 8;
    if (t->pointer_level > 0 || t->kind == TYPEK_PTR) return 8;
    switch (t->kind) {
        case TYPEK_LONG: return 8;
        case TYPEK_INT: return 4;
        case TYPEK_CHAR: return 1;
        case TYPEK_ARRAY:
            return t->array_size * get_type_size(t->base);
        default:
            return 8;
    }
}

/* ----------------------------------------------------------------------
 * AST
 * ---------------------------------------------------------------------- */

typedef enum {
    AST_PROGRAM,
    AST_FUNC,
    AST_PARAM,
    AST_COMPOUND,
    AST_RETURN,
    AST_IF,
    AST_WHILE,
    AST_FOR,
    AST_BREAK,
    AST_CONTINUE,
    AST_VAR_DECL,
    AST_EXPR_STMT,
    AST_ASSIGN,
    AST_BINOP,
    AST_UNOP,
    AST_CALL,
    AST_NUM,
    AST_CHAR,
    AST_STRING,
    AST_IDENT,
    AST_ADDR,
    AST_DEREF,
    AST_CAST
} ASTKind;

typedef struct Var {
    char *name;
    Type *type;
    int offset;         // offset from rbp (positive number)
    struct Var *next;
} Var;

typedef struct Function {
    char *name;
    Type *ret_type;
    Var *params[MAX_ARGS];
    int num_params;
    Var *locals;        // linked list
    int stack_size;
    struct ASTNode *body;
    int label_break;
    int label_continue;
} Function;

typedef struct ASTNode {
    ASTKind kind;
    Token *tok;                 // token associated (for error reporting)
    Type *dtype;                // computed type
    struct ASTNode *children[MAX_AST_CHILDREN];
    int child_count;
    // For specific node kinds:
    long num_val;               // for AST_NUM and AST_CHAR
    char *str_val;              // for AST_STRING and AST_IDENT
    char op;                    // for simple binop/unop representation
    Function *func_def;         // for AST_FUNC
    Var *var_def;               // for AST_VAR_DECL or AST_IDENT resolution
} ASTNode;

static ASTNode *new_node(ASTKind k, Token *tok) {
    ASTNode *n = calloc(1, sizeof(ASTNode));
    n->kind = k;
    n->tok = tok;
    return n;
}

static void ast_add_child(ASTNode *parent, ASTNode *child) {
    if (!parent || !child) return;
    if (parent->child_count >= MAX_AST_CHILDREN) error_at(parent->tok ? parent->tok->loc : current_input, "AST node has too many children");
    parent->children[parent->child_count++] = child;
}

/* ----------------------------------------------------------------------
 * SYMBOL TABLE HELPERS
 * ---------------------------------------------------------------------- */

static Var *add_local(Function *f, char *name, Type *type) {
    Var *v = calloc(1, sizeof(Var));
    v->name = name;
    v->type = type;
    int size = get_type_size(type);
    if (size <= 0) size = 8;
    // allocate on stack: increase stack_size and set offset
    f->stack_size = ALIGN_TO(f->stack_size + size, 8);
    v->offset = f->stack_size;
    v->next = f->locals;
    f->locals = v;
    return v;
}

static Var *find_var_in_function(Function *f, const char *name) {
    for (Var *v = f->locals; v; v = v->next) {
        if (!strcmp(v->name, name)) return v;
    }
    for (int i = 0; i < f->num_params; i++) {
        if (f->params[i] && !strcmp(f->params[i]->name, name)) return f->params[i];
    }
    return NULL;
}

/* ----------------------------------------------------------------------
 * PARSER
 * ---------------------------------------------------------------------- */

static Type *parse_type_specifier(void);

static ASTNode *parse_expression(void);
static ASTNode *parse_assignment(void);
static ASTNode *parse_logical_or(void);
static ASTNode *parse_logical_and(void);
static ASTNode *parse_equality(void);
static ASTNode *parse_relational(void);
static ASTNode *parse_add(void);
static ASTNode *parse_mul(void);
static ASTNode *parse_unary(void);
static ASTNode *parse_primary(void);
static ASTNode *parse_statement(void);
static ASTNode *parse_compound(void);
static ASTNode *parse_function(void);
static ASTNode *parse_program(void);

static int is_type_keyword(Token *t) {
    if (!t) return 0;
    return t->type == TOK_INT || t->type == TOK_CHAR || t->type == TOK_LONG || t->type == TOK_VOID || t->type == TOK_UNSIGNED;
}

static Type *parse_type_specifier(void) {
    // Basic types
    int is_unsigned = 0;
    if (consume_if(TOK_UNSIGNED)) is_unsigned = 1;

    Type *base = NULL;
    if (consume_if(TOK_INT)) base = type_int();
    else if (consume_if(TOK_CHAR)) base = type_char();
    else if (consume_if(TOK_LONG)) base = type_long();
    else if (consume_if(TOK_VOID)) base = type_void();
    else error_at(cur_token()->loc, "Expected type specifier");

    base->is_unsigned = is_unsigned;

    // Pointer levels
    while (consume_if(TOK_STAR)) {
        Type *p = type_ptr(base);
        p->base = base;
        base = p;
    }

    // Array (only in declarations, not in general type expressions)
    if (consume_if(TOK_LBRACKET)) {
        Token *size_tok = consume_token(TOK_NUMBER);
        consume_token(TOK_RBRACKET);
        Type *arr = type_array(base, (int)size_tok->val);
        return arr;
    }

    return base;
}

/* Primary expressions */
static ASTNode *parse_primary(void) {
    Token *t = cur_token();
    if (consume_if(TOK_LPAREN)) {
        ASTNode *n = parse_expression();
        consume_token(TOK_RPAREN);
        return n;
    }
    if (t->type == TOK_NUMBER) {
        ASTNode *n = new_node(AST_NUM, consume_token(TOK_NUMBER));
        n->num_val = t->val;
        n->dtype = type_int();
        return n;
    }
    if (t->type == TOK_CHAR_LITERAL) {
        ASTNode *n = new_node(AST_CHAR, consume_token(TOK_CHAR_LITERAL));
        n->num_val = t->val;
        n->dtype = type_char();
        return n;
    }
    if (t->type == TOK_STRING_LITERAL) {
        ASTNode *n = new_node(AST_STRING, consume_token(TOK_STRING_LITERAL));
        n->str_val = strdup(t->str);
        // Register string literal in table
        if (string_table_count >= MAX_STRING_LITERALS) error_at(t->loc, "Too many string literals");
        string_table[string_table_count].id = string_table_count;
        string_table[string_table_count].s = strdup(n->str_val);
        n->num_val = string_table_count; // store id
        string_table_count++;
        // string literal type is pointer to char
        Type *pt = type_ptr(type_char());
        n->dtype = pt;
        return n;
    }
    if (t->type == TOK_IDENT) {
        // Could be a function call or identifier
        Token *idtok = consume_ident();
        if (consume_if(TOK_LPAREN)) {
            // function call
            ASTNode *call = new_node(AST_CALL, idtok);
            // parse args
            if (!consume_if(TOK_RPAREN)) {
                ASTNode *arg = parse_expression();
                ast_add_child(call, arg);
                while (consume_if(TOK_COMMA)) {
                    ASTNode *a = parse_expression();
                    ast_add_child(call, a);
                }
                consume_token(TOK_RPAREN);
            }
            call->dtype = type_int(); // assume int return for now
            return call;
        } else {
            ASTNode *n = new_node(AST_IDENT, idtok);
            n->str_val = strdup(idtok->str);
            return n;
        }
    }

    error_at(t->loc, "Expected primary expression");
    return NULL;
}

/* Unary */
static ASTNode *parse_unary(void) {
    Token *t = cur_token();
    if (consume_if(TOK_PLUS)) return parse_unary();
    if (consume_if(TOK_MINUS)) {
        ASTNode *n = new_node(AST_UNOP, t);
        n->op = '-';
        ASTNode *child = parse_unary();
        ast_add_child(n, child);
        return n;
    }
    if (consume_if(TOK_NOT)) {
        ASTNode *n = new_node(AST_UNOP, t);
        n->op = '!';
        ASTNode *child = parse_unary();
        ast_add_child(n, child);
        return n;
    }
    if (consume_if(TOK_STAR)) {
        ASTNode *n = new_node(AST_DEREF, t);
        ASTNode *child = parse_unary();
        ast_add_child(n, child);
        return n;
    }
    if (consume_if(TOK_AMP)) {
        ASTNode *n = new_node(AST_ADDR, t);
        ASTNode *child = parse_unary();
        ast_add_child(n, child);
        return n;
    }
    return parse_primary();
}

/* Multiplicative */
static ASTNode *parse_mul(void) {
    ASTNode *node = parse_unary();
    while (cur_token()->type == TOK_STAR || cur_token()->type == TOK_SLASH) {
        Token *op = &tokens[token_idx++];
        ASTNode *n = new_node(AST_BINOP, op);
        n->op = (op->type == TOK_STAR) ? '*' : '/';
        ast_add_child(n, node);
        ASTNode *r = parse_unary();
        ast_add_child(n, r);
        node = n;
    }
    return node;
}

/* Additive */
static ASTNode *parse_add(void) {
    ASTNode *node = parse_mul();
    while (cur_token()->type == TOK_PLUS || cur_token()->type == TOK_MINUS) {
        Token *op = &tokens[token_idx++];
        ASTNode *n = new_node(AST_BINOP, op);
        n->op = (op->type == TOK_PLUS) ? '+' : '-';
        ast_add_child(n, node);
        ASTNode *r = parse_mul();
        ast_add_child(n, r);
        node = n;
    }
    return node;
}

/* Relational */
static ASTNode *parse_relational(void) {
    ASTNode *node = parse_add();
    while (cur_token()->type == TOK_LT || cur_token()->type == TOK_GT || cur_token()->type == TOK_LE || cur_token()->type == TOK_GE) {
        Token *op = &tokens[token_idx++];
        ASTNode *n = new_node(AST_BINOP, op);
        if (op->type == TOK_LT) n->op = '<';
        else if (op->type == TOK_GT) n->op = '>';
        else if (op->type == TOK_LE) n->op = 'l'; // 'l' for <=
        else n->op = 'g'; // 'g' for >=
        ast_add_child(n, node);
        ASTNode *r = parse_add();
        ast_add_child(n, r);
        node = n;
    }
    return node;
}

/* Equality */
static ASTNode *parse_equality(void) {
    ASTNode *node = parse_relational();
    while (cur_token()->type == TOK_EQ || cur_token()->type == TOK_NEQ) {
        Token *op = &tokens[token_idx++];
        ASTNode *n = new_node(AST_BINOP, op);
        n->op = (op->type == TOK_EQ) ? '=' : '!';
        ast_add_child(n, node);
        ASTNode *r = parse_relational();
        ast_add_child(n, r);
        node = n;
    }
    return node;
}

/* Logical AND */
static ASTNode *parse_logical_and(void) {
    ASTNode *node = parse_equality();
    while (consume_if(TOK_AND)) {
        Token *op = &tokens[token_idx - 1];
        ASTNode *n = new_node(AST_BINOP, op);
        n->op = '&';
        ast_add_child(n, node);
        ASTNode *r = parse_equality();
        ast_add_child(n, r);
        node = n;
    }
    return node;
}

/* Logical OR */
static ASTNode *parse_logical_or(void) {
    ASTNode *node = parse_logical_and();
    while (consume_if(TOK_OR)) {
        Token *op = &tokens[token_idx - 1];
        ASTNode *n = new_node(AST_BINOP, op);
        n->op = '|';
        ast_add_child(n, node);
        ASTNode *r = parse_logical_and();
        ast_add_child(n, r);
        node = n;
    }
    return node;
}

/* Assignment */
static ASTNode *parse_assignment(void) {
    ASTNode *node = parse_logical_or();
    if (consume_if(TOK_ASSIGN)) {
        Token *op = &tokens[token_idx - 1];
        ASTNode *n = new_node(AST_ASSIGN, op);
        ast_add_child(n, node);
        ASTNode *r = parse_assignment();
        ast_add_child(n, r);
        return n;
    }
    return node;
}

static ASTNode *parse_expression(void) {
    return parse_assignment();
}

/* Statement parsing */

static ASTNode *parse_compound(void) {
    consume_token(TOK_LBRACE);
    ASTNode *blk = new_node(AST_COMPOUND, cur_token());
    while (!consume_if(TOK_RBRACE)) {
        ASTNode *s = parse_statement();
        ast_add_child(blk, s);
    }
    return blk;
}

static ASTNode *parse_statement(void) {
    Token *t = cur_token();

    if (consume_if(TOK_LBRACE)) {
        // roll back one token because parse_compound expects '{' consumed
        token_idx--;
        return parse_compound();
    }

    if (consume_if(TOK_IF)) {
        ASTNode *n = new_node(AST_IF, cur_token() - 1);
        consume_token(TOK_LPAREN);
        ASTNode *cond = parse_expression();
        consume_token(TOK_RPAREN);
        ast_add_child(n, cond);
        ASTNode *then = parse_statement();
        ast_add_child(n, then);
        if (consume_if(TOK_ELSE)) {
            ASTNode *els = parse_statement();
            ast_add_child(n, els);
        }
        return n;
    }

    if (consume_if(TOK_WHILE)) {
        ASTNode *n = new_node(AST_WHILE, cur_token() - 1);
        consume_token(TOK_LPAREN);
        ASTNode *cond = parse_expression();
        consume_token(TOK_RPAREN);
        ast_add_child(n, cond);
        ASTNode *body = parse_statement();
        ast_add_child(n, body);
        return n;
    }

    if (consume_if(TOK_FOR)) {
        ASTNode *n = new_node(AST_FOR, cur_token() - 1);
        consume_token(TOK_LPAREN);
        // init
        if (!consume_if(TOK_SEMI)) {
            if (is_type_keyword(cur_token())) {
                // declaration in for-init
                Type *t = parse_type_specifier();
                Token *id = consume_ident();
                ASTNode *decl = new_node(AST_VAR_DECL, id);
                decl->dtype = t;
                if (consume_if(TOK_ASSIGN)) {
                    ASTNode *init = parse_expression();
                    ast_add_child(decl, init);
                }
                consume_token(TOK_SEMI);
                ast_add_child(n, decl);
            } else {
                ASTNode *init = parse_expression();
                consume_token(TOK_SEMI);
                ast_add_child(n, init);
            }
        } else {
            ast_add_child(n, NULL);
        }
        // cond
        if (!consume_if(TOK_SEMI)) {
            ASTNode *cond = parse_expression();
            consume_token(TOK_SEMI);
            ast_add_child(n, cond);
        } else {
            ast_add_child(n, NULL);
        }
        // incr
        if (!consume_if(TOK_RPAREN)) {
            ASTNode *incr = parse_expression();
            consume_token(TOK_RPAREN);
            ast_add_child(n, incr);
        } else {
            ast_add_child(n, NULL);
        }
        ASTNode *body = parse_statement();
        ast_add_child(n, body);
        return n;
    }

    if (consume_if(TOK_BREAK)) {
        ASTNode *n = new_node(AST_BREAK, cur_token() - 1);
        consume_token(TOK_SEMI);
        return n;
    }

    if (consume_if(TOK_CONTINUE)) {
        ASTNode *n = new_node(AST_CONTINUE, cur_token() - 1);
        consume_token(TOK_SEMI);
        return n;
    }

    if (consume_if(TOK_RETURN)) {
        ASTNode *n = new_node(AST_RETURN, cur_token() - 1);
        if (!consume_if(TOK_SEMI)) {
            ASTNode *e = parse_expression();
            ast_add_child(n, e);
            consume_token(TOK_SEMI);
        }
        return n;
    }

    // Declaration
    if (is_type_keyword(cur_token())) {
        Type *t = parse_type_specifier();
        Token *id = consume_ident();
        ASTNode *decl = new_node(AST_VAR_DECL, id);
        decl->dtype = t;
        if (consume_if(TOK_ASSIGN)) {
            ASTNode *init = parse_expression();
            ast_add_child(decl, init);
        }
        consume_token(TOK_SEMI);
        return decl;
    }

    // Expression statement
    ASTNode *expr = parse_expression();
    consume_token(TOK_SEMI);
    ASTNode *stmt = new_node(AST_EXPR_STMT, expr->tok);
    ast_add_child(stmt, expr);
    return stmt;
}

/* Function parsing */
static ASTNode *parse_function(void) {
    Type *ret = parse_type_specifier();
    Token *name_tok = consume_ident();
    ASTNode *fn = new_node(AST_FUNC, name_tok);
    Function *f = calloc(1, sizeof(Function));
    f->name = strdup(name_tok->str);
    f->ret_type = ret;
    fn->func_def = f;

    consume_token(TOK_LPAREN);
    int param_idx = 0;
    if (!consume_if(TOK_RPAREN)) {
        while (1) {
            Type *pt = parse_type_specifier();
            Token *pname = consume_ident();
            Var *pv = calloc(1, sizeof(Var));
            pv->name = strdup(pname->str);
            pv->type = pt;
            f->params[param_idx++] = pv;
            if (consume_if(TOK_COMMA)) continue;
            consume_token(TOK_RPAREN);
            break;
        }
    }
    f->num_params = param_idx;

    ASTNode *body = parse_compound();
    ast_add_child(fn, body);
    f->body = body;
    return fn;
}

/* Program */
static ASTNode *parse_program(void) {
    ASTNode *prog = new_node(AST_PROGRAM, NULL);
    while (!at_eof()) {
        ASTNode *fn = parse_function();
        ast_add_child(prog, fn);
    }
    return prog;
}

/* ----------------------------------------------------------------------
 * SEMANTIC ANALYSIS
 * ---------------------------------------------------------------------- */

static Function *global_functions[MAX_FUNCTIONS];
static int global_function_count = 0;

static void register_function(Function *f) {
    if (global_function_count >= MAX_FUNCTIONS) error_at(current_input, "Too many functions");
    global_functions[global_function_count++] = f;
}

static void semantic_resolve(ASTNode *node, Function *curf);

static void semantic_program(ASTNode *prog) {
    // Register functions and perform per-function semantic checks
    for (int i = 0; i < prog->child_count; i++) {
        ASTNode *fnnode = prog->children[i];
        if (fnnode->kind != AST_FUNC) error_at(fnnode->tok ? fnnode->tok->loc : current_input, "Top-level node is not a function");
        Function *f = fnnode->func_def;
        register_function(f);
    }
    // Now process each function
    for (int i = 0; i < prog->child_count; i++) {
        ASTNode *fnnode = prog->children[i];
        Function *f = fnnode->func_def;
        // Add parameters to locals (and compute offsets)
        f->stack_size = 0;
        for (int j = 0; j < f->num_params; j++) {
            Var *p = f->params[j];
            // For simplicity, treat params as locals with offsets
            add_local(f, p->name, p->type);
        }
        // Walk body
        semantic_resolve(fnnode->children[0], f);
        // finalize stack size alignment to 16 bytes for ABI
        f->stack_size = ALIGN_TO(f->stack_size, 16);
    }
}

static void semantic_resolve(ASTNode *node, Function *curf) {
    if (!node) return;
    switch (node->kind) {
        case AST_COMPOUND:
            for (int i = 0; i < node->child_count; i++) semantic_resolve(node->children[i], curf);
            break;
        case AST_VAR_DECL: {
            // Add to locals
            Token *id = node->tok;
            if (!id || id->type != TOK_IDENT) error_at(node->tok ? node->tok->loc : current_input, "Invalid variable declaration");
            Var *v = add_local(curf, strdup(id->str), node->dtype);
            node->var_def = v;
            if (node->child_count > 0 && node->children[0]) {
                semantic_resolve(node->children[0], curf);
                // Optionally check type compatibility
            }
            break;
        }
        case AST_IDENT: {
            // Resolve variable
            Var *v = find_var_in_function(curf, node->str_val);
            if (!v) error_at(node->tok->loc, "Unknown identifier: %s", node->str_val);
            node->var_def = v;
            node->dtype = v->type;
            break;
        }
        case AST_NUM:
        case AST_CHAR:
        case AST_STRING:
            // types already set during parsing
            break;
        case AST_CALL:
            // resolve args
            for (int i = 0; i < node->child_count; i++) semantic_resolve(node->children[i], curf);
            node->dtype = type_int(); // assume int return
            break;
        case AST_UNOP:
        case AST_DEREF:
        case AST_ADDR:
            semantic_resolve(node->children[0], curf);
            // set dtype for deref/addr
            if (node->kind == AST_DEREF) {
                if (!node->children[0]->dtype || node->children[0]->dtype->pointer_level == 0) {
                    error_at(node->tok->loc, "Dereferencing non-pointer");
                }
                node->dtype = node->children[0]->dtype->base ? node->children[0]->dtype->base : type_int();
            } else if (node->kind == AST_ADDR) {
                node->dtype = type_ptr(node->children[0]->dtype);
            } else {
                // unary op result type is int for now
                node->dtype = type_int();
            }
            break;
        case AST_BINOP:
        case AST_ASSIGN:
            // resolve children
            for (int i = 0; i < node->child_count; i++) semantic_resolve(node->children[i], curf);
            // set dtype (simple rules)
            if (node->kind == AST_ASSIGN) {
                node->dtype = node->children[0]->dtype;
            } else {
                // binary ops produce int or pointer arithmetic (simplified)
                node->dtype = type_int();
            }
            break;
        case AST_EXPR_STMT:
            semantic_resolve(node->children[0], curf);
            break;
        case AST_IF:
            semantic_resolve(node->children[0], curf);
            semantic_resolve(node->children[1], curf);
            if (node->child_count > 2 && node->children[2]) semantic_resolve(node->children[2], curf);
            break;
        case AST_WHILE:
            semantic_resolve(node->children[0], curf);
            semantic_resolve(node->children[1], curf);
            break;
        case AST_FOR:
            if (node->child_count > 0 && node->children[0]) semantic_resolve(node->children[0], curf);
            if (node->child_count > 1 && node->children[1]) semantic_resolve(node->children[1], curf);
            if (node->child_count > 2 && node->children[2]) semantic_resolve(node->children[2], curf);
            if (node->child_count > 3 && node->children[3]) semantic_resolve(node->children[3], curf);
            break;
        case AST_RETURN:
            if (node->child_count > 0 && node->children[0]) semantic_resolve(node->children[0], curf);
            break;
        case AST_BREAK:
        case AST_CONTINUE:
            // validity checked in codegen (presence of labels)
            break;
        default:
            // fallback: resolve children
            for (int i = 0; i < node->child_count; i++) semantic_resolve(node->children[i], curf);
            break;
    }
}

/* ----------------------------------------------------------------------
 * CODE GENERATION (AT&T syntax)
 * ---------------------------------------------------------------------- */

static int label_counter = 0;
#define NEW_LABEL() (label_counter++)

static const char *arg_regs[] = { "%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9" };

/* Forward declarations */
static void codegen_program(ASTNode *prog);
static void codegen_function(Function *f);
static void codegen_stmt(ASTNode *node, Function *f);
static void codegen_expr(ASTNode *node, Function *f);

/* Emit prologue */
static void emit_prologue(Function *f) {
    printf("  pushq %%rbp\n");
    printf("  movq %%rsp, %%rbp\n");
    if (f->stack_size > 0) {
        printf("  subq $%d, %%rsp\n", f->stack_size);
    }
}

/* Emit epilogue */
static void emit_epilogue(Function *f) {
    printf("  movq %%rbp, %%rsp\n");
    printf("  popq %%rbp\n");
    printf("  ret\n");
}

/* Helper: push immediate 64-bit */
static void emit_push_imm(long v) {
    printf("  movq $%ld, %%rax\n", v);
    printf("  pushq %%rax\n");
}

/* Helper: load variable address into stack (push address) */
static void codegen_lvalue_address(ASTNode *node, Function *f) {
    if (node->kind == AST_IDENT) {
        Var *v = node->var_def;
        if (!v) error_at(node->tok->loc, "Undefined variable in lvalue");
        // address = rbp - offset -> AT&T: -offset(%rbp)
        printf("  leaq -%d(%%rbp), %%rax\n", v->offset);
        printf("  pushq %%rax\n");
        return;
    } else if (node->kind == AST_DEREF) {
        // evaluate child to get address
        codegen_expr(node->children[0], f);
        // result is address on stack already
        return;
    } else {
        error_at(node->tok ? node->tok->loc : current_input, "Left-hand side is not an lvalue");
    }
}

/* Load value from address on top of stack and push value */
static void codegen_load_from_address(Type *t) {
    printf("  popq %%rax\n"); // address
    int sz = get_type_size(t);
    if (sz == 1) {
        // movsbq (%rax), %rax  -> sign-extend byte to quad
        printf("  movsbq (%%rax), %%rax\n");
    } else if (sz == 4) {
        // movslq (%rax), %rax -> sign-extend dword to quad
        printf("  movslq (%%rax), %%rax\n");
    } else {
        printf("  movq (%%rax), %%rax\n");
    }
    printf("  pushq %%rax\n");
}

/* Store value into address: stack: [ ... , addr, value ] -> store and push value back */
static void codegen_store_to_address(Type *t) {
    printf("  popq %%rdi\n"); // value
    printf("  popq %%rax\n"); // address
    int sz = get_type_size(t);
    if (sz == 1) {
        // store low byte of %rdi into (%rax)
        printf("  movb %%dil, (%%rax)\n");
    } else if (sz == 4) {
        printf("  movl %%edi, (%%rax)\n");
    } else {
        printf("  movq %%rdi, (%%rax)\n");
    }
    // push stored value back
    printf("  pushq %%rdi\n");
}

/* Expression codegen */
static void codegen_expr(ASTNode *node, Function *f) {
    if (!node) return;
    switch (node->kind) {
        case AST_NUM:
            emit_push_imm(node->num_val);
            return;
        case AST_CHAR:
            emit_push_imm(node->num_val);
            return;
        case AST_STRING: {
            int id = (int)node->num_val;
            printf("  leaq .LC%d(%%rip), %%rax\n", id);
            printf("  pushq %%rax\n");
            return;
        }
        case AST_IDENT:
            // push address then load
            codegen_lvalue_address(node, f);
            codegen_load_from_address(node->dtype);
            return;
        case AST_ADDR:
            // push address of child
            codegen_lvalue_address(node->children[0], f);
            return;
        case AST_DEREF:
            // evaluate child to get address, then load
            codegen_expr(node->children[0], f);
            codegen_load_from_address(node->dtype);
            return;
        case AST_CALL: {
            // Evaluate args left-to-right and move into registers
            int nargs = node->child_count;
            // Evaluate args and leave them on stack
            for (int i = 0; i < nargs; i++) {
                codegen_expr(node->children[i], f);
            }
            // Move args into registers (pop in reverse)
            for (int i = nargs - 1; i >= 0; i--) {
                if (i < (int)(sizeof(arg_regs)/sizeof(arg_regs[0]))) {
                    printf("  popq %s\n", arg_regs[i]);
                } else {
                    // more args than registers: not fully implemented; pop into rax as fallback
                    printf("  popq %%rax\n");
                    printf("  pushq %%rax\n");
                }
            }
            // Call function
            printf("  call %s\n", node->tok->str);
            printf("  pushq %%rax\n");
            return;
        }
        case AST_UNOP:
            if (node->op == '-') {
                codegen_expr(node->children[0], f);
                printf("  popq %%rax\n");
                printf("  negq %%rax\n");
                printf("  pushq %%rax\n");
                return;
            } else if (node->op == '!') {
                codegen_expr(node->children[0], f);
                printf("  popq %%rax\n");
                printf("  cmpq $0, %%rax\n");
                printf("  sete %%al\n");
                printf("  movzbl %%al, %%eax\n");
                printf("  movslq %%eax, %%rax\n");
                printf("  pushq %%rax\n");
                return;
            }
            break;
        case AST_BINOP: {
            // Short-circuit for && and ||
            if (node->op == '&' && node->tok->type == TOK_AND) {
                int lbl_false = NEW_LABEL();
                int lbl_end = NEW_LABEL();
                codegen_expr(node->children[0], f);
                printf("  popq %%rax\n");
                printf("  cmpq $0, %%rax\n");
                printf("  je .Lfalse%d\n", lbl_false);
                codegen_expr(node->children[1], f);
                printf("  popq %%rax\n");
                printf("  cmpq $0, %%rax\n");
                printf("  je .Lfalse%d\n", lbl_false);
                printf("  movq $1, %%rax\n");
                printf("  jmp .Lend%d\n", lbl_end);
                printf(".Lfalse%d:\n", lbl_false);
                printf("  movq $0, %%rax\n");
                printf(".Lend%d:\n", lbl_end);
                printf("  pushq %%rax\n");
                return;
            } else if (node->op == '|' && node->tok->type == TOK_OR) {
                int lbl_true = NEW_LABEL();
                int lbl_end = NEW_LABEL();
                codegen_expr(node->children[0], f);
                printf("  popq %%rax\n");
                printf("  cmpq $0, %%rax\n");
                printf("  jne .Ltrue%d\n", lbl_true);
                codegen_expr(node->children[1], f);
                printf("  popq %%rax\n");
                printf("  cmpq $0, %%rax\n");
                printf("  jne .Ltrue%d\n", lbl_true);
                printf("  movq $0, %%rax\n");
                printf("  jmp .Lend%d\n", lbl_end);
                printf(".Ltrue%d:\n", lbl_true);
                printf("  movq $1, %%rax\n");
                printf(".Lend%d:\n", lbl_end);
                printf("  pushq %%rax\n");
                return;
            }
            // Evaluate left and right
            codegen_expr(node->children[0], f);
            codegen_expr(node->children[1], f);
            printf("  popq %%rdi\n"); // right
            printf("  popq %%rax\n"); // left
            switch (node->op) {
                case '+': printf("  addq %%rdi, %%rax\n"); break;
                case '-': printf("  subq %%rdi, %%rax\n"); break;
                case '*': printf("  imulq %%rdi, %%rax\n"); break;
                case '/': printf("  cqto\n  idivq %%rdi\n"); break;
                case '=': // ==
                    printf("  cmpq %%rdi, %%rax\n  sete %%al\n  movzbl %%al, %%eax\n  movslq %%eax, %%rax\n"); break;
                case '!': // !=
                    printf("  cmpq %%rdi, %%rax\n  setne %%al\n  movzbl %%al, %%eax\n  movslq %%eax, %%rax\n"); break;
                case '<':
                    printf("  cmpq %%rdi, %%rax\n  setl %%al\n  movzbl %%al, %%eax\n  movslq %%eax, %%rax\n"); break;
                case '>':
                    printf("  cmpq %%rdi, %%rax\n  setg %%al\n  movzbl %%al, %%eax\n  movslq %%eax, %%rax\n"); break;
                case 'l': // <=
                    printf("  cmpq %%rdi, %%rax\n  setle %%al\n  movzbl %%al, %%eax\n  movslq %%eax, %%rax\n"); break;
                case 'g': // >=
                    printf("  cmpq %%rdi, %%rax\n  setge %%al\n  movzbl %%al, %%eax\n  movslq %%eax, %%rax\n"); break;
                default:
                    error_at(node->tok ? node->tok->loc : current_input, "Unsupported binary op");
            }
            printf("  pushq %%rax\n");
            return;
        }
        case AST_ASSIGN: {
            // Evaluate RHS
            codegen_expr(node->children[1], f);
            // Compute LHS address
            codegen_lvalue_address(node->children[0], f);
            // Now stack: [..., addr, value]
            codegen_store_to_address(node->children[0]->dtype);
            return;
        }
        default:
            error_at(node->tok ? node->tok->loc : current_input, "Unhandled expression kind %d", node->kind);
    }
}

/* Statement codegen */
static void codegen_stmt(ASTNode *node, Function *f) {
    if (!node) return;
    switch (node->kind) {
        case AST_COMPOUND:
            for (int i = 0; i < node->child_count; i++) codegen_stmt(node->children[i], f);
            break;
        case AST_VAR_DECL:
            // initialization if present
            if (node->child_count > 0 && node->children[0]) {
                // create an assign node and reuse codegen
                ASTNode *ident = new_node(AST_IDENT, node->tok);
                ident->str_val = strdup(node->tok->str);
                ident->var_def = node->var_def;
                ident->dtype = node->dtype;
                ASTNode *assign = new_node(AST_ASSIGN, node->tok);
                ast_add_child(assign, ident);
                ast_add_child(assign, node->children[0]);
                codegen_expr(assign, f);
                // pop result
                printf("  addq $8, %%rsp\n");
            }
            break;
        case AST_EXPR_STMT:
            codegen_expr(node->children[0], f);
            printf("  addq $8, %%rsp\n");
            break;
        case AST_IF: {
            int lbl_else = NEW_LABEL();
            int lbl_end = NEW_LABEL();
            codegen_expr(node->children[0], f);
            printf("  popq %%rax\n");
            printf("  cmpq $0, %%rax\n");
            printf("  je .Lelse%d\n", lbl_else);
            codegen_stmt(node->children[1], f);
            printf("  jmp .Lend%d\n", lbl_end);
            printf(".Lelse%d:\n", lbl_else);
            if (node->child_count > 2 && node->children[2]) codegen_stmt(node->children[2], f);
            printf(".Lend%d:\n", lbl_end);
            break;
        }
        case AST_WHILE: {
            int lbl_begin = NEW_LABEL();
            int lbl_end = NEW_LABEL();
            int prev_break = f->label_break;
            int prev_continue = f->label_continue;
            f->label_break = lbl_end;
            f->label_continue = lbl_begin;
            printf(".Lbegin%d:\n", lbl_begin);
            codegen_expr(node->children[0], f);
            printf("  popq %%rax\n");
            printf("  cmpq $0, %%rax\n");
            printf("  je .Lend%d\n", lbl_end);
            codegen_stmt(node->children[1], f);
            printf("  jmp .Lbegin%d\n", lbl_begin);
            printf(".Lend%d:\n", lbl_end);
            f->label_break = prev_break;
            f->label_continue = prev_continue;
            break;
        }
        case AST_FOR: {
            int lbl_begin = NEW_LABEL();
            int lbl_end = NEW_LABEL();
            int prev_break = f->label_break;
            int prev_continue = f->label_continue;
            f->label_break = lbl_end;
            f->label_continue = lbl_begin;
            // init
            if (node->child_count > 0 && node->children[0]) {
                if (node->children[0]->kind == AST_VAR_DECL) {
                    // variable declaration with optional init
                    if (node->children[0]->child_count > 0 && node->children[0]->children[0]) {
                        ASTNode *ident = new_node(AST_IDENT, node->children[0]->tok);
                        ident->str_val = strdup(node->children[0]->tok->str);
                        ident->var_def = node->children[0]->var_def;
                        ident->dtype = node->children[0]->dtype;
                        ASTNode *assign = new_node(AST_ASSIGN, node->children[0]->tok);
                        ast_add_child(assign, ident);
                        ast_add_child(assign, node->children[0]->children[0]);
                        codegen_expr(assign, f);
                        printf("  addq $8, %%rsp\n");
                    }
                } else {
                    codegen_expr(node->children[0], f);
                    printf("  addq $8, %%rsp\n");
                }
            }
            printf(".Lbegin%d:\n", lbl_begin);
            if (node->child_count > 1 && node->children[1]) {
                codegen_expr(node->children[1], f);
                printf("  popq %%rax\n");
                printf("  cmpq $0, %%rax\n");
                printf("  je .Lend%d\n", lbl_end);
            }
            codegen_stmt(node->children[3], f);
            if (node->child_count > 2 && node->children[2]) {
                codegen_expr(node->children[2], f);
                printf("  addq $8, %%rsp\n");
            }
            printf("  jmp .Lbegin%d\n", lbl_begin);
            printf(".Lend%d:\n", lbl_end);
            f->label_break = prev_break;
            f->label_continue = prev_continue;
            break;
        }
        case AST_BREAK:
            if (f->label_break < 0) error_at(node->tok->loc, "break not within loop");
            printf("  jmp .Lend%d\n", f->label_break);
            break;
        case AST_CONTINUE:
            if (f->label_continue < 0) error_at(node->tok->loc, "continue not within loop");
            printf("  jmp .Lbegin%d\n", f->label_continue);
            break;
        case AST_RETURN:
            if (node->child_count > 0 && node->children[0]) {
                codegen_expr(node->children[0], f);
                printf("  popq %%rax\n");
            } else {
                printf("  movq $0, %%rax\n");
            }
            emit_epilogue(f);
            break;
        default:
            error_at(node->tok ? node->tok->loc : current_input, "Unhandled statement kind %d", node->kind);
    }
}

/* Top-level program codegen */
static void codegen_program(ASTNode *prog) {
    // Emit text section
    printf("  .text\n");
    for (int i = 0; i < prog->child_count; i++) {
        ASTNode *fnnode = prog->children[i];
        Function *f = fnnode->func_def;
        // initialize labels
        f->label_break = -1;
        f->label_continue = -1;
        // Emit global symbol
        printf("  .globl %s\n", f->name);
        printf("%s:\n", f->name);
        emit_prologue(f);
        // Move parameters from registers into local slots if needed
        for (int p = 0; p < f->num_params; p++) {
            Var *pv = f->params[p];
            if (!pv) continue;
            if (p < (int)(sizeof(arg_regs)/sizeof(arg_regs[0]))) {
                // movq %rdi, -offset(%rbp) -> AT&T: movq %rdi, -offset(%rbp)
                printf("  movq %s, -%d(%%rbp)\n", arg_regs[p], pv->offset);
            } else {
                // additional args on stack not handled
            }
        }
        // Generate body
        codegen_stmt(fnnode->children[0], f);
        // If function falls through without return, emit epilogue
        emit_epilogue(f);
    }

    // Emit data section for string literals
    if (string_table_count > 0) {
        printf("  .data\n");
        for (int i = 0; i < string_table_count; i++) {
            // Escape quotes and backslashes in the string for assembler
            char *s = string_table[i].s ? string_table[i].s : "";
            // Simple escaping for double quotes and backslashes
            printf(".LC%d:\n", i);
            printf("  .string \"");
            for (const char *c = s; *c; ++c) {
                if (*c == '\\') printf("\\\\");
                else if (*c == '\"') printf("\\\"");
                else if (*c == '\n') printf("\\n");
                else if (*c == '\t') printf("\\t");
                else putchar(*c);
            }
            printf("\"\n");
        }
    }
}

/* ----------------------------------------------------------------------
 * DEBUG / UTILITIES
 * ---------------------------------------------------------------------- */

static void print_ast(ASTNode *n, int depth) {
    if (!n) return;
    for (int i = 0; i < depth; i++) printf("  ");
    printf("AST %d", n->kind);
    if (n->tok && n->tok->str) printf(" (%s)", n->tok->str);
    if (n->kind == AST_NUM) printf(" num=%ld", n->num_val);
    if (n->kind == AST_STRING) printf(" str_id=%ld", n->num_val);
    printf("\n");
    for (int i = 0; i < n->child_count; i++) print_ast(n->children[i], depth + 1);
}

/* ----------------------------------------------------------------------
 * DRIVER / MAIN
 * ---------------------------------------------------------------------- */

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <source.c>\n", argv[0]);
        return 1;
    }

    // Read source file
    FILE *f = fopen(argv[1], "rb");
    if (!f) { perror("fopen"); return 1; }
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *src = malloc(sz + 1);
    if (!src) { perror("malloc"); fclose(f); return 1; }
    if (fread(src, 1, sz, f) != (size_t)sz) { perror("fread"); free(src); fclose(f); return 1; }
    src[sz] = '\0';
    fclose(f);

    // Tokenize
    tokens = tokenize(src);
    token_idx = 0;

    // Parse
    ASTNode *prog = parse_program();

    // Semantic analysis
    semantic_program(prog);

    // Optional debug output
    // print_ast(prog, 0);

    // Code generation (AT&T syntax)
    codegen_program(prog);

    // Cleanup (basic; not exhaustive)
    free(src);
    // Note: many allocations remain (tokens, AST, types). For a short-lived compiler process this is acceptable.

    return 0;
}
