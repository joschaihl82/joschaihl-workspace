/* sparrow.c
 *
 * Tiny single-file JavaScript-like compiler that emits x86-64 AT&T assembly.
 * See earlier notes for usage and limitations.
 *
 * Verbesserte Version: Division, BinOps, String-Escaping, vollständiges main.
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

/* ---------------------------
 * Utilities
 * --------------------------- */

static char *global_input = NULL;
void error_at(const char *loc, const char *fmt, ...) {
    va_list ap; va_start(ap, fmt);
    int pos = (int)(loc - global_input);
    fprintf(stderr, "\n--- ERROR ---\n");
    if (global_input) {
        fprintf(stderr, "%s\n", global_input);
        fprintf(stderr, "%*s^ ", pos, "");
    }
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n-------------\n");
    va_end(ap);
    exit(1);
}
static char* read_file(const char *path) {
    FILE *f = fopen(path, "rb"); if (!f) return NULL;
    if (fseek(f, 0, SEEK_END) != 0) { fclose(f); return NULL; }
    long sz = ftell(f); if (sz < 0) sz = 0; rewind(f);
    char *buf = malloc((size_t)sz + 1); if (!buf) { fclose(f); return NULL; }
    size_t n = fread(buf, 1, (size_t)sz, f); buf[n] = '\0'; fclose(f); return buf;
}

/* ---------------------------
 * Lexer
 * --------------------------- */

typedef enum {
    TK_EOF=0, TK_IDENT, TK_NUMBER, TK_STRING,
    TK_VAR, TK_FUNCTION, TK_RETURN, TK_IF, TK_ELSE, TK_WHILE, TK_FOR,
    TK_LPAREN, TK_RPAREN, TK_LBRACE, TK_RBRACE, TK_COMMA, TK_SEMI,
    TK_PLUS, TK_MINUS, TK_STAR, TK_SLASH,
    TK_ASSIGN, TK_EQ, TK_NEQ, TK_LT, TK_GT, TK_LE, TK_GE,
    TK_AND, TK_OR, TK_NOT,
    TK_UNKNOWN
} TokenKind;

typedef struct {
    TokenKind kind;
    char *lexeme;
    double num_val;
    char *loc;
} Token;

typedef struct {
    Token *arr; int count; int cap; int idx;
} TokenStream;

static void ts_init(TokenStream *ts){ ts->arr=NULL; ts->count=ts->cap=ts->idx=0; }
static void ts_push(TokenStream *ts, Token t){
    if (ts->count+1>ts->cap){ ts->cap = ts->cap?ts->cap*2:256; ts->arr = realloc(ts->arr, ts->cap*sizeof(Token)); }
    ts->arr[ts->count++] = t;
}
static Token ts_peek(TokenStream *ts){ return ts->arr[ts->idx]; }
static Token ts_next(TokenStream *ts){ return ts->arr[ts->idx++]; }
static int ts_eof(TokenStream *ts){ return ts->arr[ts->idx].kind==TK_EOF; }

static int is_ident_start(char c){ return isalpha((unsigned char)c) || c=='_'; }
static int is_ident_char(char c){ return isalnum((unsigned char)c) || c=='_'; }

static Token make_token(TokenKind k, const char *loc, const char *lex){
    Token t; t.kind=k; t.loc=(char*)loc; t.lexeme = lex?strdup(lex):NULL; t.num_val=0; return t;
}
static Token make_number_token(const char *loc, double v, const char *lex){
    Token t = make_token(TK_NUMBER, loc, lex); t.num_val = v; return t;
}
static Token make_string_token(const char *loc, const char *s){ return make_token(TK_STRING, loc, s); }

static TokenKind keyword_kind(const char *s){
    if (!strcmp(s,"var")) return TK_VAR;
    if (!strcmp(s,"function")) return TK_FUNCTION;
    if (!strcmp(s,"return")) return TK_RETURN;
    if (!strcmp(s,"if")) return TK_IF;
    if (!strcmp(s,"else")) return TK_ELSE;
    if (!strcmp(s,"while")) return TK_WHILE;
    if (!strcmp(s,"for")) return TK_FOR;
    return TK_IDENT;
}

