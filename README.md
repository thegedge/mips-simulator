# MIPS Assembler/Simulator

This is a project from one of my earlier undergrad courses (circa 2005). It's
kind of rough around the edges (I was just learning C at the time), but I
thought I'd share it with everyone. It's a simple assembler/virtual machine
for a subset of MIPS.

## Running

1. Run `make` to build the system
2. There are a set of example assembly files in the root directory (i.e.,
   test\*.txt). You can run `./Assembler/assembler test1.txt test1.bin`
   to assemble the file into the appropriate BIN file to feed to the
   simulator.
3. Run `./Simulator/simulator test1.bin` to start an interactive session

## Interactive Session

The interactive session has a set of commands one can use to execute the
input BIN file:

* `help` will print the set of commands available.
* `step` will step through a single instruction
* `mstep <n>` will step through _n_ instructions
* `pc` will print the location of the program counter
* `setpc <n>` will set the program counter to the specified offset
* `read m <address>` will read the value from the specified memory address
* `read r <num>` will read the value from the specified register
* `write m <address> <val>` will write the given  the value to the specified
   memory address
* `write r <num> <val>` will write the given value to the specified register
* `printinfo` will print information about the current instruction (i.e., the
  instruction specified at the address given by the program counter)
* `shutdown` will terminate the interactive session

## License

This project is MIT-licensed. See LICENSE.txt.
