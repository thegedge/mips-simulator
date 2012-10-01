#ifndef __TYPES_H__
#define __TYPES_H__


// Basica data types used by the assembler/simulator
typedef short int DATA_WORD;
typedef unsigned short int UDATA_WORD;
typedef short int INSTR_WORD;
//#define DATA_WORD short int
//#define INSTR_WORD short int

// The opcodes of different instructions (ascending order)
#define OP_ADD		0x00
#define OP_ADDU		0x01
#define OP_SUB		0x02
#define OP_JR		0x03
#define OP_AND		0x04
#define OP_OR		0x05
#define OP_ADDI		0x08
#define OP_LOAD		0x0A
#define OP_STORE	0x0B
#define OP_SETBIT	0x0C
#define OP_LSHIFT	0x0D
#define OP_RSHIFT	0x0E
#define OP_BZ		0x0F
#define OP_MOV		0x10
#define OP_JAL		0x20

// The number of possible opcodes (1 << 6)
#define MAX_OP		0x40	// ops can be 6 bits

#ifdef NULL
#	undef NULL
#endif
#define NULL  0


#endif // __TYPES_H__
