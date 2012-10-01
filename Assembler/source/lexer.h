#ifndef __LEXER_H__
#define __LEXER_H__

#include <stdio.h>
#include "tokens.h"


#define LEX_BUFFER_SIZE	4096
#define IMMEDIATE_MAX	10
#define INSTR_MAX		5


typedef struct _LexerState {
	int line;					// current scanning line
	char buf[LEX_BUFFER_SIZE];	// buffered source
	int buf_size;				// number of chars in buffer
	char* pos;					// current position in buffer
	char c;						// current character in buffer
} LexerState;

typedef struct _Lexer {
	LexerState state;	// current state of this lexer
	int lexeme_id;		// token id for last scanned lexeme
	char* lexeme;		// last scanned lexeme
	int lexeme_len;		// length of last scanned lexeme
	FILE* file;			// file used for lexer
} Lexer;


// Create a new lexer
Lexer* lexer_create(FILE* f);
// Destroy an existing lexer
void lexer_destroy(Lexer* lex);
// Fetch the next token from the input stream
int lexer_next_token(Lexer* lex);


#endif // __LEXER_H__
