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
 * memory operands like -8(%rbp), instruction operand order is src, dest.
 * - push/pop use pushq/popq with register operands (e.g., pushq %rax).
 * - movq $imm, %rax for immediates; leaq -8(%rbp), %rax for addresses.
 *
 * This is educational code and not production-grade.
 * All arrays and limits have been made dynamic.
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

#define TEMP_TOKEN_BUFFER_SIZE 1024 // local buffer for string/char literal parsing
#define ALIGN_TO(n, a) (((n) + ((a)-1)) & ~((a)-1))

/* ----------------------------------------------------------------------
 * error handling
 * ---------------------------------------------------------------------- */

static char *current_input = NULL;

/* * modified: shows only the line where the error occurred.
 */
void error_at(char *loc, const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);

	// 1. find line start: scan backwards until \n or beginning of input
	char *line_start = loc;
	while (line_start > current_input && line_start[-1] != '\n') {
		line_start--;
	}

	// 2. find line end: scan forwards until \n or end of input
	char *line_end = loc;
	while (*line_end != '\0' && *line_end != '\n') {
		line_end++;
	}

	// 3. calculate position of 'loc' relative to line start
	int line_pos = (int) (loc - line_start);

	// 4. print error preamble and the single line
	fprintf(stderr, "\n--- compile error ---\n");
	// print the line content (using precision to stop at line_end)
	fprintf(stderr, "%.*s\n", (int)(line_end - line_start), line_start);

	// 5. print the caret (^) at the correct position and the error message
	fprintf(stderr, "%*s^ ", line_pos, "");
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n---------------------\n");

	va_end(ap);
	exit(1);
}

/* ----------------------------------------------------------------------
 * tiny preprocessor (inline includes only, with /usr/include fallback)
 * ---------------------------------------------------------------------- */

/* read entire file into a newly allocated buffer (caller must free) */
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

/* helper: append string to dynamic buffer */
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

/* very small preprocess: inline includes only. no macro support. */
static char* preprocess(const char *input) {
    const char *p = input;
    char *out = NULL;
    size_t outcap = 0;
    size_t outlen = 0;

    while (*p) {
        /* detect line start and optional leading whitespace */
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

                /* read included file and inline it. */
                char *inc = NULL;

                /* try to open the file as given first */
                {
                    FILE *fp = fopen(fname, "rb");
                    if (fp) {
                        fclose(fp);
                        inc = read_file(fname);
                    }
                }
                char sys_path[2048];


                /* if not found, try /usr/include/<fname> as a fallback */
                if (!inc) {
                    snprintf(sys_path, sizeof(sys_path), "/usr/include/%s", fname);
                    FILE *fp2 = fopen(sys_path, "rb");
                    if (fp2) {
                        fclose(fp2);
                        inc = read_file(sys_path);
                    }
                }

                if (!inc) {
                    fprintf(stderr, "error: cannot open include file '%s' or '/usr/include/%s'\n", fname, sys_path);
                    exit(1);
                }

                /* recursively preprocess included content to allow nested includes */
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
 * lexer
 * ---------------------------------------------------------------------- */

typedef enum {
	TOK_EOF = 0,
	// keywords
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
	// literals and identifiers
	TOK_IDENT,
	TOK_NUMBER,
	TOK_CHAR_LITERAL,
	TOK_STRING_LITERAL,
	// operators and punctuation
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
	TOK_ARROW, // ->
	TOK_INC, // ++
	TOK_DEC, // --
	TOK_UNKNOWN
} TokenType;

typedef struct Token Token;
struct Token {
	TokenType type;
	char *str;      // for identifiers and string literals
	long val;       // for numeric and char literals / string literal id
	char *loc;      // pointer into original input (for error reporting)
	Token *next;    // used during initial lexing
};

static Token *tokens = NULL; // token array pointer
static int token_idx = 0;    // index into tokens array

// string literal table (dynamic array)
typedef struct {
	int id;
	char *s;
} StringLit;
static StringLit *string_table = NULL;
static int string_table_count = 0;
static int string_table_capacity = 0;

static Token* tokenize(const char *p_in) {
	// build linked list of tokens first
	current_input = (char*) p_in;
	Token head = { 0 };
	Token *cur = &head;
	const char *p = p_in;

	// reset string table for a fresh run
	string_table_count = 0;
	if (string_table) free(string_table);
	string_table = NULL;
	string_table_capacity = 0;

	while (*p) {
		// skip whitespace
		if (isspace((unsigned char )*p)) {
			p++;
			continue;
		}

		// comments
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

		// numbers (decimal only for now)
		if (isdigit((unsigned char )*p)) {
			cur = cur->next = calloc(1, sizeof(Token));
			cur->type = TOK_NUMBER;
			cur->loc = (char*) p;
			char *end;
			cur->val = strtol(p, &end, 10);
			p = end;
			continue;
		}

		// identifiers and keywords
		if (isalpha((unsigned char)*p) || *p == '_') {
			const char *start = p;
			int len = 0;
			while (isalnum((unsigned char)p[len]) || p[len] == '_')
				len++;
			char *s = strndup(start, len);
			cur = cur->next = calloc(1, sizeof(Token));
			cur->loc = (char*) start;
			cur->str = s;
			// check keywords
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

		// character literal
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
				error_at((char*) p, "unterminated character literal");
			p++;
			cur->type = TOK_CHAR_LITERAL;
			cur->val = c;
			continue;
		}

		// string literal
		if (*p == '"') {
			cur = cur->next = calloc(1, sizeof(Token));
			cur->loc = (char*) p;
			p++;
			char buf[TEMP_TOKEN_BUFFER_SIZE];
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
				if (i >= TEMP_TOKEN_BUFFER_SIZE - 1)
					error_at((char*) p, "string literal too long");
			}
			if (*p != '"')
				error_at((char*) p, "unterminated string literal");
			buf[i] = '\0';
			p++;
			cur->type = TOK_STRING_LITERAL;
			cur->str = strndup(buf, i);

			// register string literal in dynamic table
			if (string_table_count >= string_table_capacity) {
				int new_cap = string_table_capacity == 0 ? 16 : string_table_capacity * 2;
				string_table = realloc(string_table, new_cap * sizeof(StringLit));
				string_table_capacity = new_cap;
			}
			// copy string literal to global table
			string_table[string_table_count].id = string_table_count;
			string_table[string_table_count].s = strdup(cur->str);
			// store the id in the token's value field
			cur->val = string_table_count;
			string_table_count++;

			continue;
		}

		// multi-char operators
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
		// added: ++
		if (!strncmp(p, "++", 2)) {
			cur = cur->next = calloc(1, sizeof(Token));
			cur->type = TOK_INC;
			cur->loc = (char*) p;
			p += 2;
			continue;
		}
		// added: --
		if (!strncmp(p, "--", 2)) {
			cur = cur->next = calloc(1, sizeof(Token));
			cur->type = TOK_DEC;
			cur->loc = (char*) p;
			p += 2;
			continue;
		}


		// single-char tokens
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
			error_at((char*) p - 1, "unexpected character '%c'", *(p - 1));
		}
	}

	cur = cur->next = calloc(1, sizeof(Token));
	cur->type = TOK_EOF;
	cur->loc = (char*) p;

	// convert linked list to array for random access
	int count = 0;
	for (Token *t = head.next; t; t = t->next)
		count++;
	Token *arr = calloc(count, sizeof(Token));
	Token *t = head.next;
	for (int i = 0; i < count; i++) {
		arr[i] = *t;
		t = t->next;
	}
	return arr;
}

