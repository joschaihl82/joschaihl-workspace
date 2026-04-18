// cc.c
// Minimal educational C compiler (amalgamated single-file)
// Reads one or more input files, preprocesses #include "file", tokenizes,
// parses, and emits x86-64 assembly.
//
// Usage: cc file1.c [file2.c ...]

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>

// --- I. Data Structures and Global State ---

// Type System
typedef enum {
	ty_int, ty_char, ty_long, ty_ptr
} type_kind;
typedef struct ty {
	type_kind kind;
	struct ty *base; // used for pointers
	int size;        // 1, 4, or 8 bytes
	int align;       // alignment requirement
	int is_unsigned;
} ty;

// Local Variables
typedef struct lvar {
	struct lvar *next;
	char *name;
	int len;
	int offset;
	ty *type;
} lvar;

// Functions
typedef struct function {
	struct function *next;
	char *name;
	struct node *body;
	lvar *locals;
	int stack_size;
	int num_params;
} function;

// Tokenizer
typedef enum {
	tk_eof, tk_reserved, tk_keyword, tk_num, tk_ident
} token_kind;
typedef struct token {
	token_kind kind;
	struct token *next;
	long val;
	char *str;
	int len;
	char *file_pos; // start of the token for error reporting
	ty *type;
} token;

// AST Nodes
typedef enum {
	nd_num,
	nd_lvar,
	nd_assign,
	nd_funcall,
	nd_return,
	nd_if,
	nd_while,
	nd_for,
	nd_block,
	nd_expr_stmt,
	nd_add,
	nd_sub,
	nd_mul,
	nd_div,
	nd_eq,
	nd_ne,
	nd_lt,
	nd_le,
	nd_addr,
	nd_deref,
	nd_break,
	nd_continue,
	nd_switch,
	nd_case
} node_kind;

typedef struct node {
	node_kind kind;
	struct node *lhs;
	struct node *rhs;
	struct node *els;
	struct node *next;
	long val;
	char *name;
	ty *type;
	lvar *var;
} node;

// Control Flow Labels for break/continue
typedef struct continue_break_labels {
	int continue_label;
	int break_label;
	struct continue_break_labels *next;
} continue_break_labels;

// --- Global Compiler State ---
char *user_input;
token *current_token;
function *program_head;
function *current_func;
continue_break_labels *loop_or_switch_labels = NULL;
int label_count = 0;

// function prototypes for recursive descent and helpers
token* tokenize(char *p);
void codegen_program(function *funcs);
void add_type(node *n);
lvar* find_lvar(token *tok, ty *type);
ty* parse_type_specifier();
ty* parse_declarator(ty *base);
int consume(char *op);
int consume_keyword(char *kw);
token* expect_ident();
void expect(char *op);
long expect_number();
node* new_num_node(long val);
node* new_node(node_kind kind, node *lhs, node *rhs);
node* expr();

// --- II. Utility and Error Functions (ENHANCED) ---

void error_at(char *loc, char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);

	int line_num = 1;
	char *line_start = user_input;
	for (char *p = user_input; p < loc; p++) {
		if (*p == '\n') {
			line_num++;
			line_start = p + 1;
		}
	}

	char *line_end = loc;
	while (*line_end != '\n' && *line_end != '\0')
		line_end++;

	int column = loc - line_start;

	fprintf(stderr, "\n--- compilation error ---\n");
	fprintf(stderr, "line %d:%d: error: ", line_num, column + 1);
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
	fprintf(stderr, "%.*s\n", (int) (line_end - line_start), line_start);
	fprintf(stderr, "%*s^\n", column, "");
	exit(1);
}

void error_tok(token *tok, char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);

	char *loc = tok->str;

	int line_num = 1;
	char *line_start = user_input;
	for (char *p = user_input; p < loc; p++) {
		if (*p == '\n') {
			line_num++;
			line_start = p + 1;
		}
	}

	char *line_end = loc;
	while (*line_end != '\n' && *line_end != '\0')
		line_end++;

	int column = loc - line_start;

	fprintf(stderr, "\n--- compilation error ---\n");
	fprintf(stderr, "line %d:%d: error: ", line_num, column + 1);
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
	fprintf(stderr, "%.*s\n", (int) (line_end - line_start), line_start);
	fprintf(stderr, "%*s^", column, "");
	for (int i = 0; i < tok->len - 1; i++)
		fprintf(stderr, "~");
	fprintf(stderr, "\n");
	exit(1);
}

// --- Type Helpers ---

