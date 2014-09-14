@ECHO OFF

SETLOCAL ENABLEDELAYEDEXPANSION

CLS

RD /Q /S out\
MD out\

RD /Q /S lib\
MD lib\

RD /Q /S bin\
MD bin\

ECHO.
ECHO BUILDING pdcurses\
ECHO ------------------

SET "PDCURSES_BINARIES="
FOR /R %%I IN (pdcurses34\pdcurses\*.c) DO (
	ECHO Building %%~nI%%~xI
	CMD /C emcc -O2 pdcurses34\pdcurses\%%~nI%%~xI -o out\%%~nI.bc -I pdcurses34\ -I pdcurses34\pdcurses\
	SET "PDCURSES_BINARIES=!PDCURSES_BINARIES! out\%%~nI.bc"
)	

ECHO.
ECHO Building library using...
ECHO %PDCURSES_BINARIES%
CMD /C emcc -O2 %PDCURSES_BINARIES% -o lib\pdcurses.o

ECHO.
ECHO BUILDING sdl1\
ECHO ------------------
SET "SDL_BINARIES="
FOR /R %%I IN (pdcurses34\sdl1\*.c) DO (
	ECHO Building %%~nI%%~xI
	CMD /C emcc -O2 pdcurses34\sdl1\%%~nI%%~xI -o out\%%~nI.bc -I pdcurses34\ -I pdcurses34\pdcurses\ -I pdcurses34\sdl1\
	SET "SDL_BINARIES=!SDL_BINARIES! out\%%~nI.bc"
)

ECHO.
ECHO Building library using...
ECHO %SDL_BINARIES%
CMD /C emcc -O2 %SDL_BINARIES% -o lib\sdl1.o

ECHO.
ECHO BUILDING demos\
ECHO ------------------
FOR /R %%I IN (pdcurses34\demos\*.c) DO (
	ECHO Building %%~nI%%~xI
	CMD /C emcc -O2 pdcurses34\demos\%%~nI%%~xI -o out\%%~nI.bc -I pdcurses34\ -I pdcurses34\pdcurses\ -I pdcurses34\sdl1\
)	


ECHO.
ECHO Building SDLTEST demo
ECHO ------------------
CMD /C emcc -s ASYNCIFY=1 --emrun -O2 lib\pdcurses.o lib\sdl1.o out\sdltest.bc -o bin\sdltest.html --preload-file pdcfont.bmp --preload-file pdcicon.bmp

ECHO.
ECHO Building XMAS demo
ECHO ------------------
CMD /C emcc -s ASYNCIFY=1 --emrun -O2 lib\pdcurses.o lib\sdl1.o out\xmas.bc -o bin\xmas.html --preload-file pdcfont.bmp --preload-file pdcicon.bmp

ECHO.s
ECHO Building WORM demo
ECHO ------------------
CMD /C emcc -s ASYNCIFY=1 --emrun -O2 lib\pdcurses.o lib\sdl1.o out\worm.bc -o bin\worm.html --preload-file pdcfont.bmp --preload-file pdcicon.bmp

ECHO.
ECHO Building FIREWORK demo
ECHO ------------------
CMD /C emcc -s ASYNCIFY=1 --emrun -O2 lib\pdcurses.o lib\sdl1.o out\firework.bc -o bin\firework.html --preload-file pdcfont.bmp --preload-file pdcicon.bmp

ECHO.
ECHO Building PTEST demo
ECHO ------------------
CMD /C emcc -s ASYNCIFY=1 --emrun -O2 lib\pdcurses.o lib\sdl1.o out\ptest.bc -o bin\ptest.html --preload-file pdcfont.bmp --preload-file pdcicon.bmp

ECHO.
ECHO Building RAIN demo
ECHO ------------------
CMD /C emcc -s ASYNCIFY=1 --emrun -O2 lib\pdcurses.o lib\sdl1.o out\rain.bc -o bin\rain.html --preload-file pdcfont.bmp --preload-file pdcicon.bmp

echo FINISH