static TokenStream tokenize(const char *input){
    global_input = (char*)input;
    TokenStream ts; ts_init(&ts);
    const char *p = input;
    while (*p){
        if (isspace((unsigned char)*p)){ p++; continue; }
        if (p[0]=='/' && p[1]=='/'){ p+=2; while(*p && *p!='\n') p++; continue; }
        if (p[0]=='/' && p[1]=='*'){ p+=2; while(*p && !(p[0]=='*' && p[1]=='/')) p++; if(*p) p+=2; continue; }
        if (is_ident_start(*p)){
            const char *s=p; while(is_ident_char(*p)) p++; int len=(int)(p-s); char *lex=strndup(s,len);
            TokenKind k = keyword_kind(lex); Token t = make_token(k, s, lex); free(lex); ts_push(&ts,t); continue;
        }
        if (isdigit((unsigned char)*p) || (*p=='.' && isdigit((unsigned char)p[1]))){
            char *end; double v = strtod(p,&end); int len=(int)(end-p); char *lex=strndup(p,len);
            Token t = make_number_token(p,v,lex); free(lex); ts_push(&ts,t); p=end; continue;
        }
        if (*p=='"' || *p=='\''){
            char q = *p++; const char *s=p; char buf[4096]; int bi=0;
            while(*p && *p!=q){
                if (*p=='\\'){ p++; if (*p=='n') buf[bi++]='\n'; else if (*p=='t') buf[bi++]='\t'; else buf[bi++]=*p; p++; }
                else buf[bi++]=*p++;
                if (bi>= (int)sizeof(buf)-1) break;
            }
            if (*p!=q) error_at(p,"Unterminated string literal");
            buf[bi]='\0'; p++; Token t = make_string_token(s-1, buf); ts_push(&ts,t); continue;
        }
        if (!strncmp(p,"==",2)){ ts_push(&ts, make_token(TK_EQ,p,"==")); p+=2; continue; }
        if (!strncmp(p,"!=",2)){ ts_push(&ts, make_token(TK_NEQ,p,"!=")); p+=2; continue; }
        if (!strncmp(p,"<=",2)){ ts_push(&ts, make_token(TK_LE,p,"<=")); p+=2; continue; }
        if (!strncmp(p,">=",2)){ ts_push(&ts, make_token(TK_GE,p,">=")); p+=2; continue; }
        if (!strncmp(p,"&&",2)){ ts_push(&ts, make_token(TK_AND,p,"&&")); p+=2; continue; }
        if (!strncmp(p,"||",2)){ ts_push(&ts, make_token(TK_OR,p,"||")); p+=2; continue; }
        TokenKind k = TK_UNKNOWN; const char *loc = p; char lex[2]={0,0};
        switch(*p){
            case '(': k=TK_LPAREN; lex[0]='('; break;
            case ')': k=TK_RPAREN; lex[0]=')'; break;
            case '{': k=TK_LBRACE; lex[0]='{'; break;
            case '}': k=TK_RBRACE; lex[0]='}'; break;
            case ',': k=TK_COMMA; lex[0]=','; break;
            case ';': k=TK_SEMI; lex[0]=';'; break;
            case '+': k=TK_PLUS; lex[0]='+'; break;
            case '-': k=TK_MINUS; lex[0]='-'; break;
            case '*': k=TK_STAR; lex[0]='*'; break;
            case '/': k=TK_SLASH; lex[0]='/'; break;
            case '=': k=TK_ASSIGN; lex[0]='='; break;
            case '<': k=TK_LT; lex[0]='<'; break;
            case '>': k=TK_GT; lex[0]='>'; break;
            case '!': k=TK_NOT; lex[0]='!'; break;
            default: error_at(p,"Unexpected character '%c'", *p);
        }
        ts_push(&ts, make_token(k, loc, lex)); p++;
    }
    ts_push(&ts, make_token(TK_EOF, (char*)input + strlen(input), "<EOF>"));
    return ts;
}

/* ---------------------------
 * AST
 * --------------------------- */

typedef enum {
    AST_PROGRAM, AST_FUNC_DECL, AST_VAR_DECL, AST_BLOCK, AST_RETURN,
    AST_IF, AST_WHILE, AST_FOR, AST_EXPR_STMT, AST_BINARY, AST_UNARY,
    AST_CALL, AST_IDENT, AST_NUMBER, AST_STRING, AST_EMPTY
} ASTKind;

typedef struct AST {
    ASTKind kind;
    Token token;
    struct AST **children; int child_count;
    char *name; double num; char *str;
    char **params; int param_count;
} AST;