/* token helpers */
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
		error_at(cur_token()->loc, "expected token %d but got %d", ty,
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
		error_at(cur_token()->loc, "expected identifier");
	return &tokens[token_idx++];
}

/* ----------------------------------------------------------------------
 * types
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

typedef struct Type Type;
struct Type {
	BaseTypeKind kind;
	int is_unsigned;
	int pointer_level;
	int array_size;      // for arrays
	struct Type *base;   // base type for pointers/arrays
};

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

/* compute size of type in bytes (simplified) */
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
		if (!t->base) return 8;
		return t->array_size * get_type_size(t->base);
	case TYPEK_VOID:
		return 0; // cannot size a void type
	case TYPEK_STRUCT:
		return 8; // placeholder
	default:
		return 8;
	}
}

/* ----------------------------------------------------------------------
 * ast
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
	AST_CAST,
	AST_PRE_INC,   // <--- added
	AST_PRE_DEC,   // <--- added
	AST_POST_INC,  // <--- added
	AST_POST_DEC   // <--- added
} ASTKind;

typedef struct Var Var;
struct Var {
	char *name;
	Type *type;
	int offset;         // offset from rbp (positive number)
	struct Var *next;
};

typedef struct Function Function;
struct Function {
	char *name;
	Type *ret_type;
	Var **params;       // dynamic array of var*
	int num_params;
	int param_capacity; // new member for dynamic array
	Var *locals;        // linked list
	int stack_size;
	struct ASTNode *body;
	int label_break;
	int label_continue;
};

typedef struct ASTNode ASTNode;
struct ASTNode {
	ASTKind kind;
	Token *tok;                 // token associated (for error reporting)
	Type *dtype;                // computed type
	struct ASTNode **children;  // dynamic array of astnode*
	int child_count;
	int child_capacity;         // new member for dynamic array
	// for specific node kinds:
	long num_val;               // for ast_num, ast_char, and ast_string id
	char *str_val;              // for ast_string and ast_ident
	char op;                    // for simple binop/unop representation
	Function *func_def;         // for ast_func
	Var *var_def;               // for ast_var_decl or ast_ident resolution
};

static ASTNode* new_node(ASTKind k, Token *tok) {
	ASTNode *n = calloc(1, sizeof(ASTNode));
	n->kind = k;
	n->tok = tok;
	n->children = NULL;
	n->child_capacity = 0;
	return n;
}

static void ast_add_child(ASTNode *parent, ASTNode *child) {
	if (!parent)
		return;
    // note: null children are sometimes added in ast_for/ast_if, so we allow it.
	if (parent->child_count >= parent->child_capacity) {
		// grow array
		int new_cap = parent->child_capacity == 0 ? 4 : parent->child_capacity * 2;
		parent->children = realloc(parent->children, new_cap * sizeof(ASTNode*));
		parent->child_capacity = new_cap;
	}
	parent->children[parent->child_count++] = child;
}

/* ----------------------------------------------------------------------
 * symbol table helpers
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
	return NULL;
}

/* ----------------------------------------------------------------------
 * parser
 * ---------------------------------------------------------------------- */

static Type* parse_type_specifier(void);
static ASTNode* parse_expression(void);
static ASTNode* parse_statement(void);
static ASTNode* parse_compound(void);
static ASTNode* parse_function(void);

static int is_type_keyword(Token *t) {
	if (!t)
		return 0;
	return t->type == TOK_INT || t->type == TOK_CHAR || t->type == TOK_LONG
			|| t->type == TOK_VOID || t->type == TOK_UNSIGNED;
}

static Type* parse_type_specifier(void) {
	// basic types
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
	else if (consume_if(TOK_STRUCT)) {
		// struct tag is here for declarations like `struct node *p;`
		consume_ident(); // struct name
		base = type_int(); // placeholder type
	}
	else
		error_at(cur_token()->loc, "expected type specifier");

	base->is_unsigned = is_unsigned;

	// pointer levels
	while (consume_if(TOK_STAR)) {
		Type *p = type_ptr(base);
		p->base = base;
		base = p;
	}

	// note: array parsing logic removed here and moved to parse_statement
	//       to correctly handle variable declarations like `int x[10];`

	return base;
}

/* renamed from parse_primary: handles basic literals, (expr), and simple identifiers (not function calls) */
static ASTNode* parse_term(void) {
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
		n->num_val = t->val;
		Type *pt = type_ptr(type_char());
		n->dtype = pt;
		return n;
	}
	if (t->type == TOK_IDENT) {
		Token *idtok = consume_ident();
		ASTNode *n = new_node(AST_IDENT, idtok);
		n->str_val = strdup(idtok->str);
		return n;
	}

	error_at(t->loc, "expected primary expression");
	return NULL;
}

