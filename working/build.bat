@ECHO OFF

SETLOCAL ENABLEDELAYEDEXPANSION

CLS

ECHO Removing folders
RD /Q /S out\
MD out\

RD /Q /S lib\
MD lib\

RD /Q /S bin\
MD bin\

ECHO Copy BMP files in bin\
COPY *.bmp bin\

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
ECHO BUILDING sdl1\
ECHO ---------------
FOR /R %%I IN (pdcurses34\sdl1\*.c) DO (
	ECHO Building %%~nI%%~xI
	CMD /C emcc -O2 pdcurses34\sdl1\%%~nI%%~xI -o out\%%~nI.bc -I pdcurses34\ -I pdcurses34\pdcurses\ -I pdcurses34\sdl1\
	SET "PDCURSES_BINARIES=!PDCURSES_BINARIES! out\%%~nI.bc"
)

ECHO.
ECHO Building library using...
ECHO %PDCURSES_BINARIES%
CMD /C emcc -O2 %PDCURSES_BINARIES% -o lib\pdcurses.o

ECHO.
ECHO BUILDING demos\
ECHO ---------------
FOR /R %%I IN (pdcurses34\demos\*.c) DO (
	ECHO Building %%~nI%%~xI
	CMD /C emcc -O2 pdcurses34\demos\%%~nI%%~xI -o out\%%~nI.bc -I pdcurses34\ -I pdcurses34\pdcurses\ -I pdcurses34\sdl1\ -I pdcurses34\demos\
	CD bin/
	CMD /C emcc -s ASYNCIFY=1 --emrun -O2 ..\lib\pdcurses.o ..\out\%%~nI.bc -o %%~nI.html --preload-file pdcfont.bmp --preload-file pdcicon.bmp
	CD ..
)	

ECHO.
ECHO Building rogue36
ECHO ----------------

SET "ROGUE_BINARIES="
FOR /R %%I IN (rogue36\*.c) DO (
	ECHO Building %%~nI%%~xI
	CMD /C emcc -O2 rogue36\%%~nI%%~xI -o out\%%~nI.bc -I rogue36\ -I pdcurses34\
	SET "ROGUE_BINARIES=!ROGUE_BINARIES! out\%%~nI.bc"
)	


ECHO.
ECHO Building tbclock
ECHO ----------------

SET "TBCLOCK_BINARIES="
FOR /R %%I IN (tbclock\*.c) DO (
	ECHO Building %%~nI%%~xI
	CMD /C emcc -O2 tbclock\%%~nI%%~xI -o out\%%~nI.bc -I tbclock\ -I pdcurses34\
	SET "TBCLOCK_BINARIES=!TBCLOCK_BINARIES! out\%%~nI.bc"
)	

CMD /C emcc -s ASYNCIFY=1 --emrun -O2 lib\pdcurses.o %TBCLOCK_BINARIES% -o bin\tbclock.html --preload-file pdcfont.bmp --preload-file pdcicon.bmp

ECHO.
ECHO Building zsnake
ECHO ---------------

CMD /C emcc -s ASYNCIFY=1 --emrun -O2 lib\pdcurses.o zsnake\main.c zsnake\soviet.c -I zsnake\ -I pdcurses34\ -o bin\zsnake.html --preload-file pdcfont.bmp --preload-file pdcicon.bmp

ECHO FINISHED