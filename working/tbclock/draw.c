/* $Id: draw.c,v 1.4 2007-02-28 12:47:35 tamentis Exp $
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


#include <sys/ioctl.h>
#include <termios.h>

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <err.h>
#include <curses.h>

#include "tbclock.h"


extern struct tbclock_data tbc;


/* tbc_draw_helpers - draw the currently selected reading help */
void
tbc_draw_helpers(int res, int hour, int min, int sec, int dsec)
{
	int space;
	char st[12];

	/* display bottom (full) helper */
	if (tbc.format.height > 3 && tbc.options.helper > 1) {
		unsigned i;
		
		/* do we need to show tenth of sec ? */
		if (res > 3)
			i = snprintf(st, 12, "%02u:%02u:%02u:%02d",
					hour, min, sec, dsec);
		else
			i = snprintf(st, 9, "%02u:%02u:%02u", hour, min, sec);

		bkgdset(COLOR_PAIR(TEXT_DEFAULT));
		mvprintw(tbc.format.height - 1 - tbc.options.frame, 
				tbc.format.width / 2 - (i / 2), st);
	}

	/* side helper if opt_helper == 1 or 3 and not vertical mode */
	if (!tbc.options.vertical && tbc.options.helper & 1) {
		unsigned int tm, lm;

		tm = tbc.format.top_margin + tbc.format.dot_h / 2;
		lm = tbc.format.width - 2 - tbc.options.frame;
		space = tbc.format.dot_h + tbc.options.border;

		bkgdset(COLOR_PAIR(TEXT_DEFAULT));
		snprintf(st, 3, "%02u", hour);
		mvprintw(tm, lm, st);
		snprintf(st, 3, "%02u", min);
		mvprintw(tm + space, lm, st);
		snprintf(st, 3, "%02u", sec);
		mvprintw(tm + space * 2, lm, st);
		if (res > 3) {
			snprintf(st, 3, "%02u", dsec);
			mvprintw(tm + space * 3, lm, st);
		}
	}

	/* bottom helper if opt_helper == 3 or 1 in vertical mode */
	if (tbc.options.vertical && tbc.options.helper == 1) {
		unsigned int tm, lm;

		tm = tbc.format.height - tbc.options.frame - 1;
		lm = tbc.format.left_margin + ((tbc.format.dot_w - 1) * 2
				     + tbc.options.border) / 2;
		space = (tbc.options.border + tbc.format.dot_w) * 2;

		wbkgdset(tbc.screen, COLOR_PAIR(TEXT_DEFAULT));
		snprintf(st, 3, "%02u", hour);
		mvwprintw(tbc.screen, tm, lm, st);
		snprintf(st, 3, "%02u", min);
		mvwprintw(tbc.screen, tm, lm + space, st);
		snprintf(st, 3, "%02u", sec);
		mvwprintw(tbc.screen, tm, lm + space * 2, st);
		if (res > 3) {
			snprintf(st, 3, "%02u", dsec);
			mvwprintw(tbc.screen, tm, lm + space * 3, st);
		}
	}
}


/* tbc_draw_dot - prints one dot if valid */
void
tbc_draw_dot(int valid, int x, int y, short color)
{
	int i;
	char *s, c = '#';

	/* display alternative character for empty 'dots' */
	if (!valid) {
		color = BLOCK_DEFAULT;
		if (tbc.options.dots)
			c = '.';
		else
			c = ' ';
	}

	/* generate a NULL-terminated line of this character */
	s = malloc(tbc.format.dot_w + 1);
	memset(s, c, tbc.format.dot_w);
	s[tbc.format.dot_w] = 0;

	/* print this line a few times */
	bkgdset(COLOR_PAIR(color));
	for (i = 0; i < tbc.format.dot_h; i++) {
		mvprintw(y + i, x, s);
	}

	free(s);
}


/* tbc_draw_line - prints 6 dots with the same color */
void
tbc_draw_line(int hms, int y, short color)
{
	int k;
	unsigned int x;

	for (k = 0; k < 6; k++) {
		x = tbc.format.left_margin 
			+ (tbc.format.dot_w + tbc.options.border) * (5 - k);
		tbc_draw_dot(hms & (1 << k), x, y + tbc.format.top_margin,
				color);
	}
}


/* tbc_draw_line_a - prints a vertical line (-a) */
void
tbc_draw_line_a(int hms, int x, short color, short max)
{
	int k;
	unsigned int y;

	for (k = 0; k < 4; k++) {
		y = tbc.format.top_margin 
			+ (tbc.format.dot_h + tbc.options.border) * (3 - k);
		if (k < max)
 			tbc_draw_dot( hms & (1<<k), x, y, color);
	}
}


