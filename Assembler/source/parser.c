#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "instructions.h"


// Get a token from the stream and check to make
//  sure it is of the required type
#define TOKEN_GET_AND_CHECK(p, lex, req_token)		\
	{												\
		int token = lexer_next_token(lex);			\
		if(token != req_token) {					\
			/* p->error_str = 1; set outside of here? */	\
			return false;							\
		}											\
	}												

// Get a token from the stream and check to make
//  sure it is one of the two required types
#define TOKEN_GET_AND_CHECK2(p, lex, rt1, rt2)		\
	{												\
		int token = lexer_next_token(lex);			\
		if(token != rt1 && token != rt2) {			\
			/* p->error_str = 1; set outside of here? */	\
			return false;							\
		}											\
	}

// This will parse an instruction and add its parsed
//   data into the parses instruction data array
bool __parse_instruction(Parser*, Instruction*);
// Will parse a register from a string (i.e. r1 returns 1)
int __parse_register(char*);
// Will parse immediate data from a string (integer or hex)
int __parse_immediate(char*);


Parser* parser_create(FILE* fin) {
	if(fin == NULL) return NULL;

	Parser* p = (Parser*)malloc(sizeof(Parser));
	if(p) {
		p->lex = lexer_create(fin);
		p->st = symtab_create();
		p->ds = ds_create();
		p->data = NULL;
		p->nData = p->nDataAlloc = 0;
		//p->error_str = NULL;
		p->fin = fin;

		if(p->lex == NULL || p->ds == NULL || p->st == NULL) {
			parser_destroy(p);
			return NULL;
		}
	}
	return p;
}

void parser_destroy(Parser* p) {
	if(p == NULL) return;

	if(p->data != NULL) free(p->data);
	lexer_destroy(p->lex);
	symtab_destroy(p->st);
	ds_destroy(p->ds);
	free(p);
}

bool parser_parse(Parser* p) {
	if(p == NULL) return false;

	Lexer* lex = p->lex;
	SymbolTable* st = p->st;

	// While there is still available tokens
	while(lexer_next_token(lex) != TOKEN_EOF) {
		char* t = lex->lexeme;
		switch(lex->lexeme_id) {
		case TOKEN_STRING: // should be instruction at this level
		{
			Instruction* instr = get_instruction(lex->lexeme);
			if(instr == NULL) { // unknown instruction
				// error string
				return false;
			}
			if(!__parse_instruction(p, instr)) { // bad parse
				// error string
				return false;
			}
			break;
		}
		case TOKEN_DIRECTIVE:
			if(strcmp(lex->lexeme, "ORIGIN") == 0) {
				// We have a .ORIGIN directive so we should add
				//   a new data block to the data segment
				TOKEN_GET_AND_CHECK(p, lex, TOKEN_IMMEDIATE);
                int start = __parse_immediate(lex->lexeme);
				ds_add_block(p->ds, start);
			}
			else if(strcmp(lex->lexeme, "DATA") == 0) {
				// We have a .DATA directive so we need to get
				//   the label and the data
				TOKEN_GET_AND_CHECK(p, lex, TOKEN_STRING);
				char label[LABEL_MAX_SIZE];
				strncpy(label, lex->lexeme, LABEL_MAX_SIZE);
				TOKEN_GET_AND_CHECK2(p, lex, TOKEN_SDATA, TOKEN_WDATA);

				DATA_WORD* data;
				int size = 0;
				switch(lex->lexeme_id) {
				case TOKEN_SDATA:
				{
					size = (strlen(lex->lexeme) * sizeof(char)) / sizeof(DATA_WORD) + 1;
					data = (DATA_WORD*)strdup(lex->lexeme);
					break;
				}
				case TOKEN_WDATA:
				{
					size = 1;
					int num, i = 0;
					char* t = lex->lexeme;
					while(*t) { if(*t++ == ',') size++; }

					data = (DATA_WORD*)malloc(size * sizeof(DATA_WORD));
					if(size > 0) {
						char* start = lex->lexeme;
						char* tend = NULL;
						char* end = start + lex->lexeme_len;
						while(start <= end) {
							num = strtol(start, &tend, 0);
							data[i++] = (num & 0xFFFF);
							start = tend + 1;
						}
					}
					break;
				}
				}
				int loc = ds_add_data(p->ds, data, size);
				symtab_add_label(p->st, label, loc);
			}
			break;
		case TOKEN_LABELDEF:
			// A label definition, so just add to symbol table
			if(lex->lexeme_len > 8) return false;
			symtab_add_label(st, lex->lexeme, p->nData * sizeof(DATA_WORD));
			break;

		case TOKEN_ERROR:
			return false;
		}
	}

	// Everything was parsed, final step is to resolve any
	//   unresolved label references
	return symtab_resolve(st, p->data);
}

void parser_add_instruction(Parser* p, INSTR_WORD instr) {
	// Ensure enough allocated memory
	if(p->nData == p->nDataAlloc) {
		p->nDataAlloc += INSTR_DATA_MIN;
		p->data = (INSTR_WORD*)realloc(p->data, sizeof(INSTR_WORD) * p->nDataAlloc);
	}
	p->data[p->nData++] = instr;
}

// Compile an R-Type instruction
#define RTYPE(op,ra,rb) ((op << 10)|((ra & 0x1F) << 5)|(rb & 0x1F))
// Compile an I-Type instruction
#define ITYPE(op,ra,imm) ((op << 10)|((ra & 0x1F) << 5)|(imm & 0x1F))
// Compile the setbit instruction
#define SBTYPE(op,ra,s,i) ((op << 10)|((ra & 0x1F) << 5)|((s & 0x1) << 4)|(i & 0xF))

