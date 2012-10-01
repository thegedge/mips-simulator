#include "decode.h"
#include "regfile.h"

#include <stdbool.h>

//
// There is a lot of redundancy and mess in this
//   file only because of the output required for the
//   project. I basically rewrote all of the operations
//   in print_instruction. I would print out information
//   in each of the op functions below, but I felt that
//   this would be bad for large mstep values
//

// Get the opcode portion of the instruction
#define GET_OPCODE(op)	((op & 0xFC00) >> 10)
// Get the rA portion of the instruction
#define GET_REGA(op)	((op & 0x03E0) >> 5)
// Get the rB/immediate A portion of the instruction
#define GET_REGB(op)	(op & 0x001F)
#define GET_IMM(op)		GET_REGB(op)
// Get a value from reg[r]
#define RGET(vm, r) (reg_get_value(vm->rf, r))
// Get a value from mem[p]
#define MGET(vm, p) (vmem_get_word(vm->vmem, p))
// Because the immediate field is signed, have to convert to
//   a negative number if applicable (high order bit is 1)
#define TO_NEGATIVE(imm) \
	{	\
	bool negative = (((imm & 0x10) >> 4) == 1);					\
	imm = (negative?((-1^0x0F)|(imm & 0x0F)):(imm & 0x0F));	\
	}	

void __instr_add(VirtualMachine* vm, int op) {
	int rA = GET_REGA(op);
	int rB = GET_REGB(op);
	DATA_WORD val = RGET(vm, rA) + RGET(vm, rB);
	reg_set_value(vm->rf, rA, val);
}

void __instr_addu(VirtualMachine* vm, int op) {
	int rA = GET_REGA(op);
	int rB = GET_REGB(op);
	DATA_WORD val = (UDATA_WORD)RGET(vm, rA) +
					(UDATA_WORD)RGET(vm, rB);
	reg_set_value(vm->rf, rA, val);
}

void __instr_sub(VirtualMachine* vm, int op) {
	int rA = GET_REGA(op);
	int rB = GET_REGB(op);
	DATA_WORD val = RGET(vm, rA) - RGET(vm, rB);
	reg_set_value(vm->rf, rA, val);
}

void __instr_and(VirtualMachine* vm, int op) {
	int rA = GET_REGA(op);
	int rB = GET_REGB(op);
	DATA_WORD val = RGET(vm, rA) & RGET(vm, rB);
	reg_set_value(vm->rf, rA, val);
}

void __instr_or(VirtualMachine* vm, int op) {
	int rA = GET_REGA(op);
	int rB = GET_REGB(op);
	DATA_WORD val = RGET(vm, rA) | RGET(vm, rB);
	reg_set_value(vm->rf, rA, val);
}

void __instr_addi(VirtualMachine* vm, int op) {
	int rA = GET_REGA(op);
	int imm = GET_IMM(op);
	TO_NEGATIVE(imm);
	DATA_WORD val = RGET(vm, rA) + imm;
	reg_set_value(vm->rf, rA, val);
}

void __instr_load(VirtualMachine* vm, int op) {
	int rA = GET_REGA(op);
	int rB = GET_REGB(op);
	DATA_WORD val = MGET(vm, RGET(vm, rB));
	reg_set_value(vm->rf, rA, val);
}

void __instr_store(VirtualMachine* vm, int op) {
	int rA = GET_REGA(op);
	int rB = GET_REGB(op);
	DATA_WORD val = RGET(vm, rA);
	vmem_set_word(vm->vmem, RGET(vm, rB), val);
}

int __setbit(int val, int bit, int set) {
	int mask = 1 << (bit - 1);
	return ((set == 1) ? (val | mask) : (val & ~mask));
}

void __instr_setbit(VirtualMachine* vm, int op) {
	int rA = GET_REGA(op);
	int lower = GET_IMM(op);
	int set = ((lower & 0x10) >> 4);
	int val = __setbit(RGET(vm, rA), lower & 0x0F, set);
	
	reg_set_value(vm->rf, rA, val);
}

void __instr_bz(VirtualMachine* vm, int op) {
	int rA = GET_REGA(op);
	int imm = GET_IMM(op);
	TO_NEGATIVE(imm);
	if(RGET(vm, rA) == 0)
		vm->PC += (imm << 1);
	else
		vm->PC += 2;
}

void __instr_jal(VirtualMachine* vm, int op) {
	int rA = GET_REGA(op);
	reg_set_value(vm->rf, 31, vm->PC + 2);
	vm->PC = RGET(vm, rA);
}

void __instr_jr(VirtualMachine* vm, int op) {
	int rA = GET_REGA(op);
	vm->PC = RGET(vm, rA);
}

void __instr_lshift(VirtualMachine* vm, int op) {
	int rA = GET_REGA(op);
	int imm = GET_IMM(op);
	DATA_WORD val = RGET(vm, rA) << imm;
	reg_set_value(vm->rf, rA, val);
}

void __instr_rshift(VirtualMachine* vm, int op) {
	int rA = GET_REGA(op);
	int imm = GET_IMM(op);
	DATA_WORD val = RGET(vm, rA) >> imm;
	reg_set_value(vm->rf, rA, val);
}

void __instr_mov(VirtualMachine* vm, int op) {
	int rA = GET_REGA(op);
	int rB = GET_REGB(op);
	DATA_WORD val = RGET(vm, rB);
	reg_set_value(vm->rf, rA, val);
}

