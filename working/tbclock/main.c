/* $Id: main.c,v 1.9 2007-02-28 12:47:35 tamentis Exp $
 *
 * Copyright (c) 2007 Bertrand Janin <tamentis@neopulsar.org>
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */


#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <err.h>
#include <curses.h>

#include "tbclock.h"


struct tbclock_data tbc;


/* tbc_next_help_value - rotate through different reading helps */
void
tbc_next_help_value()
{
	unsigned int tm, lm;

	tm = tbc.format.top_margin + tbc.format.dot_h / 2;
	lm = tbc.format.width - tbc.format.left_margin / 2 - 1;

	if (tbc.options.helper > 2)
		tbc.options.helper = 0;
	else    
		tbc.options.helper++;

}


/* tbc_set_default - set the starting vars, default options */
void
tbc_set_default()
{
	tbc.bigbang = time(NULL);

	tbc.format.width = 80;
	tbc.format.height = 24;

	tbc.options.frame = tbc.options.frame_default = 1;
	tbc.options.border = tbc.options.border_default = 1;
	tbc.options.dots = 1;
	tbc.options.vertical = 0;
	tbc.options.helper = 0;
	tbc.options.ampm = 0;

	tbc.colors.hour = COLOR_BLUE;
	tbc.colors.minute = COLOR_RED;
	tbc.colors.second = COLOR_YELLOW;
	tbc.colors.tenth = COLOR_GREEN;
}



/* tbc_configure - setup layout and all those things, could be called from
 * anywhere, when term is resized, at the beginning of modules... it
 * deals with weither something is tolerated or not depending of
 * a size. */
void
tbc_configure()
{

	/* terminal is too small, removing frame & border */
	if (tbc.format.height < 10 || tbc.format.width < 19)
		tbc.options.frame = tbc.options.border = 0;
	else {
		tbc.options.frame = tbc.options.frame_default;
		tbc.options.border = tbc.options.border_default;
	}

	/* term not wide enough for horizontal helper */
	if (!tbc.options.vertical && tbc.format.width < 8 
			&& tbc.options.helper & 1)
		tbc.options.helper --;

	/* term is not tall enough for bottom help in vertical mode */
	if (tbc.options.vertical && tbc.format.height < 5 
			&& tbc.options.helper > 0)
		tbc.options.helper = 0;

	/* term is not tall enough for bottom help */
	if (tbc.format.height < 4 && tbc.options.helper > 1)
		tbc.options.helper -= 2;

	/* height of a dot (term height - frame - spaces - helper) */
	tbc.format.dot_h = 
	    ( tbc.format.height
		- tbc.options.frame * 2
		- (tbc.format.res_y - 1) * tbc.options.border
		- (!tbc.options.vertical && tbc.options.helper > 1 ? 1 : 0)
		- (tbc.options.vertical && tbc.options.helper ? 1 : 0)
	    ) / tbc.format.res_y;

	/* width of a dot */
	tbc.format.dot_w = 
	    ( tbc.format.width
		- tbc.options.frame * 2
		- (tbc.format.res_x - 1) * tbc.options.border
		- (!tbc.options.vertical && tbc.options.helper & 1 ? 2 : 0)
	    ) / tbc.format.res_x;

	/* top margin */
	tbc.format.top_margin = tbc.options.frame +
	    ( tbc.format.height
		- tbc.options.frame * 2 
		- tbc.format.res_y * tbc.format.dot_h
		- (tbc.format.res_y - 1) * tbc.options.border
		- (!tbc.options.vertical && tbc.options.helper & 1 ? 2 : 0)
	    ) / 2;

	/* left margin */
	tbc.format.left_margin = tbc.options.frame + 
	    ( tbc.format.width
		- tbc.options.frame * 2
		- tbc.format.res_x * tbc.format.dot_w
		- (tbc.format.res_x - 1) * tbc.options.border
		- (!tbc.options.vertical && tbc.options.helper & 1 ? 2 : 0)
	    ) / 2;

	tbc_clear();

}


/* this doesn't do anything, I was thinking about removing it. */
#define SET_COLOR(x) i = atoi(optarg); if (i >= 0 && i < 8) x = i;
int
main(int ac, char **av)
{
	int ch, i;
	char modulename[9] = "clock";

	tbc_set_default();

	while ((ch = getopt(ac, av, "abdefg:hvm:pH:M:S:T:")) != -1) {
		switch (ch) {
		case 'a':
			tbc.options.vertical = 1;
			break;
		case 'b':
			tbc.options.border = tbc.options.border_default = 0;
			break;
		case 'd':
			tbc.options.dots = 0;
			break;
		case 'e':
			tbc.options.helper++;
			break;
		case 'f':
			tbc.options.frame = tbc.options.frame_default = 0;
			break;
		case 'g':
		case 'm':
			strncpy(modulename, optarg, 9);
			modulename[8] = 0;
			break;
		case 'p':
			tbc.options.ampm = 1;
			break;
		case 'H':
			SET_COLOR(tbc.colors.hour);
			break;
		case 'M':
			SET_COLOR(tbc.colors.minute);
			break;
		case 'S':
			SET_COLOR(tbc.colors.second);
			break;
		case 'T':
			SET_COLOR(tbc.colors.tenth);
			break;
		case 'v':
			printf(TBCCOPY);
			exit(-1);
		case 'h':
		default:
			printf(USAGE_FMT, av[0]);
			exit(-1);
		}
	}
	ac -= optind;
	av += optind;

	tbc_display_init();

	/* module selection */
	if (strncmp(modulename, "guessbin", 9) == 0) {
		tbc.options.frame = 1;
		tbc.options.border = 1;
		mod_guessbin();
	} else if (strncmp(modulename, "chrono", 7) == 0) {
		mod_chrono();
	} else {
		mod_clock();
	}

	endwin();
	printf("Thank you for using tbclock!\n");

	return (0);
}

