#
# Makefile for rogue
# %W% (Berkeley) %G%
#
# Rogue: Exploring the Dungeons of Doom
# Copyright (C) 1980, 1981 Michael Toy, Ken Arnold and Glenn Wichman
# All rights reserved.
#
# See the file LICENSE.TXT for full copyright and licensing information.
#

DISTNAME=rogue3.6.3
PROGRAM=rogue36

O=o

HDRS= 	rogue.h machdep.h

OBJS1 = vers.$(O) armor.$(O) chase.$(O) command.$(O) daemon.$(O) daemons.$(O) \
        fight.$(O) init.$(O) io.$(O) list.$(O) main.$(O) mdport.$(O) \
	misc.$(O) monsters.$(O) move.$(O) newlevel.$(O) options.$(O) 
OBJS2 =	pack.$(O) passages.$(O) potions.$(O) rings.$(O) rip.$(O) rooms.$(O) \
	save.$(O) scrolls.$(O) state.$(O) sticks.$(O) things.$(O) \
	weapons.$(O) wizard.$(O) xcrypt.$(O)
OBJS  = $(OBJS1) $(OBJS2)

CFILES= vers.c armor.c chase.c command.c daemon.c daemons.c fight.c \
	init.c io.c list.c main.c mdport.c misc.c monsters.c move.c newlevel.c \
	options.c pack.c passages.c potions.c rings.c rip.c rooms.c \
	save.c scrolls.c state.c sticks.c things.c weapons.c wizard.c xcrypt.c


MISC_C=
DOCSRC= rogue.6 rogue.r
DOCS  = $(PROGRAM).doc $(PROGRAM).cat $(PROGRAM).html readme36.html
MISC  =	Makefile $(MISC_C) LICENSE.TXT $(PROGRAM).sln $(PROGRAM).vcproj $(DOCS)\
	$(DOCSRC)

CC    = gcc
ROPTS =
COPTS = -O3
CFLAGS= $(COPTS) $(ROPTS)
LIBS  = -lcurses
RM    = rm -f
LD    = $(CC)
LDOUT = -o 

.SUFFIXES: .obj

.c.obj:
	$(CC) $(CFLAGS) /c $*.c

$(PROGRAM)$(EXE): $(HDRS) $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) $(LIBS) $(LDOUT)$@

clean:
	$(RM) $(OBJS1)
	$(RM) $(OBJS2)
	$(RM) core $(PROGRAM) $(PROGRAM).exe $(DISTNAME).tar $(DISTNAME).tar.gz 
	$(RM) $(DISTNAME).zip

dist.src:
	make clean
	tar cf $(DISTNAME)-src.tar $(CFILES) $(HDRS) $(MISC) $(DOCS)
	gzip -f $(DISTNAME)-src.tar

dist.irix:
	@$(MAKE) clean
	@$(MAKE) CC=cc CFLAGS="-woff 1116 -O3" $(PROGRAM)
#	tbl rogue.r | nroff -ms | colcrt - > $(PROGRAM).doc
#	nroff -man rogue.6 | colcrt - > $(PROGRAM).cat
	tar cf $(DISTNAME)-irix.tar $(PROGRAM) LICENSE.TXT $(DOCS)
	gzip -f $(DISTNAME)-irix.tar

dist.aix:
	@$(MAKE) clean
	@$(MAKE) CC=xlc CFLAGS="-qmaxmem=16768 -O3 -qstrict" $(PROGRAM)
#	tbl rogue.r | nroff -ms | colcrt - > $(ROGUE).doc
#	nroff -man rogue.6 | colcrt - > $(ROGUE).cat
	tar cf $(DISTNAME)-aix.tar $(PROGRAM) LICENSE.TXT $(DOCS)
	gzip -f $(DISTNAME)-aix.tar

dist.linux:
	@$(MAKE) clean
	@$(MAKE) $(PROGRAM)
#	groff -P-c -t -ms -Tascii rogue.r | sed -e 's/.\x08//g' > $(PROGRAM).doc
#	groff -man rogue.6 | sed -e 's/.\x08//g' > $(PROGRAM).cat
	tar cf $(DISTNAME)-linux.tar $(PROGRAM) LICENSE.TXT $(DOCS)
	gzip -f $(DISTNAME)-linux.tar
	
dist.interix: 
	@$(MAKE) clean
	@$(MAKE) COPTS="-ansi" $(PROGRAM)
#	groff -P-b -P-u -t -ms -Tascii rogue.r > $(PROGRAM).doc
#	groff -P-b -P-u -man -Tascii rogue.6 > $(PROGRAM).cat
	tar cf $(DISTNAME)-interix.tar $(PROGRAM) LICENSE.TXT $(DOCS)
	gzip -f $(DISTNAME)-interix.tar	

dist.cygwin:
	@$(MAKE) --no-print-directory clean
	@$(MAKE) COPTS="-I/usr/include/ncurses" --no-print-directory $(PROGRAM)
#	groff -P-c -t -ms -Tascii rogue.r | sed -e 's/.\x08//g' > $(PROGRAM).doc
#	groff -P-c -man -Tascii rogue.6 | sed -e 's/.\x08//g' > $(PROGRAM).cat
	tar cf $(DISTNAME)-cygwin.tar $(PROGRAM).exe LICENSE.TXT $(DOCS)
	gzip -f $(DISTNAME)-cygwin.tar

#
# Use MINGW32-MAKE to build this target
#
dist.mingw32:
	@$(MAKE) --no-print-directory RM="cmd /c del" clean
	@$(MAKE) --no-print-directory COPTS="-I../pdcurses" LIBS="../pdcurses/pdcurses.a" $(PROGRAM)
	cmd /c del $(DISTNAME)-mingw32.zip
	zip $(DISTNAME)-mingw32.zip $(PROGRAM).exe LICENSE.TXT $(DOCS)
	
#
# Seperate doc targets for DJGPP prevent strange SIGSEGV in groff
# in that environment.
#
doc.djgpp:
	groff -t -ms -Tascii rogue.r | sed -e 's/.\x08//g' > $(PROGRAM).doc

cat.djgpp:
	groff -man -Tascii rogue.6 | sed -e 's/.\x08//g' > $(PROGRAM).cat

dist.djgpp: 
	@$(MAKE) --no-print-directory clean
	@$(MAKE) --no-print-directory LDFLAGS="-L$(DJDIR)/LIB" \
		LIBS="-lpdcur" $(PROGRAM)
#	@$(MAKE) --no-print-directory doc.djgpp
#	@$(MAKE) --no-print-directory cat.djgpp
	rm -f $(DISTNAME)-djgpp.zip
	zip $(DISTNAME)-djgpp.zip $(PROGRAM) LICENSE.TXT $(DOCS)

#
# Use NMAKE to build this target
#
dist.win32:
	@$(MAKE) /NOLOGO O="obj" RM="-del" clean
	@$(MAKE) /NOLOGO O="obj" CC="@CL" LD="link" LDOUT="/OUT:" EXE=".exe"\
	    LIBS="/NODEFAULTLIB:LIBC ..\pdcurses\pdcurses.lib shell32.lib user32.lib Advapi32.lib" \
	    COPTS="-nologo -D_CRT_SECURE_NO_DEPRECATE -I..\pdcurses \
	    -Ox -wd4033 -wd4716" $(PROGRAM).exe
	-del $(DISTNAME)-win32.zip
	zip $(DISTNAME)-win32.zip $(PROGRAM).exe LICENSE.TXT $(DOCS)