/* new function: handles primary expressions followed by postfix operators (e.g., [], (), ++, --, ->) */
static ASTNode* parse_postfix(void) {
	// 1. get the basic term (e.g., `a`, `10`, `(b+c)`)
	ASTNode *node = parse_term();

	while (1) {
		Token *t = cur_token();

		// function call: ident(...)
		if (consume_if(TOK_LPAREN)) {
			ASTNode *call = new_node(AST_CALL, t);
			// the function identifier is the current 'node' (ast_ident)
			ast_add_child(call, node);
			// parse args
			if (!consume_if(TOK_RPAREN)) {
				ast_add_child(call, parse_expression());
				while (consume_if(TOK_COMMA)) {
					ast_add_child(call, parse_expression());
				}
				consume_token(TOK_RPAREN);
			}
			call->dtype = type_int(); // placeholder
			node = call;
			continue;
		}

		// postfix increment (expr++)
		if (consume_if(TOK_INC)) {
			ASTNode *n = new_node(AST_POST_INC, &tokens[token_idx - 1]);
			ast_add_child(n, node);
			node = n;
			continue;
		}
		// postfix decrement (expr--)
		if (consume_if(TOK_DEC)) {
			ASTNode *n = new_node(AST_POST_DEC, &tokens[token_idx - 1]);
			ast_add_child(n, node);
			node = n;
			continue;
		}

		// array indexing: a[b] is syntactic sugar for *(a + b)
		if (consume_if(TOK_LBRACKET)) {
			ASTNode *idx = parse_expression();
			consume_token(TOK_RBRACKET);

			// create add node (a + b)
			ASTNode *add_node = new_node(AST_BINOP, node->tok);
			add_node->op = '+';
			ast_add_child(add_node, node);
			ast_add_child(add_node, idx);

			// create deref node *(a + b)
			ASTNode *deref_node = new_node(AST_DEREF, node->tok);
			ast_add_child(deref_node, add_node);
			node = deref_node;
			continue;
		}

		// member access (->) - minimal support for token consumption, full
		// implementation requires struct type handling in semantic analysis.
		if (consume_if(TOK_ARROW)) {
			// p->m is equivalent to (*p).m. we just parse the tokens.
			ASTNode *deref_node = new_node(AST_DEREF, &tokens[token_idx - 1]);
			ast_add_child(deref_node, node);
			node = deref_node;

			// consume member name: requires type-checking later to find offset
			consume_ident();
			continue;
		}

		return node;
	}
}


/* unary: handles prefix operators */
static ASTNode* parse_unary(void) {
	Token *t = cur_token();

	// prefix increment (pre-inc)
    if (consume_if(TOK_INC)) {
        ASTNode *n = new_node(AST_PRE_INC, t);
        ast_add_child(n, parse_unary());
        return n;
    }
	// prefix decrement (pre-dec)
    if (consume_if(TOK_DEC)) {
        ASTNode *n = new_node(AST_PRE_DEC, t);
        ast_add_child(n, parse_unary());
        return n;
    }

	if (consume_if(TOK_PLUS))
		return parse_unary();
	if (consume_if(TOK_MINUS)) {
		ASTNode *n = new_node(AST_UNOP, t);
		n->op = '-';
		ast_add_child(n, parse_unary());
		return n;
	}
	if (consume_if(TOK_NOT)) {
		ASTNode *n = new_node(AST_UNOP, t);
		n->op = '!';
		ast_add_child(n, parse_unary());
		return n;
	}
	if (consume_if(TOK_STAR)) {
		ASTNode *n = new_node(AST_DEREF, t);
		ast_add_child(n, parse_unary());
		return n;
	}
	if (consume_if(TOK_AMP)) {
		ASTNode *n = new_node(AST_ADDR, t);
		ast_add_child(n, parse_unary());
		return n;
	}
	// changed: call parse_postfix instead of parse_primary
	return parse_postfix();
}

/* multiplicative */
static ASTNode* parse_mul(void) {
	ASTNode *node = parse_unary(); // uses the updated parse_unary
	while (cur_token()->type == TOK_STAR || cur_token()->type == TOK_SLASH) {
		Token *op = &tokens[token_idx++];
		ASTNode *n = new_node(AST_BINOP, op);
		n->op = (op->type == TOK_STAR) ? '*' : '/';
		ast_add_child(n, node);
		ast_add_child(n, parse_unary());
		node = n;
	}
	return node;
}

/* additive */
static ASTNode* parse_add(void) {
	ASTNode *node = parse_mul();
	while (cur_token()->type == TOK_PLUS || cur_token()->type == TOK_MINUS) {
		Token *op = &tokens[token_idx++];
		ASTNode *n = new_node(AST_BINOP, op);
		n->op = (op->type == TOK_PLUS) ? '+' : '-';
		ast_add_child(n, node);
		ast_add_child(n, parse_mul());
		node = n;
	}
	return node;
}

/* relational */
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
		ast_add_child(n, parse_add());
		node = n;
	}
	return node;
}

/* equality */
static ASTNode* parse_equality(void) {
	ASTNode *node = parse_relational();
	while (cur_token()->type == TOK_EQ || cur_token()->type == TOK_NEQ) {
		Token *op = &tokens[token_idx++];
		ASTNode *n = new_node(AST_BINOP, op);
		n->op = (op->type == TOK_EQ) ? '=' : '!';
		ast_add_child(n, node);
		ast_add_child(n, parse_relational());
		node = n;
	}
	return node;
}

/* logical and */
static ASTNode* parse_logical_and(void) {
	ASTNode *node = parse_equality();
	while (consume_if(TOK_AND)) {
		Token *op = &tokens[token_idx - 1];
		ASTNode *n = new_node(AST_BINOP, op);
		n->op = '&';
		ast_add_child(n, node);
		ast_add_child(n, parse_equality());
		node = n;
	}
	return node;
}

/* logical or */
static ASTNode* parse_logical_or(void) {
	ASTNode *node = parse_logical_and();
	while (consume_if(TOK_OR)) {
		Token *op = &tokens[token_idx - 1];
		ASTNode *n = new_node(AST_BINOP, op);
		n->op = '|';
		ast_add_child(n, node);
		ast_add_child(n, parse_logical_and());
		node = n;
	}
	return node;
}

/* assignment */
static ASTNode* parse_assignment(void) {
	ASTNode *node = parse_logical_or();
	if (consume_if(TOK_ASSIGN)) {
		Token *op = &tokens[token_idx - 1];
		ASTNode *n = new_node(AST_ASSIGN, op);
		ast_add_child(n, node);
		ast_add_child(n, parse_assignment());
		return n;
	}
	return node;
}

static ASTNode* parse_expression(void) {
	return parse_assignment();
}

