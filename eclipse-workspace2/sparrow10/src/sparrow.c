/*
 * sparrow.c
 *
 * Refactored to support JSON-global flattening and external printf calls.
 * Fixed: Escaped % in assembly stubs and added missing function prototypes.
 * FIXED: Escaped inner quotes and newlines in string literals for correct
 * assembly syntax, resolving "junk at end of line" errors.
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdint.h>
#include <assert.h>
#include <time.h>
#include <unistd.h>

#define MAX_LEXEME 4096
#define INITIAL_TOKEN_CAP 512
#define LABEL_RANDOM_MASK 0xFFFFF

/* ----------------------------------------------------------------------
 * UTILITIES & ERROR HANDLING
 * ---------------------------------------------------------------------- */

static char *global_input = NULL;
static const char *prog_name = "sparrow";

static void error_at(const char *loc, const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	int pos = (int) (loc - global_input);
	fprintf(stderr, "\n--- COMPILE ERROR ---\n");
	if (global_input) {
		fprintf(stderr, "%s\n", global_input);
		fprintf(stderr, "%*s^ ", pos, "");
	}
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n---------------------\n");
	va_end(ap);
	exit(1);
}

static void fatal(const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	fprintf(stderr, "\nFATAL: ");
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
	va_end(ap);
	exit(1);
}

/* ----------------------------------------------------------------------
 * FILE IO
 * ---------------------------------------------------------------------- */

static char* read_file(const char *path) {
	FILE *f = fopen(path, "rb");
	if (!f)
		return NULL;
	if (fseek(f, 0, SEEK_END) != 0) {
		fclose(f);
		return NULL;
	}
	long sz = ftell(f);
	if (sz < 0)
		sz = 0;
	rewind(f);
	char *buf = malloc((size_t) sz + 1);
	if (!buf) {
		fclose(f);
		return NULL;
	}
	size_t n = fread(buf, 1, (size_t) sz, f);
	buf[n] = '\0';
	fclose(f);
	return buf;
}

/* ----------------------------------------------------------------------
 * LEXER
 * ---------------------------------------------------------------------- */

typedef enum {
	TK_EOF = 0,
	TK_IDENT,
	TK_NUMBER,
	TK_STRING,
	/* keywords */
	TK_VAR,
	TK_FUNCTION,
	TK_RETURN,
	TK_IF,
	TK_ELSE,
	TK_WHILE,
	TK_FOR,
	TK_TRUE,
	TK_FALSE,
	/* punctuation */
	TK_LPAREN,
	TK_RPAREN,
	TK_LBRACE,
	TK_RBRACE,
	TK_LBRACKET,
	TK_RBRACKET,
	TK_COMMA,
	TK_SEMI,
	TK_COLON,
	TK_DOT,
	/* operators */
	TK_PLUS,
	TK_MINUS,
	TK_STAR,
	TK_SLASH,
	TK_ASSIGN,
	TK_EQ,
	TK_NEQ,
	TK_LT,
	TK_GT,
	TK_LE,
	TK_GE,
	TK_AND,
	TK_OR,
	TK_NOT,
	TK_UNKNOWN
} TokenKind;

typedef struct {
	TokenKind kind;
	char *lexeme; /* allocated for identifiers and strings */
	double num_val;
	char *loc; /* pointer into original input for error reporting */
} Token;

typedef struct {
	Token *arr;
	int count;
	int cap;
	int idx;
} TokenStream;

static void ts_init(TokenStream *ts) {
	ts->arr = NULL;
	ts->count = ts->cap = ts->idx = 0;
}
static void ts_push(TokenStream *ts, Token t) {
	if (ts->count + 1 > ts->cap) {
		ts->cap = ts->cap ? ts->cap * 2 : INITIAL_TOKEN_CAP;
		ts->arr = realloc(ts->arr, ts->cap * sizeof(Token));
	}
	ts->arr[ts->count++] = t;
}
static Token ts_peek(TokenStream *ts) {
	return ts->arr[ts->idx];
}
static Token ts_next(TokenStream *ts) {
	return ts->arr[ts->idx++];
}

static int is_ident_start(char c) {
	return isalpha((unsigned char)c) || c == '_';
}
static int is_ident_char(char c) {
	return isalnum((unsigned char)c) || c == '_';
}

static Token make_token(TokenKind k, const char *loc, const char *lex) {
	Token t;
	t.kind = k;
	t.loc = (char*) loc;
	t.lexeme = lex ? strdup(lex) : NULL;
	t.num_val = 0;
	return t;
}
static Token make_number_token(const char *loc, double v, const char *lex) {
	Token t = make_token(TK_NUMBER, loc, lex);
	t.num_val = v;
	return t;
}
static Token make_string_token(const char *loc, const char *s) {
	return make_token(TK_STRING, loc, s);
}

static TokenKind keyword_kind(const char *s) {
	if (!strcmp(s, "var"))
		return TK_VAR;
	if (!strcmp(s, "function"))
		return TK_FUNCTION;
	if (!strcmp(s, "return"))
		return TK_RETURN;
	if (!strcmp(s, "if"))
		return TK_IF;
	if (!strcmp(s, "else"))
		return TK_ELSE;
	if (!strcmp(s, "while"))
		return TK_WHILE;
	if (!strcmp(s, "for"))
		return TK_FOR;
	if (!strcmp(s, "true"))
		return TK_TRUE;
	if (!strcmp(s, "false"))
		return TK_FALSE;
	return TK_IDENT;
}