int __parse_register(char* reg) {
	int t = atoi(reg + 1);
	return (t & 0x001F);
}

int __parse_immediate(char* imm) {
	int t = strtol(imm, NULL, 0);
	return (t & 0xFFFF);
}

#define OPCODE(instr) (instructions[instr].opcode)

bool __parse_instruction(Parser* p, Instruction* instr) {
	Lexer* lex = p->lex;
	int oc = instr->opcode;

	switch(instr->id) {
		case INSTR_ADD:
		case INSTR_ADDU:
		case INSTR_SUB:
		case INSTR_AND:
		case INSTR_OR:
		case INSTR_LOAD:
		case INSTR_STORE:
		case INSTR_MOV:
		{
			int rA, rB;
			TOKEN_GET_AND_CHECK(p, lex, TOKEN_REGISTER)
			rA = __parse_register(lex->lexeme);
			TOKEN_GET_AND_CHECK(p, lex, TOKEN_COMMA)
			TOKEN_GET_AND_CHECK(p, lex, TOKEN_REGISTER)
			rB = __parse_register(lex->lexeme);
			parser_add_instruction(p, RTYPE(oc, rA, rB));
			break;
		}
		case INSTR_ADDI:
		case INSTR_LSHIFT:
		case INSTR_RSHIFT:
		{
			int rA, imm;
			TOKEN_GET_AND_CHECK(p, lex, TOKEN_REGISTER)
			rA = __parse_register(lex->lexeme);
			TOKEN_GET_AND_CHECK(p, lex, TOKEN_COMMA)
			TOKEN_GET_AND_CHECK(p, lex, TOKEN_IMMEDIATE)
			imm = __parse_immediate(lex->lexeme);
			parser_add_instruction(p, ITYPE(oc, rA, imm));
			break;
		}
		case INSTR_SETBIT:
		{
			int rA, set, bit;
			TOKEN_GET_AND_CHECK(p, lex, TOKEN_REGISTER)
			rA = __parse_register(lex->lexeme);
			TOKEN_GET_AND_CHECK(p, lex, TOKEN_COMMA)
			TOKEN_GET_AND_CHECK(p, lex, TOKEN_IMMEDIATE)
			set = __parse_immediate(lex->lexeme);
			TOKEN_GET_AND_CHECK(p, lex, TOKEN_COMMA)
			TOKEN_GET_AND_CHECK(p, lex, TOKEN_IMMEDIATE)
			bit = __parse_immediate(lex->lexeme);
			parser_add_instruction(p, SBTYPE(oc, rA, set, bit));
			break;
		}
		case INSTR_BZ:
		{
			int rA, loc = 0;
			TOKEN_GET_AND_CHECK(p, lex, TOKEN_REGISTER)
			rA = __parse_register(lex->lexeme);
			TOKEN_GET_AND_CHECK(p, lex, TOKEN_COMMA)
			TOKEN_GET_AND_CHECK2(p, lex, TOKEN_IMMEDIATE, TOKEN_STRING)
			if(lex->lexeme_id == TOKEN_IMMEDIATE)
				loc = __parse_immediate(lex->lexeme);
			else {
				if(lex->lexeme_len > 8) return false;
				symtab_add_unresolved(p->st, p->nData, INSTR_BZ, lex->lexeme);
			}
			parser_add_instruction(p, ITYPE(oc, rA, loc));
			break;
		}
		case INSTR_JAL:
		case INSTR_JR:
		{
			int rA;
			TOKEN_GET_AND_CHECK(p, lex, TOKEN_REGISTER)
			rA = __parse_register(lex->lexeme);
			parser_add_instruction(p, RTYPE(oc, rA, 0));
			break;
		}
		case INSTR_SET:
		case INSTR_CLR:
		{
			int rA, set, bit;
			TOKEN_GET_AND_CHECK(p, lex, TOKEN_REGISTER)
			rA = __parse_register(lex->lexeme);
			TOKEN_GET_AND_CHECK(p, lex, TOKEN_COMMA)
			TOKEN_GET_AND_CHECK(p, lex, TOKEN_IMMEDIATE)
			bit = __parse_immediate(lex->lexeme);
			set = (instr->id == INSTR_SET ? 1 : 0);
			parser_add_instruction(p, SBTYPE(oc, rA, set, bit));
			break;
		}
		case INSTR_LA:
		{
			int rA;
			int oc_mov = OPCODE(INSTR_MOV);
			int oc_addi = OPCODE(INSTR_ADDI);
			int oc_lshift = OPCODE(INSTR_LSHIFT);

			TOKEN_GET_AND_CHECK(p, lex, TOKEN_REGISTER)
			rA = __parse_register(lex->lexeme);
			TOKEN_GET_AND_CHECK(p, lex, TOKEN_COMMA)
			TOKEN_GET_AND_CHECK(p, lex, TOKEN_STRING)

			if(lex->lexeme_len > 8) return false;
			symtab_add_unresolved(p->st, p->nData, INSTR_LA, lex->lexeme);
			
			parser_add_instruction(p, RTYPE(oc_mov, rA, 0));
			parser_add_instruction(p, ITYPE(oc_addi, rA, 0));
			parser_add_instruction(p, ITYPE(oc_lshift, rA, 4));
			parser_add_instruction(p, ITYPE(oc_addi, rA, 0));
			parser_add_instruction(p, ITYPE(oc_lshift, rA, 4));
			parser_add_instruction(p, ITYPE(oc_addi, rA, 0));
			parser_add_instruction(p, ITYPE(oc_lshift, rA, 4));
			parser_add_instruction(p, ITYPE(oc_addi, rA, 0));
			break;
		}
		default:
			break;
	}

	return true;
}
