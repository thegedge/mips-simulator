#ifndef __INSTRUCTIONS_H__
#define __INSTRUCTIONS_H__

#include "../../include/types.h"


#define INSTR_ADD		 0
#define INSTR_ADDU		 1
#define INSTR_SUB		 2
#define INSTR_AND		 3
#define INSTR_OR		 4
#define INSTR_ADDI		 5
#define INSTR_LOAD		 6
#define INSTR_STORE		 7
#define INSTR_SETBIT	 8
#define INSTR_BZ		 9
#define INSTR_JAL		10
#define INSTR_JR		11
#define INSTR_SET		12
#define INSTR_CLR		13
#define INSTR_LSHIFT	14
#define INSTR_RSHIFT	15
#define INSTR_LA		16
#define INSTR_MOV		17


typedef struct _Instruction {
	char* name;		// the associated text
	int id;			// the INSTR_* above (also used as index)
	int opcode;		// the actual opcode
} Instruction;

// Return information about an instruction or NULL
//  if the instruction is unknown
Instruction* get_instruction(const char* str);

// A table of valid instructions
extern Instruction instructions[];


#endif // __INSTRUCTIONS_H__