static TokenStream tokenize(const char *input) {
	global_input = (char*) input;
	TokenStream ts;
	ts_init(&ts);
	const char *p = input;

	while (*p) {
		if (isspace((unsigned char )*p)) {
			p++;
			continue;
		}
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

		if (is_ident_start(*p)) {
			const char *s = p;
			while (is_ident_char(*p))
				p++;
			int len = (int) (p - s);
			char *lex = strndup(s, len);
			TokenKind k = keyword_kind(lex);
			Token t = make_token(k, s, lex);
			free(lex);
			ts_push(&ts, t);
			continue;
		}

		if (isdigit((unsigned char )*p)
				|| (*p == '.' && isdigit((unsigned char )p[1]))) {
			char *end;
			double v = strtod(p, &end);
			int len = (int) (end - p);
			char *lex = strndup(p, len);
			Token t = make_number_token(p, v, lex);
			free(lex);
			ts_push(&ts, t);
			p = end;
			continue;
		}

		if (*p == '"' || *p == '\'') {
			char quote = *p++;
			const char *start = p;
			char buf[MAX_LEXEME];
			int bi = 0;
			while (*p && *p != quote) {
				if (*p == '\\') {
					p++;
					if (*p == 'n')
						buf[bi++] = '\n';
					else if (*p == 't')
						buf[bi++] = '\t';
					else if (*p == '\\')
						buf[bi++] = '\\';
					else
						buf[bi++] = *p;
					p++;
				} else {
					buf[bi++] = *p++;
				}
				if (bi >= MAX_LEXEME - 1)
					break;
			}
			if (*p != quote)
				error_at((char*) p, "Unterminated string literal");
			buf[bi] = '\0';
			p++;
			Token t = make_string_token(start - 1, buf);
			ts_push(&ts, t);
			continue;
		}

		if (!strncmp(p, "==", 2)) {
			ts_push(&ts, make_token(TK_EQ, p, "=="));
			p += 2;
			continue;
		}
		if (!strncmp(p, "!=", 2)) {
			ts_push(&ts, make_token(TK_NEQ, p, "!="));
			p += 2;
			continue;
		}
		if (!strncmp(p, "<=", 2)) {
			ts_push(&ts, make_token(TK_LE, p, "<="));
			p += 2;
			continue;
		}
		if (!strncmp(p, ">=", 2)) {
			ts_push(&ts, make_token(TK_GE, p, ">="));
			p += 2;
			continue;
		}
		if (!strncmp(p, "&&", 2)) {
			ts_push(&ts, make_token(TK_AND, p, "&&"));
			p += 2;
			continue;
		}
		if (!strncmp(p, "||", 2)) {
			ts_push(&ts, make_token(TK_OR, p, "||"));
			p += 2;
			continue;
		}

		TokenKind tk = TK_UNKNOWN;
		const char *loc = p;
		char lex[2] = { 0, 0 };
		switch (*p++) {
		case '+':
			tk = TK_PLUS;
			lex[0] = '+';
			break;
		case '-':
			tk = TK_MINUS;
			lex[0] = '-';
			break;
		case '*':
			tk = TK_STAR;
			lex[0] = '*';
			break;
		case '/':
			tk = TK_SLASH;
			lex[0] = '/';
			break;
		case '(':
			tk = TK_LPAREN;
			lex[0] = '(';
			break;
		case ')':
			tk = TK_RPAREN;
			lex[0] = ')';
			break;
		case '{':
			tk = TK_LBRACE;
			lex[0] = '{';
			break;
		case '}':
			tk = TK_RBRACE;
			lex[0] = '}';
			break;
		case '[':
			tk = TK_LBRACKET;
			lex[0] = '[';
			break;
		case ']':
			tk = TK_RBRACKET;
			lex[0] = ']';
			break;
		case ',':
			tk = TK_COMMA;
			lex[0] = ',';
			break;
		case ';':
			tk = TK_SEMI;
			lex[0] = ';';
			break;
		case ':':
			tk = TK_COLON;
			lex[0] = ':';
			break;
		case '.':
			tk = TK_DOT;
			lex[0] = '.';
			break;
		case '=':
			tk = TK_ASSIGN;
			lex[0] = '=';
			break;
		case '<':
			tk = TK_LT;
			lex[0] = '<';
			break;
		case '>':
			tk = TK_GT;
			lex[0] = '>';
			break;
		case '!':
			tk = TK_NOT;
			lex[0] = '!';
			break;
		default:
			error_at((char*) loc, "Unexpected character '%c'", *(loc));
		}
		ts_push(&ts, make_token(tk, loc, lex));
	}

	ts_push(&ts, make_token(TK_EOF, (char*) input + strlen(input), "<EOF>"));
	return ts;
}

/* ----------------------------------------------------------------------
 * AST
 * ---------------------------------------------------------------------- */

typedef enum {
	AST_PROGRAM,
	AST_FUNC_DECL,
	AST_VAR_DECL,
	AST_BLOCK,
	AST_RETURN,
	AST_IF,
	AST_WHILE,
	AST_FOR,
	AST_EXPR_STMT,
	AST_BINARY,
	AST_UNARY,
	AST_CALL,
	AST_IDENT,
	AST_NUMBER,
	AST_STRING,
	AST_BOOL,
	AST_OBJECT, /* object literal */
	AST_ARRAY, /* array literal */
	AST_PROP_ACCESS, /* obj.prop */
	AST_INDEX, /* obj[idx] */
	AST_EMPTY
} ASTKind;

typedef struct AST {
	ASTKind kind;
	Token token;
	struct AST **children;
	int child_count;
	/* node-specific */
	char *name;
	double num;
	char *str;
	int bool_val;
	char **params; /* reused for object keys or function params */
	int param_count;
} AST;

static AST* ast_new(ASTKind k, Token t) {
	AST *n = calloc(1, sizeof(AST));
	n->kind = k;
	n->token = t;
	n->children = NULL;
	n->child_count = 0;
	n->name = NULL;
	n->num = 0.0;
	n->str = NULL;
	n->bool_val = 0;
	n->params = NULL;
	n->param_count = 0;
	return n;
}
static void ast_add_child(AST *parent, AST *child) {
	if (!parent || !child)
		return;
	parent->children = realloc(parent->children,
			sizeof(AST*) * (parent->child_count + 1));
	parent->children[parent->child_count++] = child;
}

/* ----------------------------------------------------------------------
 * PARSER (recursive descent)
 * ---------------------------------------------------------------------- */

typedef struct {
	TokenStream *ts;
} Parser;

