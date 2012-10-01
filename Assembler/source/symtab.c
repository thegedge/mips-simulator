#include <stdlib.h>
#include <string.h>
#include "symtab.h"


SymbolTable* symtab_create() {
	SymbolTable* st = (SymbolTable*)malloc(sizeof(SymbolTable));
	if(st) {
		st->end = st->labels - 1;
		st->nLabels = st->nRes = 0;
		st->labels = (STLabel*)malloc(sizeof(STLabel) * SYMTAB_LABELS_MIN);
		st->nLabelsAlloc = SYMTAB_LABELS_MIN;
		st->res = (ResolveEntity*)malloc(sizeof(ResolveEntity) * SYMTAB_RES_MIN);
		st->nResAlloc = SYMTAB_RES_MIN;
	}
	return st;
}

void symtab_destroy(SymbolTable* st) {
	if(st == NULL) return;

	free(st->labels);
	free(st->res);
	free(st);
}

STLabel* symtab_add_label(SymbolTable* st, const char* name, int loc) {
	// Ensure enough memory is allocated
	if(st->nLabels == st->nLabelsAlloc) {
		st->nLabelsAlloc = st->nLabelsAlloc + SYMTAB_LABELS_MIN;
		st->labels = (STLabel*)realloc(st->labels, sizeof(STLabel) * st->nLabelsAlloc);
		if(st->labels == NULL) {
			st->nLabels = 0;
			st->nLabelsAlloc = 0;
			return NULL;
		}
	}

	STLabel* lbl = &st->labels[st->nLabels++];
	strncpy(lbl->name, name, LABEL_MAX_SIZE);
	lbl->location = loc;
	st->end = lbl;
	return lbl;
}

STLabel* symtab_get_label(SymbolTable* st, const char* name) {
	STLabel* lbl = st->labels;
	STLabel* end = st->end;
	// Loop through and try to find the label
	while(lbl <= end) {
		if(strncmp(lbl->name, name, LABEL_MAX_SIZE) == 0)
			return lbl;
		lbl++;
	}
	return NULL;
}

void symtab_add_unresolved(SymbolTable* st, int location, int instr, const char* lbl) {
	// Ensure enough memory is allocated
	if(st->nRes == st->nResAlloc) {
		st->nResAlloc += SYMTAB_RES_MIN;
		st->res = (ResolveEntity*)realloc(st->res, sizeof(ResolveEntity) * st->nResAlloc);
		if(st->res == NULL) {
			st->nRes = 0;
			st->nResAlloc = 0;
			return;
		}
	}
	ResolveEntity* re = &st->res[st->nRes++];
	strncpy(re->label, lbl, LABEL_MAX_SIZE);
	re->instr = instr;
	re->location = location;
}

bool symtab_resolve(SymbolTable* st, INSTR_WORD* data) {
	ResolveEntity* re = st->res;
	for(int i = 0; i < st->nRes; i++, re++) {
		STLabel* lbl = symtab_get_label(st, re->label);
		if(lbl == NULL) return false;

		switch(re->instr) {
		case INSTR_BZ:
			// If it is a BZ instruction, we need to calculate
			//   the offset of the label relative to the
			//   instruction location and put that in the
			//   immediate field of the instruction's word.
			//   The below calculation for loc actually gets
			//   the absolute address, but because we lshift by
			//   one in the simulation, I needed to rshift by 1
			//   here in the resolution. Sad, I know...
			{
				int loc = (lbl->location / sizeof(INSTR_WORD) - re->location) & 0x001F;
				data[re->location] |= loc;
				break;
			}
		case INSTR_LA:
			// A LA pseudo-instruction has to be split up
			//   into 7 instructions, 4 of which are used to
			//   reference the label's location. This takes care
			//   of the individual bytes needed for each.
			{
				int loc = lbl->location;
				data[re->location+1] |= ((loc >> 12) & 0x000F);
				data[re->location+3] |= ((loc >> 8) & 0x000F);
				data[re->location+5] |= ((loc >> 4) & 0x000F);
				data[re->location+7] |= (loc & 0x000F);
				break;
			}
		default:
			return false;
		}
	}

	return true;
}