/* tbc_draw_time - draw the time, with or without tenth of seconds (res) */
void
tbc_draw_time(int res, int hour, int min, int sec, int dsec)
{
	int space;
	unsigned int ml = tbc.format.left_margin;
	
	if (tbc.options.ampm && hour > 12)
		hour -= 12;

	if (tbc.options.vertical) {
		space = tbc.options.border + tbc.format.dot_w;
		tbc_draw_line_a(hour / 10, ml,             BLOCK_HOUR,   2);
		tbc_draw_line_a(hour % 10, ml + space,     BLOCK_HOUR,   4);
		tbc_draw_line_a(min / 10,  ml + space * 2, BLOCK_MINUTE, 3);
		tbc_draw_line_a(min % 10,  ml + space * 3, BLOCK_MINUTE, 4);
		tbc_draw_line_a(sec / 10,  ml + space * 4, BLOCK_SECOND, 3);
		tbc_draw_line_a(sec % 10,  ml + space * 5, BLOCK_SECOND, 4);
		if (res > 3)
			tbc_draw_line_a(dsec, ml + space * 6, BLOCK_TENTH, 4);
	} else {
		space = tbc.format.dot_h + tbc.options.border;
		tbc_draw_line(hour, 0, BLOCK_HOUR);
		tbc_draw_line(min, space, BLOCK_MINUTE);
		tbc_draw_line(sec, space * 2, BLOCK_SECOND);
		if (res > 3)
			tbc_draw_line(dsec, space * 3, BLOCK_TENTH);
	}

	tbc_draw_helpers(res, hour, min, sec, dsec);

	move(0, 0);
}


/* resize - called when the terminal is resized ... */
void
resize(int signal)
{
	struct winsize ws;

	clear();

	if (ioctl(STDIN_FILENO, TIOCGWINSZ, &ws) != -1) {
		resize_term(ws.ws_row, ws.ws_col);
		tbc.format.width = ws.ws_col;
		tbc.format.height = ws.ws_row;
	} else {
		return;
	}

	tbc_configure();
}


/* tbc_clear - clear everything and redraw frame if needed */
void
tbc_clear(void)
{
	wbkgdset(tbc.screen, COLOR_PAIR(TEXT_DEFAULT));
	clear();

	/* prepare inside frame */
	if (tbc.options.frame) {
		box(tbc.screen, ACS_VLINE, ACS_HLINE);
		mvwprintw(tbc.screen, 0, tbc.format.width-14, TBCVER);
	}
}


/* tbc_display_init - starts curses, obtains term size, set colors */
void
tbc_display_init()
{
	struct winsize ws;

	/* terminal size stuff */
	signal(SIGWINCH, resize);
	if (ioctl(STDIN_FILENO, TIOCGWINSZ, &ws) != -1) {
		tbc.format.width = ws.ws_col;
		tbc.format.height = ws.ws_row;
	}

	/* curses screen init */
	tbc.screen = initscr();
	noecho();
	cbreak();
	curs_set(0);
	nodelay(tbc.screen, TRUE);

	/* prepare colors */
	start_color();
        if (use_default_colors() != ERR) {
		init_pair(TEXT_DEFAULT, -1, -1);
		init_pair(BLOCK_HOUR,	tbc.colors.hour, tbc.colors.hour);
		init_pair(BLOCK_MINUTE,	tbc.colors.minute, tbc.colors.minute);
		init_pair(BLOCK_SECOND,	tbc.colors.second, tbc.colors.second);
		init_pair(BLOCK_TENTH,	tbc.colors.tenth, tbc.colors.tenth);
		init_pair(BLOCK_GREEN,	COLOR_GREEN, COLOR_GREEN);
		init_pair(BLOCK_BLUE,	COLOR_BLUE, COLOR_BLUE);
		init_pair(BLOCK_YELLOW,	COLOR_YELLOW, COLOR_YELLOW);
		init_pair(BLOCK_RED,	COLOR_RED, COLOR_RED);
		init_pair(BLOCK_GREEN,	COLOR_GREEN, COLOR_GREEN);
		init_pair(BLOCK_BLUE,	COLOR_BLUE, COLOR_BLUE);
		init_pair(BLOCK_YELLOW,	COLOR_YELLOW, COLOR_YELLOW);
		init_pair(TEXT_RED,	COLOR_RED, -1);
		init_pair(TEXT_GREEN,	COLOR_GREEN, -1);
		init_pair(TEXT_BLACK,	COLOR_BLACK, -1);
		init_pair(BACK_YELLOW,	COLOR_BLACK, COLOR_YELLOW);
	}
}