static Token cur(Parser *p) {
	return p->ts->arr[p->ts->idx];
}
static int accept(Parser *p, TokenKind k) {
	if (p->ts->arr[p->ts->idx].kind == k) {
		p->ts->idx++;
		return 1;
	}
	return 0;
}
static Token expect(Parser *p, TokenKind k) {
	Token t = cur(p);
	if (t.kind != k)
		error_at(t.loc, "Expected token %d but got %d", k, t.kind);
	p->ts->idx++;
	return t;
}

/* Forward declarations */
static AST* parse_program(Parser *p);
static AST* parse_statement(Parser *p);
static AST* parse_expression(Parser *p);
static AST* parse_assignment(Parser *p);
static AST* parse_unary(Parser *p);
static AST* parse_object_literal(Parser *p);
static AST* parse_array_literal(Parser *p);
static AST* parse_function_decl(Parser *p); /* Added forward decl */

/* Primary with postfix (., [ ]) handling */
static AST* parse_primary(Parser *p) {
	Token t = cur(p);
	AST *node = NULL;

	if (t.kind == TK_LBRACE) {
		node = parse_object_literal(p);
	} else if (t.kind == TK_LBRACKET) {
		node = parse_array_literal(p);
	} else if (accept(p, TK_LPAREN)) {
		AST *e = parse_expression(p);
		expect(p, TK_RPAREN);
		node = e;
	} else if (t.kind == TK_NUMBER) {
		Token nt = expect(p, TK_NUMBER);
		AST *n = ast_new(AST_NUMBER, nt);
		n->num = nt.num_val;
		node = n;
	} else if (t.kind == TK_STRING) {
		Token st = expect(p, TK_STRING);
		AST *n = ast_new(AST_STRING, st);
		n->str = strdup(st.lexeme ? st.lexeme : "");
		node = n;
	} else if (t.kind == TK_TRUE) {
		Token bt = expect(p, TK_TRUE);
		AST *n = ast_new(AST_BOOL, bt);
		n->bool_val = 1;
		node = n;
	} else if (t.kind == TK_FALSE) {
		Token bt = expect(p, TK_FALSE);
		AST *n = ast_new(AST_BOOL, bt);
		n->bool_val = 0;
		node = n;
	} else if (t.kind == TK_IDENT) {
		Token id = expect(p, TK_IDENT);
		AST *ident = ast_new(AST_IDENT, id);
		ident->name = strdup(id.lexeme ? id.lexeme : "");
		/* function call? */
		if (accept(p, TK_LPAREN)) {
			AST *call = ast_new(AST_CALL, id);
			if (!accept(p, TK_RPAREN)) {
				AST *arg = parse_expression(p);
				ast_add_child(call, arg);
				while (accept(p, TK_COMMA)) {
					AST *a = parse_expression(p);
					ast_add_child(call, a);
				}
				expect(p, TK_RPAREN);
			}
			ast_add_child(call, ident); /* callee as last child */
			node = call;
		} else {
			node = ident;
		}
	} else {
		error_at(t.loc, "Expected primary expression");
	}

	/* Postfix: .prop and [expr] and method calls chaining */
	while (1) {
		if (accept(p, TK_DOT)) {
			Token prop = expect(p, TK_IDENT);
			AST *propNode = ast_new(AST_STRING, prop);
			propNode->str = strdup(prop.lexeme ? prop.lexeme : "");
			AST *acc = ast_new(AST_PROP_ACCESS, prop);
			ast_add_child(acc, node); /* base */
			ast_add_child(acc, propNode); /* property name as string node */
			node = acc;
			/* allow method call: .push(...) */
			if (accept(p, TK_LPAREN)) {
				AST *call = ast_new(AST_CALL, prop);
				/* first child(s) are args, last child is callee */
				if (!accept(p, TK_RPAREN)) {
					AST *arg = parse_expression(p);
					ast_add_child(call, arg);
					while (accept(p, TK_COMMA)) {
						AST *a = parse_expression(p);
						ast_add_child(call, a);
					}
					expect(p, TK_RPAREN);
				}
				ast_add_child(call, node); /* callee is the prop access node */
				node = call;
			}
			continue;
		}
		if (accept(p, TK_LBRACKET)) {
			AST *idx = parse_expression(p);
			expect(p, TK_RBRACKET);
			AST *acc = ast_new(AST_INDEX, idx->token);
			ast_add_child(acc, node); /* base */
			ast_add_child(acc, idx); /* index */
			node = acc;
			/* allow method call on index result */
			if (accept(p, TK_LPAREN)) {
				AST *call = ast_new(AST_CALL, cur(p));
				if (!accept(p, TK_RPAREN)) {
					AST *arg = parse_expression(p);
					ast_add_child(call, arg);
					while (accept(p, TK_COMMA)) {
						AST *a = parse_expression(p);
						ast_add_child(call, a);
					}
					expect(p, TK_RPAREN);
				}
				ast_add_child(call, node);
				node = call;
			}
			continue;
		}
		break;
	}

	return node;
}

/* parse object literal: { "k": expr, ... } */
static AST* parse_object_literal(Parser *p) {
	expect(p, TK_LBRACE);
	AST *obj = ast_new(AST_OBJECT, cur(p));
	if (accept(p, TK_RBRACE))
		return obj;
	while (1) {
		Token keytok = cur(p);
		if (keytok.kind != TK_STRING)
			error_at(keytok.loc, "Expected string key in object literal");
		expect(p, TK_STRING);
		obj->params = realloc(obj->params,
				sizeof(char*) * (obj->param_count + 1));
		obj->params[obj->param_count++] = strdup(
				keytok.lexeme ? keytok.lexeme : "");
		expect(p, TK_COLON);
		AST *val = parse_expression(p);
		ast_add_child(obj, val);
		if (accept(p, TK_COMMA))
			continue;
		if (accept(p, TK_RBRACE))
			break;
		Token t = cur(p);
		error_at(t.loc, "Expected ',' or '}' in object literal");
	}
	return obj;
}

