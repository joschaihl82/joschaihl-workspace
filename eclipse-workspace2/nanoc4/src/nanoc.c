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
	int pos = (int) (loc - current_input);
	fprintf(stderr, "\n--- COMPILE ERROR ---\n");
	fprintf(stderr, "%s\n", current_input);
	fprintf(stderr, "%*s^ ", pos, "");
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n---------------------\n");
	va_end(ap);
	exit(1);
}

/* ----------------------------------------------------------------------
 * TINY PREPROCESSOR (inline includes only, with /usr/include fallback)
 * - Supports #include "file" and #include <file>
 * - Recursively inlines included files before tokenization
 * - No macro support, no conditional compilation
 * ---------------------------------------------------------------------- */

/* Read entire file into a newly allocated buffer (caller must free) */
static char* read_file(const char *path) {
    FILE *fp = fopen(path, "rb");
    if (!fp) {
        return NULL;
    }
    if (fseek(fp, 0, SEEK_END) != 0) {
        fclose(fp);
        return NULL;
    }
    long size = ftell(fp);
    if (size < 0) size = 0;
    rewind(fp);
    char *buf = calloc((size_t)size + 1, 1);
    if (!buf) {
        fclose(fp);
        return NULL;
    }
    size_t n = fread(buf, 1, (size_t)size, fp);
    buf[n] = '\0';
    fclose(fp);
    return buf;
}

/* Helper: append string to dynamic buffer */
static void append_to_buf(char **outp, size_t *outcap, size_t *outlen, const char *s, size_t slen) {
    if (*outlen + slen + 1 > *outcap) {
        size_t newcap = (*outcap == 0) ? (slen + 1024) : (*outcap * 2 + slen);
        *outp = realloc(*outp, newcap);
        *outcap = newcap;
    }
    memcpy(*outp + *outlen, s, slen);
    *outlen += slen;
    (*outp)[*outlen] = '\0';
}

/* Very small preprocess: inline includes only. No macro support. */
static char* preprocess(const char *input) {
    const char *p = input;
    char *out = NULL;
    size_t outcap = 0;
    size_t outlen = 0;

    while (*p) {
        /* Detect line start and optional leading whitespace */
        const char *line_start = p;
        const char *scan = p;
        while (*scan && *scan != '\n' && isspace((unsigned char)*scan)) scan++;
        if (*scan == '#') {
            const char *hash = scan;
            const char *q = hash + 1;
            while (isspace((unsigned char)*q)) q++;
            if (!strncmp(q, "include", 7) && isspace((unsigned char)q[7])) {
                q += 7;
                while (isspace((unsigned char)*q)) q++;
                char fname[1024] = {0};
                int fi = 0;
                char delim = 0;
                if (*q == '"' || *q == '<') {
                    delim = *q++;
                    while (*q && *q != (delim == '<' ? '>' : '"') && fi < (int)sizeof(fname)-1) {
                        fname[fi++] = *q++;
                    }
                    fname[fi] = '\0';
                    if (*q) q++; /* skip closing quote or > */
                } else {
                    /* malformed include: copy the line as-is and continue */
                    const char *ln = hash;
                    while (*ln && *ln != '\n') ln++;
                    append_to_buf(&out, &outcap, &outlen, hash, (size_t)(ln - hash));
                    if (*ln == '\n') {
                        append_to_buf(&out, &outcap, &outlen, "\n", 1);
                        p = ln + 1;
                    } else {
                        p = ln;
                    }
                    continue;
                }

                /* Read included file and inline it.
                   Try the filename as given first; if not found, try /usr/include/<fname>.
                   For angle-bracket includes (<...>) it's common to prefer system include paths,
                   but here we try given name first then /usr/include as a fallback for simplicity. */
                char *inc = NULL;

                /* Try to open the file as given first */
                {
                    FILE *fp = fopen(fname, "rb");
                    if (fp) {
                        fclose(fp);
                        inc = read_file(fname);
                    }
                }

                /* If not found, try /usr/include/<fname> as a fallback */
                if (!inc) {
                    char sys_path[2048];
                    snprintf(sys_path, sizeof(sys_path), "/usr/include/%s", fname);
                    FILE *fp2 = fopen(sys_path, "rb");
                    if (fp2) {
                        fclose(fp2);
                        inc = read_file(sys_path);
                    }
                }

                if (!inc) {
                    fprintf(stderr, "error: cannot open include file '%s' or '/usr/include/%s'\n", fname, fname);
                    exit(1);
                }

                /* Recursively preprocess included content to allow nested includes */
                char *inc_pp = preprocess(inc);
                append_to_buf(&out, &outcap, &outlen, inc_pp, strlen(inc_pp));
                free(inc_pp);
                free(inc);

                /* skip to end of current line in original input */
                const char *ln = q;
                while (*ln && *ln != '\n') ln++;
                if (*ln == '\n') {
                    append_to_buf(&out, &outcap, &outlen, "\n", 1);
                    p = ln + 1;
                } else {
                    p = ln;
                }
                continue;
            }
            /* not an include: fallthrough to copy the line */
        }

        /* copy one character */
        append_to_buf(&out, &outcap, &outlen, p, 1);
        p++;
    }

    if (!out) {
        out = calloc(1, 1);
    }
    return out;
}

