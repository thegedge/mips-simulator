#include "vmachine.h"
#include "decode.h"
#include "prompt.h"

#include <stdlib.h>
#include <stdbool.h>


VirtualMachine* vmachine_create() {
	VirtualMachine* vm = (VirtualMachine*)malloc(sizeof(VirtualMachine));
	if(vm) {
		vm->rf = reg_create();
		vm->vmem = vmem_create();
		vm->PC = 0;
		vm->limit = 0;

		if(vm->rf == NULL || vm->vmem == NULL) {
			vmachine_destroy(vm);
			return NULL;
		}
	}
	return vm;
}

void vmachine_destroy(VirtualMachine* vm) {
	if(vm == NULL) return;

	free(vm->rf);
	free(vm->vmem);
	free(vm);
}

// Is there any instructions left to execute?
#define INSTRUCTIONS_LEFT(vm) (vm->PC != -1 && vm->PC < vm->limit)

void vmachine_run(VirtualMachine* vm, FILE* f) {
	int numDataBlocks, numInstructions, lastMalloc = -1, j = 0;
	bool machine_running = true;
	DATA_WORD* data;

	// First read in the data and isntructions
	fprintf(stderr, "Reading file...");
	fread(&numDataBlocks, sizeof(int), 1, f);
	fprintf(stderr, "%d data blocks found! Loading...\n", numDataBlocks);
	for(int i = 0; i < numDataBlocks; i++) {
		int start, size;
		fread(&start, sizeof(int), 1, f);
		fread(&size, sizeof(int), 1, f);

		if(size > 0) {
			fprintf(stderr, "\tBlock %d: start=%d, size=%d\n", j++, start, size);
			// This should produce less mallocs and improve
			//  efficiency (which is good!)
			if(size > lastMalloc) {
				data = (DATA_WORD*)malloc(sizeof(DATA_WORD) * size);
				lastMalloc = size;
			}
			fread(data, sizeof(DATA_WORD), size, f);
			vmem_set_words(vm->vmem, start, data, size);
		}
	}
	free(data);

	fread(&numInstructions, sizeof(int), 1, f);
	fprintf(stderr, "\tInstruction Block: size=%lu\n", numInstructions * sizeof(INSTR_WORD));

	// Load the instructions straight into memory
	fread(vm->vmem->addressable.mem, sizeof(INSTR_WORD), numInstructions, f);

	INSTR_WORD instr;
	vm->limit = sizeof(INSTR_WORD) * numInstructions;
	while(machine_running) {
		// Prompt the user for an action
		PromptResult pr = prompt_machine(vm);
		switch(pr.result) {
		case RUN_INSTRUCTION:
			// pr.data will be the number of instructions
			//  to step through
			while(INSTRUCTIONS_LEFT(vm) && pr.data-- > 0) {
				instr = vmem_get_word(vm->vmem, vm->PC);
				// to simplify things, execute_instruction
				//   will actually increment the PC
				execute_instruction(vm, instr);
			}

			if(!INSTRUCTIONS_LEFT(vm)) 
				fprintf(stderr, "The machine has run out of instructions!\n");
			else {
				instr = vmem_get_word(vm->vmem, vm->PC);
				print_instruction(vm, instr);
			}

			break;
		case SHUTDOWN_MACHINE:
			machine_running = false;
			break;
		case DO_NOTHING:
		default:
			break;
		}
	}
}