static AST* ast_new(ASTKind k, Token t){ AST *a=calloc(1,sizeof(AST)); a->kind=k; a->token=t; return a; }
static void ast_add(AST *p, AST *c){ if(!p||!c) return; p->children = realloc(p->children, sizeof(AST*)*(p->child_count+1)); p->children[p->child_count++]=c; }

/* ---------------------------
 * Parser
 * --------------------------- */

typedef struct { TokenStream *ts; } Parser;
static Token cur(Parser *p){ return p->ts->arr[p->ts->idx]; }
static int accept(Parser *p, TokenKind k){ if (p->ts->arr[p->ts->idx].kind==k){ p->ts->idx++; return 1;} return 0; }
static Token expect(Parser *p, TokenKind k){ Token t = cur(p); if (t.kind!=k) error_at(t.loc,"Expected token %d got %d",k,t.kind); p->ts->idx++; return t; }

static AST* parse_program(Parser *p);
static AST* parse_statement(Parser *p);
static AST* parse_expression(Parser *p);
static AST* parse_primary(Parser *p);
static AST* parse_function_decl(Parser *p);

static AST* parse_primary(Parser *p){
    Token t = cur(p);
    if (t.kind==TK_NUMBER){ expect(p,TK_NUMBER); AST *n=ast_new(AST_NUMBER,t); n->num=t.num_val; return n; }
    if (t.kind==TK_STRING){ expect(p,TK_STRING); AST *n=ast_new(AST_STRING,t); n->str=strdup(t.lexeme?t.lexeme:""); return n; }
    if (t.kind==TK_IDENT){ expect(p,TK_IDENT); AST *n=ast_new(AST_IDENT,t); n->name=strdup(t.lexeme?t.lexeme:"");
        if (accept(p,TK_LPAREN)){
            AST *call = ast_new(AST_CALL,t);
            if (!accept(p,TK_RPAREN)){
                do { AST *arg = parse_expression(p); ast_add(call,arg); } while(accept(p,TK_COMMA));
                expect(p,TK_RPAREN);
            }
            ast_add(call,n); // callee as last child
            return call;
        }
        return n;
    }
    if (accept(p,TK_LPAREN)){ AST *e = parse_expression(p); expect(p,TK_RPAREN); return e; }
    error_at(t.loc,"Unexpected token in primary");
    return NULL;
}

static AST* parse_unary(Parser *p){
    Token t = cur(p);
    if (accept(p,TK_NOT) || accept(p,TK_MINUS)){
        AST *op = ast_new(AST_UNARY,t);
        AST *c = parse_unary(p); ast_add(op,c); return op;
    }
    return parse_primary(p);
}

static int prec(TokenKind k){
    switch(k){
        case TK_OR: return 1;
        case TK_AND: return 2;
        case TK_EQ: case TK_NEQ: return 3;
        case TK_LT: case TK_GT: case TK_LE: case TK_GE: return 4;
        case TK_PLUS: case TK_MINUS: return 5;
        case TK_STAR: case TK_SLASH: return 6;
        default: return -1;
    }
}

static AST* parse_bin_rhs(Parser *p, int expr_prec, AST *lhs){
    while(1){
        Token t = cur(p); int tok_prec = prec(t.kind);
        if (tok_prec < expr_prec) return lhs;
        Token op = t; p->ts->idx++;
        AST *rhs = parse_unary(p);
        Token next = cur(p); int next_prec = prec(next.kind);
        if (tok_prec < next_prec) rhs = parse_bin_rhs(p, tok_prec+1, rhs);
        AST *bin = ast_new(AST_BINARY, op);
        ast_add(bin, lhs); ast_add(bin, rhs);
        lhs = bin;
    }
}

static AST* parse_expression(Parser *p){
    AST *lhs = parse_unary(p);
    return parse_bin_rhs(p, 0, lhs);
}