/* ----------------------------------------------------------------------
 * LEXER
 * ---------------------------------------------------------------------- */

typedef enum {
	TOK_EOF = 0,
	// Keywords
	TOK_INT,
	TOK_CHAR,
	TOK_LONG,
	TOK_UNSIGNED,
	TOK_RETURN,
	TOK_IF,
	TOK_ELSE,
	TOK_WHILE,
	TOK_FOR,
	TOK_STRUCT,
	TOK_VOID,
	TOK_BREAK,
	TOK_CONTINUE,
	// Literals and identifiers
	TOK_IDENT,
	TOK_NUMBER,
	TOK_CHAR_LITERAL,
	TOK_STRING_LITERAL,
	// Operators and punctuation
	TOK_SEMI,
	TOK_COMMA,
	TOK_LPAREN,
	TOK_RPAREN,
	TOK_LBRACE,
	TOK_RBRACE,
	TOK_LBRACKET,
	TOK_RBRACKET,
	TOK_PLUS,
	TOK_MINUS,
	TOK_STAR,
	TOK_SLASH,
	TOK_EQ,
	TOK_NEQ,
	TOK_ASSIGN,
	TOK_LT,
	TOK_LE,
	TOK_GT,
	TOK_GE,
	TOK_AMP,
	TOK_AND,
	TOK_OR,
	TOK_NOT,
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

static Token* tokenize(const char *p_in) {
	// Build linked list of tokens first
	current_input = (char*) p_in;
	Token head = { 0 };
	Token *cur = &head;
	const char *p = p_in;

	while (*p) {
		// Skip whitespace
		if (isspace((unsigned char )*p)) {
			p++;
			continue;
		}

		// Comments
		if (p[0] == '/' && p[1] == '/') {
			p += 2;
			while (*p && *p != '\n')
				p++;
			continue;
		}
		if (p[0] == '/' && p[1] == '*') {
			p += 2;
			while (*p && !(p[0] == '*' && p[1] == '/'))
				p++;
			if (*p)
				p += 2;
			continue;
		}

		// Numbers (decimal only for now)
		if (isdigit((unsigned char )*p)) {
			cur = cur->next = calloc(1, sizeof(Token));
			cur->type = TOK_NUMBER;
			cur->loc = (char*) p;
			char *end;
			cur->val = strtol(p, &end, 10);
			p = end;
			continue;
		}

		// Identifiers and keywords
		if (isalpha((unsigned char)*p) || *p == '_') {
			const char *start = p;
			int len = 0;
			while (isalnum((unsigned char)p[len]) || p[len] == '_')
				len++;
			char *s = strndup(start, len);
			cur = cur->next = calloc(1, sizeof(Token));
			cur->loc = (char*) start;
			cur->str = s;
			// Check keywords
			if (!strcmp(s, "int"))
				cur->type = TOK_INT;
			else if (!strcmp(s, "char"))
				cur->type = TOK_CHAR;
			else if (!strcmp(s, "long"))
				cur->type = TOK_LONG;
			else if (!strcmp(s, "unsigned"))
				cur->type = TOK_UNSIGNED;
			else if (!strcmp(s, "return"))
				cur->type = TOK_RETURN;
			else if (!strcmp(s, "if"))
				cur->type = TOK_IF;
			else if (!strcmp(s, "else"))
				cur->type = TOK_ELSE;
			else if (!strcmp(s, "while"))
				cur->type = TOK_WHILE;
			else if (!strcmp(s, "for"))
				cur->type = TOK_FOR;
			else if (!strcmp(s, "struct"))
				cur->type = TOK_STRUCT;
			else if (!strcmp(s, "void"))
				cur->type = TOK_VOID;
			else if (!strcmp(s, "break"))
				cur->type = TOK_BREAK;
			else if (!strcmp(s, "continue"))
				cur->type = TOK_CONTINUE;
			else
				cur->type = TOK_IDENT;
			p += len;
			continue;
		}

		// Character literal
		if (*p == '\'') {
			cur = cur->next = calloc(1, sizeof(Token));
			cur->loc = (char*) p;
			p++;
			int c = 0;
			if (*p == '\\') {
				p++;
				if (*p == 'n')
					c = '\n';
				else if (*p == 't')
					c = '\t';
				else if (*p == '\\')
					c = '\\';
				else
					c = *p;
				p++;
			} else {
				c = *p++;
			}
			if (*p != '\'')
				error_at((char*) p, "Unterminated character literal");
			p++;
			cur->type = TOK_CHAR_LITERAL;
			cur->val = c;
			continue;
		}

		// String literal
		if (*p == '"') {
			cur = cur->next = calloc(1, sizeof(Token));
			cur->loc = (char*) p;
			p++;
			char buf[MAX_TOKEN_LEN];
			int i = 0;
			while (*p && *p != '"') {
				if (*p == '\\') {
					p++;
					if (*p == 'n')
						buf[i++] = '\n';
					else if (*p == 't')
						buf[i++] = '\t';
					else if (*p == '\\')
						buf[i++] = '\\';
					else
						buf[i++] = *p;
					p++;
				} else {
					buf[i++] = *p++;
				}
				if (i >= MAX_TOKEN_LEN - 1)
					break;
			}
			if (*p != '"')
				error_at((char*) p, "Unterminated string literal");
			buf[i] = '\0';
			p++;
			cur->type = TOK_STRING_LITERAL;
			cur->str = strndup(buf, i);
			continue;
		}

		// Multi-char operators
		if (!strncmp(p, "==", 2)) {
			cur = cur->next = calloc(1, sizeof(Token));
			cur->type = TOK_EQ;
			cur->loc = (char*) p;
			p += 2;
			continue;
		}
		if (!strncmp(p, "!=", 2)) {
			cur = cur->next = calloc(1, sizeof(Token));
			cur->type = TOK_NEQ;
			cur->loc = (char*) p;
			p += 2;
			continue;
		}
		if (!strncmp(p, "<=", 2)) {
			cur = cur->next = calloc(1, sizeof(Token));
			cur->type = TOK_LE;
			cur->loc = (char*) p;
			p += 2;
			continue;
		}
		if (!strncmp(p, ">=", 2)) {
			cur = cur->next = calloc(1, sizeof(Token));
			cur->type = TOK_GE;
			cur->loc = (char*) p;
			p += 2;
			continue;
		}
		if (!strncmp(p, "&&", 2)) {
			cur = cur->next = calloc(1, sizeof(Token));
			cur->type = TOK_AND;
			cur->loc = (char*) p;
			p += 2;
			continue;
		}
		if (!strncmp(p, "||", 2)) {
			cur = cur->next = calloc(1, sizeof(Token));
			cur->type = TOK_OR;
			cur->loc = (char*) p;
			p += 2;
			continue;
		}
		if (!strncmp(p, "->", 2)) {
			cur = cur->next = calloc(1, sizeof(Token));
			cur->type = TOK_ARROW;
			cur->loc = (char*) p;
			p += 2;
			continue;
		}

		// Single-char tokens
		cur = cur->next = calloc(1, sizeof(Token));
		cur->loc = (char*) p;
		switch (*p++) {
		case '+':
			cur->type = TOK_PLUS;
			break;
		case '-':
			cur->type = TOK_MINUS;
			break;
		case '*':
			cur->type = TOK_STAR;
			break;
		case '/':
			cur->type = TOK_SLASH;
			break;
		case '(':
			cur->type = TOK_LPAREN;
			break;
		case ')':
			cur->type = TOK_RPAREN;
			break;
		case '{':
			cur->type = TOK_LBRACE;
			break;
		case '}':
			cur->type = TOK_RBRACE;
			break;
		case '[':
			cur->type = TOK_LBRACKET;
			break;
		case ']':
			cur->type = TOK_RBRACKET;
			break;
		case ';':
			cur->type = TOK_SEMI;
			break;
		case ',':
			cur->type = TOK_COMMA;
			break;
		case '=':
			cur->type = TOK_ASSIGN;
			break;
		case '<':
			cur->type = TOK_LT;
			break;
		case '>':
			cur->type = TOK_GT;
			break;
		case '&':
			cur->type = TOK_AMP;
			break;
		case '!':
			cur->type = TOK_NOT;
			break;
		default:
			cur->type = TOK_UNKNOWN;
			error_at((char*) p - 1, "Unexpected character '%c'", *(p - 1));
		}
	}

	cur = cur->next = calloc(1, sizeof(Token));
	cur->type = TOK_EOF;
	cur->loc = (char*) p;

	// Convert linked list to array for random access
	int count = 0;
	for (Token *t = head.next; t; t = t->next)
		count++;
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
static Token* cur_token(void) {
	return &tokens[token_idx];
}
static Token* peek_token(int offset) {
	return &tokens[token_idx + offset];
}
static int at_eof(void) {
	return cur_token()->type == TOK_EOF;
}

static Token* consume_token(TokenType ty) {
	if (cur_token()->type != ty) {
		error_at(cur_token()->loc, "Expected token %d but got %d", ty,
				cur_token()->type);
	}
	return &tokens[token_idx++];
}

static int consume_if(TokenType ty) {
	if (cur_token()->type == ty) {
		token_idx++;
		return 1;
	}
	return 0;
}

static Token* consume_ident(void) {
	if (cur_token()->type != TOK_IDENT)
		error_at(cur_token()->loc, "Expected identifier");
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

static Type* type_int(void) {
	Type *t = calloc(1, sizeof(Type));
	t->kind = TYPEK_INT;
	return t;
}
static Type* type_char(void) {
	Type *t = calloc(1, sizeof(Type));
	t->kind = TYPEK_CHAR;
	return t;
}
static Type* type_long(void) {
	Type *t = calloc(1, sizeof(Type));
	t->kind = TYPEK_LONG;
	return t;
}
static Type* type_void(void) {
	Type *t = calloc(1, sizeof(Type));
	t->kind = TYPEK_VOID;
	return t;
}
static Type* type_ptr(Type *base) {
	Type *t = calloc(1, sizeof(Type));
	t->kind = TYPEK_PTR;
	t->pointer_level = base ? base->pointer_level + 1 : 1;
	t->base = base;
	return t;
}
static Type* type_array(Type *base, int size) {
	Type *t = calloc(1, sizeof(Type));
	t->kind = TYPEK_ARRAY;
	t->array_size = size;
	t->base = base;
	return t;
}

/* Compute size of type in bytes (simplified) */
int get_type_size(Type *t) {
	if (!t)
		return 8;
	if (t->pointer_level > 0 || t->kind == TYPEK_PTR)
		return 8;
	switch (t->kind) {
	case TYPEK_LONG:
		return 8;
	case TYPEK_INT:
		return 4;
	case TYPEK_CHAR:
		return 1;
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

static ASTNode* new_node(ASTKind k, Token *tok) {
	ASTNode *n = calloc(1, sizeof(ASTNode));
	n->kind = k;
	n->tok = tok;
	return n;
}

static void ast_add_child(ASTNode *parent, ASTNode *child) {
	if (!parent || !child)
		return;
	if (parent->child_count >= MAX_AST_CHILDREN)
		error_at(parent->tok ? parent->tok->loc : current_input,
				"AST node has too many children");
	parent->children[parent->child_count++] = child;
}

/* ----------------------------------------------------------------------
 * SYMBOL TABLE HELPERS
 * ---------------------------------------------------------------------- */

static Var* add_local(Function *f, char *name, Type *type) {
	Var *v = calloc(1, sizeof(Var));
	v->name = name;
	v->type = type;
	int size = get_type_size(type);
	if (size <= 0)
		size = 8;
	// allocate on stack: increase stack_size and set offset
	f->stack_size = ALIGN_TO(f->stack_size + size, 8);
	v->offset = f->stack_size;
	v->next = f->locals;
	f->locals = v;
	return v;
}

static Var* find_var_in_function(Function *f, const char *name) {
	for (Var *v = f->locals; v; v = v->next) {
		if (!strcmp(v->name, name))
			return v;
	}
	for (int i = 0; i < f->num_params; i++) {
		if (f->params[i] && !strcmp(f->params[i]->name, name))
			return f->params[i];
	}
	return NULL;
}

/* ----------------------------------------------------------------------
 * PARSER
 * ---------------------------------------------------------------------- */

static Type* parse_type_specifier(void);

static ASTNode* parse_expression(void);
static ASTNode* parse_assignment(void);
static ASTNode* parse_logical_or(void);
static ASTNode* parse_logical_and(void);
static ASTNode* parse_equality(void);
static ASTNode* parse_relational(void);
static ASTNode* parse_add(void);
static ASTNode* parse_mul(void);
static ASTNode* parse_unary(void);
static ASTNode* parse_primary(void);
static ASTNode* parse_statement(void);
static ASTNode* parse_compound(void);
static ASTNode* parse_function(void);
static ASTNode* parse_program(void);

static int is_type_keyword(Token *t) {
	if (!t)
		return 0;
	return t->type == TOK_INT || t->type == TOK_CHAR || t->type == TOK_LONG
			|| t->type == TOK_VOID || t->type == TOK_UNSIGNED;
}

static Type* parse_type_specifier(void) {
	// Basic types
	int is_unsigned = 0;
	if (consume_if(TOK_UNSIGNED))
		is_unsigned = 1;

	Type *base = NULL;
	if (consume_if(TOK_INT))
		base = type_int();
	else if (consume_if(TOK_CHAR))
		base = type_char();
	else if (consume_if(TOK_LONG))
		base = type_long();
	else if (consume_if(TOK_VOID))
		base = type_void();
	else
		error_at(cur_token()->loc, "Expected type specifier");

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
		Type *arr = type_array(base, (int) size_tok->val);
		return arr;
	}

	return base;
}

/* Primary expressions */
static ASTNode* parse_primary(void) {
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
		if (string_table_count >= MAX_STRING_LITERALS)
			error_at(t->loc, "Too many string literals");
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
static ASTNode* parse_unary(void) {
	Token *t = cur_token();
	if (consume_if(TOK_PLUS))
		return parse_unary();
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
static ASTNode* parse_mul(void) {
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
static ASTNode* parse_add(void) {
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
static ASTNode* parse_relational(void) {
	ASTNode *node = parse_add();
	while (cur_token()->type == TOK_LT || cur_token()->type == TOK_GT
			|| cur_token()->type == TOK_LE || cur_token()->type == TOK_GE) {
		Token *op = &tokens[token_idx++];
		ASTNode *n = new_node(AST_BINOP, op);
		if (op->type == TOK_LT)
			n->op = '<';
		else if (op->type == TOK_GT)
			n->op = '>';
		else if (op->type == TOK_LE)
			n->op = 'l'; // 'l' for <=
		else
			n->op = 'g'; // 'g' for >=
		ast_add_child(n, node);
		ASTNode *r = parse_add();
		ast_add_child(n, r);
		node = n;
	}
	return node;
}

/* Equality */
static ASTNode* parse_equality(void) {
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
static ASTNode* parse_logical_and(void) {
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
static ASTNode* parse_logical_or(void) {
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
static ASTNode* parse_assignment(void) {
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

static ASTNode* parse_expression(void) {
	return parse_assignment();
}

/* Statement parsing */

static ASTNode* parse_compound(void) {
	consume_token(TOK_LBRACE);
	ASTNode *blk = new_node(AST_COMPOUND, cur_token());
	while (!consume_if(TOK_RBRACE)) {
		ASTNode *s = parse_statement();
		ast_add_child(blk, s);
	}
	return blk;
}

static ASTNode* parse_statement(void) {
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
static ASTNode* parse_function(void) {
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
			if (consume_if(TOK_COMMA))
				continue;
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
static ASTNode* parse_program(void) {
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
	if (global_function_count >= MAX_FUNCTIONS)
		error_at(current_input, "Too many functions");
	global_functions[global_function_count++] = f;
}

static void semantic_resolve(ASTNode *node, Function *curf);

static void semantic_program(ASTNode *prog) {
	// Register functions and perform per-function semantic checks
	for (int i = 0; i < prog->child_count; i++) {
		ASTNode *fnnode = prog->children[i];
		if (fnnode->kind != AST_FUNC)
			error_at(fnnode->tok ? fnnode->tok->loc : current_input,
					"Top-level node is not a function");
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
	if (!node)
		return;
	switch (node->kind) {
	case AST_COMPOUND:
		for (int i = 0; i < node->child_count; i++)
			semantic_resolve(node->children[i], curf);
		break;
	case AST_VAR_DECL: {
		// Add to locals
		Token *id = node->tok;
		if (!id || id->type != TOK_IDENT)
			error_at(node->tok ? node->tok->loc : current_input,
					"Invalid variable declaration");
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
		if (!v)
			error_at(node->tok->loc, "Unknown identifier: %s", node->str_val);
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
		for (int i = 0; i < node->child_count; i++)
			semantic_resolve(node->children[i], curf);
		node->dtype = type_int(); // assume int return
		break;
	case AST_UNOP:
	case AST_DEREF:
	case AST_ADDR:
		semantic_resolve(node->children[0], curf);
		// set dtype for deref/addr
		if (node->kind == AST_DEREF) {
			if (!node->children[0]->dtype
					|| node->children[0]->dtype->pointer_level == 0) {
				error_at(node->tok->loc, "Dereferencing non-pointer");
			}
			node->dtype =
					node->children[0]->dtype->base ?
							node->children[0]->dtype->base : type_int();
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
		for (int i = 0; i < node->child_count; i++)
			semantic_resolve(node->children[i], curf);
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
		if (node->child_count > 2 && node->children[2])
			semantic_resolve(node->children[2], curf);
		break;
	case AST_WHILE:
		semantic_resolve(node->children[0], curf);
		semantic_resolve(node->children[1], curf);
		break;
	case AST_FOR:
		if (node->child_count > 0 && node->children[0])
			semantic_resolve(node->children[0], curf);
		if (node->child_count > 1 && node->children[1])
			semantic_resolve(node->children[1], curf);
		if (node->child_count > 2 && node->children[2])
			semantic_resolve(node->children[2], curf);
		if (node->child_count > 3 && node->children[3])
			semantic_resolve(node->children[3], curf);
		break;
	case AST_RETURN:
		if (node->child_count > 0 && node->children[0])
			semantic_resolve(node->children[0], curf);
		break;
	case AST_BREAK:
	case AST_CONTINUE:
		// validity checked in codegen (presence of labels)
		break;
	default:
		// fallback: resolve children
		for (int i = 0; i < node->child_count; i++)
			semantic_resolve(node->children[i], curf);
		break;
	}
}

/* ----------------------------------------------------------------------
 * (The rest of your compiler: code generation, utilities, etc.)
 * ---------------------------------------------------------------------- */

/* Integration example: call preprocess before tokenize and parsing.
   Adapt this main to your existing main or replace your current main with it. */

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <source.c>\n", argv[0]);
        return 1;
    }

    /* Read original source */
    char *src = read_file(argv[1]);
    if (!src) {
        fprintf(stderr, "error: cannot open source file '%s'\n", argv[1]);
        return 1;
    }

    /* Preprocess (inline includes, with /usr/include fallback) */
    char *pp_src = preprocess(src);
    free(src);

    /* Tokenize preprocessed source */
    tokens = tokenize(pp_src);
    token_idx = 0;

    /* Parse program */
    ASTNode *prog = parse_program();

    /* Semantic analysis */
    semantic_program(prog);

    /* Continue with code generation as before (not shown here) */

    /* Cleanup (not exhaustive) */
    free(pp_src);

    return 0;
}