/* parse array literal: [ expr, expr, ... ] */
static AST* parse_array_literal(Parser *p) {
	expect(p, TK_LBRACKET);
	AST *arr = ast_new(AST_ARRAY, cur(p));
	if (accept(p, TK_RBRACKET))
		return arr; /* empty array */
	while (1) {
		AST *val = parse_expression(p);
		ast_add_child(arr, val);
		if (accept(p, TK_COMMA))
			continue;
		if (accept(p, TK_RBRACKET))
			break;
		Token t = cur(p);
		error_at(t.loc, "Expected ',' or ']' in array literal");
	}
	return arr;
}

/* Unary */
static AST* parse_unary(Parser *p) {
	Token t = cur(p);
	if (accept(p, TK_PLUS))
		return parse_unary(p);
	if (accept(p, TK_MINUS)) {
		AST *n = ast_new(AST_UNARY, t);
		n->token = t;
		n->token.lexeme = strdup("-");
		AST *child = parse_unary(p);
		ast_add_child(n, child);
		return n;
	}
	if (accept(p, TK_NOT)) {
		AST *n = ast_new(AST_UNARY, t);
		n->token = t;
		n->token.lexeme = strdup("!");
		AST *child = parse_unary(p);
		ast_add_child(n, child);
		return n;
	}
	return parse_primary(p);
}

/* Multiplicative */
static AST* parse_mul(Parser *p) {
	AST *node = parse_unary(p);
	while (cur(p).kind == TK_STAR || cur(p).kind == TK_SLASH) {
		Token op = cur(p);
		p->ts->idx++;
		AST *n = ast_new(AST_BINARY, op);
		n->token = op;
		ast_add_child(n, node);
		AST *r = parse_unary(p);
		ast_add_child(n, r);
		node = n;
	}
	return node;
}

/* Additive */
static AST* parse_add(Parser *p) {
	AST *node = parse_mul(p);
	while (cur(p).kind == TK_PLUS || cur(p).kind == TK_MINUS) {
		Token op = cur(p);
		p->ts->idx++;
		AST *n = ast_new(AST_BINARY, op);
		n->token = op;
		ast_add_child(n, node);
		AST *r = parse_mul(p);
		ast_add_child(n, r);
		node = n;
	}
	return node;
}

/* Relational */
static AST* parse_relational(Parser *p) {
	AST *node = parse_add(p);
	while (cur(p).kind == TK_LT || cur(p).kind == TK_GT || cur(p).kind == TK_LE
			|| cur(p).kind == TK_GE) {
		Token op = cur(p);
		p->ts->idx++;
		AST *n = ast_new(AST_BINARY, op);
		n->token = op;
		ast_add_child(n, node);
		AST *r = parse_add(p);
		ast_add_child(n, r);
		node = n;
	}
	return node;
}

/* Equality */
static AST* parse_equality(Parser *p) {
	AST *node = parse_relational(p);
	while (cur(p).kind == TK_EQ || cur(p).kind == TK_NEQ) {
		Token op = cur(p);
		p->ts->idx++;
		AST *n = ast_new(AST_BINARY, op);
		n->token = op;
		ast_add_child(n, node);
		AST *r = parse_relational(p);
		ast_add_child(n, r);
		node = n;
	}
	return node;
}

/* Logical AND */
static AST* parse_logical_and(Parser *p) {
	AST *node = parse_equality(p);
	while (accept(p, TK_AND)) {
		Token op = { .kind = TK_AND, .lexeme = strdup("&&"), .loc = cur(p).loc };
		AST *n = ast_new(AST_BINARY, op);
		n->token = op;
		ast_add_child(n, node);
		AST *r = parse_equality(p);
		ast_add_child(n, r);
		node = n;
	}
	return node;
}

/* Logical OR */
static AST* parse_logical_or(Parser *p) {
	AST *node = parse_logical_and(p);
	while (accept(p, TK_OR)) {
		Token op = { .kind = TK_OR, .lexeme = strdup("||"), .loc = cur(p).loc };
		AST *n = ast_new(AST_BINARY, op);
		n->token = op;
		ast_add_child(n, node);
		AST *r = parse_logical_and(p);
		ast_add_child(n, r);
		node = n;
	}
	return node;
}

/* Assignment (right-associative) */
static AST* parse_assignment(Parser *p) {
	AST *node = parse_logical_or(p);
	if (accept(p, TK_ASSIGN)) {
		Token asg = { .kind = TK_ASSIGN, .lexeme = strdup("="), .loc =
				cur(p).loc };
		AST *n = ast_new(AST_BINARY, asg);
		ast_add_child(n, node);
		AST *r = parse_assignment(p);
		ast_add_child(n, r);
		return n;
	}
	return node;
}

static AST* parse_expression(Parser *p) {
	return parse_assignment(p);
}

/* Compound statement */
static AST* parse_compound(Parser *p) {
	expect(p, TK_LBRACE);
	AST *blk = ast_new(AST_BLOCK, cur(p));
	while (!accept(p, TK_RBRACE)) {
		AST *s = parse_statement(p);
		ast_add_child(blk, s);
	}
	return blk;
}