ty* new_type(type_kind kind, int size, int align) {
	ty *t = calloc(1, sizeof(ty));
	t->kind = kind;
	t->size = size;
	t->align = align;
	t->is_unsigned = 0;
	return t;
}
ty* ty_int_def() {
	return new_type(ty_int, 4, 4);
}
ty* ty_char_def() {
	return new_type(ty_char, 1, 1);
}
ty* ty_long_def() {
	return new_type(ty_long, 8, 8);
}
ty* new_type_ptr(ty *base) {
	ty *t = new_type(ty_ptr, 8, 8);
	t->base = base;
	return t;
}
ty* get_common_type(ty *t1, ty *t2) {
	if (t1->kind == ty_ptr)
		return t1;
	if (t2->kind == ty_ptr)
		return t2;
	if (t1->size >= t2->size)
		return t1;
	return t2;
}
void add_type(node *n) {
	if (!n)
		return;
	if (n->type)
		return;

	add_type(n->lhs);
	add_type(n->rhs);
	add_type(n->els);

	switch (n->kind) {
	case nd_addr:
		n->type = new_type_ptr(n->lhs->type);
		return;
	case nd_deref:
		if (n->lhs->type->kind != ty_ptr)
			error_tok(current_token, "cannot dereference non-pointer");
		n->type = n->lhs->type->base;
		return;
	case nd_assign:
		n->type = n->lhs->type;
		return;
	case nd_add:
	case nd_sub:
	case nd_mul:
	case nd_div:
		n->type = get_common_type(n->lhs->type, n->rhs->type);
		return;
	case nd_eq:
	case nd_ne:
	case nd_lt:
	case nd_le:
		n->type = ty_int_def();
		return;
	case nd_lvar:
	case nd_num:
	case nd_funcall:
		return;
	default:
		break;
	}
}

// --- III. Preprocessor: handle #include "file" and strip other directives ---

// Read an entire file into a malloc'd buffer (null-terminated). Caller frees.
static char* read_file_to_buffer(const char *path) {
	FILE *f = fopen(path, "rb");
	if (!f)
		return NULL;
	if (fseek(f, 0, SEEK_END) != 0) {
		fclose(f);
		return NULL;
	}
	long sz = ftell(f);
	if (sz < 0) {
		fclose(f);
		return NULL;
	}
	rewind(f);
	char *buf = malloc((size_t) sz + 1);
	if (!buf) {
		fclose(f);
		return NULL;
	}
	size_t n = fread(buf, 1, (size_t) sz, f);
	fclose(f);
	if (n != (size_t) sz) {
		free(buf);
		return NULL;
	}
	buf[n] = '\0';
	return buf;
}

// Helper to ensure output buffer capacity
static void ensure_capacity(char **outp, size_t *cap, size_t need) {
	if (*cap >= need)
		return;
	size_t newcap = (*cap == 0) ? 4096 : *cap;
	while (newcap < need)
		newcap *= 2;
	*outp = realloc(*outp, newcap);
	*cap = newcap;
}

// Forward declaration for recursion
static char* preprocess_includes(const char *input, int depth);