/* statement parsing */

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
	// check and consume for compound statement (block)
	if (cur_token()->type == TOK_LBRACE) {
		return parse_compound();
	}

	if (consume_if(TOK_IF)) {
		ASTNode *n = new_node(AST_IF, cur_token() - 1);
		consume_token(TOK_LPAREN);
		ast_add_child(n, parse_expression());
		consume_token(TOK_RPAREN);
		ast_add_child(n, parse_statement());
		if (consume_if(TOK_ELSE)) {
			ast_add_child(n, parse_statement());
		}
		return n;
	}

	if (consume_if(TOK_WHILE)) {
		ASTNode *n = new_node(AST_WHILE, cur_token() - 1);
		consume_token(TOK_LPAREN);
		ast_add_child(n, parse_expression());
		consume_token(TOK_RPAREN);
		ast_add_child(n, parse_statement());
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
					ast_add_child(decl, parse_expression());
				}
				consume_token(TOK_SEMI);
				ast_add_child(n, decl);
			} else {
				ast_add_child(n, parse_expression());
				consume_token(TOK_SEMI);
			}
		} else {
			ast_add_child(n, NULL);
		}
		// cond
		if (!consume_if(TOK_SEMI)) {
			ast_add_child(n, parse_expression());
			consume_token(TOK_SEMI);
		} else {
			ast_add_child(n, NULL);
		}
		// incr
		if (!consume_if(TOK_RPAREN)) {
			ast_add_child(n, parse_expression());
			consume_token(TOK_RPAREN);
		} else {
			ast_add_child(n, NULL);
		}
		ast_add_child(n, parse_statement());
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
			ast_add_child(n, parse_expression());
			consume_token(TOK_SEMI);
		}
		return n;
	}

	// declaration or struct definition
	if (is_type_keyword(cur_token()) || cur_token()->type == TOK_STRUCT) {
		Token *type_tok = cur_token();
		Type *t = parse_type_specifier();

		// handle struct definition: struct name { members... };
		if (type_tok->type == TOK_STRUCT && cur_token()->type == TOK_LBRACE) {
			// struct definition: struct name { ... } ;
			// this block only consumes the tokens and returns a dummy decl node.
			consume_token(TOK_LBRACE);
			while (!consume_if(TOK_RBRACE)) {
				// parse members: type + ident + semi
				parse_type_specifier();
				consume_ident();
				consume_token(TOK_SEMI);
			}
			consume_token(TOK_SEMI);

			// return dummy node
			ASTNode *dummy_struct = new_node(AST_VAR_DECL, type_tok);
			dummy_struct->dtype = type_void();
			return dummy_struct;
		}

		// normal declaration: type id [array_dims] [= init] ;
		Token *id = consume_ident();
		ASTNode *decl = new_node(AST_VAR_DECL, id);
		decl->dtype = t;

        // *** start: array parsing (fixed) ***
        if (consume_if(TOK_LBRACKET)) {
            int size = 0;
            if (cur_token()->type == TOK_NUMBER) {
                size = (int) consume_token(TOK_NUMBER)->val;
            } else {
                // allows char s[] syntax, size is 0 (handled by semantic analysis for strings)
                // or treated as an incomplete type/pointer
            }
            consume_token(TOK_RBRACKET);
            // the variable's type is updated to be an array of the base type
            decl->dtype = type_array(t, size);
        }
        // *** end: array parsing (fixed) ***

		if (consume_if(TOK_ASSIGN)) {
			ast_add_child(decl, parse_expression());
		}
		consume_token(TOK_SEMI);
		return decl;
	}

	// expression statement
	ASTNode *expr = parse_expression();
	consume_token(TOK_SEMI);
	ASTNode *stmt = new_node(AST_EXPR_STMT, expr->tok);
	ast_add_child(stmt, expr);
	return stmt;
}

/* function parsing */
static ASTNode* parse_function(void) {
	Type *ret = parse_type_specifier();
	Token *name_tok = consume_ident();
	ASTNode *fn = new_node(AST_FUNC, name_tok);
	Function *f = calloc(1, sizeof(Function));
	f->name = strdup(name_tok->str);
	f->ret_type = ret;
	fn->func_def = f;

	// initialize dynamic parameter array
	f->params = NULL;
	f->num_params = 0;
	f->param_capacity = 0;

	consume_token(TOK_LPAREN);
	if (!consume_if(TOK_RPAREN)) {
		while (1) {
			Type *pt = parse_type_specifier();
			Token *pname = consume_ident();
			Var *pv = calloc(1, sizeof(Var));
			pv->name = strdup(pname->str);
			pv->type = pt;

			// dynamic array resize for parameters
			if (f->num_params >= f->param_capacity) {
				int new_cap = f->param_capacity == 0 ? 4 : f->param_capacity * 2;
				f->params = realloc(f->params, new_cap * sizeof(Var*));
				f->param_capacity = new_cap;
			}

			f->params[f->num_params++] = pv;

			if (consume_if(TOK_COMMA))
				continue;
			consume_token(TOK_RPAREN);
			break;
		}
	}

	ASTNode *body = parse_compound();
	ast_add_child(fn, body);
	f->body = body;
	return fn;
}

/* program */
static ASTNode* parse_program(void) {
	ASTNode *prog = new_node(AST_PROGRAM, NULL);
	while (!at_eof()) {
        Token *t = cur_token();

        // handle top-level struct definition/declaration
        if (t->type == TOK_STRUCT) {
            ASTNode *decl = parse_statement();
            ast_add_child(prog, decl);
            continue;
        }

        // otherwise, assume it's a function definition/declaration
		ASTNode *fn = parse_function();
		ast_add_child(prog, fn);
	}
	return prog;
}

/* ----------------------------------------------------------------------
 * semantic analysis
 * ---------------------------------------------------------------------- */

static Function **global_functions = NULL; // dynamic array
static int global_function_count = 0;
static int global_function_capacity = 0;

static void register_function(Function *f) {
	if (global_function_count >= global_function_capacity) {
		int new_cap = global_function_capacity == 0 ? 16 : global_function_capacity * 2;
		global_functions = realloc(global_functions, new_cap * sizeof(Function*));
		global_function_capacity = new_cap;
	}
	global_functions[global_function_count++] = f;
}

static Function* find_function(const char *name) {
	for (int i = 0; i < global_function_count; i++) {
		if (!strcmp(global_functions[i]->name, name))
			return global_functions[i];
	}
	return NULL;
}

static void semantic_resolve(ASTNode *node, Function *curf);