static AST* parse_statement(Parser *p){
    Token t = cur(p);
    if (t.kind==TK_SEMI){ expect(p,TK_SEMI); return ast_new(AST_EMPTY,t); }
    if (t.kind==TK_LBRACE){ expect(p,TK_LBRACE); AST *blk=ast_new(AST_BLOCK,t); while(cur(p).kind!=TK_RBRACE && cur(p).kind!=TK_EOF){ AST *s=parse_statement(p); ast_add(blk,s);} expect(p,TK_RBRACE); return blk; }
    if (t.kind==TK_VAR){ expect(p,TK_VAR); Token id = expect(p,TK_IDENT); AST *decl=ast_new(AST_VAR_DECL,id); decl->name=strdup(id.lexeme?id.lexeme:""); if (accept(p,TK_ASSIGN)){ AST *init=parse_expression(p); ast_add(decl,init);} expect(p,TK_SEMI); return decl; }
    if (t.kind==TK_RETURN){ expect(p,TK_RETURN); AST *r=ast_new(AST_RETURN,t); if (cur(p).kind!=TK_SEMI){ AST *e=parse_expression(p); ast_add(r,e);} expect(p,TK_SEMI); return r; }
    if (t.kind==TK_IF){ expect(p,TK_IF); expect(p,TK_LPAREN); AST *cond=parse_expression(p); expect(p,TK_RPAREN); AST *then=parse_statement(p); AST *ifs=ast_new(AST_IF,t); ast_add(ifs,cond); ast_add(ifs,then); if (accept(p,TK_ELSE)){ AST *els=parse_statement(p); ast_add(ifs,els);} return ifs; }
    if (t.kind==TK_WHILE){ expect(p,TK_WHILE); expect(p,TK_LPAREN); AST *cond=parse_expression(p); expect(p,TK_RPAREN); AST *body=parse_statement(p); AST *w=ast_new(AST_WHILE,t); ast_add(w,cond); ast_add(w,body); return w; }
    if (t.kind==TK_FOR){ expect(p,TK_FOR); expect(p,TK_LPAREN); AST *f=ast_new(AST_FOR,t);
        if (cur(p).kind!=TK_SEMI){ if (cur(p).kind==TK_VAR){ AST *init=parse_statement(p); ast_add(f,init);} else { AST *init=parse_expression(p); ast_add(f,init); expect(p,TK_SEMI);} } else { expect(p,TK_SEMI); ast_add(f,NULL); }
        if (cur(p).kind!=TK_SEMI){ AST *cond=parse_expression(p); ast_add(f,cond); expect(p,TK_SEMI);} else { expect(p,TK_SEMI); ast_add(f,NULL); }
        if (cur(p).kind!=TK_RPAREN){ AST *inc=parse_expression(p); ast_add(f,inc); expect(p,TK_RPAREN);} else { expect(p,TK_RPAREN); ast_add(f,NULL); }
        AST *body=parse_statement(p); ast_add(f,body); return f;
    }
    if (t.kind==TK_FUNCTION) return parse_function_decl(p);
    AST *e = parse_expression(p); expect(p,TK_SEMI); AST *stmt = ast_new(AST_EXPR_STMT,e->token); ast_add(stmt,e); return stmt;
}

static AST* parse_function_decl(Parser *p){
    Token tk = expect(p,TK_FUNCTION); Token name = expect(p,TK_IDENT);
    AST *fn = ast_new(AST_FUNC_DECL,name); fn->name = strdup(name.lexeme?name.lexeme:"");
    expect(p,TK_LPAREN);
    fn->params = NULL; fn->param_count = 0;
    if (!accept(p,TK_RPAREN)){
        do { Token pid = expect(p,TK_IDENT); fn->params = realloc(fn->params, sizeof(char*)*(fn->param_count+1)); fn->params[fn->param_count++]=strdup(pid.lexeme?pid.lexeme:""); } while(accept(p,TK_COMMA));
        expect(p,TK_RPAREN);
    }
    AST *body = parse_statement(p); ast_add(fn, body); return fn;
}

static AST* parse_program(Parser *p){
    AST *prog = ast_new(AST_PROGRAM, cur(p));
    while (cur(p).kind != TK_EOF){ AST *s = parse_statement(p); ast_add(prog,s); }
    return prog;
}

/* ---------------------------
 * Codegen
 * --------------------------- */

typedef struct StrEntry { char *s; int id; struct StrEntry *next; } StrEntry;
typedef struct GVar { char *name; int id; struct GVar *next; } GVar;
typedef struct FuncEntry { char *name; AST *fn; struct FuncEntry *next; } FuncEntry;

static StrEntry *strs = NULL; static int str_count = 0;
static GVar *gvars = NULL; static int gvar_count = 0;
static FuncEntry *funcs = NULL;

