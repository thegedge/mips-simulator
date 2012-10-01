#ifndef __REGFILE_H__
#define __REGFILE_H__

#include "../../include/types.h"


#define NUM_REGISTERS 32

struct _Register;

typedef DATA_WORD (*REG_GETTER)(struct _Register*);
typedef void (*REG_SETTER)(struct _Register*, DATA_WORD);

typedef struct _Register {
	DATA_WORD value;	// the value in a register
	REG_GETTER getter;	// function for getting its value
	REG_SETTER setter;	// function for setting its value
} Register;

typedef struct _RegFile {
	Register registers[NUM_REGISTERS];
} RegFile;


// Create a new register file object
RegFile* reg_create();
// Destroy a register file object
void reg_destroy(RegFile* rf);
// Set the setter function of a register in the regfile
void reg_set_setter(RegFile* rf, int regnum, REG_SETTER setter);
// Set the getter function of a register in the regfile
void reg_set_getter(RegFile* rf, int regnum, REG_GETTER getter);
// Set the value of a register in the regfile
void reg_set_value(RegFile* rf, int regnum, DATA_WORD val);
// Get the value of a register in the regfile
DATA_WORD reg_get_value(RegFile* rf, int regnum);


#endif // __REGFILE_H__