static void semantic_program(ASTNode *prog) {
	// register functions
	for (int i = 0; i < prog->child_count; i++) {
		ASTNode *fnnode = prog->children[i];
		if (fnnode->kind == AST_FUNC) {
            Function *f = fnnode->func_def;
            register_function(f);
        } else if (fnnode->kind == AST_VAR_DECL && fnnode->tok->type == TOK_STRUCT) {
            // ignore dummy struct nodes
            continue;
        } else {
            error_at(fnnode->tok ? fnnode->tok->loc : current_input,
					"top-level node is not a function or struct definition");
        }
	}

	// now process each function
	for (int i = 0; i < prog->child_count; i++) {
		ASTNode *fnnode = prog->children[i];
        if (fnnode->kind != AST_FUNC) continue;

		Function *f = fnnode->func_def;

		f->stack_size = 0;
		f->locals = NULL;

		// correctly calculate parameter offsets and link into locals (reverse order for stack)
		for (int j = f->num_params - 1; j >= 0; j--) {
			Var *p = f->params[j];
			int size = get_type_size(p->type);
			if (size <= 0) size = 8;
			f->stack_size = ALIGN_TO(f->stack_size + size, 8);
			p->offset = f->stack_size;
			// link parameter directly into locals list
			p->next = f->locals;
			f->locals = p;
		}

		// walk body
		semantic_resolve(fnnode->children[0], f);
		// finalize stack size alignment to 16 bytes for abi
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
		// ignore the dummy struct declaration node
		if (node->dtype && node->dtype->kind == TYPEK_VOID && node->tok && node->tok->type == TOK_STRUCT)
			break;

		// add to locals
		Token *id = node->tok;
		if (!id || id->type != TOK_IDENT)
			error_at(node->tok ? node->tok->loc : current_input,
					"invalid variable declaration");
		Var *v = find_var_in_function(curf, id->str);
		if (v) {
			// Variable already added as parameter
			node->var_def = v;
		} else {
			// Add new local
			v = add_local(curf, strdup(id->str), node->dtype);
			node->var_def = v;
		}

		if (node->child_count > 0 && node->children[0]) {
			semantic_resolve(node->children[0], curf);
		}
		break;
	}
	case AST_IDENT: {
		// resolve variable
		Var *v = find_var_in_function(curf, node->str_val);
		if (!v) {
			// check for function name
			if (find_function(node->str_val)) {
				node->dtype = type_int(); // function pointer type (simplified)
				break;
			}
			error_at(node->tok->loc, "unknown identifier: %s", node->str_val);
		}
		node->var_def = v;
		node->dtype = v->type;

		// array decay: array used in expression decays to pointer to first element
		if (node->dtype->kind == TYPEK_ARRAY) {
			node->dtype = type_ptr(node->dtype->base);
		}
		break;
	}
	case AST_NUM:
	case AST_CHAR:
	case AST_STRING:
		break;
	case AST_CALL:
		// the function name is the first child, arguments follow
		for (int i = 0; i < node->child_count; i++)
			semantic_resolve(node->children[i], curf);
		// simplified function return type is always int for now
		Function *target_func = find_function(node->children[0]->str_val);
		node->dtype = target_func ? target_func->ret_type : type_int();
		break;
	case AST_PRE_INC:
	case AST_PRE_DEC:
	case AST_POST_INC:
	case AST_POST_DEC: {
		semantic_resolve(node->children[0], curf);
		ASTNode *operand = node->children[0];

		// check if the operand is a modifiable lvalue
		if (operand->kind != AST_IDENT && operand->kind != AST_DEREF) {
			error_at(node->tok->loc, "operand of '%s' must be a modifiable lvalue",
					node->kind == AST_PRE_INC || node->kind == AST_POST_INC ? "++" : "--");
		}
		// the type of the increment/decrement expression is the type of the operand
		node->dtype = operand->dtype;
		break;
	}
	case AST_UNOP:
	case AST_DEREF:
	case AST_ADDR:
		semantic_resolve(node->children[0], curf);
		if (node->kind == AST_DEREF) {
            // apply array decay if the expression being dereferenced is an array
            Type *t = node->children[0]->dtype;
            if (t && t->kind == TYPEK_ARRAY) {
                // decay array to a pointer to its base type
                node->children[0]->dtype = type_ptr(t->base);
                t = node->children[0]->dtype;
            }

            // after potential decay, the type must be a pointer
			if (!t || (t->pointer_level == 0 && t->kind != TYPEK_PTR)) {
				error_at(node->tok->loc, "dereferencing non-pointer");
			}

			node->dtype = t->base ? t->base : type_int();
		} else if (node->kind == AST_ADDR) {
			node->dtype = type_ptr(node->children[0]->dtype);
		} else {
			node->dtype = type_int();
		}
		break;
	case AST_BINOP:
	case AST_ASSIGN:
		for (int i = 0; i < node->child_count; i++)
			semantic_resolve(node->children[i], curf);

		if (node->kind == AST_BINOP) {
			ASTNode *lhs = node->children[0];
			ASTNode *rhs = node->children[1];

			// pointer arithmetic and array decay has been handled in parse_postfix and AST_IDENT.
			// now just check the resulting types:
			Type *lhs_type = lhs->dtype;
			Type *rhs_type = rhs->dtype;

			if (node->op == '+' || node->op == '-') {
				// P +/- I or I + P -> P
				if (lhs_type->kind == TYPEK_PTR && rhs_type->kind != TYPEK_PTR) {
					node->dtype = lhs_type;
				} else if (rhs_type->kind == TYPEK_PTR && lhs_type->kind != TYPEK_PTR) {
					node->dtype = rhs_type;
				}
				// P - P -> long (difference in elements)
				else if (lhs_type->kind == TYPEK_PTR && rhs_type->kind == TYPEK_PTR) {
					if (node->op == '-') {
						node->dtype = type_long();
					} else {
						error_at(node->tok->loc, "cannot add two pointers");
					}
				} else {
					// regular arithmetic: result is int
					node->dtype = type_int();
				}
			} else {
				// relational, equality, logical, mul/div: result is int
				node->dtype = type_int();
			}

		} else if (node->kind == AST_ASSIGN) {
			// check for assignment to non-lvalue
			if (node->children[0]->kind != AST_IDENT && node->children[0]->kind != AST_DEREF) {
				error_at(node->tok->loc, "assignment target is not an l-value");
			}
			node->dtype = node->children[0]->dtype;
		}
		break;

	case AST_EXPR_STMT:
		semantic_resolve(node->children[0], curf);
		break;
	case AST_IF:
	case AST_WHILE:
	case AST_FOR:
		for (int i = 0; i < node->child_count; i++)
			semantic_resolve(node->children[i], curf);
		break;
	case AST_RETURN:
		if (node->child_count > 0 && node->children[0])
			semantic_resolve(node->children[0], curf);
		break;
	case AST_BREAK:
	case AST_CONTINUE:
		break;
	default:
		for (int i = 0; i < node->child_count; i++)
			semantic_resolve(node->children[i], curf);
		break;
	}
}

/* ----------------------------------------------------------------------
 * code generation
 * ---------------------------------------------------------------------- */

static int next_label_id = 1;
// AT&T argument registers
static char *arg_reg_64[] = { "%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9" };

static void gen_push(void) {
	printf("  pushq %%rax\n");
}
static void gen_pop_rax(void) {
	printf("  popq %%rax\n");
}
static void gen_pop_rdi(void) {
	printf("  popq %%rdi\n");
}

