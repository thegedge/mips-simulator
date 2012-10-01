#
#
#

CC	= gcc
CFLAGS = -std=c99 
SRC	= source/decode.c source/regfile.c source/simulator.c source/vmem.c source/vmachine.c source/prompt.c
OBJ = $(SRC:.c=.o)
OUT	= simulator

$(OUT): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(OUT)

.c.o: $(CC) $(CFGLAGS) $< -o $@

clean:
	-@rm -rf $(OBJ) $(OUT)
