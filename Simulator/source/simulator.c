#include <stdio.h>
#include "vmachine.h"


int main(int argc, char** argv)
{
	FILE* fin = NULL;

	if(argc <= 1 || (fin = fopen(argv[1], "rb")) == NULL) {
		fprintf(stderr, "Usage: simulator [inputfile]\n");
		return 0;
	}

	fprintf(stderr, "Starting Machine Simulation...\n");
	VirtualMachine* vm = vmachine_create();
	if(vm) {
		fprintf(stderr, "Machine Created...\n");
		// Because registers start with a value of 0, if we
		//  elimiinate the ability to write to the zero register
		//  then it should always return 0
		reg_set_setter(vm->rf, 0, NULL);

		fprintf(stderr, "Zero register configured!\n");
		fprintf(stderr, "Running program code...\n");
		vmachine_run(vm, fin);
		fprintf(stderr, "Destroying machine...\n");
		vmachine_destroy(vm);
	}
	else {
		fprintf(stderr, "Unable to create virtual machine!\n");
	}
	fprintf(stderr, "Ending Machine Simulation...\n");

	fclose(fin);
}
