#
#
#

CC	= gcc
CFLAGS = -std=c99
SRC	= source/assembler.c source/dataseg.c source/instructions.c source/lexer.c source/parser.c source/symtab.c
OBJ = $(SRC:.c=.o)
OUT	= assembler

$(OUT): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(OUT)

.c.o: $(CC) $(CFGLAGS) $< -o $@

clean:
	-@rm -rf $(OUT) $(OBJ) 