// Preprocess includes: expand #include "file" recursively, ignore #include <...> and other directives.
// depth limits recursion to avoid infinite loops.
static char* preprocess_includes(const char *input, int depth) {
	if (depth > 64) {
		error_at((char*) input, "include recursion too deep");
	}

	size_t cap = 0;
	char *out = NULL;
	size_t outlen = 0;

	const char *p = input;
	while (*p) {
		// Find end of current line
		const char *line_start = p;
		const char *line_end = strchr(p, '\n');
		if (!line_end)
			line_end = p + strlen(p);

		// Skip leading whitespace
		const char *s = p;
		while (*s == ' ' || *s == '\t')
			s++;

		if (*s == '#') {
			// It's a preprocessor directive line. Parse directive name.
			const char *d = s + 1;
			while (*d == ' ' || *d == '\t')
				d++;
			// Check for "include"
			if (strncmp(d, "include", 7) == 0 && !isalnum((unsigned char )d[7])
					&& d[7] != '_') {
				const char *q = d + 7;
				while (*q == ' ' || *q == '\t')
					q++;
				if (*q == '"') {
					// quoted include: "file"
					q++;
					const char *qend = q;
					while (*qend && *qend != '"' && qend < line_end)
						qend++;
					if (*qend == '"') {
						size_t fnlen = qend - q;
						char *fname = malloc(fnlen + 1);
						memcpy(fname, q, fnlen);
						fname[fnlen] = '\0';

						char *incbuf = read_file_to_buffer(fname);
						if (!incbuf) {
							free(fname);
							error_at((char*) line_start,
									"cannot open include file: %s", fname);
						}
						// Recursively preprocess included content
						char *incpp = preprocess_includes(incbuf, depth + 1);
						free(incbuf);

						// Append included preprocessed content
						size_t need = outlen + strlen(incpp) + 1;
						ensure_capacity(&out, &cap, need);
						memcpy(out + outlen, incpp, strlen(incpp));
						outlen += strlen(incpp);
						out[outlen] = '\0';

						free(incpp);
						free(fname);

						// advance p to next line
						p = (*line_end == '\n') ? line_end + 1 : line_end;
						continue;
					}
				} else if (*q == '<') {
					// system include <...> - ignore (do not expand)
					p = (*line_end == '\n') ? line_end + 1 : line_end;
					continue;
				} else {
					// malformed include - skip the line
					p = (*line_end == '\n') ? line_end + 1 : line_end;
					continue;
				}
			} else {
				// Other preprocessor directives: skip the line entirely
				p = (*line_end == '\n') ? line_end + 1 : line_end;
				continue;
			}
		}

		// Not a preprocessor directive: copy the line as-is
		size_t linelen = line_end - p;
		ensure_capacity(&out, &cap, outlen + linelen + 2);
		memcpy(out + outlen, p, linelen);
		outlen += linelen;
		if (*line_end == '\n') {
			out[outlen++] = '\n';
			p = line_end + 1;
		} else {
			p = line_end;
		}
	}

	// Null-terminate
	ensure_capacity(&out, &cap, outlen + 1);
	out[outlen] = '\0';
	return out;
}

// --- IV. Lexer Implementation (skips // and /* */ comments) ---

token* new_token(token_kind kind, token *cur, char *str, int len) {
	token *tok = calloc(1, sizeof(token));
	tok->kind = kind;
	tok->str = str;
	tok->len = len;
	tok->file_pos = str;
	cur->next = tok;
	return tok;
}

int is_ident_char(char c) {
	return isalnum((unsigned char)c) || c == '_';
}

int consume(char *op) {
	if (current_token->kind != tk_reserved
			|| current_token->len != (int) strlen(op)
			|| strncmp(current_token->str, op, current_token->len) != 0)
		return 0;
	current_token = current_token->next;
	return 1;
}

int consume_keyword(char *kw) {
	if (current_token->kind != tk_keyword
			|| current_token->len != (int) strlen(kw)
			|| strncmp(current_token->str, kw, current_token->len) != 0)
		return 0;
	current_token = current_token->next;
	return 1;
}

token* expect_ident() {
	if (current_token->kind != tk_ident)
		error_tok(current_token, "expected identifier");
	token *t = current_token;
	current_token = current_token->next;
	return t;
}

void expect(char *op) {
	if (!consume(op))
		error_tok(current_token, "expected '%s'", op);
}

long expect_number() {
	if (current_token->kind != tk_num)
		error_tok(current_token, "expected number");
	long val = current_token->val;
	current_token = current_token->next;
	return val;
}

struct {
	char *name;
	int len;
} keywords[] = { { "return", 6 }, { "if", 2 }, { "else", 4 }, { "while", 5 }, {
		"for", 3 }, { "break", 5 }, { "continue", 8 }, { "switch", 6 }, {
		"case", 4 }, { "default", 7 }, { "int", 3 }, { "char", 4 },
		{ "long", 4 }, { "unsigned", 8 }, { NULL, 0 } };

