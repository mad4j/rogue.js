@ECHO OFF
SETLOCAL ENABLEDELAYEDEXPANSION

SET EMCC_PARAMS=-s ASYNCIFY=1 -s ALIASING_FUNCTION_POINTERS=0 -s EMULATE_FUNCTION_POINTER_CASTS=1 -s ASSERTIONS=2 --emrun
SET EMCC_PRELOAD=--use-preload-plugins --preload-file pdcfont.bmp --preload-file pdcicon.bmp
SET GCC_PARAMS=-O2

CLS

ECHO.
ECHO Building rogue.js

ECHO.
ECHO Removing folders

RD /Q /S out\
MD out\

RD /Q /S dist\
MD dist\

ECHO Copy BMP files from curses.js\
COPY curses.js\*.bmp .\

ECHO.
ECHO Building game using...

ECHO.
ECHO EMCC params...
ECHO %EMCC_PARAMS%
ECHO.
ECHO EMCC preload files...
ECHO %EMCC_PRELOAD%
ECHO.
ECHO GCC params...
ECHO %GCC_PARAMS%

ECHO.
ECHO Compiling source files...

SET "ROGUE_FILES="
FOR /R %%I IN (src\*.c) DO (
	ECHO Building %%~nI%%~xI
	CMD /C emcc %GCC_PARAMS% %ROGUE_FILES% src\%%~nI%%~xI -o out\%%~nI.bc -I src\ -I curses.js\
	SET "ROGUE_FILES=!ROGUE_FILES! out\%%~nI.bc"
)

ECHO.
ECHO Compiling binaries using...
ECHO %ROGUE_FILES%
CMD /C emcc %GCC_PARAMS% %ROGUE_FILES% -o out\rogue.bc -I rogue5.4.4\ -I curses.js\

ECHO.
ECHO Linking application...
CMD /C emcc %EMCC_PARAMS% %EMCC_PRELOAD% %GCC_PARAMS% curses.js\libcurses.o out\rogue.bc -o dist\rogue.html --shell-file template.html

ECHO.
ECHO FINISHED