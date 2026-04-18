#include "assert.h"
#include "stdbool.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

// Exit the program with a given error message
void panic(char const *msg) {
    puts(msg);
    exit(-1);
}

// x86-64 system v calling convention registers for integer/pointer arguments
char const *arg_regs_pre[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
#define num_arg_regs_pre (sizeof(arg_regs_pre) / sizeof(arg_regs_pre[0]))

/** CHARACTER UTILITIES **/
// The first size of string we try and allocate
#define BASE_STRING_SIZE 16
// Check whether this character is alpha
#define IS_ALPHA(c) (((c) >= 'a' && (c) <= 'z') || ((c) >= 'A' && (c) <= 'Z'))
// Check whether this character is numeric
#define IS_NUMERIC(c) ((c) >= '0' && (c) <= '9')
// Check whether this character is alphanumeric
#define IS_ALPHA_NUMERIC(c) (IS_ALPHA(c) || IS_NUMERIC(c))

/** LEXING **/

// Represents a type of token our lexer produces.
typedef enum TokenType {
    // Single character tokens
    T_LEFT_PARENS,
    T_RIGHT_PARENS,
    T_LEFT_BRACE,
    T_RIGHT_BRACE,
    T_LEFT_BRACKET, // new: [
    T_RIGHT_BRACKET, // new: ]
    T_DOT, // new: .
    T_SEMICOLON,
    T_COMMA,
    T_EQUALS,
    T_PLUS,
    T_MINUS,
    T_SLASH,
    T_ASTERISK,
    T_PERCENT,
    T_EXCLAMATION,
    T_TILDE,
    T_AMPERSAND,
    T_VERT_BAR,
    T_CARET,
    // Double character tokens
    T_EQUALS_EQUALS,
    T_EXCLAMATION_EQUALS,
    T_ARROW, // new: ->
    // Keywords
    T_INT,
    T_RETURN,
    T_IF,
    T_ELSE,
    T_WHILE,
    T_BREAK,
    T_CONTINUE,
    // Litteral
    T_LITT_NUMBER,
    T_LITT_STRING, // new
    T_IDENTIFIER,
    // Special
    T_START,
    T_EOF
} TokenType;

// Represents the type of data that can be contained in a token
typedef union TokenData {
    // A numeric litteral
    int litt;
    // String data for an identifier, or a string litteral
    char *string;
} TokenData;

// Represents a token produced in the lexing phase.
typedef struct Token {
    // Holds which type of token this is.
    TokenType type;
    // Holds information about the data contained in this token
    TokenData data;
} Token;

// Print out a representation of a token to a stream
void token_print(Token t, FILE *fp) {
    switch (t.type) {
    case T_LEFT_PARENS:
        fputs("(\n", fp);
        break;
    case T_RIGHT_PARENS:
        fputs(")\n", fp);
        break;
    case T_LEFT_BRACE:
        fputs("{\n", fp);
        break;
    case T_RIGHT_BRACE:
        fputs("}\n", fp);
        break;
    case T_LEFT_BRACKET: // new
        fputs("[\n", fp);
        break;
    case T_RIGHT_BRACKET: // new
        fputs("]\n", fp);
        break;
    case T_DOT: // new
        fputs(".\n", fp);
        break;
    case T_SEMICOLON:
        fputs(";\n", fp);
        break;
    case T_COMMA:
        fputs(",\n", fp);
        break;
    case T_EQUALS:
        fputs("=\n", fp);
        break;
    case T_PLUS:
        fputs("+\n", fp);
        break;
    case T_MINUS:
        fputs("-\n", fp);
        break;
    case T_SLASH:
        fputs("/\n", fp);
        break;
    case T_ASTERISK:
        fputs("*\n", fp);
        break;
    case T_PERCENT:
        fputs("%\n", fp);
        break;
    case T_EXCLAMATION:
        fputs("!\n", fp);
        break;
    case T_TILDE:
        fputs("~\n", fp);
        break;
    case T_VERT_BAR:
        fputs("|\n", fp);
        break;
    case T_CARET:
        fputs("^\n", fp);
        break;
    case T_AMPERSAND:
        fputs("&\n", fp);
        break;
    case T_EQUALS_EQUALS:
        fputs("==\n", fp);
        break;
    case T_EXCLAMATION_EQUALS:
        fputs("!=\n", fp);
        break;
    case T_ARROW: // new
        fputs("->\n", fp);
        break;
    case T_INT:
        fputs("int\n", fp);
        break;
    case T_RETURN:
        fputs("return\n", fp);
        break;
    case T_IF:
        fputs("if\n", fp);
        break;
    case T_ELSE:
        fputs("else\n", fp);
        break;
    case T_WHILE:
        fputs("while\n", fp);
        break;
    case T_BREAK:
        fputs("break\n", fp);
        break;
    case T_CONTINUE:
        fputs("continue\n", fp);
        break;
    case T_LITT_NUMBER:
        fprintf(fp, "%d\n", t.data.litt);
        break;
    case T_LITT_STRING: // new
        fprintf(fp, "\"%s\"\n", t.data.string);
        break;
    case T_IDENTIFIER:
        fprintf(fp, "%s\n", t.data.string);
        break;
    case T_START:
        fputs("START\n", fp);
        break;
    case T_EOF:
        fputs("EOF\n", fp);
        break;
    }
}

typedef struct LexState {
    // The underlying program (we don't own)
    char const *program;
    // The index we're currently at in the program
    size_t index; // changed from long to size_t
} LexState;

LexState lex_init(char const *program) {
    LexState ret = {.program = program, .index = 0};
    return ret;
}

Token lex_next(LexState *st) {
    Token token = {.type = T_EOF, .data = {.litt = 0}};
    // We always return unless we continue
    for (;;) {
        char next = st->program[st->index];
        if (next == 0) {
            token.type = T_EOF;
            return token; // added return for EOF
        } else if (next == '(') {
            st->index++;
            token.type = T_LEFT_PARENS;
        } else if (next == ')') {
            st->index++;
            token.type = T_RIGHT_PARENS;
        } else if (next == '{') {
            st->index++;
            token.type = T_LEFT_BRACE;
        } else if (next == '}') {
            st->index++;
            token.type = T_RIGHT_BRACE;
        } else if (next == '[') { // new
            st->index++;
            token.type = T_LEFT_BRACKET;
        } else if (next == ']') { // new
            st->index++;
            token.type = T_RIGHT_BRACKET;
        } else if (next == '.') { // new
            st->index++;
            token.type = T_DOT;
        } else if (next == ';') {
            st->index++;
            token.type = T_SEMICOLON;
        } else if (next == ',') {
            st->index++;
            token.type = T_COMMA;
        } else if (next == '=') {
            st->index++;
            next = st->program[st->index];
            if (next == '=') {
                st->index++;
                token.type = T_EQUALS_EQUALS;
            } else {
                token.type = T_EQUALS;
            }
        } else if (next == '+') {
            st->index++;
            token.type = T_PLUS;
        } else if (next == '-') {
            char next_next = st->program[st->index + 1]; // check for ->
            if (next_next == '>') {
                st->index += 2;
                token.type = T_ARROW;
            } else {
                st->index++;
                token.type = T_MINUS;
            }
        } else if (next == '/') {
            st->index++;
            next = st->program[st->index];
            if (next == '/') {
                while (next != '\n') {
                    next = st->program[st->index++];
                }
                continue;
            } else if (next == '*') {
                // This avoids us matching `/*/` as a complete comment
                st->index++;
                int stage = 0;
                for (;;) {
                    next = st->program[st->index++];
                    if (next == 0) {
                        panic("unterminated block comment");
                    }
                    if (stage == 1 && next == '/') {
                        break;
                    } else if (next == '*') {
                        stage = 1;
                    } else {
                        stage = 0;
                    }
                }
                continue;
            } else {
                token.type = T_SLASH;
            }
        } else if (next == '*') {
            st->index++;
            token.type = T_ASTERISK;
        } else if (next == '%') {
            st->index++;
            token.type = T_PERCENT;
        } else if (next == '!') {
            st->index++;
            next = st->program[st->index];
            if (next == '=') {
                st->index++;
                token.type = T_EXCLAMATION_EQUALS;
            } else {
                token.type = T_EXCLAMATION;
            }
        } else if (next == '~') {
            st->index++;
            token.type = T_TILDE;
        } else if (next == '&') {
            st->index++;
            token.type = T_AMPERSAND;
        } else if (next == '|') {
            st->index++;
            token.type = T_VERT_BAR;
        } else if (next == '^') {
            st->index++;
            token.type = T_CARET;
        } else if (next == '"') { // new: handle string literal
            st->index++;
            size_t size = BASE_STRING_SIZE;
            char *buf = malloc(size);
            unsigned int index = 0;

            for (;;) {
                next = st->program[st->index++];
                if (next == 0) {
                    panic("unterminated string literal");
                }
                if (next == '"') {
                    break;
                }

                // minimal escape sequence handling
                if (next == '\\') {
                    next = st->program[st->index++];
                    // handle common escapes here if needed (\n, \t, etc.)
                    // for bootstrap, we only check for escaped quote/backslash
                    if (next == 0) panic("unterminated string literal after escape");
                    if (next != '"' && next != '\\') {
                        // simplest approach for bootstrap: treat any other escape as the character itself
                        st->index--;
                        next = '\\'; // put back the slash, treat as literal char
                    }
                }

                if (index >= size - 1) {
                    size <<= 1;
                    buf = realloc(buf, size);
                    if (buf == NULL) panic("failed to realloc buffer");
                }
                buf[index++] = next;
            }
            buf[index] = 0; // null-terminate the string
            token.type = T_LITT_STRING;
            token.data.string = buf; // the token now owns the allocated string
            return token;
        } else if (IS_ALPHA(next)) {
            size_t size = BASE_STRING_SIZE;
            char *buf = malloc(size);
            unsigned int index = 0;
            for (; IS_ALPHA_NUMERIC(next); next = st->program[st->index]) {
                // Leave space for the last byte
                if (index >= size - 1) {
                    size <<= 1;
                    buf = realloc(buf, size);
                }
                buf[index++] = next;
                st->index++;
            }
            buf[index] = 0;
            if (strcmp(buf, "return") == 0) {
                token.type = T_RETURN;
            } else if (strcmp(buf, "int") == 0) {
                token.type = T_INT;
            } else if (strcmp(buf, "if") == 0) {
                token.type = T_IF;
            } else if (strcmp(buf, "else") == 0) {
                token.type = T_ELSE;
            } else if (strcmp(buf, "while") == 0) {
                token.type = T_WHILE;
            } else if (strcmp(buf, "break") == 0) {
                token.type = T_BREAK;
            } else if (strcmp(buf, "continue") == 0) {
                token.type = T_CONTINUE;
            } else {
                token.type = T_IDENTIFIER;
                token.data.string = buf;
            }
        } else if (IS_NUMERIC(next)) {
            int buf = 0;
            for (; IS_NUMERIC(next); next = st->program[st->index]) {
                buf = buf * 10 + next - '0';
                st->index++;
            }
            token.type = T_LITT_NUMBER;
            token.data.litt = buf;
        } else if (next == '\t' || next == '\n' || next == ' ' || next == '\r') {
            st->index++;
            continue; // continue the loop to skip whitespace
        } else {
            printf("Error at index %zu:\n", st->index);
            panic("Unexpected character");
        }
        return token;
    }
}

/** AST DEFINITIONS **/

typedef struct AstNode AstNode;

// This enum identifies what kind of node we're dealing with in a tre
typedef enum AstKind {
    // Represents an int main function with a sequence of statements
    K_INT_MAIN,
    // Holds the top level declarations
    K_TOP_LEVEL,
    // Represents a function with a body
    K_FUNCTION,
    // Represents the inputs to a function call or definition
    K_PARAMS,
    // Represents a function call
    K_CALL,
    // Represents a block containing a series of statements
    K_BLOCK,
    // Represents an expression statement e.g. `foo();`
    K_EXPR_STATEMENT,
    // Represents a declaration statement e.g. `int x = 1;`
    K_DECLARATION,
    // Represents a return statement e.g. `return 1;`
    K_RETURN,
    // Represents a break statement
    K_BREAK,
    // Represents a continue statement
    K_CONTINUE,
    // Represents an if statement e.g. `if (x) return 2;`
    K_IF,
    // Represents a while loop, e.g. `while (x) return 1;`
    K_WHILE,
    // Type qualifiers
    K_DEREFERENCE, // *expr
    K_ADDRESS_OF, // &expr
    K_SUBSCRIPT, // expr[index]
    K_MEMBER_ACCESS, // expr.member or expr->member
    // Represents a declaration with initialization
    K_INIT_DECLARATION,
    // Represents a declaration without initialization
    K_NO_INIT_DECLARATION,
    // Represents a top level expression, which can be seperated by commas
    K_TOP_EXPR,
    // x = y
    K_ASSIGN,
    // x == y
    K_EQUALS,
    // x != y
    K_NOT_EQUALS,
    // a + b
    K_ADD,
    // a - b
    K_SUB,
    // a * b
    K_MUL,
    // a / b
    K_DIV,
    // a % b
    K_MOD,
    // !a
    K_LOGICAL_NOT,
    // ~a
    K_BIT_NOT,
    // a & b
    K_BIT_AND,
    // a | b
    K_BIT_OR,
    // a ^ b
    K_BIT_XOR,
    // -a
    K_NEGATE,
    // Represents an identifier.
    K_IDENTIFIER,
    // Represents a numeric litteral
    K_NUMBER,
    // Represents a string litteral
    K_LITT_STRING // new
} AstKind;

// Holds one of the possible
typedef union AstData {
    // A numeric litteral
    int num;
    // A string litteral or identifier
    char *string;
    // The nodes beneath us
    AstNode *children;
} AstData;

// The root syntax node type
struct AstNode {
    // What kind of node this is
    AstKind kind;
    // If relevant, how many children under us
    unsigned int count;
    // The payload for this node
    AstData data;
};

#define BASE_CHILDREN_SIZE 8

/** GLOBALS FOR STRING LITERALS **/
// FIX: Moved here after AstNode definition
static AstNode **lit_list_pre = NULL;
static size_t lit_count_pre = 0;
static size_t lit_size_pre = 0;

void ast_print_rec(AstNode *node, FILE *fp) {
    if (node == NULL) {
        return;
    }
    char *name;
    switch (node->kind) {
    case K_INT_MAIN:
        name = "int-main";
        break;
    case K_TOP_LEVEL:
        name = "top-level";
        break;
    case K_FUNCTION:
        name = "function";
        break;
    case K_PARAMS:
        name = "params";
        break;
    case K_CALL:
        name = "call";
        break;
    case K_BLOCK:
        name = "block";
        break;
    case K_EXPR_STATEMENT:
        name = "expr-statement";
        break;
    case K_DECLARATION:
        name = "declaration";
        break;
    case K_RETURN:
        name = "return";
        break;
    case K_BREAK:
        name = "break";
        break;
    case K_CONTINUE:
        name = "continue";
        break;
    case K_IF:
        name = "if";
        break;
    case K_WHILE:
        name = "while";
        break;
    case K_DEREFERENCE: // new
        name = "*";
        break;
    case K_ADDRESS_OF: // new
        name = "&";
        break;
    case K_SUBSCRIPT: // new
        name = "[]";
        break;
    case K_MEMBER_ACCESS: // new
        name = ".";
        break;
    case K_NO_INIT_DECLARATION:
        name = "declare";
        break;
    case K_TOP_EXPR:
        name = "top-expr";
        break;
    case K_LOGICAL_NOT:
        name = "!";
        break;
    case K_BIT_NOT:
        name = "~";
        break;
    case K_BIT_AND:
        name = "&";
        break;
    case K_BIT_OR:
        name = "|";
        break;
    case K_BIT_XOR:
        name = "^";
        break;
    case K_NEGATE:
        name = "-";
        break;
    case K_INIT_DECLARATION:
        name = "declare";
        break;
    case K_ASSIGN:
        name = "=";
        break;
    case K_EQUALS:
        name = "==";
        break;
    case K_NOT_EQUALS:
        name = "!=";
        break;
    case K_ADD:
        name = "+";
        break;
    case K_SUB:
        name = "-";
        break;
    case K_MUL:
        name = "*";
        break;
    case K_DIV:
        name = "/";
        break;
    case K_MOD:
        name = "%";
        break;
    // We won't use the name here
    case K_IDENTIFIER:
    case K_NUMBER:
    case K_LITT_STRING: // new
        break;
    }
    int variant;
    switch (node->kind) {
    case K_IDENTIFIER:
    case K_LITT_STRING: // new
        variant = 1;
        break;
    case K_NUMBER:
        variant = 0;
        break;
    default:
        variant = 2;
        break;
    }
    switch (variant) {
    case 0:
        fprintf(fp, "%d", node->data.num);
        break;
    case 1:
        if (node->kind == K_LITT_STRING) {
            fprintf(fp, "\"%s\"", node->data.string);
        } else {
            fprintf(fp, "%s", node->data.string);
        }
        break;
    case 2:
        fprintf(fp, "(%s", name);
        for (unsigned int i = 0; i < node->count; ++i) {
            fputc(' ', fp);
            ast_print_rec(node->data.children + i, fp);
        }
        fputc(')', fp);
        break;
    }
}

void ast_print(AstNode *node, FILE *fp) {
    ast_print_rec(node, fp);
    fputs("\n", fp);
}

/** PARSING **/

typedef struct ParseState {
    // Holds the state of the lexer
    LexState lex_st;
    // The next token
    Token peek;
    // This holds the previous token we parsed
    Token prev;
    // Whether or not the head has been initialized
    bool has_peek;
} ParseState;

ParseState parse_init(LexState lex_st) {
    Token start = {.type = T_START, .data = {.litt = 0}};
    ParseState st = {
        .lex_st = lex_st, .peek = start, .prev = start, .has_peek = false};
    return st;
}

Token parse_peek(ParseState *st) {
    if (!st->has_peek) {
        st->peek = lex_next(&st->lex_st);
        st->has_peek = true;
    }
    return st->peek;
}

bool parse_at_end(ParseState *st) { return parse_peek(st).type == T_EOF; }

Token parse_advance(ParseState *st) {
    st->has_peek = false;
    Token temp = st->peek;
    st->prev = temp;
    return temp;
}

bool parse_check(ParseState *st, TokenType type) {
    if (parse_at_end(st)) {
        return false;
    }
    return parse_peek(st).type == type;
}

bool parse_match(ParseState *st, TokenType *types, unsigned int type_count) {
    for (unsigned int i = 0; i < type_count; ++i) {
        if (parse_check(st, types[i])) {
            parse_advance(st);
            return true;
        }
    }
    return false;
}

// FIX: parse_consume is void, should not be used in assignment
void parse_consume(ParseState *st, TokenType type, const char *msg) {
    if (parse_check(st, type)) {
        parse_advance(st);
        return;
    }
    printf("Error at index %zu:\n", st->lex_st.index);
    panic(msg);
}

// forward declaration
void parse_assignment_expr(ParseState *st, AstNode *node);

void parse_primary(ParseState *st, AstNode *node) {
    if (parse_check(st, T_LEFT_PARENS)) {
        parse_advance(st);
        parse_assignment_expr(st, node);
        parse_consume(st, T_RIGHT_PARENS, "Expected matching `)`");
    } else if (parse_check(st, T_LITT_NUMBER)) {
        Token t = parse_advance(st);
        node->kind = K_NUMBER;
        node->count = 0;
        node->data.num = t.data.litt;
    } else if (parse_check(st, T_LITT_STRING)) { // new
        Token t = parse_advance(st);
        node->kind = K_LITT_STRING;
        node->count = 0;
        node->data.string = t.data.string;
    } else if (parse_check(st, T_IDENTIFIER)) {
        Token t = parse_advance(st);
        node->kind = K_IDENTIFIER;
        node->count = 0;
        node->data.string = t.data.string;
    } else {
        printf("Error at index %zu:\n", st->lex_st.index);
        puts("Unexpected Token:");
        token_print(st->peek, stdout);
        exit(-1);
    }
}

// Function to handle postfix expressions (calls, array access, member access)
void parse_postfix(ParseState *st, AstNode *node) {
    parse_primary(st, node);
    while (parse_check(st, T_LEFT_BRACKET) || parse_check(st, T_DOT) ||
           parse_check(st, T_ARROW) || parse_check(st, T_LEFT_PARENS)) {

        if (parse_check(st, T_LEFT_PARENS)) { // function call
            parse_advance(st); // consume '('

            AstNode *children = malloc(sizeof(AstNode)); // space for the function expression
            children[0] = *node; // the function expression (e.g., identifier or pointer to function)

            node->kind = K_CALL;
            node->count = 1;

            if (!parse_check(st, T_RIGHT_PARENS)) {
                // parse arguments
                for (;;) {
                    // allocate space for the next argument
                    children = realloc(children, (node->count + 1) * sizeof(AstNode));
                    if (children == NULL) panic("failed to realloc arguments");

                    parse_assignment_expr(st, children + node->count);
                    node->count++;

                    if (parse_check(st, T_RIGHT_PARENS)) break;
                    parse_consume(st, T_COMMA, "expected ',' or ')' in function call");
                }
            }
            node->data.children = children;
            parse_consume(st, T_RIGHT_PARENS, "expected ')' after function call");
        } else if (parse_check(st, T_LEFT_BRACKET)) { // array subscript
            parse_advance(st);
            AstNode *children = malloc(2 * sizeof(AstNode));
            children[0] = *node; // base expression (e.g., array name)
            node->kind = K_SUBSCRIPT;
            node->count = 2;
            parse_assignment_expr(st, children + 1); // index expression
            node->data.children = children;
            parse_consume(st, T_RIGHT_BRACKET, "expected `]` after array index");
        } else if (parse_check(st, T_DOT) || parse_check(st, T_ARROW)) { // member access
            Token op = parse_advance(st);

            // FIX: parse_consume is void, use st->prev to get the token
            parse_consume(st, T_IDENTIFIER, "expected member name after . or ->");
            Token member_name = st->prev;

            AstNode *children = malloc(2 * sizeof(AstNode));
            // children[0] is the struct/pointer expression
            children[0] = *node;
            // children[1] will be a node to hold the member's name as a string
            children[1].kind = K_IDENTIFIER;
            children[1].count = 0;
            children[1].data.string = member_name.data.string;

            node->kind = K_MEMBER_ACCESS;
            node->count = 2;
            node->data.children = children;

            // if it was a '->', we implicitly dereference the base expression (children[0])
            if (op.type == T_ARROW) {
                // create a temporary dereference node (*(expr)) around the base
                AstNode *temp = malloc(sizeof(AstNode));
                temp->kind = K_DEREFERENCE;
                temp->count = 1;
                temp->data.children = malloc(sizeof(AstNode));
                *(temp->data.children) = children[0]; // put base expr inside deref
                children[0] = *temp; // replace base expr with dereferenced base expr
                free(temp); // clean up the temporary pointer
            }
        } else {
            // should not happen
            break;
        }
    }
}


void parse_unary(ParseState *st, AstNode *node) {
    for (;;) {
        if (parse_check(st, T_EXCLAMATION)) {
            parse_advance(st);
            node->kind = K_LOGICAL_NOT;
            node->count = 1;
            node->data.children = malloc(sizeof(AstNode));
            node = node->data.children;
        } else if (parse_check(st, T_TILDE)) {
            parse_advance(st);
            node->kind = K_BIT_NOT;
            node->count = 1;
            node->data.children = malloc(sizeof(AstNode));
            node = node->data.children;
        } else if (parse_check(st, T_MINUS)) {
            parse_advance(st);
            node->kind = K_NEGATE;
            node->count = 1;
            node->data.children = malloc(sizeof(AstNode));
            node = node->data.children;
        } else if (parse_check(st, T_ASTERISK)) { // new: dereference
            parse_advance(st);
            node->kind = K_DEREFERENCE;
            node->count = 1;
            node->data.children = malloc(sizeof(AstNode));
            node = node->data.children;
        } else if (parse_check(st, T_AMPERSAND)) { // new: address of
            parse_advance(st);
            node->kind = K_ADDRESS_OF;
            node->count = 1;
            node->data.children = malloc(sizeof(AstNode));
            node = node->data.children;
        } else {
            break;
        }
    }
    parse_postfix(st, node); // changed from parse_primary
}

void parse_multiply(ParseState *st, AstNode *node) {
    parse_unary(st, node);
    TokenType operators[] = {T_ASTERISK, T_SLASH, T_PERCENT};
    while (parse_match(st, operators, 3)) {
        AstNode *children = malloc(2 * sizeof(AstNode));
        children[0] = *node;
        TokenType matched = st->prev.type;
        if (matched == T_ASTERISK) {
            node->kind = K_MUL;
        } else if (matched == T_SLASH) {
            node->kind = K_DIV;
        } else {
            node->kind = K_MOD;
        }
        node->count = 2;
        parse_unary(st, children + 1);
        node->data.children = children;
    }
}

void parse_add(ParseState *st, AstNode *node) {
    parse_multiply(st, node);
    TokenType operators[] = {T_PLUS, T_MINUS};
    while (parse_match(st, operators, 2)) {
        AstNode *children = malloc(2 * sizeof(AstNode));
        children[0] = *node;
        TokenType matched = st->prev.type;
        if (matched == T_PLUS) {
            node->kind = K_ADD;
        } else {
            node->kind = K_SUB;
        }
        node->count = 2;
        parse_multiply(st, children + 1);
        node->data.children = children;
    }
}

void parse_equality(ParseState *st, AstNode *node) {
    parse_add(st, node);
    TokenType operators[] = {T_EQUALS_EQUALS, T_EXCLAMATION_EQUALS};
    while (parse_match(st, operators, 2)) {
        AstNode *children = malloc(2 * sizeof(AstNode));
        children[0] = *node;
        TokenType matched = st->prev.type;
        if (matched == T_EQUALS_EQUALS) {
            node->kind = K_EQUALS;
        } else {
            node->kind = K_NOT_EQUALS;
        }
        node->count = 2;
        parse_add(st, children + 1);
        node->data.children = children;
    }
}

void parse_and(ParseState *st, AstNode *node) {
    parse_equality(st, node);
    while (parse_check(st, T_AMPERSAND)) {
        parse_advance(st);
        AstNode *children = malloc(2 * sizeof(AstNode));
        children[0] = *node;
        node->kind = K_BIT_AND;
        node->count = 2;
        parse_equality(st, children + 1);
        node->data.children = children;
    }
}

void parse_exclusive_or(ParseState *st, AstNode *node) {
    parse_and(st, node);
    while (parse_check(st, T_CARET)) {
        parse_advance(st);
        AstNode *children = malloc(2 * sizeof(AstNode));
        children[0] = *node;
        node->kind = K_BIT_XOR;
        node->count = 2;
        parse_and(st, children + 1);
        node->data.children = children;
    }
}

void parse_inclusive_or(ParseState *st, AstNode *node) {
    parse_exclusive_or(st, node);
    while (parse_check(st, T_VERT_BAR)) {
        parse_advance(st);
        AstNode *children = malloc(2 * sizeof(AstNode));
        children[0] = *node;
        node->kind = K_BIT_OR;
        node->count = 2;
        parse_exclusive_or(st, children + 1);
        node->data.children = children;
    }
}

void parse_assignment_expr(ParseState *st, AstNode *node) {
    ParseState rewind = *st;
    if (parse_check(st, T_IDENTIFIER)) {
        parse_advance(st);
        // We need to rewind everything if the next token isn't =
        if (parse_check(st, T_EQUALS)) {
            char *identifier = st->prev.data.string;
            parse_advance(st);
            node->kind = K_ASSIGN;
            node->count = 2;
            AstNode *children = malloc(2 * sizeof(AstNode));
            node->data.children = children;
            children[0].kind = K_IDENTIFIER;
            children[0].count = 0;
            children[0].data.string = identifier;
            parse_assignment_expr(st, children + 1);
        } else {
            *st = rewind;
            parse_inclusive_or(st, node);
        }
    } else {
        parse_inclusive_or(st, node);
    }
}

AstNode *parse_top_expr(ParseState *st) {
    AstNode *node = malloc(sizeof(AstNode));
    node->kind = K_TOP_EXPR;
    node->count = 1;
    unsigned int allocated = BASE_CHILDREN_SIZE;
    node->data.children = malloc(allocated * sizeof(AstNode));
    parse_assignment_expr(st, node->data.children);
    while (parse_check(st, T_COMMA)) {
        parse_advance(st);
        int offset = node->count++;
        if (node->count > allocated) {
            allocated <<= 1;
            size_t size = allocated * sizeof(AstNode);
            node->data.children = realloc(node->data.children, size);
        }
        parse_assignment_expr(st, node->data.children + offset);
    }
    return node;
}

void parse_top_expr_opt(ParseState *st, AstNode *node) {
    if (parse_check(st, T_SEMICOLON)) {
        node->count = 0;
        node->data.children = NULL;
    } else {
        node->count = 1;
        AstNode *top_expr = parse_top_expr(st);
        node->data.children = top_expr->data.children; // re-use children array
        node->count = top_expr->count;
        free(top_expr); // free the temporary K_TOP_EXPR node
    }
}

AstNode *parse_declarator(ParseState *st) {
    AstNode *node = malloc(sizeof(AstNode));
    // The declarator starts as a simple identifier node
    node->kind = K_IDENTIFIER;
    node->count = 0;

    // Simple pointer support: count asterisks
    int pointers = 0;
    while (parse_check(st, T_ASTERISK)) {
        parse_advance(st);
        pointers++;
    }

    int parens = 0;
    while (parse_check(st, T_LEFT_PARENS)) {
        parse_advance(st);
        ++parens;
    }
    parse_consume(st, T_IDENTIFIER, "Declarator must contain identifier");
    node->data.string = st->prev.data.string;
    for (; parens > 0; --parens) {
        parse_consume(st, T_RIGHT_PARENS,
                      "Must have matching parens around identifier");
    }

    // Wrap the identifier in K_DEREFERENCE nodes for each pointer level
    for (int i = 0; i < pointers; ++i) {
        AstNode *new_node = malloc(sizeof(AstNode));
        new_node->kind = K_DEREFERENCE;
        new_node->count = 1;
        new_node->data.children = malloc(sizeof(AstNode));
        new_node->data.children[0] = *node;
        *node = *new_node;
        free(new_node);
    }
    return node;
}

void parse_declaration(ParseState *st, AstNode *node) {
    AstNode *declarator = parse_declarator(st);
    if (parse_check(st, T_EQUALS)) {
        parse_advance(st);
        node->kind = K_INIT_DECLARATION;
        node->count = 2;
        node->data.children = malloc(2 * sizeof(AstNode));
        node->data.children[0] = *declarator;
        parse_assignment_expr(st, node->data.children + 1);
    } else {
        node->kind = K_NO_INIT_DECLARATION;
        node->count = 1;
        node->data.children = declarator;
    }
}

void parse_block_or_statement(ParseState *st, AstNode *node);

void parse_statement(ParseState *st, AstNode *node) {
    if (parse_check(st, T_RETURN)) {
        parse_advance(st);
        node->kind = K_RETURN;
        parse_top_expr_opt(st, node);
        parse_consume(st, T_SEMICOLON, "Expected semicolon to end statement");
    } else if (parse_check(st, T_BREAK)) {
        parse_advance(st);
        node->kind = K_BREAK;
        node->count = 0;
        parse_consume(st, T_SEMICOLON, "Expected semicolon to end statement");
    } else if (parse_check(st, T_CONTINUE)) {
        parse_advance(st);
        node->kind = K_CONTINUE;
        node->count = 0;
        parse_consume(st, T_SEMICOLON, "Expected semicolon to end statement");
    } else if (parse_check(st, T_INT)) {
        parse_advance(st);
        node->kind = K_DECLARATION;
        node->count = 1;
        unsigned int allocated = BASE_CHILDREN_SIZE;
        node->data.children = malloc(allocated * sizeof(AstNode));
        parse_declaration(st, node->data.children);
        while (parse_check(st, T_COMMA)) {
            parse_advance(st);
            int offset = node->count++;
            if (node->count > allocated) {
                allocated <<= 1;
                size_t size = allocated * sizeof(AstNode);
                node->data.children = realloc(node->data.children, size);
            }
            parse_declaration(st, node->data.children + offset);
        }
        parse_consume(st, T_SEMICOLON, "Expected semicolon to end statement");
    } else if (parse_check(st, T_IF)) {
        parse_advance(st);
        parse_consume(st, T_LEFT_PARENS, "Expected `(` after `if`");
        node->kind = K_IF;
        node->count = 2;
        AstNode *children = malloc(3 * sizeof(AstNode));
        node->data.children = children;
        parse_assignment_expr(st, children);
        parse_consume(st, T_RIGHT_PARENS, "Expected `)` to close `(`");
        parse_block_or_statement(st, children + 1);
        if (parse_check(st, T_ELSE)) {
            parse_advance(st);
            node->count = 3;
            parse_block_or_statement(st, children + 2);
        }
    } else if (parse_check(st, T_WHILE)) {
        parse_advance(st);
        parse_consume(st, T_LEFT_PARENS, "Expected `(` after `while`");
        node->kind = K_WHILE;
        node->count = 2;
        AstNode *children = malloc(2 * sizeof(AstNode));
        node->data.children = children;
        parse_assignment_expr(st, children);
        parse_consume(st, T_RIGHT_PARENS, "Expected `)` to close `(`");
        parse_block_or_statement(st, children + 1);
    } else {
        node->kind = K_EXPR_STATEMENT;
        parse_top_expr_opt(st, node);
        parse_consume(st, T_SEMICOLON, "Expected semicolon to end statement");
    }
}

void parse_block(ParseState *st, AstNode *node) {
    parse_consume(st, T_LEFT_BRACE, "Expected `{` to start block");
    node->kind = K_BLOCK;
    node->count = 0;
    unsigned int allocated = BASE_CHILDREN_SIZE;
    node->data.children = malloc(allocated * sizeof(AstNode));
    while (!parse_check(st, T_RIGHT_BRACE) && !parse_at_end(st)) {
        int offset = node->count++;
        if (node->count > allocated) {
            allocated <<= 1;
            size_t size = allocated * sizeof(AstNode);
            node->data.children = realloc(node->data.children, size);
        }
        parse_block_or_statement(st, node->data.children + offset);
    }
    if (parse_at_end(st)) {
        printf("Error at index %zu:\n", st->lex_st.index);
        panic("Unexpected EOF");
    }
    parse_advance(st);
}

void parse_block_or_statement(ParseState *st, AstNode *node) {
    if (parse_check(st, T_LEFT_BRACE)) {
        parse_block(st, node);
    } else {
        parse_statement(st, node);
    }
}

void parse_param_definition(ParseState *st, AstNode *node) {
    parse_consume(st, T_INT, "Expected a param with type int");

    // Parameter declarator (supports pointers)
    AstNode *declarator = parse_declarator(st);
    *node = *declarator;
    free(declarator);
}

void parse_params_def(ParseState *st, AstNode *node) {
    parse_consume(st, T_LEFT_PARENS,
                  "Expected `(` to start function param definition");
    node->kind = K_PARAMS;
    node->count = 0;
    unsigned int allocated = BASE_CHILDREN_SIZE;
    node->data.children = malloc(allocated * sizeof(AstNode));
    if (!parse_check(st, T_RIGHT_PARENS)) {
        node->count = 1;
        parse_param_definition(st, node->data.children);
    }
    while (parse_check(st, T_COMMA)) {
        parse_advance(st);
        int offset = node->count++;
        if (node->count > allocated) {
            allocated <<= 1;
            size_t size = allocated * sizeof(AstNode);
            node->data.children = realloc(node->data.children, size);
        }
        parse_param_definition(st, node->data.children + offset);
    }
    parse_consume(st, T_RIGHT_PARENS, "Expected `)` to end function params");
}

void parse_function(ParseState *st, AstNode *node) {
    node->kind = K_FUNCTION;
    node->count = 3;
    node->data.children = malloc(3 * sizeof(AstNode));
    parse_consume(st, T_IDENTIFIER, "Function definition must have identifier");
    node->data.children[0].kind = K_IDENTIFIER;
    node->data.children[0].count = 0;
    node->data.children[0].data.string = st->prev.data.string;
    parse_params_def(st, node->data.children + 1);
    parse_block(st, node->data.children + 2);
}

AstNode *parse_top_level(ParseState *st) {
    AstNode *node = malloc(sizeof(AstNode));
    node->kind = K_TOP_LEVEL;
    node->count = 0;
    unsigned int allocated = BASE_CHILDREN_SIZE;
    node->data.children = malloc(allocated * sizeof(AstNode));
    while (parse_check(st, T_INT)) {
        parse_advance(st);
        int offset = node->count++;
        if (node->count > allocated) {
            allocated <<= 1;
            size_t size = allocated * sizeof(AstNode);
            node->data.children = realloc(node->data.children, size);
        }
        parse_function(st, node->data.children + offset);
    }
    return node;
}

/** SCOPES AND SYMBOL MANAGEMENT **/
// (No changes to Scope/Scopes structure, using existing implementation)

typedef struct Identifiers {
    // An array of strings holding our identifiers
    char **identifiers;
    // The number of slots we've filled
    unsigned int count;
    // The number of slots we have available
    unsigned int capacity;
    // Array of sizes for each identifier (e.g. 4 for int, 8 for pointer)
    int *sizes;
} Identifiers;

void idents_init(Identifiers *idents) {
    idents->count = 0;
    idents->capacity = 4;
    idents->identifiers = malloc(idents->capacity * sizeof(char *));
    idents->sizes = malloc(idents->capacity * sizeof(int));
}

void idents_insert(Identifiers *idents, char *new, int size) {
    if (idents->count == idents->capacity) {
        idents->capacity <<= 1;
        idents->identifiers =
            realloc(idents->identifiers, idents->capacity * sizeof(char *));
        idents->sizes = realloc(idents->sizes, idents->capacity * sizeof(int));
    }
    idents->identifiers[idents->count] = new;
    idents->sizes[idents->count] = size;
    idents->count++;
}

// Returns < 0 for negative indices
int idents_index_of(Identifiers *idents, char *target) {
    for (unsigned int i = 0; i < idents->count; ++i) {
        if (strcmp(idents->identifiers[i], target) == 0) {
            return i;
        }
    }
    return -1;
}

typedef struct Scope {
    // The set of identifiers created inside this scope
    Identifiers identifiers;
    // The number of bytes of stack we've allocated in this scope
    int allocated_stack;
    // The initial offset for this scope
    int initial_offset;
} Scope;

typedef struct Scopes {
    // The stack of scopes
    Scope *scopes;
    // The number of slots filled
    unsigned int count;
    // The number of slots available
    unsigned int capacity;
} Scopes;

void scopes_init(Scopes *new) {
    new->count = 0;
    new->capacity = 2;
    new->scopes = malloc(new->capacity * sizeof(Scope));
}

// Enter a new scope
void scopes_enter(Scopes *scopes) {
    if (scopes->count == scopes->capacity) {
        scopes->capacity <<= 1;
        scopes->scopes =
            realloc(scopes->scopes, scopes->capacity * sizeof(Scope));
    }
    Scope *new = scopes->scopes + scopes->count++;
    // We don't have an old scope to look at
    if (scopes->count == 1) {
        // The first will take stack bytes [-4,0[
        new->initial_offset = 0; // stack frame starts at 0 offset from rbp
    } else {
        Scope *old = new - 1;
        // 8 bytes for each identifier created in the previous scope (for alignment/64-bit)
        int previous_var_size = 0;
        for (unsigned int i = 0; i < old->identifiers.count; ++i) {
            // align everything to 8 bytes for simplicity on 64-bit stack
            previous_var_size += (old->identifiers.sizes[i] + 7) & ~7;
        }
        new->initial_offset = old->initial_offset + previous_var_size;
    }
    idents_init(&new->identifiers);
    new->allocated_stack = 0;
}

void scopes_exit(Scopes *scopes) {
    --scopes->count;
    free(scopes->scopes[scopes->count].identifiers.identifiers);
    free(scopes->scopes[scopes->count].identifiers.sizes);
}

// Returns < 0 if no identifier found
int scopes_offset_of(Scopes *scopes, char *identifier) {
    // int current_offset = 0; // WARNING: removed unused variable 'current_offset'
    for (int i = scopes->count - 1; i >= 0; --i) {
        Scope *scope = scopes->scopes + i;
        int index = idents_index_of(&scope->identifiers, identifier);
        if (index >= 0) {
            // Calculate absolute offset based on size of variables declared before it in this scope
            int offset = scope->initial_offset;
            for (int j = 0; j < index; ++j) {
                offset += (scope->identifiers.sizes[j] + 7) & ~7;
            }
            // Return offset from RBP (negative on stack)
            // FIX: added parentheses to correct operator precedence
            return offset + ((scope->identifiers.sizes[index] + 7) & ~7);
        }
    }
    return -1;
}

int scopes_total_allocated(Scopes *scopes) {
    if (scopes->count == 0) return 0;
    Scope *current = scopes->scopes + scopes->count - 1;
    int total_size = current->initial_offset;
    for (unsigned int i = 0; i < current->identifiers.count; ++i) {
        total_size += (current->identifiers.sizes[i] + 7) & ~7;
    }
    return total_size;
}

typedef struct AsmState {
    Scopes scopes;
    // The name of the current function
    char *function_name;
    // The current label index
    int label_index;
    // The stream we're generating to
    FILE *out;
} AsmState;

AsmState *asm_init(FILE *out) {
    AsmState *st = malloc(sizeof(AsmState));
    st->out = out;
    scopes_init(&st->scopes);
    return st;
}

void asm_enter_function(AsmState *st, char *function_name) {
    st->function_name = function_name;
    st->label_index = 0;
    scopes_enter(&st->scopes);
}

int type_size(AstNode *node) {
    // Simplified type system for bootstrap:
    // A simple identifier node might represent an int (4) or a char (1) - for simplicity assume 4
    if (node->kind == K_IDENTIFIER) return 4;
    // Pointers, Dereference, AddressOf, Call, Subscript, MemberAccess are 8 bytes
    if (node->kind == K_DEREFERENCE || node->kind == K_ADDRESS_OF ||
        node->kind == K_SUBSCRIPT || node->kind == K_MEMBER_ACCESS ||
        node->kind == K_CALL)
    {
        return 8; // pointer/address is 8 bytes in x86-64
    }
    // Assignment and literals are typically 4 bytes (int)
    if (node->kind == K_ASSIGN || node->kind == K_NUMBER || node->kind == K_LITT_STRING) return 4;

    // Default to 4 bytes (int)
    return 4;
}

void asm_new_ident(AsmState *st, AstNode *declarator) {
    assert(declarator->kind == K_IDENTIFIER || declarator->kind == K_DEREFERENCE);

    char *new = declarator->kind == K_IDENTIFIER
                ? declarator->data.string
                : declarator->data.children[0].data.string; // For *x or **x

    int size = type_size(declarator);

    if (scopes_offset_of(&st->scopes, new) >= 0) {
        Scope *current = st->scopes.scopes + st->scopes.count - 1;
        bool in_current = idents_index_of(&current->identifiers, new) >= 0;
        if (in_current) {
            puts("Error:");
            printf("Attempting to declare identifier %s twice\n", new);
            exit(-1);
        }
    }
    Scope *current = st->scopes.scopes + st->scopes.count - 1;
    idents_insert(&current->identifiers, new, size);
}

// We might not need to clear the stack if there was a return
void asm_exit_scope(AsmState *st, bool clear_stack) {
    Scope *current = st->scopes.scopes + st->scopes.count - 1;
    if (clear_stack && current->allocated_stack > 0) {
        fprintf(st->out, "\tadd\trsp, %d\n", current->allocated_stack);
    }
    scopes_exit(&st->scopes);
}

char *asm_reg_for_nth_function_param(bool is64, int n) {
    // Only supports 64-bit argument registers
    if (n >= num_arg_regs_pre) {
        puts("Function has more than 6 parameters");
        exit(-1);
    }
    // WARNING: removed const qualifier warning
    return (char *)arg_regs_pre[n];
}

char *reg_for_size(int size) {
    // Selects the appropriate register part based on the variable size
    switch (size) {
        case 1: return "al";
        case 4: return "eax";
        case 8: return "rax";
        default: panic("unsupported variable size"); return "";
    }
}

// Hardcoded member offsets for bootstrap compilation
int get_member_offset(char const *member_name) {
    // Offsets for LexState struct: char *program (8), size_t index (8)
    if (strcmp(member_name, "program") == 0) return 0;
    if (strcmp(member_name, "index") == 0) return 8;

    // Offsets for Token struct: TokenType type (4), TokenData data (4+padding)
    // Assuming 8-byte alignment for 64-bit machine.
    if (strcmp(member_name, "type") == 0) return 0;
    if (strcmp(member_name, "litt") == 0) return 4; // union starts at 4
    if (strcmp(member_name, "string") == 0) return 8; // FIX: TokenData is a union, 8 bytes size, but struct layout is type(4) + padding(4) + data(8) or type(4) + data(8) if unaligned. Sticking to simple fixed offsets.
    if (strcmp(member_name, "string") == 0) return 8; // union starts at 8

    // Offsets for AstNode struct: AstKind kind (4), count (4), AstData data (8)
    if (strcmp(member_name, "kind") == 0) return 0;
    if (strcmp(member_name, "count") == 0) return 4;
    if (strcmp(member_name, "num") == 0) return 8; // union starts at 8
    if (strcmp(member_name, "string") == 0) return 8; // union starts at 8
    if (strcmp(member_name, "children") == 0) return 8; // union starts at 8

    return -1; // member not found/unsupported
}

// FORWARD DECLARATION (FIX: moved up)
void asm_expr(AsmState *st, AstNode *node);

// Generates code to compute the memory address (lvalue) of an expression and pushes it to stack
void asm_lvalue(AsmState *st, AstNode *node) {
    switch (node->kind) {
    case K_IDENTIFIER: {
        char *ident = node->data.string;
        int offset = scopes_offset_of(&st->scopes, ident);
        if (offset < 0) {
            printf("Error:\nUse of undeclared identifier %s\n", ident);
            exit(-1);
        }
        // Address of local var: RBP - offset
        fprintf(st->out, "\tlea\trax, [rbp - %d]\n", offset);
        fputs("\tpush\trax\n", st->out);
    } break;
    case K_DEREFERENCE: // *expr (if we treat the dereference node as a pointer itself)
    case K_SUBSCRIPT: { // expr[index] is *(expr + index * size)
        // If the node is *p, the address of *p is p.
        asm_expr(st, node->data.children); // p is pushed to stack
    } break;
    case K_MEMBER_ACCESS: { // expr.member or expr->member
        // compile the base expression (e.g., the struct variable or the pointer)
        // This is tricky: we need the lvalue of the base expression.
        // The base is in children[0]. If the original expression was `a.b`, children[0] is `a` (K_IDENTIFIER).
        // If the original was `p->b`, children[0] is `*p` (K_DEREFERENCE).

        // This relies on the property that an lvalue of a struct/union is its address.
        asm_lvalue(st, node->data.children); // pushes address of the base struct/union

        char *member_name = node->data.children[1].data.string;
        int offset = get_member_offset(member_name);
        if (offset < 0) {
            panic("unsupported struct member access for self-hosting");
        }

        fputs("\tpop\trax\n", st->out); // rax = base address

        // rax = base address + member offset
        fprintf(st->out, "\tadd\trax, %d\n", offset);

        fputs("\tpush\trax\n", st->out); // push the address of the member
    } break;
    default:
        panic("expression is not an lvalue");
    }
}

void asm_load(AsmState *st, int size) {
    // Pop address, load value, push value
    fputs("\tpop\trax\n", st->out); // rax = address
    if (size == 1) {
        fputs("\tmovzx\teax, BYTE PTR [rax]\n", st->out); // load 1 byte, zero extend to eax
    } else if (size == 4) {
        fputs("\tmov\teax, DWORD PTR [rax]\n", st->out); // load 4 bytes
    } else if (size == 8) {
        fputs("\tmov\trax, QWORD PTR [rax]\n", st->out); // load 8 bytes
    } else {
        panic("unsupported size for asm_load");
    }
    fputs("\tpush\trax\n", st->out);
}

void asm_store(AsmState *st, int size) {
    // Pop value (rax), Pop address (rbx), Store [rbx] = rax
    fputs("\tpop\trbx\n", st->out); // rbx = address
    fputs("\tpop\trax\n", st->out); // rax = value

    if (size == 1) {
        fputs("\tmov\tBYTE PTR [rbx], al\n", st->out);
    } else if (size == 4) {
        fputs("\tmov\tDWORD PTR [rbx], eax\n", st->out);
    } else if (size == 8) {
        fputs("\tmov\tQWORD PTR [rbx], rax\n", st->out);
    } else {
        panic("unsupported size for asm_store");
    }
    fputs("\tpush\trax\n", st->out); // leave value on stack
}

void asm_call(AsmState *st, AstNode *node) {
    assert(node->kind == K_CALL);

    AstNode *func_name_node = node->data.children;
    if (func_name_node->kind != K_IDENTIFIER) {
        panic("function call must use an identifier");
    }
    char *func_name = func_name_node->data.string;

    int num_args = node->count - 1;

    // 1. push arguments onto the stack
    for (int i_pre = 0; i_pre < num_args; i_pre++) {
        asm_expr(st, node->data.children + 1 + i_pre);
    }

    // 2. move argument values from stack into registers
    // pop arguments into registers in reverse order (r9 down to rdi)
    for (int i_pre = num_args - 1; i_pre >= 0; i_pre--) {
        if (i_pre < num_arg_regs_pre) {
            // pop into an argument register
            fprintf(st->out, "\tpop\t%s\n", arg_regs_pre[i_pre]);
        } else {
            // arguments beyond the 6th are left on the stack (handled by caller)
            panic("function call with more than 6 arguments is not supported for bootstrap");
        }
    }

    // 3. align stack (rsp must be 16-byte aligned before call)
    // for simplicity in bootstrap, we rely on the function prologue/epilogue aligning correctly

    // 4. perform the call
    fprintf(st->out, "\tcall\t%s\n", func_name);

    // 5. push return value (rax) back onto the stack
    fputs("\tpush\trax\n", st->out);
}


void asm_expr(AsmState *st, AstNode *node) {
    switch (node->kind) {
    case K_NUMBER:
        fprintf(st->out, "\tpush\t%d\n", node->data.num);
        break;
    case K_LITT_STRING: { // new: load string address
        // the label for this string will be .lstrX where X is its index in lit_list_pre
        unsigned int index = 0;
        // find the index of this node in the global list
        while (index < lit_count_pre && lit_list_pre[index] != node) {
            index++;
        }
        assert(index < lit_count_pre);

        // load the address of the label into rax
        fprintf(st->out, "\tlea\trax, .lstr%d\n", index);
        fputs("\tpush\trax\n", st->out);
    } break;
    case K_IDENTIFIER: {
        // Evaluate identifier as rvalue (load value from address)
        asm_lvalue(st, node);
        asm_load(st, type_size(node));
    } break;
    case K_CALL:
        asm_call(st, node);
        break;
    case K_ASSIGN: {
        // Assign requires the left side to be an lvalue
        asm_lvalue(st, node->data.children);
        asm_expr(st, node->data.children + 1); // push value (rvalue)
        asm_store(st, type_size(node->data.children)); // Store value at address
    } break;
    case K_ADDRESS_OF: // &expr
        asm_lvalue(st, node->data.children); // lvalue is already the address
        break;
    case K_DEREFERENCE: // *expr (rvalue)
    case K_SUBSCRIPT: // expr[index] (rvalue)
    case K_MEMBER_ACCESS: { // expr.member or expr->member (rvalue)
        // Compute lvalue (address) then load the value
        asm_lvalue(st, node);
        asm_load(st, type_size(node));
    } break;
    case K_EQUALS:
        asm_expr(st, node->data.children);
        asm_expr(st, node->data.children + 1);
        fputs("\tpop\trbx\n", st->out);
        fputs("\tpop\trax\n", st->out);
        fputs("\tcmp\trax, rbx\n", st->out);
        fputs("\tsete\tal\n", st->out);
        fputs("\tmovzx\teax, al\n", st->out);
        fputs("\tpush\trax\n", st->out);
        break;
    case K_NOT_EQUALS:
        asm_expr(st, node->data.children);
        asm_expr(st, node->data.children + 1);
        fputs("\tpop\trbx\n", st->out);
        fputs("\tpop\trax\n", st->out);
        fputs("\tcmp\trax, rbx\n", st->out);
        fputs("\tsetne\tal\n", st->out);
        fputs("\tmovzx\teax, al\n", st->out);
        fputs("\tpush\trax\n", st->out);
        break;
    case K_ADD:
        asm_expr(st, node->data.children);
        asm_expr(st, node->data.children + 1);
        fputs("\tpop\trbx\n", st->out);
        fputs("\tpop\trax\n", st->out);
        fputs("\tadd\teax, ebx\n", st->out);
        fputs("\tpush\trax\n", st->out);
        break;
    case K_SUB:
        asm_expr(st, node->data.children);
        asm_expr(st, node->data.children + 1);
        fputs("\tpop\trbx\n", st->out);
        fputs("\tpop\trax\n", st->out);
        fputs("\tsub\teax, ebx\n", st->out);
        fputs("\tpush\trax\n", st->out);
        break;
    case K_MUL:
        asm_expr(st, node->data.children);
        asm_expr(st, node->data.children + 1);
        fputs("\tpop\trbx\n", st->out);
        fputs("\tpop\trax\n", st->out);
        fputs("\timul\teax, ebx\n", st->out);
        fputs("\tpush\trax\n", st->out);
        break;
    case K_DIV:
        asm_expr(st, node->data.children);
        asm_expr(st, node->data.children + 1);
        fputs("\tpop\trbx\n", st->out);
        fputs("\tpop\trax\n", st->out);
        fputs("\tcdq\n", st->out);
        fputs("\tidiv\tebx\n", st->out);
        fputs("\tpush\trax\n", st->out);
        break;
    case K_MOD:
        asm_expr(st, node->data.children);
        asm_expr(st, node->data.children + 1);
        fputs("\tpop\trbx\n", st->out);
        fputs("\tpop\trax\n", st->out);
        fputs("\tcdq\n", st->out);
        fputs("\tidiv\tebx\n", st->out);
        fputs("\tpush\trdx\n", st->out);
        break;
    case K_BIT_AND:
        asm_expr(st, node->data.children);
        asm_expr(st, node->data.children + 1);
        fputs("\tpop\trbx\n", st->out);
        fputs("\tpop\trax\n", st->out);
        fputs("\tand\teax, ebx\n", st->out);
        fputs("\tpush\trax\n", st->out);
        break;
    case K_BIT_OR:
        asm_expr(st, node->data.children);
        asm_expr(st, node->data.children + 1);
        fputs("\tpop\trbx\n", st->out);
        fputs("\tpop\trax\n", st->out);
        fputs("\tor\teax, ebx\n", st->out);
        fputs("\tpush\trax\n", st->out);
        break;
    case K_BIT_XOR:
        asm_expr(st, node->data.children);
        asm_expr(st, node->data.children + 1);
        fputs("\tpop\trbx\n", st->out);
        fputs("\tpop\trax\n", st->out);
        fputs("\txor\teax, ebx\n", st->out);
        fputs("\tpush\trax\n", st->out);
        break;
    case K_BIT_NOT:
        asm_expr(st, node->data.children);
        fputs("\tpop\trax\n", st->out);
        fputs("\tnot\teax\n", st->out);
        fputs("\tpush\trax\n", st->out);
        break;
    case K_NEGATE:
        asm_expr(st, node->data.children);
        fputs("\tpop\trax\n", st->out);
        fputs("\tneg\teax\n", st->out);
        fputs("\tpush\trax\n", st->out);
        break;
    case K_LOGICAL_NOT:
        asm_expr(st, node->data.children);
        fputs("\tpop\trax\n", st->out);
        fputs("\ttest\teax, eax\n", st->out);
        fputs("\tsete\tal\n", st->out);
        fputs("\tmovzx\teax, al\n", st->out);
        fputs("\tpush\trax\n", st->out);
        break;
    default:
        break;
    }
}

void asm_declare(AsmState *st, AstNode *node) {
    AstNode *declarator;
    AstNode *initializer = NULL;

    if (node->kind == K_NO_INIT_DECLARATION) {
        declarator = node->data.children;
    } else if (node->kind == K_INIT_DECLARATION) {
        declarator = node->data.children;
        initializer = node->data.children + 1;
    } else {
        panic("Tried to process declaration, but kind was invalid");
    }

    asm_new_ident(st, declarator); // register the identifier (with size)

    if (initializer) {
        char *identifier = declarator->kind == K_IDENTIFIER
                        ? declarator->data.string
                        : declarator->data.children[0].data.string;

        asm_expr(st, initializer);
        fputs("\tpop\trax\n", st->out);

        int offset = scopes_offset_of(&st->scopes, identifier);
        int size = type_size(declarator);

        if (offset < 0) {
            printf("Error:\nStack offset %d < 0\n", offset);
            exit(-1);
        }

        // Use appropriate move instruction based on size
        if (size == 4) {
            fprintf(st->out, "\tmov\tDWORD PTR [rbp - %d], eax\n", offset);
        } else if (size == 8) {
            fprintf(st->out, "\tmov\tQWORD PTR [rbp - %d], rax\n", offset);
        } else {
            panic("unsupported size for declaration initialization");
        }
    }
}

void asm_top_expr(AsmState *st, AstNode *node) {
    // node->count is the number of expressions, node->data.children holds the array
    if (node->count == 0) return;

    for (unsigned int i = 0; i < node->count; ++i) {
        asm_expr(st, node->data.children + i);
    }

    // Clean up all but the last expression result (which is the return value)
    if (node->count > 1) {
        // pop (count - 1) expressions
        fprintf(st->out, "\tadd\trsp, %d\n", (node->count - 1) << 3);
    }
}

// Return true if code appearing after this statement is unreachable
bool asm_statement(AsmState *st, AstNode *node, int start_label,
                   int end_label) {
    bool after_unreachable = false;
    if (node->kind == K_RETURN) {
        // K_RETURN now contains the K_TOP_EXPR's children directly (or null if empty)
        AstNode temp_node = {.kind = K_TOP_EXPR, .count = node->count, .data.children = node->data.children};
        asm_top_expr(st, &temp_node);
        fputs("\tpop\trax\n", st->out);
        fputs("\tmov\trsp, rbp\n", st->out);
        fputs("\tpop\trbp\n", st->out);
        fputs("\tret\n", st->out);
        after_unreachable = true;
    } else if (node->kind == K_EXPR_STATEMENT) {
        if (node->count > 0) {
            AstNode temp_node = {.kind = K_TOP_EXPR, .count = node->count, .data.children = node->data.children};
            asm_top_expr(st, &temp_node);
            fputs("\tadd\trsp, 8\n", st->out); // pop the result of the last expression
        }
    } else if (node->kind == K_DECLARATION) {
        for (unsigned int i = 0; i < node->count; ++i) {
            asm_declare(st, node->data.children + i);
        }
    } else if (node->kind == K_IF) {
        int label_if_end = st->label_index++;
        int label_else = st->label_index++;

        asm_expr(st, node->data.children); // child 0: condition
        fputs("\tpop\trax\n", st->out);
        fputs("\ttest\teax, eax\n", st->out);

        if (node->count == 2) { // no else branch
            fprintf(st->out, "\tje\t.%s%d\n", st->function_name, label_if_end);
            bool if_returns = asm_statement(st, node->data.children + 1, start_label, end_label);
            fprintf(st->out, ".%s%d:\n", st->function_name, label_if_end);
            after_unreachable = if_returns;
        } else if (node->count == 3) { // with else branch
            fprintf(st->out, "\tje\t.%s%d\n", st->function_name, label_else);

            // if body
            bool if_returns = asm_statement(st, node->data.children + 1, start_label, end_label);
            fprintf(st->out, "\tjmp\t.%s%d\n", st->function_name, label_if_end);

            // else body
            fprintf(st->out, ".%s%d:\n", st->function_name, label_else);
            bool else_returns = asm_statement(st, node->data.children + 2, start_label, end_label);

            fprintf(st->out, ".%s%d:\n", st->function_name, label_if_end);
            after_unreachable = if_returns && else_returns;
        }
    } else if (node->kind == K_WHILE) {
        int label_start = st->label_index++;
        int label_end = st->label_index++;

        fprintf(st->out, ".%s%d:\n", st->function_name, label_start);

        asm_expr(st, node->data.children); // child 0: condition
        fputs("\tpop\trax\n", st->out);
        fputs("\ttest\teax, eax\n", st->out);
        fprintf(st->out, "\tje\t.%s%d\n", st->function_name, label_end);

        asm_statement(st, node->data.children + 1, label_start, label_end); // child 1: body

        fprintf(st->out, "\tjmp\t.%s%d\n", st->function_name, label_start);
        fprintf(st->out, ".%s%d:\n", st->function_name, label_end);
        after_unreachable = false;
    } else if (node->kind == K_BLOCK) {
        scopes_enter(&st->scopes);
        for (unsigned int i = 0; i < node->count; ++i) {
            if (asm_statement(st, node->data.children + i, start_label,
                              end_label)) {
                asm_exit_scope(st, false);
                return true;
            }
        }
        asm_exit_scope(st, true);
    } else if (node->kind == K_BREAK) {
        if (end_label < 0) panic("break outside of loop");
        fprintf(st->out, "\tjmp\t.%s%d\n", st->function_name, end_label);
        after_unreachable = true;
    } else if (node->kind == K_CONTINUE) {
        if (start_label < 0) panic("continue outside of loop");
        fprintf(st->out, "\tjmp\t.%s%d\n", st->function_name, start_label);
        after_unreachable = true;
    } else {
        panic("Unable to handle statement type");
    }
    return after_unreachable;
}

void asm_function(AsmState *st, AstNode *node) {
    assert(node->kind == K_FUNCTION);
    AstNode *name = node->data.children;
    assert(name->kind == K_IDENTIFIER);

    // 1. Function setup
    asm_enter_function(st, name->data.string);
    fprintf(st->out, "\t.globl %s\n", name->data.string);
    fprintf(st->out, "%s:\n", name->data.string);
    fputs("\tpush\trbp\n", st->out);
    fputs("\tmov\trbp, rsp\n", st->out);

    AstNode *params = node->data.children + 1;
    assert(params->kind == K_PARAMS);

    // 2. Handle parameters (Save from registers to stack)
    for (unsigned int i = 0; i < params->count; ++i) {
        AstNode *param_decl = params->data.children + i;

        // Register the parameter as a local variable
        asm_new_ident(st, param_decl);

        char *param_id = param_decl->kind == K_IDENTIFIER
                        ? param_decl->data.string
                        : param_decl->data.children[0].data.string;

        int offset = scopes_offset_of(&st->scopes, param_id);
        int size = type_size(param_decl);

        if (offset < 0) {
            printf("Error:\nStack offset %d < 0\n", offset);
            exit(-1);
        }

        if (i < num_arg_regs_pre) {
            char *reg = asm_reg_for_nth_function_param(true, i); // use 64-bit reg

            // Move register value to stack slot (size-specific move)
            if (size == 4) {
                fprintf(st->out, "\tmov\tDWORD PTR [rbp - %d], %s\n", offset, reg);
            } else if (size == 8) {
                // For 8-byte pointer arguments, use 64-bit register name
                fprintf(st->out, "\tmov\tQWORD PTR [rbp - %d], %s\n", offset, reg);
            } else {
                panic("unsupported parameter size");
            }
        } else {
            panic("function definition with more than 6 parameters is not supported for bootstrap");
        }
    }

    // 3. Allocate stack space for all locals (including parameters already saved)
    int total_allocated = scopes_total_allocated(&st->scopes);
    // Align total allocation to 16 bytes for ABI compliance
    total_allocated = (total_allocated + 15) & ~15;

    if (total_allocated > 0) {
        fprintf(st->out, "\tsub\trsp, %d\n", total_allocated);
        // Track allocated stack in current scope for cleanup if block scope exits manually
        st->scopes.scopes[st->scopes.count - 1].allocated_stack = total_allocated;
    }

    // 4. Generate code for function body
    AstNode *block = node->data.children + 2;
    assert(block->kind == K_BLOCK);

    // Iterate over statements in the block
    for (unsigned int i = 0; i < block->count; ++i) {
        if (asm_statement(st, block->data.children + i, -1, -1)) {
            // Function exited/returned, so we are done with this scope
            asm_exit_scope(st, false);
            return;
        }
    }

    // 5. Implicit return/function epilogue
    asm_exit_scope(st, true);

    // Fallthrough: if no explicit return, return 0
    fputs("\tmov\teax, 0\n", st->out);
    fputs("\tmov\trsp, rbp\n", st->out);
    fputs("\tpop\trbp\n", st->out);
    fputs("\tret\n", st->out);
}


// utility to register unique string literal
void register_string_literal(AstNode *node) {
    assert(node->kind == K_LITT_STRING);
    // simplicity: treat every K_LITT_STRING node as a unique string to be stored

    if (lit_count_pre >= lit_size_pre) {
        if (lit_size_pre == 0) {
            lit_size_pre = 4;
            // FIX: lit_list_pre is AstNode**, allocation size is correct
            lit_list_pre = malloc(lit_size_pre * sizeof(AstNode *));
        } else {
            lit_size_pre <<= 1;
            // FIX: lit_list_pre is AstNode**, realloc size is correct
            lit_list_pre = realloc(lit_list_pre, lit_size_pre * sizeof(AstNode *));
        }
        if (lit_list_pre == NULL) panic("failed to allocate literal list");
    }
    // FIX: assignment is AstNode* = AstNode*, which is correct
    lit_list_pre[lit_count_pre++] = node;
}

void walk_ast_for_literals(AstNode *node) {
    if (node == NULL) return;

    // recursive descent for children
    for (size_t i_pre = 0; i_pre < node->count; i_pre++) {
        walk_ast_for_literals(node->data.children + i_pre);
    }

    // register after children so they are processed first (less critical)
    if (node->kind == K_LITT_STRING) {
        register_string_literal(node);
    }
}


void asm_gen(AsmState *st, AstNode *root) {
    // 1. walk ast to register all string literals
    walk_ast_for_literals(root);

    fputs("\t.intel_syntax noprefix\n", st->out);

    // 2. emit assembly data section
    if (lit_count_pre > 0) {
        fputs("section .data\n", st->out);
        for (unsigned int i_pre = 0; i_pre < lit_count_pre; i_pre++) {
            // FIX: initialization is AstNode * = AstNode *
            AstNode *node = lit_list_pre[i_pre];
            // .lstr0: db "string content", 0
            fprintf(st->out, ".lstr%d: db \"", i_pre);
            // print string content, escaping quotes and nulls
            char const *s = node->data.string;
            for (unsigned int j_pre = 0; s[j_pre] != 0; j_pre++) {
                if (s[j_pre] == '"') {
                    fputs("\\\"", st->out); // escape double quotes
                } else if (s[j_pre] == '\\') {
                    fputs("\\\\", st->out); // escape backslashes
                } else {
                    fputc(s[j_pre], st->out);
                }
            }
            fputs("\", 0\n", st->out); // null-terminate string
        }
    }

    // 3. emit assembly text (code) section
    fputs("section .text\n", st->out);

    assert(root->kind == K_TOP_LEVEL);
    for (unsigned int i = 0; i < root->count; ++i) {
        asm_function(st, root->data.children + i);
    }
}

typedef enum CompileStage {
    STAGE_LEX,
    STAGE_PARSE,
    STAGE_COMPILE
} CompileStage;

int main(int argc, char **argv) {
    if (argc < 2) {
        panic("Must have a file to compile as an argument.");
    }
    char *in_filename = argv[1];
    char *out_filename = "a.s";
    if (argc > 2) {
        out_filename = argv[2];
    }
    CompileStage stage = STAGE_COMPILE;
    if (argc > 3) {
        char *stage_str = argv[3];
        if (strcmp(stage_str, "lex") == 0) {
            stage = STAGE_LEX;
        } else if (strcmp(stage_str, "parse") == 0) {
            stage = STAGE_PARSE;
        } else if (strcmp(stage_str, "compile") == 0) {
            stage = STAGE_COMPILE;
        }
    }
    FILE *in = fopen(in_filename, "r");
    if (in == NULL) {
        panic("Failed to open the input file.");
    }
    size_t length;
    if (fseek(in, 0, SEEK_END)) {
        panic("Failed to seek to the end of the input file");
    }
    length = ftell(in);
    if (fseek(in, 0, SEEK_SET)) {
        panic("Failed to rewind input file");
    }
    // FIX: Added +1 for null terminator and correct usage of in_data[length] = '\0'
    char *in_data = malloc(length + 1);
    if (in_data == NULL) {
        panic("Failed to allocate input buffer.");
    }
    if (fread(in_data, sizeof(char), length, in) != length) {
        panic("Failed to read into input buffer.");
    }
    in_data[length] = '\0'; // Null-terminate the string
    fclose(in);

    FILE *out;
    if (strcmp(out_filename, "stdout") == 0) {
        out = stdout;
    } else {
        out = fopen(out_filename, "w");
        if (out == NULL) {
            panic("Failed to open output file");
        }
    }
    LexState lexer = lex_init(in_data);
    if (stage == STAGE_LEX) {
        for (Token t = lex_next(&lexer); t.type != T_EOF;
             t = lex_next(&lexer)) {
            token_print(t, out);
        }
        return 0;
    }
    ParseState parser = parse_init(lexer);
    AstNode *root = parse_top_level(&parser);
    if (stage == STAGE_PARSE) {
        ast_print(root, out);
        return 0;
    }
    AsmState *generator = asm_init(out);
    asm_gen(generator, root);

    // clean up globals
    if (lit_list_pre) {
        free(lit_list_pre);
        lit_list_pre = NULL;
        lit_count_pre = 0;
        lit_size_pre = 0;
    }

    // cleanup
    free(in_data);
    if (out != stdout) {
        fclose(out);
    }

    return 0;
}
