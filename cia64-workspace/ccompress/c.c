
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
typedef enum {
	S_NORMAL,
	S_IN_STRING,
	S_IN_CHAR,
	S_IN_LINE_COMMENT,
	S_IN_BLOCK_COMMENT,
	S_IN_BLOCK_COMMENT_ASTERISK
} state_t;
#define TAB_WIDTH 4
static int write_tabs(FILE *out, int count) {
	for (int i = 0; i < count; ++i) {
		if (fputc('\t', out) == EOF) return -1;
	}
	return 0;
}
static int write_spaces(FILE *out, int count) {
	for (int i = 0; i < count; ++i) {
		if (fputc(' ', out) == EOF) return -1;
	}
	return 0;
}
static int flush_leading_spaces(FILE *out, int *pending_leading_spaces, int *at_line_start, int *last_emitted_newline) {
	if (*pending_leading_spaces > 0) {
		int tabs = *pending_leading_spaces / TAB_WIDTH;
		int rem = *pending_leading_spaces % TAB_WIDTH;
		if (write_tabs(out, tabs) != 0) return -1;
		if (write_spaces(out, rem) != 0) return -1;
		*pending_leading_spaces = 0;
		*at_line_start = 0; 
		*last_emitted_newline = 0;
	}
	return 0;
}
static int process_stream(FILE *in, FILE *out) {
	state_t state = S_NORMAL;
	int c;
	int at_line_start = 1; 
	int pending_leading_spaces = 0; 
	int last_emitted_newline = 0; 
	while ((c = fgetc(in)) != EOF) {
		if (state == S_NORMAL) {
			if (at_line_start) {
				
				if (c == ' ') {
					pending_leading_spaces++;
					continue;
				}
				if (c == '\t') {
					
					if (flush_leading_spaces(out, &pending_leading_spaces, &at_line_start, &last_emitted_newline) != 0) return -1;
					if (fputc('\t', out) == EOF) return -1;
					at_line_start = 0;
					last_emitted_newline = 0;
					continue;
				}
				
				if (c == '\n') {
					if (flush_leading_spaces(out, &pending_leading_spaces, &at_line_start, &last_emitted_newline) != 0) return -1;
					
					if (!last_emitted_newline) {
						if (fputc('\n', out) == EOF) return -1;
						last_emitted_newline = 1;
					}
					at_line_start = 1;
					continue;
				}
				
				if (pending_leading_spaces > 0) {
					if (flush_leading_spaces(out, &pending_leading_spaces, &at_line_start, &last_emitted_newline) != 0) return -1;
				}
			}
			
			if (c == '/') {
				int n = fgetc(in);
				if (n == '/') {
					
					
					if (at_line_start && pending_leading_spaces > 0) {
						if (flush_leading_spaces(out, &pending_leading_spaces, &at_line_start, &last_emitted_newline) != 0) return -1;
					}
					state = S_IN_LINE_COMMENT;
					
				} else if (n == '*') {
					
					if (at_line_start && pending_leading_spaces > 0) {
						if (flush_leading_spaces(out, &pending_leading_spaces, &at_line_start, &last_emitted_newline) != 0) return -1;
					}
					state = S_IN_BLOCK_COMMENT;
				} else {
					
					if (fputc('/', out) == EOF) return -1;
					if (n != EOF) ungetc(n, in);
					at_line_start = 0;
					last_emitted_newline = 0;
				}
			} else if (c == '"') {
				if (fputc('"', out) == EOF) return -1;
				state = S_IN_STRING;
				at_line_start = 0;
				last_emitted_newline = 0;
			} else if (c == '\'') {
				if (fputc('\'', out) == EOF) return -1;
				state = S_IN_CHAR;
				at_line_start = 0;
				last_emitted_newline = 0;
			} else if (c == '\n') {
				
				if (flush_leading_spaces(out, &pending_leading_spaces, &at_line_start, &last_emitted_newline) != 0) return -1;
				if (!last_emitted_newline) {
					if (fputc('\n', out) == EOF) return -1;
					last_emitted_newline = 1;
				}
				at_line_start = 1;
			} else {
				
				if (fputc(c, out) == EOF) return -1;
				at_line_start = 0;
				last_emitted_newline = 0;
			}
		} else if (state == S_IN_STRING) {
			
			if (c == '\\') {
				
				if (fputc('\\', out) == EOF) return -1;
				int n = fgetc(in);
				if (n == EOF) break;
				if (fputc(n, out) == EOF) return -1;
			} else if (c == '"') {
				if (fputc('"', out) == EOF) return -1;
				state = S_NORMAL;
			} else {
				if (fputc(c, out) == EOF) return -1;
			}
			at_line_start = 0;
			last_emitted_newline = 0;
		} else if (state == S_IN_CHAR) {
			
			if (c == '\\') {
				if (fputc('\\', out) == EOF) return -1;
				int n = fgetc(in);
				if (n == EOF) break;
				if (fputc(n, out) == EOF) return -1;
			} else if (c == '\'') {
				if (fputc('\'', out) == EOF) return -1;
				state = S_NORMAL;
			} else {
				if (fputc(c, out) == EOF) return -1;
			}
			at_line_start = 0;
			last_emitted_newline = 0;
		} else if (state == S_IN_LINE_COMMENT) {
			
			if (c == '\n') {
				
				state = S_NORMAL;
				
				
				
			} else {
				
			}
		} else if (state == S_IN_BLOCK_COMMENT) {
			
			if (c == '*') {
				state = S_IN_BLOCK_COMMENT_ASTERISK;
			} else {
				
			}
		} else if (state == S_IN_BLOCK_COMMENT_ASTERISK) {
			if (c == '/') {
				
				state = S_NORMAL;
				
			} else if (c == '*') {
				
				state = S_IN_BLOCK_COMMENT_ASTERISK;
			} else {
				
				state = S_IN_BLOCK_COMMENT;
			}
		}
	}
	
	if (pending_leading_spaces > 0) {
		int tabs = pending_leading_spaces / TAB_WIDTH;
		int rem = pending_leading_spaces % TAB_WIDTH;
		if (write_tabs(out, tabs) != 0) return -1;
		if (write_spaces(out, rem) != 0) return -1;
		last_emitted_newline = 0;
	}
	return 0;
}
int main(int argc, char **argv) {
	if (argc != 3) {
		fprintf(stderr, "Usage: %s input-file output-file\n", argv[0]);
		return 2;
	}
	const char *inpath = argv[1];
	const char *outpath = argv[2];
	FILE *in = fopen(inpath, "rb");
	if (!in) {
		fprintf(stderr, "ccompress: cannot open input '%s': %s\n", inpath, strerror(errno));
		return 3;
	}
	
	char tmp[4096];
	if (snprintf(tmp, sizeof(tmp), "%s.tmp", outpath) >= (int)sizeof(tmp)) {
		fprintf(stderr, "ccompress: output path too long\n");
		fclose(in);
		return 4;
	}
	FILE *out = fopen(tmp, "wb");
	if (!out) {
		fprintf(stderr, "ccompress: cannot open output '%s': %s\n", tmp, strerror(errno));
		fclose(in);
		return 5;
	}
	int rc = process_stream(in, out);
	fclose(in);
	if (fflush(out) != 0) rc = -1;
	if (fclose(out) != 0) rc = -1;
	if (rc != 0) {
		fprintf(stderr, "ccompress: processing failed\n");
		remove(tmp);
		return 6;
	}
	
	if (rename(tmp, outpath) != 0) {
		fprintf(stderr, "ccompress: cannot rename '%s' -> '%s': %s\n", tmp, outpath, strerror(errno));
		remove(tmp);
		return 7;
	}
	return 0;
}