token* tokenize(char *p) {
	token *head = calloc(1, sizeof(token));
	token *cur = head;

	while (*p) {
		// Skip whitespace
		if (isspace((unsigned char )*p)) {
			p++;
			continue;
		}

		// Skip single-line comments //
		if (p[0] == '/' && p[1] == '/') {
			p += 2;
			while (*p && *p != '\n')
				p++;
			continue;
		}

		// Skip block comments /* ... */
		if (p[0] == '/' && p[1] == '*') {
			p += 2;
			while (*p) {
				if (p[0] == '*' && p[1] == '/') {
					p += 2;
					break;
				}
				p++;
			}
			if (!*p)
				break; // EOF inside comment
			continue;
		}

		// Two-character punctuators
		if (strncmp(p, "==", 2) == 0 || strncmp(p, "!=", 2) == 0
				|| strncmp(p, "<=", 2) == 0 || strncmp(p, ">=", 2) == 0) {
			cur = new_token(tk_reserved, cur, p, 2);
			p += 2;
			continue;
		}

		// Single-character punctuators and operators (include comma)
		if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '='
				|| *p == '(' || *p == ')' || *p == ';' || *p == '<' || *p == '>'
				|| *p == '{' || *p == '}' || *p == '&' || *p == ':'
				|| *p == ',') {
			cur = new_token(tk_reserved, cur, p, 1);
			p++;
			continue;
		}

		// Keywords
		int is_kw = 0;
		for (int i = 0; keywords[i].name; i++) {
			if (strncmp(p, keywords[i].name, keywords[i].len) == 0
					&& !is_ident_char(p[keywords[i].len])) {
				cur = new_token(tk_keyword, cur, p, keywords[i].len);
				p += keywords[i].len;
				is_kw = 1;
				break;
			}
		}
		if (is_kw)
			continue;

		// Identifier
		if (isalpha((unsigned char)*p) || *p == '_') {
			char *q = (char*) p;
			while (is_ident_char(*q))
				q++;
			cur = new_token(tk_ident, cur, p, q - p);
			p = q;
			continue;
		}

		// Number literal
		if (isdigit((unsigned char )*p)) {
			char *q = (char*) p;
			cur = new_token(tk_num, cur, p, 0);
			cur->val = strtol(p, &p, 10);
			cur->len = p - q;
			cur->type = ty_int_def();
			continue;
		}

		error_at((char*) p, "invalid token");
	}

	new_token(tk_eof, cur, p, 0);
	return head->next;
}

// --- V. Parser Implementation ---

// function prototypes for recursive descent
node* block();
node* stmt();
node* expr();
node* assign();
node* equality();
node* relational();
node* add();
node* mul();
node* unary();
node* primary();

// node constructors
node* new_node(node_kind kind, node *lhs, node *rhs) {
	node *n = calloc(1, sizeof(node));
	n->kind = kind;
	n->lhs = lhs;
	n->rhs = rhs;
	return n;
}

node* new_num_node(long val) {
	node *n = calloc(1, sizeof(node));
	n->kind = nd_num;
	n->val = val;
	n->type = ty_int_def();
	return n;
}

// variable helper
lvar* find_lvar(token *tok, ty *type) {
	for (lvar *var = current_func->locals; var; var = var->next) {
		if (var->len == tok->len && !strncmp(tok->str, var->name, var->len)) {
			return var;
		}
	}

	lvar *var = calloc(1, sizeof(lvar));
	var->next = current_func->locals;
	var->name = tok->str;
	var->len = tok->len;
	var->type = type;

	int aligned_size = (type->size + type->align - 1) & ~(type->align - 1);
	var->offset = (current_func->locals ? current_func->locals->offset : 0)
			+ aligned_size;
	current_func->locals = var;
	current_func->stack_size = var->offset;
	return var;
}

// Type/Declarator parsing
ty* parse_type_specifier() {
	int is_unsigned = 0;
	if (consume_keyword("unsigned"))
		is_unsigned = 1;

	ty *t;
	if (consume_keyword("char"))
		t = ty_char_def();
	else if (consume_keyword("long"))
		t = ty_long_def();
	else if (consume_keyword("int"))
		t = ty_int_def();
	else if (is_unsigned)
		t = ty_int_def();
	else
		error_tok(current_token, "expected type specifier");

	t->is_unsigned = is_unsigned;
	return t;
}

ty* parse_declarator(ty *base) {
	while (consume("*")) {
		base = new_type_ptr(base);
	}
	return base;
}

// Declaration helper
void parse_declaration(function *func) {
	ty *t = parse_type_specifier();
	t = parse_declarator(t);
	token *var_name = expect_ident();
	find_lvar(var_name, t);
	expect(";");
}

// Function definition
function* parse_fundef() {
	ty *ret_type = parse_type_specifier();
	ret_type = parse_declarator(ret_type);

	function *func = calloc(1, sizeof(function));
	current_func = func;

	token *name_tok = expect_ident();
	func->name = strndup(name_tok->str, name_tok->len);

	expect("(");

	if (!consume(")")) {
		// First parameter
		ty *param_type = parse_type_specifier();
		param_type = parse_declarator(param_type);
		token *param_tok = expect_ident();
		find_lvar(param_tok, param_type); // 1st param
		func->num_params = 1;

		// Additional parameters
		while (consume(",")) {
			ty *p_type = parse_type_specifier();
			p_type = parse_declarator(p_type);
			token *p_tok = expect_ident();
			find_lvar(p_tok, p_type);
			func->num_params++;
		}

		expect(")");
	}

	expect("{");
	func->body = block();
	expect("}");

	func->next = program_head;
	program_head = func;
	return func;
}

