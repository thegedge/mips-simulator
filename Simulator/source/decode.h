#ifndef __DECODE_H__
#define __DECODE_H__

#include "vmachine.h"
#include "../../include/types.h"


// Print information about an instruction
void print_instruction(VirtualMachine* vm, int instr);
// Execute an instruction
void execute_instruction(VirtualMachine* vm, int instr);


#endif // __DECODE_H__