/* Statement */
static AST* parse_statement(Parser *p) {
	Token t = cur(p);
	(void) t;
	if (accept(p, TK_LBRACE)) {
		p->ts->idx--; /* roll back; parse_compound expects '{' consumed */
		return parse_compound(p);
	}
	if (accept(p, TK_IF)) {
		AST *n = ast_new(AST_IF, cur(p));
		expect(p, TK_LPAREN);
		AST *cond = parse_expression(p);
		expect(p, TK_RPAREN);
		ast_add_child(n, cond);
		AST *then = parse_statement(p);
		ast_add_child(n, then);
		if (accept(p, TK_ELSE)) {
			AST *els = parse_statement(p);
			ast_add_child(n, els);
		}
		return n;
	}
	if (accept(p, TK_WHILE)) {
		AST *n = ast_new(AST_WHILE, cur(p));
		expect(p, TK_LPAREN);
		AST *cond = parse_expression(p);
		expect(p, TK_RPAREN);
		ast_add_child(n, cond);
		AST *body = parse_statement(p);
		ast_add_child(n, body);
		return n;
	}
	if (accept(p, TK_FOR)) {
		AST *n = ast_new(AST_FOR, cur(p));
		expect(p, TK_LPAREN);
		if (!accept(p, TK_SEMI)) {
			if (cur(p).kind == TK_VAR) {
				AST *decl = parse_statement(p);
				ast_add_child(n, decl);
			} else {
				AST *init = parse_expression(p);
				expect(p, TK_SEMI);
				ast_add_child(n, init);
			}
		} else {
			ast_add_child(n, NULL);
		}
		if (!accept(p, TK_SEMI)) {
			AST *cond = parse_expression(p);
			expect(p, TK_SEMI);
			ast_add_child(n, cond);
		} else {
			ast_add_child(n, NULL);
		}
		if (!accept(p, TK_RPAREN)) {
			AST *incr = parse_expression(p);
			expect(p, TK_RPAREN);
			ast_add_child(n, incr);
		} else {
			ast_add_child(n, NULL);
		}
		AST *body = parse_statement(p);
		ast_add_child(n, body);
		return n;
	}
	if (accept(p, TK_RETURN)) {
		AST *n = ast_new(AST_RETURN, cur(p));
		if (!accept(p, TK_SEMI)) {
			AST *e = parse_expression(p);
			ast_add_child(n, e);
			expect(p, TK_SEMI);
		}
		return n;
	}
	if (cur(p).kind == TK_VAR) {
		expect(p, TK_VAR);
		Token id = expect(p, TK_IDENT);
		AST *decl = ast_new(AST_VAR_DECL, id);
		decl->name = strdup(id.lexeme ? id.lexeme : "");
		if (accept(p, TK_ASSIGN)) {
			AST *init = parse_expression(p);
			ast_add_child(decl, init);
		}
		expect(p, TK_SEMI);
		return decl;
	}
	if (cur(p).kind == TK_FUNCTION) {
		return parse_function_decl(p);
	}
	AST *expr = parse_expression(p);
	expect(p, TK_SEMI);
	AST *stmt = ast_new(AST_EXPR_STMT, expr->token);
	ast_add_child(stmt, expr);
	return stmt;
}

/* Function declaration */
static AST* parse_function_decl(Parser *p) {
	Token fn_tok = expect(p, TK_FUNCTION);
	(void) fn_tok;
	Token name_tok = expect(p, TK_IDENT);
	AST *fn = ast_new(AST_FUNC_DECL, name_tok);
	fn->name = strdup(name_tok.lexeme ? name_tok.lexeme : "");
	expect(p, TK_LPAREN);
	if (!accept(p, TK_RPAREN)) {
		while (1) {
			Token pid = expect(p, TK_IDENT);
			fn->params = realloc(fn->params,
					sizeof(char*) * (fn->param_count + 1));
			fn->params[fn->param_count++] = strdup(
					pid.lexeme ? pid.lexeme : "");
			if (accept(p, TK_COMMA))
				continue;
			expect(p, TK_RPAREN);
			break;
		}
	}
	AST *body = parse_statement(p);
	ast_add_child(fn, body);
	return fn;
}

/* Program */
static AST* parse_program(Parser *p) {
	AST *prog = ast_new(AST_PROGRAM, cur(p));
	while (cur(p).kind != TK_EOF) {
		AST *s = parse_statement(p);
		ast_add_child(prog, s);
	}
	return prog;
}

/* ----------------------------------------------------------------------
 * STRING / GLOBAL / FUNCTION TABLES
 * ---------------------------------------------------------------------- */

/* String literal table */
typedef struct StrEntry {
	char *s;
	int id;
	struct StrEntry *next;
} StrEntry;

static StrEntry *str_table = NULL;
static int str_table_count = 0;

static int add_string_literal(const char *s) {
	for (StrEntry *e = str_table; e; e = e->next) {
		if (!strcmp(e->s, s))
			return e->id;
	}
	StrEntry *n = calloc(1, sizeof(StrEntry));
	n->s = strdup(s);
	n->id = str_table_count++;
	n->next = str_table;
	str_table = n;
	return n->id;
}

/* Global variables table */
typedef struct GVar {
	char *name;
	int id;
	int is_external; /* if 1, don't emit storage */
	struct GVar *next;
} GVar;

static GVar *gvars = NULL;
static int gvar_count = 0;

static int add_global_var(const char *name) {
	for (GVar *g = gvars; g; g = g->next) {
		if (!strcmp(g->name, name))
			return g->id;
	}
	GVar *n = calloc(1, sizeof(GVar));
	n->name = strdup(name);
	n->id = gvar_count++;
	n->next = gvars;
	gvars = n;
	return n->id;
}
static int find_global_var(const char *name) {
	for (GVar *g = gvars; g; g = g->next) {
		if (!strcmp(g->name, name))
			return g->id;
	}
	return -1;
}

/* Function registry */
typedef struct FuncEntry {
	char *name;
	AST *fn;
	struct FuncEntry *next;
} FuncEntry;

static FuncEntry *funcs = NULL;

static void register_function(const char *name, AST *fn) {
	FuncEntry *f = calloc(1, sizeof(FuncEntry));
	f->name = strdup(name);
	f->fn = fn;
	f->next = funcs;
	funcs = f;
}

/* ----------------------------------------------------------------------
 * CODEGEN (x86-64 AT&T)
 * ---------------------------------------------------------------------- */

typedef struct {
	FILE *out;
	int label_counter;
} CodeGenCtx;

static int new_label(CodeGenCtx *c) {
	return (rand() & LABEL_RANDOM_MASK) ^ (++c->label_counter);
}

static void emit(CodeGenCtx *c, const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	vfprintf(c->out, fmt, ap);
	fprintf(c->out, "\n");
	va_end(ap);
}

