#include <string.h>
#include "instructions.h"

Instruction instructions[] = {
	{"add",		INSTR_ADD,		OP_ADD},
	{"addu",	INSTR_ADDU,		OP_ADDU},
	{"sub",		INSTR_SUB,		OP_SUB},
	{"and",		INSTR_AND,		OP_AND},
	{"or",		INSTR_OR,		OP_OR},
	{"addi",	INSTR_ADDI,		OP_ADDI},
	{"load",	INSTR_LOAD,		OP_LOAD},
	{"store",	INSTR_STORE,	OP_STORE},
	{"setbit",	INSTR_SETBIT,	OP_SETBIT},
	{"bz",		INSTR_BZ,		OP_BZ},
	{"jal",		INSTR_JAL,		OP_JAL},
	{"jr",		INSTR_JR,		OP_JR},
	{"set",		INSTR_SET,		OP_SETBIT},
	{"clr",		INSTR_CLR,		OP_SETBIT},
	{"lshift",	INSTR_LSHIFT,	OP_LSHIFT},
	{"rshift",	INSTR_RSHIFT,	OP_RSHIFT},
	{"la",		INSTR_LA,		0},
	{"mov",		INSTR_MOV,		OP_MOV},
	{NULL,		0,				0}
};

Instruction* get_instruction(const char* str) {
	Instruction* t = instructions;
	while(t->name != NULL) {
		if(strcmp(t->name, str) == 0)
			return t;
		t++;
	}
	return NULL;
}
