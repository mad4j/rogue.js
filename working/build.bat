@ECHO OFF
SETLOCAL ENABLEDELAYEDEXPANSION

SET EMCC_PARAMS=-s ASYNCIFY=1 --emrun
SET EMCC_PRELOAD=--preload-file pdcfont.bmp --preload-file pdcicon.bmp
SET GCC_PARAMS=-O2 -Wno-parentheses -Wno-implicit-int -Wno-return-type -Wno-implicit-function-declaration -Wno-format

CLS

ECHO.
ECHO Building rogue.js

ECHO.
ECHO Removing folders

RD /Q /S out\
MD out\

RD /Q /S dist\
MD dist\

ECHO Copy BMP files in dist\
COPY *.bmp dist\

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
FOR /R %%I IN (rogue36\*.c) DO (
	ECHO Building %%~nI%%~xI
	CMD /C emcc %GCC_PARAMS% %ROGUE_FILES% rogue36\%%~nI%%~xI -o out\%%~nI.bc -I rogue36\ -I curses.js\
	SET "ROGUE_FILES=!ROGUE_FILES! out\%%~nI.bc"
)

ECHO.
ECHO Compiling binaries using...
ECHO %ROGUE_FILES%
CMD /C emcc %GCC_PARAMS% %ROGUE_FILES% -o out\rogue.bc -I rogue36\ -I curses.js\

ECHO.
ECHO Linking application...
CMD /C emcc %EMCC_PARAMS% %EMCC_PRELOAD% %GCC_PARAMS% curses.js\libcurses.o out\rogue.bc -o dist\rogue.html

ECHO.
ECHO FINISHED