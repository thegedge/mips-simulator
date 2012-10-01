#ifndef __VMACHINE_H__
#define __VMACHINE_H__

#include <stdio.h>
#include "vmem.h"
#include "regfile.h"
#include "../../include/types.h"


typedef struct _VirtualMachine {
	RegFile* rf;		// the register file
	VirtualMem* vmem;	// the virtual memory
	int PC;				// the program counter
	int limit;			// the end of instructions
} VirtualMachine;


// Create a new virtual machine object
VirtualMachine* vmachine_create();
// Destroy a virtual machine object
void vmachine_destroy(VirtualMachine* vm);
// Run the instructions contained in an assembled file
void vmachine_run(VirtualMachine* vm, FILE* f);


#endif // __VMACHINE_H__
