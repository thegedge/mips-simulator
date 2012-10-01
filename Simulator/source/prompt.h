#ifndef __PROMPT_H__
#define __PROMPT_H__

#include "../../include/types.h"
#include "vmachine.h"

enum PROMPT_RESULT {
	DO_NOTHING = 0,
	RUN_INSTRUCTION = 1,
	SHUTDOWN_MACHINE = 2
};

typedef struct _PR {
	enum PROMPT_RESULT result; // the result
	int data;                  // other result information
} PromptResult;

// Will set up a basic prompt for the machine
PromptResult prompt_machine(VirtualMachine* vm);


#endif // __PROMPT_H__
