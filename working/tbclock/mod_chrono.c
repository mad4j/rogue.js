/* $Id: mod_chrono.c,v 1.8 2007-02-28 12:47:35 tamentis Exp $
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
 *
 */

#include <sys/ioctl.h>
#include <sys/time.h>

#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <curses.h>
#include <stdlib.h>
#include <time.h>

#include "tbclock.h"

extern struct tbclock_data tbc;

/* returns the number of tenth of seconds since start of program (bigbang) */
unsigned long
chrono_nds()
{
	struct timeval tp;
	struct timezone tzp;
	unsigned long dsec = 0;

	gettimeofday(&tp, &tzp);

	dsec = (tp.tv_sec - tbc.bigbang) * 10 + (tp.tv_usec / 100000);

	return (dsec);
}


/* mod_chrono */
void
mod_chrono()
{
	unsigned long start, elapsed, paused = 0;
	unsigned int hour, min, sec, dsec;
	int pause = 0;
	signed char c;

	tbc.format.res_y = 4;

	if (tbc.options.vertical)
		tbc.format.res_x = 7;
	else
		tbc.format.res_x = 6;

	tbc_configure();

	start = chrono_nds();

	for (;;) {
		c = getch();

		if (c == KB_SPACE) {
			if (pause == 0) {
				pause = 1;
				paused = chrono_nds();
			} else {
				pause = 0;
				wbkgdset(tbc.screen, COLOR_PAIR(TEXT_DEFAULT));
				mvwprintw(tbc.screen, 2, tbc.format.width / 2 - 6, 
						"             ");
				start = start + chrono_nds() - paused;
			}
		} else if (c == KB_BACKSPACE) {
			if (pause) {
				tbc_clear();
				refresh();
				paused = start;
			} else {
				start = chrono_nds();
			}
		} else if (c == KB_H) {
			tbc_next_help_value();
			tbc_configure();
		} else if (c == KB_A) {
			if (tbc.options.vertical) {
				tbc.options.vertical = 0;
				tbc.format.res_x = 6;
			} else {
				tbc.options.vertical = 1;
				tbc.format.res_x = 7;
			}
			tbc_configure();
		} else if (c > 0)
			return;

		if (!pause) {
			elapsed = chrono_nds() - start;

			hour = elapsed / 36000;
			min  = (elapsed % 36000) / 600;
			sec  = (elapsed % 36000) % 600 / 10;
			dsec = (elapsed % 36000) % 600 % 10;

			tbc_draw_time(4, hour, min, sec, dsec);
		} else {
			wbkgdset(tbc.screen, COLOR_PAIR(BACK_YELLOW));
			mvwprintw(tbc.screen, 2, tbc.format.width / 2 - 6,
					"--- PAUSE ---");
		}

		refresh();

		usleep(10000);
	}
}