node* program() {
	while (current_token->kind != tk_eof) {
		parse_fundef();
	}
	return NULL;
}

node* block() {
	node *head = calloc(1, sizeof(node));
	node *cur = head;

	while (!consume("}")) {
		cur->next = stmt();
		cur = cur->next;
	}
	return head->next;
}

// Control flow block helpers
void enter_control_block(int cont_label, int break_label) {
	continue_break_labels *new_block = calloc(1, sizeof(continue_break_labels));
	new_block->continue_label = cont_label;
	new_block->break_label = break_label;
	new_block->next = loop_or_switch_labels;
	loop_or_switch_labels = new_block;
}

void exit_control_block() {
	if (loop_or_switch_labels) {
		loop_or_switch_labels = loop_or_switch_labels->next;
	}
}

// Statement parsing
node* stmt() {
	node *n;

	// Declarations
	if (current_token->kind == tk_keyword
			&& (strncmp(current_token->str, "int", 3) == 0
					|| strncmp(current_token->str, "char", 4) == 0
					|| strncmp(current_token->str, "long", 4) == 0
					|| strncmp(current_token->str, "unsigned", 8) == 0)) {
		parse_declaration(current_func);
		return new_node(nd_block, NULL, NULL);
	}

	if (consume_keyword("return")) {
		n = new_node(nd_return, expr(), NULL);
		expect(";");
	} else if (consume_keyword("if")) {
		expect("(");
		node *cond = expr();
		expect(")");
		node *then_stmt = stmt();
		node *els_stmt = consume_keyword("else") ? stmt() : NULL;
		n = new_node(nd_if, cond, then_stmt);
		n->els = els_stmt;
	} else if (consume_keyword("while")) {
		expect("(");
		node *cond = expr();
		expect(")");
		n = new_node(nd_while, cond, stmt());
	} else if (consume_keyword("for")) {
		expect("(");
		node *init = NULL;
		if (!consume(";")) {
			init = expr();
			expect(";");
		}
		node *cond = NULL;
		if (!consume(";")) {
			cond = expr();
			expect(";");
		}
		node *inc = NULL;
		if (!consume(")")) {
			inc = expr();
			expect(")");
		}
		n = new_node(nd_for, init, cond);
		n->els = inc;
		n->next = stmt();
	} else if (consume_keyword("switch")) {
		expect("(");
		node *cond = expr();
		expect(")");
		n = new_node(nd_switch, cond, stmt());
	} else if (consume_keyword("case")) {
		node *lhs = new_num_node(expect_number());
		expect(":");
		n = new_node(nd_case, lhs, stmt());
	} else if (consume_keyword("default")) {
		expect(":");
		n = new_node(nd_case, new_num_node(-1), stmt());
	} else if (consume_keyword("break")) {
		if (!loop_or_switch_labels)
			error_tok(current_token, "break must be inside control block");
		n = new_node(nd_break, NULL, NULL);
		expect(";");
	} else if (consume_keyword("continue")) {
		if (!loop_or_switch_labels)
			error_tok(current_token, "continue must be inside control block");
		n = new_node(nd_continue, NULL, NULL);
		expect(";");
	} else {
		n = new_node(nd_expr_stmt, expr(), NULL);
		expect(";");
	}
	return n;
}

