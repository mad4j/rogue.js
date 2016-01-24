
SRC = $(wildcard src/*.c)
OBJ = $(patsubst src/%.c,out/%.bc,$(SRC))

LIB = out/rogue.bc
EXE = dist/rogue.html

CC = emcc

GCC_PARAMS = -O2
EMCC_PARAMS = -s ASYNCIFY=1 -s ALIASING_FUNCTION_POINTERS=0 -s EMULATE_FUNCTION_POINTER_CASTS=1 -s ASSERTIONS=2 --emrun
EMCC_PRELOAD = --use-preload-plugins --preload-file curses.js/pdcfont.bmp@/ --preload-file curses.js/pdcicon.bmp@/
EMCC_TEMPLATE = --shell-file rogue-template.html

INCLUDES = -I src/ -I curses.js/
LIBS = curses.js/libcurses.o $(LIB)

all: $(EXE)
	@echo "removing intermediate files ..."
	@rm -fR out
	@echo "copying media files ..."
	@cp resources/fav* dist/

$(EXE): $(LIB)
	@mkdir -p dist/
	@echo "building $(EXE) ..."
	@$(CC) $(EMCC_PARAMS) $(EMCC_PRELOAD) $(GCC_PARAMS) $(LIBS) -o $(EXE) $(EMCC_TEMPLATE)

$(LIB): $(OBJ)
	@mkdir -p out/
	@echo "building $(LIB) ..."
	@$(CC) $(GCC_PARAMS) $(OBJ) -o $(LIB) $(INCLUDES)

out/%.bc: src/%.c
	@mkdir -p out/
	@echo "building $< ..."
	@$(CC) $(GCC_PARAMS) $< -o $@ $(INCLUDES)

clean:
	rm -fR dist/ out/
