
SRC = $(wildcard src/*.c)

LIB = out/rogue.bc
EXE = dist/rogue.html

CC = emcc

GCC_PARAMS = -O2
EMCC_PARAMS = -s ASYNCIFY=1 -s ALIASING_FUNCTION_POINTERS=0 -s EMULATE_FUNCTION_POINTER_CASTS=1 -s ASSERTIONS=2 --emrun
EMCC_PRELOAD = --use-preload-plugins --preload-file curses.js/pdcfont.bmp@/ --preload-file curses.js/pdcicon.bmp@/
EMCC_TEMPLATE = --shell-file rogue-template.html

INCLUDES = -I src/ -I curses.js/
LIBS = curses.js/libcurses.o $(LIB)


all: show $(EXE)
	@echo "removing intermediate files ..."
	@rm -fR out
	@echo "copying media files ..."
	@cp resources/fav* dist/

$(EXE): $(LIB)
	@mkdir -p dist/
	@echo "building $(EXE) ..."
	@$(CC) $(EMCC_PARAMS) $(EMCC_PRELOAD) $(GCC_PARAMS) $(LIBS) -o $(EXE) $(EMCC_TEMPLATE)

$(LIB): $(SRC)
	@mkdir -p out/
	@echo "building $(LIB) ..."
	@$(CC) $(GCC_PARAMS) $(SRC) -o $(LIB) $(INCLUDES)

show:
	@echo 
	@echo "Bulding Rogue.JS"
	@echo "----------------"
	@echo "using GCC params: $(GCC_PARAMS)"
	@echo "using EMCC params: $(EMCC_PARAMS)"
	@echo "using EMCC Preload params: $(EMCC_PRELOAD)"
	@echo "using EMCC Template params: $(EMCC_TEMPLATE)"
	@echo

clean:
	@echo "cleaning output folders..."
	@rm -fR dist/ out/