static int add_string_literal(const char *s){
    for (StrEntry *e=strs;e;e=e->next) if (!strcmp(e->s,s)) return e->id;
    StrEntry *n = calloc(1,sizeof(StrEntry)); n->s = strdup(s); n->id = str_count++; n->next = strs; strs = n; return n->id;
}
static int add_global_var(const char *name){
    for (GVar *g=gvars; g; g=g->next) if (!strcmp(g->name,name)) return g->id;
    GVar *n = calloc(1,sizeof(GVar)); n->name = strdup(name); n->id = gvar_count++; n->next = gvars; gvars = n; return n->id;
}
static void add_func_entry(const char *name, AST *fn){
    FuncEntry *f = calloc(1,sizeof(FuncEntry)); f->name = strdup(name); f->fn = fn; f->next = funcs; funcs = f;
}

static void asm_printf(const char *fmt, ...){
    va_list ap; va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);
}

/* Escape for .asciz */
static char* escape_for_asciz(const char *s){
    size_t n = strlen(s); char *out = malloc(n*4 + 1); char *o = out;
    for (size_t i=0;i<n;i++){
        unsigned char c = (unsigned char)s[i];
        if (c == '"'){ *o++='\\'; *o++='"'; }
        else if (c == '\\'){ *o++='\\'; *o++='\\'; }
        else if (c == '\n'){ *o++='\\'; *o++='n'; }
        else if (c == '\t'){ *o++='\\'; *o++='t'; }
        else if (c < 32 || c > 126){ o += sprintf(o, "\\x%02x", c); }
        else *o++ = c;
    }
    *o = '\0'; return out;
}

/* Forward */
static void cg_emit_stmt(AST *node);
static void cg_emit_expr(AST *node);

static const char* gvar_label_for(const char *name){
    add_global_var(name);
    return name;
}