/* Escape inner double quotes and newlines for the assembler's .string directive */
static char* escape_string_for_asm(const char *s) {
    if (!s) return strdup("");

    // Initial size: 2x the original size (a safe overestimate)
    size_t len = strlen(s);
    size_t buf_sz = len * 2 + 1;
    char *buf = malloc(buf_sz);
    if (!buf) fatal("malloc failed in escape_string_for_asm");

    char *d = buf;
    const char *p = s;

    while (*p) {
        if (d - buf > buf_sz - 5) { // Realloc if near end (need space for 2-4 chars + '\0')
            size_t current_len = d - buf;
            buf_sz *= 2;
            buf = realloc(buf, buf_sz);
            if (!buf) fatal("realloc failed in escape_string_for_asm");
            d = buf + current_len; // Update destination pointer
        }

        // Escape characters for the assembler's .string directive
        if (*p == '\\') {
            *d++ = '\\';
            *d++ = '\\';
        } else if (*p == '"') { // Inner double quotes must be escaped
            *d++ = '\\';
            *d++ = '"';
        } else if (*p == '\n') { // Literal newlines must be escaped
            *d++ = '\\';
            *d++ = 'n';
        } else if (*p == '\t') {
            *d++ = '\\';
            *d++ = 't';
        } else {
            *d++ = *p;
        }
        p++;
    }
    *d = '\0';
    return buf;
}

/* Utility: serialize an AST node to a JSON string (for global data init) */
static char* ast_to_json_string(AST *node) {
	if (!node)
		return strdup("null");
	if (node->kind == AST_STRING) {
		char buf[4096];
		snprintf(buf, sizeof(buf), "\"%s\"", node->str);
		return strdup(buf);
	}
	if (node->kind == AST_NUMBER) {
		char buf[64];
		snprintf(buf, sizeof(buf), "%g", node->num);
		return strdup(buf);
	}
	if (node->kind == AST_BOOL) {
		return strdup(node->bool_val ? "true" : "false");
	}
	if (node->kind == AST_ARRAY) {
		size_t sz = 256;
		char *s = malloc(sz);
		strcpy(s, "[");
		for (int i = 0; i < node->child_count; ++i) {
			if (i > 0) {
				size_t len = strlen(s);
				if (len + 2 > sz) {
					sz *= 2;
					s = realloc(s, sz);
				}
				strcat(s, ",");
			}
			char *sub = ast_to_json_string(node->children[i]);
			size_t need = strlen(s) + strlen(sub) + 2;
			if (need > sz) {
				sz = sz * 2 + need;
				s = realloc(s, sz);
			}
			strcat(s, sub);
			free(sub);
		}
		strcat(s, "]");
		return s;
	}
	if (node->kind == AST_OBJECT) {
		size_t sz = 256;
		char *s = malloc(sz);
		strcpy(s, "{");
		for (int i = 0; i < node->child_count; ++i) {
			if (i > 0) {
				size_t len = strlen(s);
				if (len + 2 > sz) {
					sz *= 2;
					s = realloc(s, sz);
				}
				strcat(s, ",");
			}
			char *key = node->params[i];
			char *sub = ast_to_json_string(node->children[i]);
			size_t need = strlen(s) + strlen(key) + strlen(sub) + 6;
			if (need > sz) {
				sz = sz * 2 + need;
				s = realloc(s, sz);
			}
			strcat(s, "\"");
			strcat(s, key);
			strcat(s, "\":");
			strcat(s, sub);
			free(sub);
		}
		strcat(s, "}");
		return s;
	}
	return strdup("null");
}

/* Helper to recursively emit flattened globals from a JSON object */
static void gen_json_globals_recursive(CodeGenCtx *c, const char *prefix,
		AST *node) {
	if (!node || node->kind != AST_OBJECT)
		return;

	for (int i = 0; i < node->child_count; ++i) {
		char *key = node->params[i];
		AST *val = node->children[i];

		/* Construct global name: prefix_key */
		char global_name[256];
		snprintf(global_name, sizeof(global_name), "%s_%s", prefix, key);

		/* Register global */
		add_global_var(global_name);

		/* Emit assembly for this global */
		emit(c, ".data");
		emit(c, ".globl %s", global_name);
		emit(c, ".align 8");
		emit(c, "%s:", global_name);

		if (val->kind == AST_STRING) {
			int sid = add_string_literal(val->str);
			emit(c, "    .quad .Lstr%d", sid);
		} else if (val->kind == AST_NUMBER) {
			/* Detect if integer or double for display purposes.
			 * test.js uses simple numbers. We store as double or int?
			 * sparrow uses movabs for doubles in xmm. To play nice with
			 * printf("%g") or simple usage, we store raw quad.
			 * If we want it to be an integer (age: 34), we can store .quad 34.
			 * If it's a double, we store IEEE bits.
			 * Let's store as integer if it's whole, else double bits.
			 */
			long long lg = (long long) val->num;
			if ((double) lg == val->num) {
				emit(c, "    .quad %lld", lg);
			} else {
				union {
					double d;
					uint64_t u;
				} conv;
				conv.d = val->num;
				emit(c, "    .quad %llu", (unsigned long long) conv.u);
			}
		} else if (val->kind == AST_BOOL) {
			emit(c, "    .quad %d", val->bool_val);
		} else if (val->kind == AST_ARRAY || val->kind == AST_OBJECT) {
			/* For complex sub-structures, emit JSON string */
			char *json = ast_to_json_string(val);
			int sid = add_string_literal(json);
			free(json);
			emit(c, "    .quad .Lstr%d", sid);
		} else {
			emit(c, "    .quad 0");
		}
	}
}

/* Forward */
static void gen_stmt(CodeGenCtx *c, AST *node);
static void gen_expr(CodeGenCtx *c, AST *node);