// forward declarations
static void gen_expr(ASTNode *node, Function *curf);
static void gen_stmt(ASTNode *node, Function *curf);


// Generates the address of an l-value and pushes it onto the stack.
static void gen_lval(ASTNode *node) {
	if (node->kind == AST_IDENT) {
		Var *v = node->var_def;
		if (!v) error_at(node->tok->loc, "local variable not resolved");
		// address = %rbp - offset
		printf("  movq %%rbp, %%rax\n");
		printf("  subq $%d, %%rax\n", v->offset);
		gen_push(); // push address onto stack
		return;
	}

	if (node->kind == AST_DEREF) {
		// address of *E is the result of E
		gen_expr(node->children[0], NULL); // E evaluates to address, which is now on stack
		return;
	}

	error_at(node->tok->loc, "cannot take address of non l-value");
}

static void gen_expr(ASTNode *node, Function *curf) {
	if (!node) return;
    int size = get_type_size(node->dtype);

	switch (node->kind) {
	case AST_NUM:
		printf("  movq $%ld, %%rax\n", node->num_val);
		gen_push();
		return;
	case AST_CHAR:
		printf("  movq $%ld, %%rax\n", node->num_val);
		gen_push();
		return;
	case AST_STRING:
		printf("  leaq .lc_str_%ld(%%rip), %%rax\n", node->num_val);
		gen_push();
		return;

	case AST_IDENT:
		gen_lval(node); // address is on stack
		gen_pop_rdi();  // %rdi = address
        // load value from address into %rax (sign-extended to 64)
        if (size == 1)      printf("  movsbq (%%rdi), %%rax\n");
        else if (size == 4) printf("  movslq (%%rdi), %%rax\n");
        else                printf("  movq (%%rdi), %%rax\n");
		gen_push();
		return;

	case AST_DEREF:
		gen_expr(node->children[0], curf); // E -> address of value (on stack)
		gen_pop_rdi();  // %rdi = address of value
        // load value from address into %rax (sign-extended to 64)
        if (size == 1)      printf("  movsbq (%%rdi), %%rax\n");
        else if (size == 4) printf("  movslq (%%rdi), %%rax\n");
        else                printf("  movq (%%rdi), %%rax\n");
		gen_push();
		return;

	case AST_ADDR:
		gen_lval(node->children[0]); // address is on stack
		return;

	case AST_ASSIGN: {
		// 1. evaluate lhs (get address)
		gen_lval(node->children[0]); // address of LHS on stack
		// 2. evaluate rhs (get value)
		gen_expr(node->children[1], curf); // value of RHS on stack

		gen_pop_rax(); // %rax = rhs value
		gen_pop_rdi(); // %rdi = lhs address
        int target_size = get_type_size(node->children[0]->dtype);

        // store %rax value at %rdi address
        if (target_size == 1)      printf("  movb %%al, (%%rdi)\n");
        else if (target_size == 4) printf("  movl %%eax, (%%rdi)\n");
        else                       printf("  movq %%rax, (%%rdi)\n");

		gen_push(); // push the assigned value (%rax) back to stack
		return;
	}

    case AST_PRE_INC:
    case AST_PRE_DEC:
    case AST_POST_INC:
    case AST_POST_DEC: {
        // 1. Get l-value address
        gen_lval(node->children[0]); // address on stack
        gen_pop_rdi();  // %rdi = address
        // Pointer arithmetic scaling factor
        Type *op_type = node->children[0]->dtype;
        long scale = (op_type->kind == TYPEK_PTR) ? (long)get_type_size(op_type->base) : 1;
        int target_size = get_type_size(op_type);

        // 2. Load current value into %rax (and save address)
        printf("  pushq %%rdi\n"); // save address to stack
        if (target_size == 1)      printf("  movsbq (%%rdi), %%rax\n");
        else if (target_size == 4) printf("  movslq (%%rdi), %%rax\n");
        else                       printf("  movq (%%rdi), %%rax\n");

        // 3. Perform operation
        printf("  movq %%rax, %%r10\n"); // save old value to %r10
        if (node->kind == AST_PRE_INC || node->kind == AST_POST_INC)
            printf("  addq $%ld, %%rax\n", scale);
        else
            printf("  subq $%ld, %%rax\n", scale);

        // 4. Store new value back
        gen_pop_rdi(); // restore address from stack
        if (target_size == 1)      printf("  movb %%al, (%%rdi)\n");
        else if (target_size == 4) printf("  movl %%eax, (%%rdi)\n");
        else                       printf("  movq %%rax, (%%rdi)\n");

        // 5. Result: Post-op returns old value (%r10), Pre-op returns new value (%rax)
        if (node->kind == AST_POST_INC || node->kind == AST_POST_DEC)
            printf("  movq %%r10, %%rax\n");

        gen_push();
        return;
    }

	case AST_UNOP: // Simple unary minus (-) and not (!)
		gen_expr(node->children[0], curf);
		gen_pop_rax();
		if (node->op == '-') {
			printf("  negq %%rax\n");
		} else if (node->op == '!') { // logical NOT
			printf("  cmpq $0, %%rax\n");
			printf("  sete %%al\n");
			printf("  movzbq %%al, %%rax\n");
		}
		gen_push();
		return;

	case AST_BINOP: {
		// Handle pointer arithmetic (P +/- I, I + P, P - P)
        if (node->op == '+' || node->op == '-') {
            Type *lhs_type = node->children[0]->dtype;
            Type *rhs_type = node->children[1]->dtype;

            // Pointer +/- Integer or Integer + Pointer
            if ((lhs_type->kind == TYPEK_PTR && rhs_type->kind != TYPEK_PTR) || (rhs_type->kind == TYPEK_PTR && lhs_type->kind != TYPEK_PTR)) {
                ASTNode *ptr_node = (lhs_type->kind == TYPEK_PTR) ? node->children[0] : node->children[1];
                ASTNode *int_node = (lhs_type->kind == TYPEK_PTR) ? node->children[1] : node->children[0];
                int scale = get_type_size(ptr_node->dtype->base);

                // 1. Calculate scaled integer (I * scale)
                gen_expr(int_node, curf);
                gen_pop_rax();
                printf("  imulq $%d, %%rax\n", scale);
                gen_push();

                // 2. Get pointer (P)
                gen_expr(ptr_node, curf);

                // 3. Perform P +/- (I * scale)
                gen_pop_rdi(); // %rdi = P
                gen_pop_rax(); // %rax = I * scale

                if (node->op == '+') {
                    printf("  addq %%rdi, %%rax\n");
                } else if (lhs_type->kind == TYPEK_PTR) { // P - I
                    printf("  subq %%rax, %%rdi\n");
                    printf("  movq %%rdi, %%rax\n"); // Move result back to rax
                } else {
                    error_at(node->tok->loc, "unsupported pointer arithmetic: int - pointer");
                }
                gen_push();
                return;
            } else if (lhs_type->kind == TYPEK_PTR && rhs_type->kind == TYPEK_PTR && node->op == '-') {
                // P - P (pointer subtraction)
                gen_expr(node->children[1], curf); // P2 (on stack)
                gen_expr(node->children[0], curf); // P1 (on stack)
                gen_pop_rax(); // %rax = P1
                gen_pop_rdi(); // %rdi = P2
                printf("  subq %%rdi, %%rax\n"); // %rax = P1 - P2 (byte difference)
                printf("  cqto\n");
                printf("  movq $%d, %%r10\n", get_type_size(lhs_type->base));
                printf("  idivq %%r10\n"); // %rax = (P1 - P2) / scale
                gen_push();
                return;
            }
        }

        // standard binary operation
		gen_expr(node->children[1], curf); // rhs
		gen_expr(node->children[0], curf); // lhs

		gen_pop_rdi(); // %rdi = lhs value
		gen_pop_rax(); // %rax = rhs value (operands are swapped on stack)

		switch (node->op) {
		case '+':
			printf("  addq %%rdi, %%rax\n");
			break;
		case '-':
			printf("  subq %%rdi, %%rax\n");
			break;
		case '*':
			printf("  imulq %%rdi, %%rax\n");
			break;
		case '/':
			printf("  cqto\n");
			printf("  idivq %%rdi\n");
			break;
		case '=': // ==
		case '!': // !=
		case '<': // <
		case '>': // >
		case 'l': // <=
		case 'g': // >=
			printf("  cmpq %%rdi, %%rax\n");
			if (node->op == '=') printf("  sete %%al\n");
			else if (node->op == '!') printf("  setne %%al\n");
			else if (node->op == '<') printf("  setl %%al\n");
			else if (node->op == '>') printf("  setg %%al\n");
			else if (node->op == 'l') printf("  setle %%al\n");
			else if (node->op == 'g') printf("  setge %%al\n");
			printf("  movzbq %%al, %%rax\n");
			break;
		case '&': // && (logical AND)
			printf("  cmpq $0, %%rax\n");
			printf("  setne %%al\n");
			printf("  cmpq $0, %%rdi\n");
			printf("  setne %%dl\n");
			printf("  andl %%edx, %%eax\n");
			printf("  movzbq %%al, %%rax\n");
			break;
		case '|': // || (logical OR)
			printf("  orq %%rdi, %%rax\n");
			printf("  setne %%al\n");
			printf("  movzbq %%al, %%rax\n");
			break;
		default:
			error_at(node->tok->loc, "unsupported binary operator");
		}
		gen_push();
		return;
	}

	case AST_CALL: {
        // 1. Evaluate arguments and push them onto the stack (right-to-left)
        int num_args = node->child_count - 1;
        for (int i = num_args; i > 0; i--) {
            gen_expr(node->children[i], curf);
        }

        // 2. Pop args into registers (%rdi, %rsi, %rdx, %rcx, %r8, %r9)
        for (int i = 0; i < num_args && i < 6; i++) {
            printf("  popq %s\n", arg_reg_64[i]);
        }

        // 3. Align stack to 16 bytes for call (System V ABI)
        int stack_adjust = 0;
        if (num_args % 2 != 0) {
            stack_adjust = 8;
            printf("  subq $8, %%rsp\n");
        }

        // 4. Call function
        printf("  call %s\n", node->children[0]->str_val);

        // 5. Clean up stack alignment
        if (stack_adjust > 0) {
            printf("  addq $8, %%rsp\n");
        }

        // 6. Clean up stack for arguments passed via stack (after 6th arg)
        if (num_args > 6) {
            printf("  addq $%d, %%rsp\n", 8 * (num_args - 6));
        }

        // 7. Push return value (%rax)
		printf("  pushq %%rax\n");
        return;
	}
	default:
		// Fallthrough for control flow statements handled in gen_stmt
		break;
	}
}