// Expression parsing (respecting C precedence)
node* expr() {
	return assign();
}
node* assign() {
	node *n = equality();
	if (consume("=")) {
		n = new_node(nd_assign, n, assign());
	}
	return n;
}
node* equality() {
	node *n = relational();
	for (;;) {
		if (consume("=="))
			n = new_node(nd_eq, n, relational());
		else if (consume("!="))
			n = new_node(nd_ne, n, relational());
		else
			return n;
	}
}
node* relational() {
	node *n = add();
	for (;;) {
		if (consume("<"))
			n = new_node(nd_lt, n, add());
		else if (consume("<="))
			n = new_node(nd_le, n, add());
		else if (consume(">"))
			n = new_node(nd_lt, add(), n);
		else if (consume(">="))
			n = new_node(nd_le, add(), n);
		else
			return n;
	}
}
node* add() {
	node *n = mul();
	for (;;) {
		if (consume("+"))
			n = new_node(nd_add, n, mul());
		else if (consume("-"))
			n = new_node(nd_sub, n, mul());
		else
			return n;
	}
}
node* mul() {
	node *n = unary();
	for (;;) {
		if (consume("*"))
			n = new_node(nd_mul, n, unary());
		else if (consume("/"))
			n = new_node(nd_div, n, unary());
		else
			return n;
	}
}
node* unary() {
	if (consume("+"))
		return primary();
	if (consume("-"))
		return new_node(nd_sub, new_num_node(0), primary());
	if (consume("*")) {
		node *n = new_node(nd_deref, unary(), NULL);
		return n;
	}
	if (consume("&")) {
		node *n = new_node(nd_addr, unary(), NULL);
		return n;
	}
	return primary();
}
node* primary() {
	if (consume("(")) {
		node *n = expr();
		expect(")");
		return n;
	}

	if (current_token->kind == tk_ident && current_token->next
			&& current_token->next->kind == tk_reserved
			&& current_token->next->len == 1
			&& current_token->next->str[0] == '(') {
		token *func_name = expect_ident();
		expect("(");
		node *n = calloc(1, sizeof(node));
		n->kind = nd_funcall;
		n->name = strndup(func_name->str, func_name->len);
		if (!consume(")")) {
			n->lhs = expr();
			while (consume(","))
				expr();
			expect(")");
		}
		return n;
	}

	if (current_token->kind == tk_ident) {
		token *tok = current_token;
		lvar *lv = find_lvar(tok, ty_int_def());
		current_token = current_token->next;
		node *n = calloc(1, sizeof(node));
		n->kind = nd_lvar;
		n->var = lv;
		return n;
	}
	if (current_token->kind == tk_num) {
		return new_num_node(expect_number());
	}

	error_tok(current_token, "expected primary expression");
}

// --- V. Code Generator Implementation (x64 Assembly) ---

char* get_reg(char *r8, char *r4, char *r2, char *r1, size_t size) {
	if (size == 8)
		return r8;
	if (size == 4)
		return r4;
	if (size == 2)
		return r2;
	if (size == 1)
		return r1;
	return r8;
}

// forward declarations
void gen_expr(node *n);
void gen_stmt(node *n);

void gen_lval(node *n) {
	if (n->kind == nd_lvar) {
		printf("  mov rax, rbp\n");
		printf("  sub rax, %d\n", n->var->offset);
		printf("  push rax\n");
		return;
	}
	if (n->kind == nd_deref) {
		gen_expr(n->lhs);
		return;
	}
	error_tok(current_token, "expected lvalue");
}

void gen_load(ty *t) {
	printf("  pop rax\n");

	if (t->kind == ty_ptr || t->size == 8) {
		printf("  mov rax, [rax]\n");
	} else if (t->is_unsigned && t->size == 4) {
		// Load 32-bit into eax; writing to eax zero-extends into rax
		printf("  mov eax, [rax]\n");
	} else if (t->size == 4) {
		printf("  movsx rax, dword ptr [rax]\n");
	} else if (t->size == 1) {
		printf("  movsx rax, byte ptr [rax]\n");
	}
	printf("  push rax\n");
}

void gen_store(ty *t) {
	printf("  pop rdi\n");
	printf("  pop rax\n");

	if (t->kind == ty_ptr || t->size == 8) {
		printf("  mov [rax], rdi\n");
	} else if (t->size == 4) {
		printf("  mov dword ptr [rax], edi\n");
	} else if (t->size == 1) {
		printf("  mov byte ptr [rax], dil\n");
	}
	printf("  push rdi\n");
}