/* Generate expression: result in RAX */
static void gen_expr(CodeGenCtx *c, AST *node) {
	if (!node)
		return;
	switch (node->kind) {
	case AST_NUMBER:
		emit(c, "    # number %g", node->num);
		{
			union {
				double d;
				uint64_t u;
			} conv;
			conv.d = node->num;
			emit(c, "    movabs $%llu, %%rax", (unsigned long long) conv.u);
			emit(c, "    movq %%rax, %%xmm0");
		}
		break;
	case AST_STRING: {
		int sid = add_string_literal(node->str ? node->str : "");
		emit(c, "    leaq .Lstr%d(%%rip), %%rax", sid);
	}
		break;
	case AST_BOOL:
		emit(c, "    mov $%d, %%rax", node->bool_val);
		break;
	case AST_IDENT:
		/* Check if global */
		if (find_global_var(node->name) >= 0) {
			emit(c, "    # load global %s", node->name);
			emit(c, "    mov %s(%%rip), %%rax", node->name);
			/* Also put in xmm0 in case it's a number */
			emit(c, "    movq %%rax, %%xmm0");
		} else {
			/* Assume external or null */
			emit(c, "    mov $0, %%rax");
		}
		break;
	case AST_BINARY: {
		TokenKind k = node->token.kind;
		AST *L = node->children[0];
		AST *R = node->children[1];
		gen_expr(c, R);
		emit(c, "    push %%rax");
		gen_expr(c, L);
		emit(c, "    pop %%rdi"); /* Right in rdi */
		/* Left in rax */
		if (k == TK_PLUS) {
			/* Simple integer add for demo */
			emit(c, "    add %%rdi, %%rax");
		} else if (k == TK_MINUS) {
			emit(c, "    sub %%rdi, %%rax");
		} else if (k == TK_STAR) {
			emit(c, "    imul %%rdi, %%rax");
		} else if (k == TK_SLASH) {
			/* integer div */
			emit(c, "    cqo");
			emit(c, "    idiv %%rdi");
		} else if (k == TK_ASSIGN) {
			/* Check if left is ident */
			if (L->kind == AST_IDENT) {
				int gid = find_global_var(L->name);
				if (gid >= 0) {
					/* R result is in rdi. Store it in global variable L. */
					emit(c, "    mov %%rdi, %s(%%rip)", L->name);
				}
			}
			/* The assignment expression result is the assigned value (RHS), which is in rdi */
			emit(c, "    mov %%rdi, %%rax");
		} else {
			/* Compare ops etc omitted for brevity in demo */
		}
	}
		break;
	case AST_CALL: {
		int n = node->child_count;
		AST *callee = node->children[n - 1];
		int argc = n - 1;
		const char *fname = (callee->kind == AST_IDENT) ? callee->name : NULL;

		/* Special case: print(...) with multiple args */
		if (fname && !strcmp(fname, "print")) {
			for (int i = 0; i < argc; ++i) {
				gen_expr(c, node->children[i]);
				emit(c, "    mov %%rax, %%rdi");
				emit(c, "    call js_print_value");
				/* space between args? */
				if (i < argc - 1) {
					emit(c, "    call js_print_space");
				}
			}
			emit(c, "    call js_print_newline");
			emit(c, "    mov $0, %%rax");
			break;
		}

		/* Evaluate args right-to-left */
		for (int i = argc - 1; i >= 0; --i) {
			gen_expr(c, node->children[i]);
			emit(c, "    push %%rax");
		}

		if (fname && !strcmp(fname, "printf")) {
			/* Standard C printf */
			const char *regs[] = { "%rdi", "%rsi", "%rdx", "%rcx", "%r8",
					"%r9" };
			for (int i = 0; i < argc && i < 6; ++i) {
				emit(c, "    pop %s", regs[i]);
				/* If argument might be a double (for %g), it should also be in XMM.
				 * Since we don't track types, we move the popped GPR to XMM just in case. */
				emit(c, "    movq %s, %%xmm%d", regs[i], i - 1 >= 0 ? i - 1 : 0);
			}
			emit(c, "    xor %%rax, %%rax"); /* 0 vector registers used */
			emit(c, "    call printf");
		} else if (fname) {
			const char *regs[] = { "%rdi", "%rsi", "%rdx", "%rcx", "%r8",
					"%r9" };
			for (int i = 0; i < argc && i < 6; ++i) {
				emit(c, "    pop %s", regs[i]);
			}
			emit(c, "    call %s", fname);
		} else {
			/* dynamic call? ignore */
			emit(c, "    mov $0, %%rax");
		}
		/* Pop stack if > 6 args? (omitted) */
	}
		break;
	default:
		break;
	}
}

/* Generate statement */
static void gen_stmt(CodeGenCtx *c, AST *node) {
	if (!node)
		return;
	switch (node->kind) {
	case AST_BLOCK:
		for (int i = 0; i < node->child_count; ++i)
			gen_stmt(c, node->children[i]);
		break;
	case AST_EXPR_STMT:
		gen_expr(c, node->children[0]);
		break;
	case AST_VAR_DECL: {
		add_global_var(node->name);
		/* Check initializer for Object Literal to flatten */
		AST *init = (node->child_count > 0) ? node->children[0] : NULL;
		if (init && init->kind == AST_OBJECT) {
			/* 1. Emit main variable as pointer to JSON string */
			char *json = ast_to_json_string(init);
			int sid = add_string_literal(json);
			free(json);
			emit(c, ".data");
			emit(c, ".globl %s", node->name);
			emit(c, ".align 8");
			emit(c, "%s: .quad .Lstr%d", node->name, sid);

			/* 2. Flatten properties into globals */
			gen_json_globals_recursive(c, node->name, init);
		} else {
			/* Normal var decl */
			emit(c, ".data");
			emit(c, ".globl %s", node->name);
			emit(c, ".align 8");
			emit(c, "%s: .quad 0", node->name);

			if (init) {
				emit(c, ".text");
				gen_expr(c, init);
				emit(c, "    mov %%rax, %s(%%rip)", node->name);
			}
		}
	}
		break;
	case AST_RETURN:
		if (node->child_count > 0)
			gen_expr(c, node->children[0]);
		else
			emit(c, "    mov $0, %%rax");
		emit(c, "    leave");
		emit(c, "    ret");
		break;
	case AST_IF: {
		int Lelse = new_label(c);
		int Lend = new_label(c);
		gen_expr(c, node->children[0]);
		emit(c, "    cmp $0, %%rax");
		emit(c, "    je .Lelse%d", Lelse);
		gen_stmt(c, node->children[1]);
		emit(c, "    jmp .Lend%d", Lend);
		emit(c, ".Lelse%d:", Lelse);
		if (node->child_count > 2)
			gen_stmt(c, node->children[2]);
		emit(c, ".Lend%d:", Lend);
	}
		break;
	case AST_WHILE: {
		int Ltop = new_label(c);
		int Lend = new_label(c);
		emit(c, ".Lwhile_top%d:", Ltop);
		gen_expr(c, node->children[0]);
		emit(c, "    cmp $0, %%rax");
		emit(c, "    je .Lwhile_end%d", Lend);
		gen_stmt(c, node->children[1]);
		emit(c, "    jmp .Lwhile_top%d", Ltop);
		emit(c, ".Lwhile_end%d:", Lend);
	}
		break;
	case AST_FUNC_DECL: {
		const char *fname = node->name;
		emit(c, ".text");
		emit(c, ".globl %s", fname);
		emit(c, "%s:", fname);
		emit(c, "    push %%rbp");
		emit(c, "    mov %%rsp, %%rbp");
		gen_stmt(c, node->children[0]);
		emit(c, "    leave");
		emit(c, "    ret");
	}
		break;
	default:
		break;
	}
}

