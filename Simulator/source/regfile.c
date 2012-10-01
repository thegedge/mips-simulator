#include <stdlib.h>
#include "regfile.h"


void __reg_basic_setter(Register* r, DATA_WORD val);
DATA_WORD __reg_basic_getter(Register* r);


RegFile* reg_create() {
	RegFile* rf = (RegFile*)malloc(sizeof(RegFile));
	if(rf) {
		// initialize all registers with 0 and basic set/get
		for(int i = 0; i < NUM_REGISTERS; i++) {
			rf->registers[i].value = 0;
			rf->registers[i].getter = __reg_basic_getter;
			rf->registers[i].setter = __reg_basic_setter;
		}
	}
	return rf;
}

void reg_destroy(RegFile* rf) {
	if(rf == NULL) return;
	free(rf);
}

void reg_set_setter(RegFile* rf, int regnum, REG_SETTER setter) {
	if(regnum >= 0 && regnum < NUM_REGISTERS)
		rf->registers[regnum].setter = setter;
}

void reg_set_getter(RegFile* rf, int regnum, REG_GETTER getter) {
	if(regnum >= 0 && regnum < NUM_REGISTERS)
		rf->registers[regnum].getter = getter;
}

void reg_set_value(RegFile* rf, int regnum, DATA_WORD val) {
	if(regnum < 0 || regnum >= NUM_REGISTERS) return;

	Register* r = &rf->registers[regnum];
	if(r->setter)
		(*r->setter)(r, val);
}

DATA_WORD reg_get_value(RegFile* rf, int regnum) {
	if(regnum < 0 || regnum >= NUM_REGISTERS) return 0;

	Register* r = &rf->registers[regnum];
	if(r->getter)
		return (*r->getter)(r);
	return 0;
}

void __reg_basic_setter(Register* r, DATA_WORD val) {
	r->value = val;
}

DATA_WORD __reg_basic_getter(Register* r) {
	return r->value;
}