static void gen_stmt(ASTNode *node, Function *curf) {
	if (!node) return;

	switch (node->kind) {
	case AST_COMPOUND:
		for (int i = 0; i < node->child_count; i++) {
			gen_stmt(node->children[i], curf);
		}
		return;

	case AST_VAR_DECL: {
		// If there is an initializer, generate the assignment
		if (node->child_count > 0 && node->children[0]) {
            // Re-create the assignment structure for the code generator
            ASTNode *ident = new_node(AST_IDENT, node->tok);
            ident->str_val = strdup(node->var_def->name);
            ident->var_def = node->var_def;
            ident->dtype = node->var_def->type;

			ASTNode *assign = new_node(AST_ASSIGN, node->tok);
			ast_add_child(assign, ident);
			ast_add_child(assign, node->children[0]);

            gen_expr(assign, curf);
            gen_pop_rax(); // discard result
		}
		return;
	}

	case AST_EXPR_STMT:
		gen_expr(node->children[0], curf);
		gen_pop_rax(); // discard result of expression statement
		return;

	case AST_RETURN:
		if (node->child_count > 0 && node->children[0]) {
			gen_expr(node->children[0], curf);
			gen_pop_rax(); // return value is in %rax
		}
		// Jump to function epilogue
		printf("  jmp .L_epilogue_%s\n", curf->name);
		return;

	case AST_IF: {
		int label_else = next_label_id++;
		int label_end = next_label_id++;

		// 1. Condition
		gen_expr(node->children[0], curf);
		gen_pop_rax();
		printf("  cmpq $0, %%rax\n");

		// 2. Jump to else/end
		if (node->child_count == 2) {
			printf("  je .L_end_%d\n", label_end);
		} else {
			printf("  je .L_else_%d\n", label_else);
		}

		// 3. Then statement
		gen_stmt(node->children[1], curf);
		printf("  jmp .L_end_%d\n", label_end);

		// 4. Else statement (if present)
		if (node->child_count > 2) {
			printf(".L_else_%d:\n", label_else);
			gen_stmt(node->children[2], curf);
		}

		// 5. End label
		printf(".L_end_%d:\n", label_end);
		return;
	}

	case AST_WHILE:
	case AST_FOR: {
		int label_begin = next_label_id++;
		int label_continue = next_label_id++;
		int label_break = next_label_id++;

        // Save current loop labels
        int old_break = curf->label_break;
        int old_continue = curf->label_continue;
        curf->label_break = label_break;
        curf->label_continue = label_continue;

        // 1. For-loop init (if applicable)
		if (node->kind == AST_FOR && node->children[0]) {
            gen_stmt(node->children[0], curf);
        }

        // 2. Loop begin/condition label
		printf(".L_begin_%d:\n", label_begin);

        // 3. Condition (if/while: child[0], for: child[1])
        ASTNode *cond = (node->kind == AST_WHILE) ? node->children[0] : node->children[1];
        if (cond) {
            gen_expr(cond, curf);
            gen_pop_rax();
            printf("  cmpq $0, %%rax\n");
            printf("  je .L_break_%d\n", label_break);
        } // else: unconditional loop (for(;;))

        // 4. Body (if/while: child[1], for: child[3])
        ASTNode *body = (node->kind == AST_WHILE) ? node->children[1] : node->children[3];
        gen_stmt(body, curf);

        // 5. Continue label (for-loop increment)
		printf(".L_continue_%d:\n", label_continue);

        // 6. For-loop increment (if applicable)
        if (node->kind == AST_FOR && node->children[2]) {
            gen_expr(node->children[2], curf);
            gen_pop_rax(); // discard result
        }

        // 7. Jump back to begin
        printf("  jmp .L_begin_%d\n", label_begin);

        // 8. Break label
		printf(".L_break_%d:\n", label_break);

        // Restore old loop labels
        curf->label_break = old_break;
        curf->label_continue = old_continue;
		return;
	}

	case AST_BREAK:
		if (!curf->label_break) error_at(node->tok->loc, "break statement not in loop");
		printf("  jmp .L_break_%d\n", curf->label_break);
		return;

	case AST_CONTINUE:
		if (!curf->label_continue) error_at(node->tok->loc, "continue statement not in loop");
		printf("  jmp .L_continue_%d\n", curf->label_continue);
		return;

	default:
		error_at(node->tok->loc, "unhandled ast kind in gen_stmt: %d", node->kind);
	}
}

