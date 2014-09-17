@ECHO OFF

SETLOCAL ENABLEDELAYEDEXPANSION

CLS

ECHO Removing folders
RD /Q /S rogue-out\
MD rogue-out\

CMD /C emcc -O2 rogue36\armor.c -o rogue-out\armor.bc -I rogue36\ -I pdcurses34\
CMD /C emcc -O2 rogue36\chase.c -o rogue-out\chase.bc -I rogue36\ -I pdcurses34\

PAUSE

SET "ROGUE_BINARIES="
FOR /R %%I IN (rogue36\*.c) DO (
	ECHO Building %%~nI%%~xI
	CMD /C emcc -O2 rogue36\%%~nI%%~xI -o out\%%~nI.bc -I rogue36\ -I pdcurses34\
	SET "ROGUE_BINARIES=!ROGUE_BINARIES! out\%%~nI.bc"
)	