void gen_expr(node *n) {
	if (!n)
		return;
	add_type(n);

	switch (n->kind) {
	case nd_num:
		printf("  push %ld\n", n->val);
		return;
	case nd_lvar:
		gen_lval(n);
		gen_load(n->type);
		return;
	case nd_assign:
		gen_lval(n->lhs);
		gen_expr(n->rhs);
		gen_store(n->lhs->type);
		return;
	case nd_addr:
		gen_lval(n->lhs);
		return;
	case nd_deref:
		gen_expr(n->lhs);
		gen_load(n->type);
		return;

	case nd_funcall: {
		if (n->lhs) {
			gen_expr(n->lhs);
			printf("  pop rdi\n");
		}
		printf("  mov rax, 0\n");
		printf("  call %s\n", n->name);
		printf("  push rax\n");
		return;
	}

	default: // Binary operators
		// Evaluate lhs then rhs so stack order is: ... lhs rhs
		gen_expr(n->lhs);
		gen_expr(n->rhs);
		// pop rhs into rdi, lhs into rax
		printf("  pop rdi\n");
		printf("  pop rax\n");

		if (n->kind == nd_add)
			printf("  add rax, rdi\n");
		else if (n->kind == nd_sub)
			printf("  sub rax, rdi\n");
		else if (n->kind == nd_mul)
			printf("  imul rax, rdi\n");
		else if (n->kind == nd_div) {
			printf("  cqo\n");
			printf("  idiv rdi\n");
		} else if (n->kind == nd_eq) {
			printf("  cmp rax, rdi\n");
			printf("  sete al\n");
			goto set_byte;
		} else if (n->kind == nd_ne) {
			printf("  cmp rax, rdi\n");
			printf("  setne al\n");
			goto set_byte;
		} else if (n->kind == nd_lt) {
			printf("  cmp rax, rdi\n");
			printf("  setl al\n");
			goto set_byte;
		} else if (n->kind == nd_le) {
			printf("  cmp rax, rdi\n");
			printf("  setle al\n");
			goto set_byte;
		}

		printf("  push rax\n");
		return;
		set_byte: printf("  movzx rax, al\n");
		printf("  push rax\n");
		return;
	}
}

void gen_stmt(node *n) {
	if (!n)
		return;

	switch (n->kind) {
	case nd_return:
		gen_expr(n->lhs);
		printf("  pop rax\n");
		printf("  jmp .L.ret.%s\n", current_func->name);
		return;

	case nd_block:
		for (node *cur = n; cur; cur = cur->next)
			gen_stmt(cur);
		return;

	case nd_if: {
		int l_end = label_count++;
		int l_else = label_count++;
		gen_expr(n->lhs);
		printf("  pop rax\n");
		printf("  cmp rax, 0\n");
		if (n->els) {
			printf("  je .L%d\n", l_else);
			gen_stmt(n->rhs);
			printf("  jmp .L%d\n", l_end);
			printf(".L%d:\n", l_else);
			gen_stmt(n->els);
		} else {
			printf("  je .L%d\n", l_end);
			gen_stmt(n->rhs);
		}
		printf(".L%d:\n", l_end);
		return;
	}

	case nd_while: {
		int l_start = label_count++;
		int l_end = label_count++;
		enter_control_block(l_start, l_end);

		printf(".L%d:\n", l_start);
		gen_expr(n->lhs);
		printf("  pop rax\n");
		printf("  cmp rax, 0\n");
		printf("  je .L%d\n", l_end);
		gen_stmt(n->rhs);
		printf("  jmp .L%d\n", l_start);
		printf(".L%d:\n", l_end);
		exit_control_block();
		return;
	}

	case nd_for: {
		int l_start = label_count++;
		int l_inc = label_count++;
		int l_end = label_count++;
		enter_control_block(l_inc, l_end);

		if (n->lhs)
			gen_expr(n->lhs);
		if (n->lhs)
			printf("  pop rax\n");
		printf(".L%d:\n", l_start);
		if (n->rhs) {
			gen_expr(n->rhs);
			printf("  pop rax\n");
			printf("  cmp rax, 0\n");
			printf("  je .L%d\n", l_end);
		}
		gen_stmt(n->next);
		printf(".L%d:\n", l_inc);
		if (n->els)
			gen_expr(n->els);
		if (n->els)
			printf("  pop rax\n");
		printf("  jmp .L%d\n", l_start);
		printf(".L%d:\n", l_end);
		exit_control_block();
		return;
	}

	case nd_break:
		printf("  jmp .L%d\n", loop_or_switch_labels->break_label);
		return;
	case nd_continue:
		printf("  jmp .L%d\n", loop_or_switch_labels->continue_label);
		return;

	case nd_switch: {
		int l_end = label_count++;
		int l_default = 0;
		enter_control_block(0, l_end);

		for (node *cur = n->rhs; cur; cur = cur->next) {
			if (cur->kind == nd_case) {
				cur->val =
						(cur->lhs->val == -1) ?
								(l_default = label_count++) : label_count++;
			}
		}

		gen_expr(n->lhs);
		printf("  pop rax\n");

		for (node *cur = n->rhs; cur; cur = cur->next) {
			if (cur->kind == nd_case && cur->lhs->val != -1) {
				printf("  cmp rax, %ld\n", cur->lhs->val);
				printf("  je .L%d\n", (int) cur->val);
			}
		}

		if (l_default) {
			printf("  jmp .L%d\n", l_default);
		} else {
			printf("  jmp .L%d\n", l_end);
		}

		for (node *cur = n->rhs; cur; cur = cur->next) {
			if (cur->kind == nd_case) {
				printf(".L%d:\n", (int) cur->val);
				gen_stmt(cur->rhs);
			} else {
				gen_stmt(cur);
			}
		}
		printf(".L%d:\n", l_end);
		exit_control_block();
		return;
	}

	case nd_expr_stmt:
		gen_expr(n->lhs);
		printf("  pop rax\n");
		return;

	default:
		return;
	}
}

