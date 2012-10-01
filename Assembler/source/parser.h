#ifndef __PARSER_H__
#define __PARSER_H__

#include <stdio.h>
#include <stdbool.h>
#include "symtab.h"
#include "lexer.h"
#include "dataseg.h"
#include "../../include/types.h"


#define INSTR_DATA_MIN 128
#define ERROR_STR 128


typedef struct _Parser {
	FILE* fin;			// the input file
	INSTR_WORD* data;	// the actual assembled data
	int nData;			// the length of the data
	int nDataAlloc;		// the amount of data allocated
//	char* error_str;	// error message (if err occured)
	Lexer* lex;			// lexer used for this parser
	SymbolTable* st;	// symbol table used for this parser
	DataSegment* ds;	// static data segment
} Parser;


// Create a new parser
Parser* parser_create(FILE* fin);
// Destroy an existing parser
void parser_destroy(Parser* p);
// Start parsing on the given parser
bool parser_parse(Parser* p);
// Add a new instruction to the instruction data of the parser
void parser_add_instruction(Parser* p, INSTR_WORD instr);


#endif // __PARSER_H__
