#ifndef __VMEM_H__
#define __VMEM_H__


#include "../../include/types.h"


#define VMEM_SIZE (2 << 16)

typedef struct _VirtualMem {
	union {
		DATA_WORD* mem;		// word addressed
		char* bytemem;		// byte addressed
	} addressable;
} VirtualMem;


// Create a new piece of virtual memory
VirtualMem* vmem_create();
// Destroy an existing virtual memory
void vmem_destroy(VirtualMem* vm);
// Set the word at a position in memory (byte addressed)
void vmem_set_word(VirtualMem* vm, int position, DATA_WORD val);
// Set a sequence of words at a position in memory (byte addressed)
void vmem_set_words(VirtualMem* vm, int position, DATA_WORD* val, int count);
// Get the word at a position in memory (byte addressed)
DATA_WORD vmem_get_word(VirtualMem* vm, int position);


#endif // __VMEM_H__
