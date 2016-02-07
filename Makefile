
SRC_FOLDER = src
OUT_FOLDER = out
RES_FOLDER = resources
CUR_FOLDER = curses.js
DIST_FOLDER = dist

SRC = $(wildcard $(SRC_FOLDER)/*.c)

VER=$(shell cat version.txt)
BUILD=$(shell git log --oneline | wc -l)

LIB = $(OUT_FOLDER)/roguejs.$(VER)-$(BUILD).bc
EXE = $(DIST_FOLDER)/roguejs.$(VER)-$(BUILD).html

CC = emcc

GCC_PARAMS = 
EMCC_PARAMS = --emrun -s ASYNCIFY=1 -s ALIASING_FUNCTION_POINTERS=0 -s EMULATE_FUNCTION_POINTER_CASTS=1 -s ASSERTIONS=2
EMCC_PRELOAD = --use-preload-plugins --preload-file $(CUR_FOLDER)/pdcfont.bmp@/ --preload-file $(CUR_FOLDER)/pdcicon.bmp@/
EMCC_TEMPLATE = --shell-file rogue-template.html

INCLUDES = -I $(SRC_FOLDER)/ -I $(CUR_FOLDER)/
LIBS = $(CUR_FOLDER)/libcurses.o $(LIB)

all: GCC_PARAMS += -O3 -Oz
all: dist

debug: GCC_PARAMS += -g4 -s SAFE_HEAP=1
debug: dist

dist: show $(EXE)
	@echo "removing intermediate files ..."
	@rm -fR $(OUT_FOLDER)/
	@echo "copying media files ..."
	@cp $(RES_FOLDER)/fav* $(DIST_FOLDER)/

$(EXE): $(LIB)
	@mkdir -p $(DIST_FOLDER)/
	@echo "building $(EXE) ..."
	@$(CC) $(EMCC_PARAMS) $(EMCC_PRELOAD) $(GCC_PARAMS) $(LIBS) -o $(EXE) $(EMCC_TEMPLATE)

$(LIB): $(SRC)
	@mkdir -p $(OUT_FOLDER)/
	@echo "building $(LIB) ..."
	@$(CC) $(GCC_PARAMS) $(EMCC_PRELOAD) $(SRC) -o $(LIB) $(INCLUDES)

show:
	@echo 
	@echo "Bulding Rogue.JS"
	@echo "----------------"
	@echo "using GCC params: $(GCC_PARAMS)"
	@echo "using EMCC params: $(EMCC_PARAMS)"
	@echo "using EMCC Preload params: $(EMCC_PRELOAD)"
	@echo "using EMCC Template params: $(EMCC_TEMPLATE)"
	@echo

run:
	@emrun $(EXE)

clean:
	@echo "cleaning output folders..."
	@rm -fR $(DIST_FOLDER)/ $(OUT_FOLDER)/
