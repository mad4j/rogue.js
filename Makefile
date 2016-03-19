
SRC_FOLDER    = src
OUT_FOLDER    = out
RES_FOLDER    = resources
CUR_FOLDER    = curses.js
DIST_FOLDER   = dist

SRC           += $(wildcard $(SRC_FOLDER)/*.c)
SRC-CURSES    += $(wildcard $(CUR_FOLDER)/pdcurses34/pdcurses/*.c)
SRC-CURSES    += $(wildcard $(CUR_FOLDER)/pdcurses34/sdl1/*.c)

VER           = $(shell cat version.txt)
BUILD         = $(shell git log --oneline | wc -l)

LIB           = $(OUT_FOLDER)/libroguejs.bc
LIB-CURSES    = $(OUT_FOLDER)/libcursesjs.bc

EXE           = $(DIST_FOLDER)/roguejs.$(VER)-$(BUILD).html

CC            = emcc

GCC_PARAMS    = 

EMCC_PARAMS   += --emrun 
EMCC_PARAMS   += -s EMTERPRETIFY=1 
EMCC_PARAMS   += -s EMTERPRETIFY_ASYNC=1
EMCC_PARAMS   += -s EMULATE_FUNCTION_POINTER_CASTS=1 
EMCC_PARAMS   += -s EMULATED_FUNCTION_POINTERS=1 
EMCC_PARAMS   += -s ASSERTIONS=2

EMCC_PRELOAD  += --use-preload-plugins 
EMCC_PRELOAD  += --preload-file $(CUR_FOLDER)/pdcfont.bmp@/ 
EMCC_PRELOAD  += --preload-file $(CUR_FOLDER)/pdcicon.bmp@/

EMCC_TEMPLATE += --shell-file rogue-template.html

INCLUDES      += -I $(SRC_FOLDER)/ 
INCLUDES      += -I $(CUR_FOLDER)/pdcurses34


all: GCC_PARAMS += -O3 -Oz
all: dist

debug: GCC_PARAMS += -g4 -s SAFE_HEAP=1
debug: dist

dist: show $(EXE)
	@echo "removing intermediate files ..."
	@rm -fR $(OUT_FOLDER)/
	@echo "copying media files ..."
	@cp $(RES_FOLDER)/fav* $(DIST_FOLDER)/
	@echo "...COMPLETED"
	@echo -en "\007"

$(EXE): $(LIB-CURSES) $(LIB)
	@mkdir -p $(DIST_FOLDER)/
	@echo "building $(EXE) ..."
	@$(CC) $(EMCC_PARAMS) $(EMCC_PRELOAD) $(GCC_PARAMS) $(LIB) $(LIB-CURSES) -o $(EXE) $(EMCC_TEMPLATE)

$(LIB): $(SRC)
	@mkdir -p $(OUT_FOLDER)/
	@echo "building $(LIB) ..."
	@$(CC) $(GCC_PARAMS) $(EMCC_PRELOAD) $(SRC) -o $(LIB) $(INCLUDES)

$(LIB-CURSES): $(SRC-CURSES)
	@mkdir -p $(OUT_FOLDER)/
	@echo "building $(LIB-CURSES) ..."
	@$(CC) $(GCC_PARAMS) $(EMCC_PRELOAD) $(SRC-CURSES) -o $(LIB-CURSES) $(INCLUDES)

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
	@echo "...COMPLETED"