/* Evaluate expression so result is in %rax */
static void cg_emit_expr(AST *node){
    if (!node) return;
    switch(node->kind){
        case AST_NUMBER: {
            long long v = (long long) node->num;
            asm_printf("\t# number %g\n", node->num);
            asm_printf("\tmovq $%lld, %%rax\n", v);
            break;
        }
        case AST_STRING: {
            int id = add_string_literal(node->str ? node->str : "");
            asm_printf("\tleaq .Lstr%d(%%rip), %%rax\n", id);
            break;
        }
        case AST_IDENT: {
            const char *lbl = gvar_label_for(node->name);
            asm_printf("\tmovq %s(%%rip), %%rax\n", lbl);
            break;
        }
        case AST_UNARY: {
            AST *child = node->children[0];
            cg_emit_expr(child);
            if (node->token.kind == TK_MINUS){
                asm_printf("\tnegq %%rax\n");
            } else if (node->token.kind == TK_NOT){
                asm_printf("\tcmpq $0, %%rax\n\tsete %%al\n\tmovzbq %%al, %%rax\n");
            } else error_at(node->token.loc, "Unsupported unary op");
            break;
        }
        case AST_BINARY: {
            AST *lhs = node->children[0];
            AST *rhs = node->children[1];
            cg_emit_expr(lhs);
            asm_printf("\tpushq %%rax\n");
            cg_emit_expr(rhs);
            asm_printf("\t# now: %%rax = right, pop left into %%rbx\n");
            asm_printf("\tpopq %%rbx\n");
            /* normalize: rcx = right, rbx = left */
            asm_printf("\tmovq %%rax, %%rcx\n");
            const char *op = node->token.lexeme ? node->token.lexeme : "";
            if (!strcmp(op, "+")) {
                asm_printf("\taddq %%rbx, %%rcx\n\tmovq %%rcx, %%rax\n");
            } else if (!strcmp(op, "-")) {
                asm_printf("\tsubq %%rcx, %%rbx\n\tmovq %%rbx, %%rax\n");
            } else if (!strcmp(op, "*")) {
                asm_printf("\timulq %%rcx, %%rbx\n\tmovq %%rbx, %%rax\n");
            } else if (!strcmp(op, "/")) {
                /* left in rbx, right in rcx */
                asm_printf("\tmovq %%rbx, %%rax\n\tcqto\n\tidivq %%rcx\n");
                /* quotient in rax */
            } else if (!strcmp(op, "==") || !strcmp(op, "!=") || !strcmp(op, "<") || !strcmp(op, ">") || !strcmp(op, "<=") || !strcmp(op, ">=")) {
                asm_printf("\tcmpq %%rcx, %%rbx\n");
                if (!strcmp(op, "==")) asm_printf("\tsete %%al\n");
                else if (!strcmp(op, "!=")) asm_printf("\tsetne %%al\n");
                else if (!strcmp(op, "<")) asm_printf("\tsetl %%al\n");
                else if (!strcmp(op, ">")) asm_printf("\tsetg %%al\n");
                else if (!strcmp(op, "<=")) asm_printf("\tsetle %%al\n");
                else if (!strcmp(op, ">=")) asm_printf("\tsetge %%al\n");
                asm_printf("\tmovzbq %%al, %%rax\n");
            } else {
                error_at(node->token.loc, "Unsupported binary op in codegen: %s", op);
            }
            break;
        }
        case AST_CALL: {
            int n = node->child_count - 1;
            AST *callee = node->children[node->child_count - 1];
            if (callee->kind != AST_IDENT) error_at(callee->token.loc, "Call target must be identifier");
            const char *fname = callee->name;
            if (!strcmp(fname, "print")) {
                char fmtbuf[512] = {0}; int pos=0;
                for (int i=0;i<n;i++){
                    AST *a = node->children[i];
                    if (a->kind == AST_STRING) pos += snprintf(fmtbuf+pos, sizeof(fmtbuf)-pos, "%%s");
                    else pos += snprintf(fmtbuf+pos, sizeof(fmtbuf)-pos, "%%g");
                    if (i+1<n) pos += snprintf(fmtbuf+pos, sizeof(fmtbuf)-pos, " ");
                }
                pos += snprintf(fmtbuf+pos, sizeof(fmtbuf)-pos, "\\n");
                int sid = add_string_literal(fmtbuf);
                /* push args right-to-left */
                for (int i=n-1;i>=0;i--){
                    cg_emit_expr(node->children[i]);
                    asm_printf("\tpushq %%rax\n");
                }
                asm_printf("\tleaq .Lstr%d(%%rip), %%rax\n", sid);
                asm_printf("\tpushq %%rax\n");
                asm_printf("\tcall printf@PLT\n");
                asm_printf("\taddq $%d, %%rsp\n", (n+1)*8);
                asm_printf("\tmovq $0, %%rax\n");
            } else {
                for (int i=n-1;i>=0;i--){
                    cg_emit_expr(node->children[i]);
                    asm_printf("\tpushq %%rax\n");
                }
                asm_printf("\tcall %s\n", fname);
                if (n>0) asm_printf("\taddq $%d, %%rsp\n", n*8);
            }
            break;
        }
        default:
            error_at(node->token.loc, "Unhandled expr kind in codegen: %d", node->kind);
    }
}

/* Statements */
static void cg_emit_stmt(AST *node){
    if (!node) return;
    switch(node->kind){
        case AST_EMPTY: break;
        case AST_BLOCK:
            for (int i=0;i<node->child_count;i++) cg_emit_stmt(node->children[i]);
            break;
        case AST_VAR_DECL: {
            const char *lbl = gvar_label_for(node->name);
            if (node->child_count>0){
                cg_emit_expr(node->children[0]);
                asm_printf("\tmovq %%rax, %s(%%rip)\n", lbl);
            } else {
                asm_printf("\tmovq $0, %%rax\n\tmovq %%rax, %s(%%rip)\n", lbl);
            }
            break;
        }
        case AST_EXPR_STMT:
            cg_emit_expr(node->children[0]);
            break;
        case AST_RETURN:
            if (node->child_count>0) cg_emit_expr(node->children[0]);
            else asm_printf("\tmovq $0, %%rax\n");
            asm_printf("\tleave\n\tret\n");
            break;
        case AST_IF: {
            int lbl_else = rand() & 0xFFFFF;
            int lbl_end = rand() & 0xFFFFF;
            cg_emit_expr(node->children[0]);
            asm_printf("\tcmpq $0, %%rax\n\tje .Lelse%d\n", lbl_else);
            cg_emit_stmt(node->children[1]);
            asm_printf("\tjmp .Lend%d\n", lbl_end);
            asm_printf(".Lelse%d:\n", lbl_else);
            if (node->child_count>2) cg_emit_stmt(node->children[2]);
            asm_printf(".Lend%d:\n", lbl_end);
            break;
        }
        case AST_WHILE: {
            int lbl_start = rand() & 0xFFFFF;
            int lbl_end = rand() & 0xFFFFF;
            asm_printf(".Lwhile%d:\n", lbl_start);
            cg_emit_expr(node->children[0]);
            asm_printf("\tcmpq $0, %%rax\n\tje .Lend%d\n", lbl_end);
            cg_emit_stmt(node->children[1]);
            asm_printf("\tjmp .Lwhile%d\n", lbl_start);
            asm_printf(".Lend%d:\n", lbl_end);
            break;
        }
        case AST_FOR: {
            AST *init = node->child_count>0?node->children[0]:NULL;
            AST *cond = node->child_count>1?node->children[1]:NULL;
            AST *incr = node->child_count>2?node->children[2]:NULL;
            AST *body = node->child_count>3?node->children[3]:NULL;
            if (init) cg_emit_stmt(init);
            int lbl_start = rand() & 0xFFFFF;
            int lbl_end = rand() & 0xFFFFF;
            asm_printf(".Lfor%d:\n", lbl_start);
            if (cond){
                cg_emit_expr(cond);
                asm_printf("\tcmpq $0, %%rax\n\tje .Lend%d\n", lbl_end);
            }
            cg_emit_stmt(body);
            if (incr) cg_emit_expr(incr);
            asm_printf("\tjmp .Lfor%d\n", lbl_start);
            asm_printf(".Lend%d:\n", lbl_end);
            break;
        }
        case AST_FUNC_DECL:
            add_func_entry(node->name, node);
            break;
        default:
            error_at(node->token.loc, "Unhandled stmt kind in codegen: %d", node->kind);
    }
}

