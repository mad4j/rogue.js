/* $Id: mod_clock.c,v 1.7 2007-02-28 12:47:35 tamentis Exp $
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

#include <unistd.h>
#include <curses.h>
#include <time.h>

#include "tbclock.h"

extern struct tbclock_data tbc;

//external definition
void emscripten_sleep(int ms);

/* mod_clock */
void
mod_clock()
{
	time_t now;
	struct tm *tm;
	signed char c;

	/* base configuration */
	tbc.format.res_x = 6;
	if (tbc.options.vertical)
		tbc.format.res_y = 4;
	else
		tbc.format.res_y = 3;
	tbc_configure();

	for (;;) {
		if ((c = getch()) > 0) {
			switch (c) {
				case KB_H:
					tbc_next_help_value();
					tbc_configure();
					break;
				case KB_A:
					if (tbc.options.vertical) {
						tbc.options.vertical = 0;
						tbc.format.res_y = 3;
					} else {
						tbc.options.vertical = 1;
						tbc.format.res_y = 4;
					}
					tbc_configure();
					break;
				default:
					return;
			}
		}


		now = time(NULL);
		tm = localtime(&now);

		tbc_draw_time(3, tm->tm_hour, tm->tm_min, tm->tm_sec, 0);

		refresh();
		//usleep(10000);
		emscripten_sleep(500);
	}
}

