/* Public Domain Curses */

#include "pdcsdl.h"

RCSID("$Id: pdcutil.c,v 1.6 2008/07/14 04:24:52 wmcbrine Exp $")

#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif

void PDC_beep(void)
{
    PDC_LOG(("PDC_beep() - called\n"));
}

void PDC_napms(int ms)
{
    PDC_LOG(("PDC_napms() - called: ms=%d\n", ms));

    PDC_update_rects();

#ifdef EMSCRIPTEN
    emscripten_sleep(ms);
#else
	SDL_Delay(ms);
#endif
}

const char *PDC_sysname(void)
{
    return "SDL";
}
