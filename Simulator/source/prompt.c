#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "prompt.h"
#include "decode.h"


// Convert a string to lowercase
void str_tolower(char* str) {
	char* c = str;
	while(*c) { *c = tolower(*c); c++; }
}

PromptResult prompt_machine(VirtualMachine* vm) {
	PromptResult pr = {RUN_INSTRUCTION, 1};

	char buffer[128]; buffer[127] = '\0';

	fprintf(stderr, ">> ");		// print a prompt
	fgets(buffer, 127, stdin);	// get the line
	
	// This will allow case insensitive comparisons
	str_tolower(buffer);
	if(strncmp(buffer, "read", 4) == 0) {
		int pos, val;
		char what[2];
		char spos[7]; spos[6] = '\0';

		pr.result = DO_NOTHING;
		if(sscanf(buffer + 4, "%1s %s", what, spos) == 2) {
			str_tolower(spos);
			if(strncmp(spos, "PC", 2) == 0)
				pos = vm->PC;
			else
				pos = strtoul(spos, NULL, 0);

			switch(what[0]) {
			case 'r':
			case 'R':
				val = reg_get_value(vm->rf, pos);
				fprintf(stderr, "reg[%d] = %d\n", pos, val);
				break;
			case 'm':
			case 'M':
				val = vmem_get_word(vm->vmem, pos);
				fprintf(stderr, "mem[%d] = %d\n", pos, val);
				break;
			default:
				fprintf(stderr, "Error in command!\n");
			}
		}
		else
			fprintf(stderr, "Error in command!\n");
	}
	else if(strncmp(buffer, "write", 5) == 0) {
		int pos, val;
		char what[2];
		char spos[7]; spos[6] = '\0';
		char sval[7]; sval[6] = '\0';

		pr.result = DO_NOTHING;
		if(sscanf(buffer + 5, "%1s %s %s", what, spos, sval) == 3) {
			pos = strtoul(spos, NULL, 0);
			val = strtoul(sval, NULL, 0);
			switch(what[0]) {
			case 'r':
			case 'R':
				reg_set_value(vm->rf, pos, val);
				break;
			case 'm':
			case 'M':
				vmem_set_word(vm->vmem, pos, val);
				break;
			default:
				fprintf(stderr, "Error in command!\n");
			}
		}
		else
			fprintf(stderr, "Error in command!\n");
	}
	else if(strncmp(buffer, "pc", 2) == 0) {
		pr.result = DO_NOTHING;
		fprintf(stderr, "PC = %d\n", vm->PC);
	}
	else if(strncmp(buffer, "setpc", 5) == 0) {
		char sval[7]; sval[6] = '\0';

		pr.result = DO_NOTHING;
		if(sscanf(buffer + 5, "%s", sval) == 1)
			vm->PC = strtoul(sval, NULL, 0);
	}
	else if(strncmp(buffer, "step", 4) == 0) {
		pr.result = RUN_INSTRUCTION;
		pr.data = 1;
	}
	else if(strncmp(buffer, "mstep", 5) == 0) {
		int numSteps;
		pr.result = RUN_INSTRUCTION;
		if(sscanf(buffer + 5, "%d", &numSteps) == 1)
			pr.data = numSteps;
		else
			fprintf(stderr, "Error in command!\n");
	}
	else if(strncmp(buffer, "shutdown", 8) == 0) {
		pr.result = SHUTDOWN_MACHINE;
	}
	else if(strncmp(buffer, "printinfo", 9) == 0) {
		int instr = vmem_get_word(vm->vmem, vm->PC);
		print_instruction(vm, instr);
		pr.result = DO_NOTHING;
	}
	else if(strncmp(buffer, "help", 4) == 0) {
		fprintf(stderr, "The following commands are supported:\n");
		fprintf(stderr, "\tread\t\twrite\t\tpc\n");
		fprintf(stderr, "\tsetpc\t\tstep\t\tmstep\n");
		fprintf(stderr, "\tshutdown\tprintinfo\thelp\n");
		pr.result = DO_NOTHING;
	}
	else {
		int len = -1;
		char cmd[128]; cmd[127] = '\0';

		pr.result = DO_NOTHING;
		sscanf(buffer, "%s%n", cmd, &len);
		if(cmd[0] != '\n') {
			if(len <= 0 || len >= 127)
				cmd[0] = '\0';
			fprintf(stderr, "Unknown command: '%s'\n", cmd);
		}
	}

	fflush(stdin);
	return pr;
}
