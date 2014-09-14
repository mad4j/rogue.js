@ECHO OFF

SETLOCAL ENABLEDELAYEDEXPANSION

CLS

RD /Q /S bin\
MD bin\

DEL *.html
DEL *.js
DEL *.mem

ECHO.
ECHO BUILDING pdcurses\
ECHO ------------------

SET "PDCURSES_BINARIES="
FOR /R %%I IN (pdcurses\*.c) DO (
	ECHO Building %%~nI%%~xI
	CMD /C emcc -O2 pdcurses\%%~nI%%~xI -o bin\%%~nI.bc -I .\ -I pdcurses\
	SET "PDCURSES_BINARIES=!PDCURSES_BINARIES! bin\%%~nI.bc"
)	

ECHO.
ECHO Building library using...
ECHO %PDCURSES_BINARIES%
CMD /C emcc -O2 %PDCURSES_BINARIES% -o bin\pdcurses.o

ECHO.
ECHO BUILDING sdl1\
ECHO ------------------
SET "SDL_BINARIES="
FOR /R %%I IN (sdl1\*.c) DO (
	ECHO Building %%~nI%%~xI
	CMD /C emcc -O2 sdl1\%%~nI%%~xI -o bin\%%~nI.bc -I .\ -I pdcurses\ -I sdl1\
	SET "SDL_BINARIES=!SDL_BINARIES! bin\%%~nI.bc"
)

ECHO.
ECHO Building library using...
ECHO %SDL_BINARIES%
CMD /C emcc -O2 %SDL_BINARIES% -o bin\sdl1.o

ECHO.
ECHO BUILDING demos\
ECHO ------------------
FOR /R %%I IN (demos\*.c) DO (
	ECHO Building %%~nI%%~xI
	CMD /C emcc -O2 demos\%%~nI%%~xI -o bin\%%~nI.bc -I .\ -I pdcurses\ -I sdl1\ -I demos\
)	


ECHO.
ECHO Building SDLTEST demo
ECHO ------------------
CMD /C emcc -s ASYNCIFY=1 --emrun -O2 bin\pdcurses.o bin\sdl1.o bin/sdltest.bc -o sdltest.html --preload-file pdcfont.bmp --preload-file pdcicon.bmp

ECHO.
ECHO Building XMAS demo
ECHO ------------------
CMD /C emcc -s ASYNCIFY=1 --emrun -O2 bin\pdcurses.o bin\sdl1.o bin\xmas.bc -o xmas.html --preload-file pdcfont.bmp --preload-file pdcicon.bmp

ECHO.
ECHO Building WORM demo
ECHO ------------------
CMD /C emcc -s ASYNCIFY=1 --emrun -O2 bin\pdcurses.o bin\sdl1.o bin\worm.bc -o worm.html --preload-file pdcfont.bmp --preload-file pdcicon.bmp

ECHO.
ECHO Building FIREWORK demo
ECHO ------------------
CMD /C emcc -s ASYNCIFY=1 --emrun -O2 bin\pdcurses.o bin\sdl1.o bin\firework.bc -o firework.html --preload-file pdcfont.bmp --preload-file pdcicon.bmp

ECHO.
ECHO Building PTEST demo
ECHO ------------------
CMD /C emcc -s ASYNCIFY=1 --emrun -O2 bin\pdcurses.o bin\sdl1.o bin\ptest.bc -o ptest.html --preload-file pdcfont.bmp --preload-file pdcicon.bmp


echo FINISH
