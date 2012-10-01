#include <stdlib.h>
#include <memory.h>
#include "vmem.h"


// Simplifies byte addressing
#define BYTE_TO_WORD(vm, p) *((DATA_WORD*)(vm->addressable.bytemem + p))


VirtualMem* vmem_create() {
	VirtualMem* vm = (VirtualMem*)malloc(sizeof(VirtualMem));
	if(vm) {
		vm->addressable.mem = (DATA_WORD*)malloc(VMEM_SIZE * sizeof(DATA_WORD));
		if(!vm->addressable.mem) {
			free(vm);
			return NULL;
		}
	}
	return vm;
}

void vmem_destroy(VirtualMem* vm) {
	if(!vm) return;
	free(vm->addressable.mem);
	free(vm);
}

void vmem_set_word(VirtualMem* vm, int position, DATA_WORD val) {
	if(position >= 0 && position < VMEM_SIZE - 1)
		BYTE_TO_WORD(vm, position) = val;
}

void vmem_set_words(VirtualMem* vm, int position, DATA_WORD* val, int count) {
	if(position >= 0 && position + count < VMEM_SIZE - 1)
		memcpy(&vm->addressable.bytemem[position], val, count * sizeof(DATA_WORD));
}

DATA_WORD vmem_get_word(VirtualMem* vm, int position) {
	if(position >= 0 && position < VMEM_SIZE - 1)
		return BYTE_TO_WORD(vm, position);
	else
		return 0;
}
