INC=.
SRC=.
OBJ=.
OUT=.

CC=clang
CFLAGS=-I$(INC) -Wno-initializer-overrides

DEPS=$(INC)/sha256.h $(INC)/bitcoin_blocks.h $(INC)/parse_cl.h $(INC)/js0n.h $(INC)/types.h $(INC)/util.h
OBJS=$(OBJ)/sha256.o $(OBJ)/bitcoin_blocks.o $(OBJ)/parse_cl.o $(OBJ)/js0n.o $(OBJ)/main.o $(OBJ)/util.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(OUT)/bcfun: $(OBJS)
	$(CC) -o $(OUT)/$@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f *.o $(OUT)/bcfun