void print_instruction(VirtualMachine* vm, int instr) {
	int opcode = (instr & 0xFC00) >> 10;
	int rA = (instr & 0x03E0) >> 5;
	int rB = instr & 0x001F;
	int newVal = -1;

	switch(opcode) {
	case OP_ADD:
		newVal = RGET(vm, rA) + RGET(vm, rB);
		fprintf(stderr, "Add Instruction (@mem[%d])\n", vm->PC);
		break;
	case OP_ADDU:
		newVal = (UDATA_WORD)RGET(vm, rA) + (UDATA_WORD)RGET(vm, rB);
		fprintf(stderr, "Add (Unsigned) Instruction (@mem[%d])\n", vm->PC);
		break;
	case OP_SUB:
		newVal = RGET(vm, rA) - RGET(vm, rB);
		fprintf(stderr, "Subtract Instruction (@mem[%d])\n", vm->PC);
		break;
	case OP_AND:
		newVal = RGET(vm, rA) & RGET(vm, rB);
		fprintf(stderr, "And Instruction (@mem[%d])\n", vm->PC);
		break;
	case OP_OR:
		newVal = RGET(vm, rA) | RGET(vm, rB);
		fprintf(stderr, "Or Instruction (@mem[%d])\n", vm->PC);		
		break;
	case OP_LOAD:
		newVal = MGET(vm, RGET(vm, rB));
		fprintf(stderr, "Load Instruction (@mem[%d])\n", vm->PC);
		break;
	case OP_STORE:
		newVal = RGET(vm, rA);
		fprintf(stderr, "Add Instruction (@mem[%d])\n", vm->PC);
		break;
	case OP_SETBIT:
		newVal = __setbit(RGET(vm, rA), rB & 0x0F, (rB & 0x10) >> 4);
		fprintf(stderr, "Setbit Instruction (@mem[%d])\n", vm->PC);
		break;
	case OP_JAL:
		newVal = RGET(vm, rA);
		fprintf(stderr, "Jump and Link Instruction (@mem[%d])\n", vm->PC);
		break;
	case OP_JR:
		newVal = RGET(vm, rA);
		fprintf(stderr, "Jump Register Instruction (@mem[%d])\n", vm->PC);
		break;
	case OP_BZ:
		TO_NEGATIVE(rB);
		newVal = vm->PC + (rB << 1);
		fprintf(stderr, "Branch on Zero Instruction (@mem[%d])\n", vm->PC);	
		break;
	case OP_ADDI:
		newVal = RGET(vm, rA) + rB;
		fprintf(stderr, "Add Immediate Instruction (@mem[%d])\n", vm->PC);
		break;
	case OP_LSHIFT:
		newVal = RGET(vm, rA) << rB;
		fprintf(stderr, "Left Shift Instruction (@mem[%d])\n", vm->PC);
		break;
	case OP_RSHIFT:
		newVal = RGET(vm, rA) >> rB;
		fprintf(stderr, "Right Shift Instruction (@mem[%d])\n", vm->PC);
		break;
	case OP_MOV:
		newVal = RGET(vm, rB);
		fprintf(stderr, "Mov Instruction (@mem[%d])\n", vm->PC);
		break;
	default:
		fprintf(stderr, "Unknown Instruction: %x (@mem[%d])\n", opcode, vm->PC);
		return;
	}

	switch(opcode) {
	case OP_ADD:
	case OP_ADDU:
	case OP_SUB:
	case OP_AND:
	case OP_OR:
	case OP_ADDI:
	case OP_LSHIFT:
	case OP_RSHIFT:
	case OP_LOAD:
	case OP_SETBIT:
	case OP_MOV:
		fprintf(stderr, "\tWriting %d to reg[%d]\n", newVal, rA);
		break;
	case OP_STORE:
	{
		int loc = RGET(vm, rB);
		fprintf(stderr, "\tWriting %d to mem[%d]\n", newVal, loc);
		break;
	}
	case OP_JAL:
	case OP_JR:
		fprintf(stderr, "\tWriting %d to PC\n", newVal);
		break;
	case OP_BZ:
		fprintf(stderr, "\tWriting %d to PC (if reg[%d] is zero)\n", newVal, rA);
		break;
	}
}

void execute_instruction(VirtualMachine* vm, int instr) {
	int opcode = (instr & 0xFC00) >> 10;
	switch( opcode ) {
	case OP_ADD:
		__instr_add(vm, instr); break;
	case OP_ADDU:
		__instr_addu(vm, instr); break;
	case OP_SUB:
		__instr_sub(vm, instr); break;
	case OP_AND:
		__instr_and(vm, instr); break;
	case OP_OR:
		__instr_or(vm, instr); break;
	case OP_ADDI:
		__instr_addi(vm, instr); break;
	case OP_LOAD:
		__instr_load(vm, instr); break;
	case OP_STORE:
		__instr_store(vm, instr); break;
	case OP_SETBIT:
		__instr_setbit(vm, instr); break;
	case OP_BZ:
		__instr_bz(vm, instr); break;
	case OP_JAL:
		__instr_jal(vm, instr); break;
	case OP_JR:
		__instr_jr(vm, instr); break;
	case OP_LSHIFT:
		__instr_lshift(vm, instr); break;
	case OP_RSHIFT:
		__instr_rshift(vm, instr); break;
	case OP_MOV:
		__instr_mov(vm, instr); break;
	//default:
	//	vm->PC = -1;	// the VM will stop when this happens
	}

	if(opcode != OP_BZ && opcode != OP_JR && opcode != OP_JAL)
		vm->PC += 2;
}