/* Generate program */
static void gen_program(CodeGenCtx *c, AST *prog) {
	/* Data section for strings will be populated as we go?
	 * No, we need to collect them. But we emit them at end or begin.
	 * Better to emit empty data section head, then text, then rodata.
	 * Actually, simpler: emit text, and append strings at end. */

	emit(c, ".text");

	/* Generate functions first */
	for (FuncEntry *f = funcs; f; f = f->next) {
		gen_stmt(c, f->fn);
	}

	/* Emit a _start that calls main and exits */
	emit(c, ".globl main");
	emit(c, "main:");
	/* execute global statements (top-level) that are not func decls */
	/* We put them in a "main" implicitly if not present, but here we run them */
	for (int i = 0; i < prog->child_count; ++i) {
		AST *s = prog->children[i];
		if (s->kind != AST_FUNC_DECL) {
			gen_stmt(c, s);
		}
	}
	/* call user main if exists */
	emit(c, "    mov $0, %%rax"); /* prepare for call */
	/* check if main exists */
	int has_main = 0;
	for (FuncEntry *f = funcs; f; f = f->next) {
		if (!strcmp(f->name, "main"))
			has_main = 1;
	}
	if (has_main)
		emit(c, "    call main");

	emit(c, "    mov $60, %%rax");
	emit(c, "    xor %%rdi, %%rdi");
	emit(c, "    syscall");

	/* Strings */
	emit(c, ".section .rodata");
	for (StrEntry *s = str_table; s; s = s->next) {
		char *escaped_s = escape_string_for_asm(s->s);
		emit(c, ".Lstr%d: .string \"%s\"", s->id, escaped_s);
		free(escaped_s);
	}
}

/* ----------------------------------------------------------------------
 * RUNTIME STUBS
 * ---------------------------------------------------------------------- */

static void append_runtime_stubs(FILE *out) {
	fprintf(out,
			"\n"
					"/* --- runtime stubs --- */\n"
					".section .text\n"
					".globl js_print_value\n"
					"js_print_value:\n"
					"    /* rdi = value. Try to guess type: small int vs pointer? */\n"
					"    /* Heuristic: if value < 0x100000, treat as int, else string */\n"
					"    cmp $0x100000, %%rdi\n"
					"    jb .print_int\n"
					"    /* It's a pointer (string) */\n"
					"    mov %%rdi, %%rsi\n"
					"    lea .Lfmt_s(%%rip), %%rdi\n"
					"    xor %%rax, %%rax\n"
					"    call printf\n"
					"    ret\n"
					".print_int:\n"
					"    mov %%rdi, %%rsi\n"
					"    lea .Lfmt_d(%%rip), %%rdi\n"
					"    xor %%rax, %%rax\n"
					"    call printf\n"
					"    ret\n"
					"\n"
					".globl js_print_space\n"
					"js_print_space:\n"
					"    lea .Lfmt_sp(%%rip), %%rdi\n"
					"    xor %%rax, %%rax\n"
					"    call printf\n"
					"    ret\n"
					"\n"
					".globl js_print_newline\n"
					"js_print_newline:\n"
					"    lea .Lfmt_nl(%%rip), %%rdi\n"
					"    xor %%rax, %%rax\n"
					"    call printf\n"
					"    ret\n"
					"\n"
					".section .rodata\n"
					".Lfmt_s: .string \"%%s\"\n"
					".Lfmt_d: .string \"%%lld\"\n" /* Print as integer for test.js */
					".Lfmt_sp: .string \" \"\n"
					".Lfmt_nl: .string \"\\n\"\n"
					".section .text\n");
}

/* ----------------------------------------------------------------------
 * MAIN
 * ---------------------------------------------------------------------- */

int main(int argc, char **argv) {
	if (argc < 2) {
		fprintf(stderr, "Usage: %s input.sparrow\n", prog_name);
		return 1;
	}
	srand((unsigned int) time(NULL));

	char *src = read_file(argv[1]);
	if (!src)
		fatal("Failed to read input file '%s'", argv[1]);

	/* Tokenize */
	TokenStream ts = tokenize(src);
	Parser parser = { .ts = &ts };
	AST *prog = parse_program(&parser);

	/* Register top-level functions */
	for (int i = 0; i < prog->child_count; ++i) {
		AST *s = prog->children[i];
		if (s->kind == AST_FUNC_DECL) {
			register_function(s->name, s);
		}
	}

	/* Prepare output */
	FILE *out = fopen("out.s", "w");
	if (!out)
		fatal("Failed to open out.s for writing");

	CodeGenCtx ctx = { .out = out, .label_counter = 0 };
	gen_program(&ctx, prog);
	append_runtime_stubs(out);
	fclose(out);

	/* Build */
	int rc;
	rc =
			system(
					"gcc -no-pie out.s -o demo -O2 -lm -static -s 2>/dev/null || gcc out.s -o demo -O2 -lm 2>/dev/null");
	if (rc != 0) {
		fprintf(stderr,
				"Warning: link step returned %d. Ensure gcc/libc is installed.\n",
				rc);
	}

	if (access("demo", X_OK) == 0) {
		printf("Running ./demo\n\n");
		rc = system("./demo");
		(void) rc;
	} else {
		fprintf(stderr, "Failed to build demo executable\n");
	}

	free(src);
	return 0;
}