void generate_code(ASTNode *prog) {
	printf(".file 1 \"nanoc5.c\"\n");
	printf(".text\n");

    // data section for string literals
    for (int i = 0; i < string_table_count; i++) {
        printf(".lc_str_%d:\n", string_table[i].id);
        printf("  .string \"%s\"\n", string_table[i].s);
    }

	// Function generation
	for (int i = 0; i < prog->child_count; i++) {
		ASTNode *fnnode = prog->children[i];
		if (fnnode->kind != AST_FUNC) continue;

		Function *f = fnnode->func_def;

		printf(".globl %s\n", f->name);
		printf("%s:\n", f->name);

		// Prologue
		printf("  pushq %%rbp\n");
		printf("  movq %%rsp, %%rbp\n");
		// Allocate stack space for locals (aligned to 16)
		printf("  subq $%d, %%rsp\n", ALIGN_TO(f->stack_size, 16));

		// Store arguments to stack
        int arg_reg_count = 0;
        for (Var *v = f->locals; v; v = v->next) {
            // Parameters are the first few locals
            if (v->offset <= f->stack_size && arg_reg_count < 6) {
                char *reg = arg_reg_64[arg_reg_count];
                // stack address = %rbp - offset
                printf("  movq %s, -%d(%%rbp)\n", reg, v->offset);
                arg_reg_count++;
            } else {
				// stop after parameters
				break;
			}
        }

		// Function body
		gen_stmt(f->body, f);

		// Epilogue label (used by return statements)
		printf(".L_epilogue_%s:\n", f->name);

		// Epilogue (sets %rax to 0 if no return was hit)
        if (f->ret_type->kind != TYPEK_VOID) {
            printf("  movq $0, %%rax\n");
        }

		printf("  movq %%rbp, %%rsp\n"); // Restore %rsp
		printf("  popq %%rbp\n");        // Restore %rbp
		printf("  ret\n");
	}
}


/* ----------------------------------------------------------------------
 * internal test suite
 * ---------------------------------------------------------------------- */

static void run_internal_tests(void) {
	const char *test_code =
"// linked list definition\n"
"struct node {\n"
"    int val;\n"
"    struct node *next;\n"
"};\n"
"// Test function with complex control flow and local variables\n"
"void func_a(int a, char b) { \n"
"    unsigned long c = a; \n"
"    int x[10];\n"
"    x[5] = 1;\n"
"    if (a > 0) { \n"
"        if (b == 'x') { c = c + b; } \n"
"        else { c = 20; } \n"
"    } \n"
"    while (c < 30) { \n"
"        for (int i = 0; i < 5; i++) { \n"
"            if (i == 2) { continue; } \n"
"            if (i == 4) { break; } \n"
"            c = c + 1; \n"
"        } \n"
"    } \n"
"    struct test_s { int x; char *p; }; \n"
"    struct node *ptr1 = 0;\n"
"    ptr1->val = 5; // test -> desugaring\n"
"    int i = 0;\n"
"    ++i; // test pre-inc\n"
"    i--; // test post-dec\n"
"    return; \n"
"} \n"
"// main function that calls the test function\n"
"int main() { \n"
"    func_a(1, 'x'); \n"
"    char *s = \"string test\\n\"; \n"
"    return 0; \n"
"}";

    fprintf(stderr, "\n--- running internal tests ---\n");

    current_input = (char*) test_code;
    tokens = tokenize(test_code);
    token_idx = 0;

    ASTNode *prog = parse_program();
    semantic_program(prog);
    generate_code(prog);

    fprintf(stderr, "\ninternal test code compiled successfully to x64 assembly.\n");
}


/* ----------------------------------------------------------------------
 * main
 * ---------------------------------------------------------------------- */

int main(int argc, char **argv) {

    if (argc < 2) {
        // run internal tests if no file is provided
        run_internal_tests();
        return 0;
    }

    /* read original source */
    char *src = read_file(argv[1]);
    if (!src) {
        fprintf(stderr, "error: cannot open source file '%s'\n", argv[1]);
        return 1;
    }

    /* preprocess (inline includes, with /usr/include fallback) */
    char *pp_src = preprocess(src);
    free(src);

    /* tokenize preprocessed source */
    current_input = pp_src;
    tokens = tokenize(pp_src);
    token_idx = 0;

    /* parse program */
    ASTNode *prog = parse_program();

    /* semantic analysis */
    semantic_program(prog);

    /* continue with code generation as before */
    generate_code(prog);

    /* cleanup (not exhaustive, only freeing the main source buffer) */
    free(pp_src);

    return 0;
}