/* Emit functions */
static void emit_function(AST *fn){
    asm_printf("\t.text\n");
    asm_printf("\t.globl %s\n", fn->name);
    asm_printf("%s:\n", fn->name);
    asm_printf("\tpushq %%rbp\n\tmovq %%rsp, %%rbp\n");
    cg_emit_stmt(fn->children[0]);
    asm_printf("\tleave\n\tret\n");
}

/* Emit data sections */
static void emit_data_section(void){
    if (strs){
        asm_printf("\t.section .rodata\n");
        for (StrEntry *e = strs; e; e = e->next){
            char *esc = escape_for_asciz(e->s);
            asm_printf(".Lstr%d:\n", e->id);
            asm_printf("\t.asciz \"%s\"\n", esc);
            free(esc);
        }
    }
    if (gvars){
        asm_printf("\t.data\n");
        for (GVar *g = gvars; g; g = g->next){
            /* ensure label is valid: replace spaces or invalid chars if any (names from parser are identifiers) */
            asm_printf("%s:\n", g->name);
            asm_printf("\t.quad 0\n");
        }
    }
}

/* Top-level codegen */
static void cg_emit_program(AST *prog){
    /* collect globals and functions */
    for (int i=0;i<prog->child_count;i++){
        AST *s = prog->children[i];
        if (s->kind == AST_VAR_DECL) add_global_var(s->name);
        if (s->kind == AST_FUNC_DECL) add_func_entry(s->name, s);
    }
    /* emit main */
    asm_printf("\t.text\n\t.globl main\nmain:\n");
    asm_printf("\tpushq %%rbp\n\tmovq %%rsp, %%rbp\n");
    for (int i=0;i<prog->child_count;i++){
        AST *s = prog->children[i];
        if (s->kind != AST_FUNC_DECL) cg_emit_stmt(s);
    }
    asm_printf("\tmovl $0, %%eax\n\tleave\n\tret\n");
    /* emit functions */
    for (FuncEntry *f = funcs; f; f = f->next) emit_function(f->fn);
    /* emit data */
    emit_data_section();
}

/* ---------------------------
 * Main
 * --------------------------- */

int main(int argc, char **argv){
    srand((unsigned)time(NULL));
    if (argc < 2){ fprintf(stderr, "Usage: %s file.sparrow > out.s\n", argv[0]); return 1; }
    char *src = read_file(argv[1]); if (!src){ fprintf(stderr, "Cannot read %s\n", argv[1]); return 1; }
    TokenStream ts = tokenize(src);
    Parser p = { .ts = &ts };
    AST *prog = parse_program(&p);

    /* generate assembly to stdout */
    cg_emit_program(prog);

    /* note: minimal cleanup omitted for brevity */
    return 0;
}