void codegen_program(function *funcs) {
	printf(".intel_syntax noprefix\n");
	printf(".global main\n");

	for (function *cur = funcs; cur; cur = cur->next) {
		current_func = cur;
		printf("\n%s:\n", cur->name);

		printf("  push rbp\n");
		printf("  mov rbp, rsp\n");
		int stack_alloc = (cur->stack_size + 15) & ~15;
		printf("  sub rsp, %d\n", stack_alloc);

		if (cur->num_params > 0) {
			lvar *first_param = cur->locals;
			if (first_param->type->size == 8) {
				printf("  mov [rbp-%d], rdi\n", first_param->offset);
			} else if (first_param->type->size == 4) {
				printf("  mov dword ptr [rbp-%d], edi\n", first_param->offset);
			} else if (first_param->type->size == 1) {
				printf("  mov byte ptr [rbp-%d], dil\n", first_param->offset);
			}
		}

		gen_stmt(cur->body);

		printf(".L.ret.%s:\n", cur->name);
		printf("  mov rsp, rbp\n");
		printf("  pop rbp\n");
		printf("  ret\n");
	}
}

// --- VI. Main Driver (reads files only, concatenates, preprocesses includes) ---

int main(int argc, char **argv) {
	if (argc < 2) {
		fprintf(stderr, "usage: cc <file> [file...]\n");
		return 1;
	}

	// Compute total size and read files
	size_t total_size = 0;
	for (int i = 1; i < argc; i++) {
		FILE *f = fopen(argv[i], "rb");
		if (!f) {
			fprintf(stderr, "cannot open file: %s\n", argv[i]);
			return 1;
		}
		if (fseek(f, 0, SEEK_END) != 0) {
			fclose(f);
			fprintf(stderr, "seek failed: %s\n", argv[i]);
			return 1;
		}
		long sz = ftell(f);
		if (sz < 0) {
			fclose(f);
			fprintf(stderr, "ftell failed: %s\n", argv[i]);
			return 1;
		}
		total_size += (size_t) sz + 1; // +1 for newline or null
		fclose(f);
	}

	user_input = malloc(total_size + 1);
	if (!user_input) {
		fprintf(stderr, "out of memory\n");
		return 1;
	}
	user_input[0] = '\0';

	size_t pos = 0;
	for (int i = 1; i < argc; i++) {
		FILE *f = fopen(argv[i], "rb");
		if (!f) {
			fprintf(stderr, "cannot open file: %s\n", argv[i]);
			free(user_input);
			return 1;
		}
		if (fseek(f, 0, SEEK_END) != 0) {
			fclose(f);
			free(user_input);
			fprintf(stderr, "seek failed: %s\n", argv[i]);
			return 1;
		}
		long sz = ftell(f);
		if (sz < 0) {
			fclose(f);
			free(user_input);
			fprintf(stderr, "ftell failed: %s\n", argv[i]);
			return 1;
		}
		rewind(f);

		if (sz > 0) {
			size_t n = fread(user_input + pos, 1, (size_t) sz, f);
			if (n != (size_t) sz) {
				fclose(f);
				free(user_input);
				fprintf(stderr, "read failed: %s\n", argv[i]);
				return 1;
			}
			pos += n;
		}
		fclose(f);

		if (i + 1 < argc) {
			user_input[pos++] = '\n';
		}
	}
	user_input[pos] = '\0';

	// Preprocess includes (expand #include "file")
	char *preprocessed = preprocess_includes(user_input, 0);
	free(user_input);
	user_input = preprocessed;

	// 1. Lexer run
	current_token = tokenize(user_input);

	// 2. Parse() run
	program();

	// 3. Codegen run (output to stdout)
	codegen_program(program_head);

	free(user_input);
	return 0;
}
