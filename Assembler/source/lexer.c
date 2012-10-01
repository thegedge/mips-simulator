#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "lexer.h"


// Will update the lexer with the string from
//   start to end and its TOKEN value
#define LEX_UPDATE(lex, start, end, TOKEN)				\
	{													\
		__lexer_lexeme_fetch(lex, start, end);			\
		lex->lexeme_id = TOKEN;							\
		return TOKEN;									\
	}

// Checks for a new line in the input stream
#define CHECK_NEW_LINE(lex, c) if(c == '\n') lex->state.line++;

// If we have nothing left in the buffer, refresh it with
//   new data from the input stream
#define BUFFER_UPDATE(lex, ls)	\
	if((ls->pos - ls->buf) >= LEX_BUFFER_SIZE || ls->buf_size <= 0) {	\
		ls->buf_size = (int)fread(ls->buf, 1, LEX_BUFFER_SIZE, lex->file);	\
		ls->pos = ls->buf;	\
	}


// This function fetches a character for the lexer. It will
//   return true if there exists a character, false otherwise.
bool __lexer_getch(Lexer* lex, LexerState* ls) {
	BUFFER_UPDATE(lex, ls);
	if(ls->buf_size <= 0)
		return false;
	ls->c = *ls->pos++;
	CHECK_NEW_LINE(lex, ls->c);
	return true;
}

Lexer* lexer_create(FILE* f) {
	if(f == NULL) return NULL;

	Lexer* lex = (Lexer*)malloc(sizeof(Lexer));
	if(lex) {
		lex->file = f;
		lex->state.line = 1;
		lex->state.buf_size = 0;
		lex->lexeme = lex->state.pos = NULL;
		lex->lexeme_id = TOKEN_ERROR;
	}
	return lex;
}

// This funtion will fetch the lexeme (string) found from
//  start to end
void __lexer_lexeme_fetch(Lexer* lex, char* start, char* end) {
	free(lex->lexeme);
	lex->lexeme = (char*)malloc(end - start + 1);
	strncpy(lex->lexeme, start, end - start);
	lex->lexeme[end - start] = '\0';
	lex->lexeme_len = end - start;
}

void lexer_destroy(Lexer* lex) {
	if(lex == NULL) return;

	free(lex->lexeme);
	free(lex);
}

// Just some macros to simplify getting a character. The first
//   will spit out an error if there are no tokens left and the
//   second will just say that it is at the EOF
#define GET_CHAR(lex, ls) \
	if(!__lexer_getch(lex, ls)) return TOKEN_ERROR;

#define GET_CHAR_EOF(lex, ls) \
	if(!__lexer_getch(lex, ls)) return TOKEN_EOF;


int lexer_next_token(Lexer* lex) {
	LexerState* ls = &lex->state;

	if(feof(lex->file) && (ls->pos - ls->buf) >= ls->buf_size) {
		lex->lexeme = NULL;
		lex->lexeme_id = TOKEN_EOF;
		return TOKEN_EOF;
	}

	// Skip whitespace before
	GET_CHAR(lex, ls)
	while(isspace(ls->c)) {
		GET_CHAR_EOF(lex, ls)
	}

	// Skip comments and whitespace after
	while(ls->c == ';') {
		bool still_comment = true;
		do {
			GET_CHAR_EOF(lex, ls)
			if(still_comment)
				still_comment = (ls->c != '\n');
		} while(still_comment || isspace(ls->c));
	}

	// LexerState->pos is always one more ahead of the current
	//   character, so  we have to subtract 1
	char* start = ls->pos - 1;


	//
	// HERE IS WHERE WE PUT OUR OWN CHARACTER SEQUENCE
	//   RECOGNIZERS
	//

	if(ls->c == ',') { // Comma
		LEX_UPDATE(lex, start, ls->pos, TOKEN_COMMA)
	}

	if(ls->c == '.') { // Directive
		// Skip whitespace
		do {
			GET_CHAR(lex, ls)
		} while(!isspace(ls->c));
		LEX_UPDATE(lex, start+1, ls->pos-1, TOKEN_DIRECTIVE)
	}

	if(ls->c == '"') { // .DATA directive, string data
		do {
			GET_CHAR(lex, ls)
		} while(ls->c != '"');
		LEX_UPDATE(lex, start+1, ls->pos-1, TOKEN_SDATA)
	}

	if(ls->c == '{') { // .DATA directive, word list data
		do {
			GET_CHAR(lex, ls)
		} while(ls->c != '}');
		LEX_UPDATE(lex, start+1, ls->pos-1, TOKEN_WDATA)
	}

	if(ls->c == '$') { // Register
		do {
			GET_CHAR(lex, ls)
		} while(!isspace(ls->c) && (ls->c != ','));
		// --ls->pos because we may have read a comma and this
		//  means that the next call to lexer_next_token would
		//  skip the comma, and then the parser would throw errors
		LEX_UPDATE(lex, start, --ls->pos, TOKEN_REGISTER)
	}

	// Label definition or Instruction/Label
	if(isalpha(ls->c) || ls->c == '_') {
		do {
			GET_CHAR(lex, ls)
		} while(isalnum(ls->c) || ls->c == '_');

		if(ls->c == ':') { // Label definition
			LEX_UPDATE(lex, start, ls->pos-1, TOKEN_LABELDEF)
		}
		else { // Instruction/Label
			LEX_UPDATE(lex, start, ls->pos-1, TOKEN_STRING)
		}
	}
	else { // Immediate data
		do {
			GET_CHAR(lex, ls)
		} while(isalnum(ls->c));
		// --ls->pos for the same reason as reading in a register
		LEX_UPDATE(lex, start, --ls->pos, TOKEN_IMMEDIATE)
	}

	return TOKEN_ERROR;
}
