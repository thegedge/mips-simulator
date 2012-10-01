#ifndef __SYMTAB_H__
#define __SYMTAB_H__

#include <stdbool.h>
#include "instructions.h"
#include "../../include/types.h"


#define SYMTAB_LABELS_MIN	10
#define SYMTAB_RES_MIN		20
#define LABEL_MAX_SIZE		8


typedef struct _STLabel {
	char name[LABEL_MAX_SIZE];	// the name of this label
	int location;				// the opcode offset
} STLabel;

typedef struct _ResolveEntity {
	int location;				// the location of this entity
	int instr;					// the associated instruction val
	char label[LABEL_MAX_SIZE]; // the label it references
} ResolveEntity;

typedef struct _SymbolTable {
	STLabel* labels;	// an array of labels
	STLabel* end;		// the last label
	int nLabels;		// the # of labels
	int nLabelsAlloc;	// the # of labels allocated

	ResolveEntity* res;	// unresolved jumps/branches/add. loads
	int nRes;			// number of used entities
	int nResAlloc;		// number of allocated entities
} SymbolTable;


// Create a new symbol table
SymbolTable* symtab_create();
// Destroy an existing symbol table
void symtab_destroy(SymbolTable* st);
// Add a label to the symbol table
STLabel* symtab_add_label(SymbolTable* st, const char* name, int loc);
// Search for a label; Returns NULL if not found
STLabel* symtab_get_label(SymbolTable* st, const char* name);
// Add an instruction that references a label
void symtab_add_unresolved(SymbolTable* st, int location, int instr, const char* lbl);
// Resolve all the instructions that reference labels
//  This will return false if an instruction references a 
//  label that doesn't exist in the symbol table.
bool symtab_resolve(SymbolTable* st, INSTR_WORD* data);

#endif // __SYMTAB_H__
