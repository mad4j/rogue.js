/* $Id: mod_guessbin.c,v 1.9 2007-02-28 12:47:35 tamentis Exp $
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
#include <stdlib.h>
#include <time.h>

#include "tbclock.h"

#define	TIME_EASY	15
#define TIME_NORMAL	10
#define TIME_HARD	5
#define TICK		1000
#define SECOND		100000

#define NUMOFQUEST	20

/* prompt display */
#define QUESTION_LEFT	5
#define PROMPT_LEFT	22
#define PROMPT_TEXT	"What time is it: "
#define PROMPT_BLANK	"         "

/* in-game comments */
#define COMMENT_BLANK	"                      "
#define COMMENT_TIMEOUT	"Time out! Next! Quick!"
#define COMMENT_BAD	"Incorrect! Try harder!"
#define COMMENT_GOOD	"Correct! Try this one!"

/* intro display */
#define INTRO_TITLE	"tbclock/guessbin: Choose your difficulty"
#define INTRO_HELP1	"Use your keyboard arrows to select your difficulty,"
#define INTRO_HELP2	"the harder it get, the faster it goes but the more"
#define INTRO_HELP3	"points you receive! You will have to read the time"
#define INTRO_HELP4	"written with binary number,  type the answer using"
#define INTRO_HELP5	"decimal number with the form HH:MM:SS."

/* score display */
#define SCORE_TOP	2
#define SCORE_TITLE	"You scored %ld points!"
#define SCORE_TIME	"TIME BONUS!          +1000 pts"
#define SCORE_DIFF	"Difficulty bonus!    +%4u pts"
#define SCORE_GOOD	"Good shots           +%4u pts"
#define SCORE_OOPS	"Mistakes             -%4u pts"
#define SCORE_LINE	"--------------------------------"
#define SCORE_FOOT	"(hit 'r' to check your answers!)"

/* history display */
#define H_OFFSET	15
#define H_HEAD1		"Take a look at your answers..."
#define H_HEAD2		"       Time         Answer"
#define H_LINE		"-----------------------------"
#define H_FOOT		"(hit 'r' to go back to the score table)"



/* types */
struct guessbin {
	unsigned int h, m, s;
	time_t t_start, t_end, t_allowed, t_elapsed, t_used;
	unsigned int q_tot, q_cur, q_ok, q_err;
	unsigned int diff;
};

struct history {
	unsigned int uh, um, us;
	unsigned int gh, gm, gs;
	int error;
};



/* global & extern vars */
extern struct tbclock_data tbc;

static char s_diff[4][7] = { "", "Easy", "Normal", "Hard" };
struct history history[NUMOFQUEST];
struct guessbin *g;



/* guessbin_score_history - displays the answers history */
void
guessbin_score_history()
{
	char sr[50];
	int i, color;
	unsigned int tm = tbc.format.height / 2 - (NUMOFQUEST + 7) / 2;

	/* result table header */
	wbkgdset(tbc.screen, COLOR_PAIR(TEXT_DEFAULT));
	mvwprintw(tbc.screen, tm, tbc.format.width / 2 - H_OFFSET, H_HEAD1);
	mvwprintw(tbc.screen, tm + 2, tbc.format.width / 2 - H_OFFSET, H_HEAD2);
	mvwprintw(tbc.screen, tm + 3, tbc.format.width / 2 - H_OFFSET, H_LINE);

	/* all answers */
	for (i = 0; i < NUMOFQUEST; i++) {
		color = TEXT_GREEN;

		if (history[i].error)
			color = TEXT_RED;

		snprintf(sr, 50, "% 2d. %02d:%02d:%02d  vs  %02d:%02d:%02d", 
				i + 1, 
				history[i].gh, history[i].gm, history[i].gs,
				history[i].uh, history[i].um, history[i].us);
		wbkgdset(tbc.screen, COLOR_PAIR(color));
		mvwprintw(tbc.screen, tm + 4 + i, tbc.format.width / 2 - 14, sr);
	}

	/* footer */
	wbkgdset(tbc.screen, COLOR_PAIR(TEXT_DEFAULT));
	mvwprintw(tbc.screen, tm + 4 + i, tbc.format.width / 2 - H_OFFSET, H_LINE);
	mvwprintw(tbc.screen, tm + 6 + i, tbc.format.width / 2 - H_OFFSET - 4, H_FOOT);
}


/* guessbin_score_table - displays the score table */
void
guessbin_score_table()
{
	char s_score[32];
	char s_bonus[32];
	int l, line = 0;
	long points = 0;

	/* time bonus */
	if ((g->t_used / g->q_tot) <= (g->t_allowed / 2)) {
		points += 1000;
		mvwprintw(tbc.screen, tbc.format.height / 2 - SCORE_TOP + line,
				tbc.format.width / 2 - 15, SCORE_TIME);
		line++;
	}

	/* difficulty bonus */
	if (g->diff > 1 && g->q_ok > 0) {
		points += 1000 * g->diff * (g->q_tot/g->q_ok);
		snprintf(s_bonus, 32, SCORE_DIFF, 1000 * g->diff);
		mvwprintw(tbc.screen, tbc.format.height / 2 - SCORE_TOP + line,
				tbc.format.width / 2 - 15, s_bonus);
		line++;
	}

	/* points for good answers */
	if (g->q_ok > 0) {
		points += 200 * g->q_ok;
		snprintf(s_bonus, 32, SCORE_GOOD, 200 * g->q_ok);
		mvwprintw(tbc.screen, tbc.format.height / 2 - SCORE_TOP + line,
				tbc.format.width / 2 - 15, s_bonus);
		line++;
	}

	/* points removed for bad answers */
	if (g->q_err > 0) {
		points -= 100 * g->q_err;
		if (points < 0) points = 0;
		snprintf(s_bonus, 32, SCORE_OOPS, 100 * g->q_err);
		mvwprintw(tbc.screen, tbc.format.height / 2 - SCORE_TOP + line,
				tbc.format.width / 2 - 15, s_bonus);
		line++;
	}

	/* footer */
	mvwprintw(tbc.screen, tbc.format.height / 2 - SCORE_TOP + line, 
			tbc.format.width / 2 - 16, SCORE_LINE);
	l = snprintf(s_score, 32, SCORE_TITLE, points);
	wbkgdset(tbc.screen, COLOR_PAIR(TEXT_GREEN));
	mvwprintw(tbc.screen, tbc.format.height / 2 - SCORE_TOP + line + 1,
			tbc.format.width / 2 - l / 2, s_score);

	wbkgdset(tbc.screen, COLOR_PAIR(TEXT_DEFAULT));
	mvwprintw(tbc.screen, tbc.format.height / 2 - SCORE_TOP + line + 4,
			tbc.format.width / 2 - 16, SCORE_FOOT);
}


/* guessbin_score - final page which displays score or answers */
void
guessbin_score()
{
	int mode = 0;
	signed char c;

	for (;;) {
		c = getch();

		if (c == KB_R) {
			tbc_clear();
			mode = mode == 0 ? 1 : 0;
		} else if (c > 0)
			break;

		if (mode == 1)
			guessbin_score_history();
		else 
			guessbin_score_table();

		refresh();
		usleep(TICK);
	}

	free(g);
}


/* guessbin_next_digit - takes a look at the first two chars of the
 * provided string *c store (maybe) the 1 or 2 digit in *t */
unsigned int
guessbin_next_digit(unsigned char *c, unsigned int *t)
{
	unsigned int offset = 0;

	if (*(c+1) != ':' && *(c+1) != 0) {
		*t = (*(c) - '0') * 10 + *(c+1) - '0';
		offset = 3;
	} else {
		*t = *(c) - '0';
		offset = 2;
	}

	return (offset);
}


/* guessbin_matches - check wether the string in *is (HH:MM:SS) is
 * equal to the three integer provided as a time. */
int
guessbin_matches(unsigned char *is, int size)
{
	unsigned int uh, um, us;
	int error = 0;

	if (size < 5)
		return (0);

	if (*is == ':')
		return (0);

	/* Hours */
	is += guessbin_next_digit(is, &uh);
	if (uh != g->h) error++;

	/* Minutes */
	is += guessbin_next_digit(is, &um);
	if (um != g->m) error++;

	/* Seconds */
	guessbin_next_digit(is, &us);
	if (us != g->s) error++;

	/* record history */
	history[g->q_cur].uh = uh;
	history[g->q_cur].um = um;
	history[g->q_cur].us = us;
	history[g->q_cur].gh = g->h;
	history[g->q_cur].gm = g->m;
	history[g->q_cur].gs = g->s;
	history[g->q_cur].error = error;
	
	if (error)
		return (0);

	return (1);
}


/* guessbin_shuffle - get a new time and draw the new blocks */
void
guessbin_shuffle()
{
	srand(time(NULL));

	g->h = rand() % 23;
	g->m = rand() % 59;
	g->s = rand() % 59;

	tbc_draw_time(3, g->h, g->m, g->s, 0);
}


/* guessbin_menu_diff - draw the difficulty menu, highlighting the option
 * given by 'highlight' */
void
guessbin_menu_diff(int highlight)
{

	if (highlight == 1)
		wbkgdset(tbc.screen, COLOR_PAIR(BACK_YELLOW));
	else
		wbkgdset(tbc.screen, COLOR_PAIR(BACK_DEFAULT));
	mvwprintw(tbc.screen, tbc.format.height/2+1, tbc.format.width/2-14, "[ Easy ]");

	wbkgdset(tbc.screen, COLOR_PAIR(0));
	mvwprintw(tbc.screen, tbc.format.height/2+1, tbc.format.width/2-6, " ");

	if (highlight == 2)
		wbkgdset(tbc.screen, COLOR_PAIR(BACK_YELLOW));
	else
		wbkgdset(tbc.screen, COLOR_PAIR(BACK_DEFAULT));
	mvwprintw(tbc.screen, tbc.format.height/2+1, tbc.format.width/2-5, "[ Normal ]");

	wbkgdset(tbc.screen, COLOR_PAIR(0));
	mvwprintw(tbc.screen, tbc.format.height/2+1, tbc.format.width/2+5, " ");

	if (highlight == 3)
		wbkgdset(tbc.screen, COLOR_PAIR(BACK_YELLOW));
	else
		wbkgdset(tbc.screen, COLOR_PAIR(BACK_DEFAULT));
	mvwprintw(tbc.screen, tbc.format.height/2+1, tbc.format.width/2+6, "[ Hard ]");

	wbkgdset(tbc.screen, COLOR_PAIR(BACK_DEFAULT));

}


/* guessbin_init - initialization, hiscore + ask for difficulty */
struct guessbin *
guessbin_init() 
{
	struct guessbin *g;
	signed char c;
	int hh, hw;

	/* Set up what never moves... */
	wbkgdset(tbc.screen, COLOR_PAIR(TEXT_DEFAULT));
	mvwprintw(tbc.screen, tbc.format.height-1, tbc.format.width-18, "Ctrl-C to Quit");

	g = malloc(sizeof(struct guessbin));
	g->q_tot = NUMOFQUEST;
	g->q_cur = 0;
	g->q_ok  = 0;
	g->q_err = 0;
	g->diff  = 1;

	/* Ask for difficulty */
	hh = tbc.format.height / 2;
	hw = tbc.format.width / 2;
	mvwprintw(tbc.screen, hh - 1, hw - 20, INTRO_TITLE);
	mvwprintw(tbc.screen, hh + 3, hw - 25, INTRO_HELP1);
	mvwprintw(tbc.screen, hh + 4, hw - 25, INTRO_HELP2);
	mvwprintw(tbc.screen, hh + 5, hw - 25, INTRO_HELP3);
	mvwprintw(tbc.screen, hh + 6, hw - 25, INTRO_HELP4);
	mvwprintw(tbc.screen, hh + 7, hw - 25, INTRO_HELP5);
	guessbin_menu_diff(g->diff);

	for (;;) {
		c = getch();

		if (c == KB_LEFT)
			guessbin_menu_diff(g->diff > 1 ? --(g->diff) : g->diff);
		else if (c == KB_RIGHT)
			guessbin_menu_diff(g->diff < 3 ? ++(g->diff) : g->diff);
		else if (c == KB_RETURN) {
			tbc_clear();
			break;
		}

		refresh();

		usleep(TICK);
	}

	g->t_start = time(NULL);
	g->t_used = 0;
	g->t_elapsed = 0;

	switch (g->diff) {
	case 3:
		g->t_allowed = TIME_HARD;
		g->t_end = g->t_start + TIME_HARD;
		break;
	case 2:
		g->t_allowed = TIME_NORMAL;
		g->t_end = g->t_start + TIME_NORMAL;
		break;
	default:
		g->t_allowed = TIME_EASY;
		g->t_end = g->t_start + TIME_EASY;
		break;
	}

	return (g);
}


/* guessbin_time - this displays the two progressbars! */
void
guessbin_timeline(unsigned long current, unsigned long total)
{
	int i, sh;
	float ratio;

	ratio = (float) current / (float)total;
	sh = (tbc.format.height - 2) * (1 - ratio);

	for (i = 1; i < tbc.format.height - 1; i++) {
		if (i < sh + 1)
			wbkgdset(tbc.screen, COLOR_PAIR(BLOCK_DEFAULT));
		else
			wbkgdset(tbc.screen, COLOR_PAIR(BLOCK_GREEN));
		mvwprintw(tbc.screen, i, tbc.format.width - 3, "  ");
		mvwprintw(tbc.screen, i, 1, "  ");
	}

	wbkgdset(tbc.screen, COLOR_PAIR(BACK_DEFAULT));
}


/* guessbin_incorrect - what to do when input is incorrect */
void
guessbin_incorrect()
{
	wbkgdset(tbc.screen, COLOR_PAIR(TEXT_RED));
	mvwprintw(tbc.screen, tbc.format.height - 4, QUESTION_LEFT, COMMENT_BAD);
	g->q_err++;
}


/* guessbin_correct - what to do when input is correct */
void
guessbin_correct()
{
	time_t now;

	now = time(NULL);

	wbkgdset(tbc.screen, COLOR_PAIR(TEXT_GREEN));
	mvwprintw(tbc.screen, tbc.format.height - 4, QUESTION_LEFT, COMMENT_GOOD);
	guessbin_shuffle();

	g->t_used += now - g->t_start;
	g->t_elapsed = 0;
	g->t_start = now;
	g->t_end = g->t_start + g->t_allowed;
	g->q_cur++;
	g->q_ok++;
}


/* guessbin_timeout - what to do when player didn't answer in time */
void
guessbin_timeout(unsigned char *is, int size)
{
	wbkgdset(tbc.screen, COLOR_PAIR(TEXT_RED));
	mvwprintw(tbc.screen, tbc.format.height - 4, QUESTION_LEFT, COMMENT_TIMEOUT);
	guessbin_shuffle();

	history[g->q_cur].gh = g->h;
	history[g->q_cur].gm = g->m;
	history[g->q_cur].gs = g->s;
	history[g->q_cur].error = 1;

	g->t_used += g->t_allowed;
	g->t_start = time(NULL);
	g->t_end = g->t_start + g->t_allowed;
	g->t_elapsed = 0;
	g->q_cur++;
}


/* guessbin_clearprompt - reset size to 0 and clear the input area */
void
guessbin_clearprompt(int *size)
{
	wbkgdset(tbc.screen, COLOR_PAIR(TEXT_DEFAULT));
	mvwprintw(tbc.screen, tbc.format.height-3, PROMPT_LEFT, PROMPT_BLANK);
	move(tbc.format.height-3, 19);
	*size = 0;
}


/* game_guessbin - setup and loop */
void
mod_guessbin()
{
	unsigned char is[8];
	unsigned char status[32];
	signed char c;
	int size = 0;
	time_t now;

	tbc.format.res_x = 6;

        if (tbc.options.vertical)
		tbc.format.res_y = 5;
	else
		tbc.format.res_y = 4;

	tbc_configure();

	g = guessbin_init();

	tbc.options.helper = 0;
	tbc.options.ampm = 0;

	/* main loop */
	mvwprintw(tbc.screen, tbc.format.height - 3, QUESTION_LEFT, PROMPT_TEXT);
	guessbin_shuffle();
	for (;;) {
		c = getch();
		now = time(NULL);

		snprintf((char*)status, 32, "%s [%u/%u]", s_diff[g->diff], 
				g->q_cur, g->q_tot);
		mvwprintw(tbc.screen, tbc.format.height-1, 4, (char*)status);

		guessbin_timeline(g->t_end - now, g->t_end - g->t_start);

		if (g->t_end <= now) {
			guessbin_timeout(is, size);
			guessbin_clearprompt(&size);
		}

		if (g->q_cur >= g->q_tot)
			break;

		if (c == KB_RETURN) {
			if (guessbin_matches(is, size)) {
				guessbin_correct();
			} else {
				guessbin_incorrect();
			}

			guessbin_clearprompt(&size);

		} else if (c == KB_BACKSPACE) {
			if (size < 1)
				continue;
			is[size-1] = 0;
			size--;
			wbkgdset(tbc.screen, COLOR_PAIR(TEXT_DEFAULT));
			mvwprintw(tbc.screen, tbc.format.height - 3, PROMPT_LEFT, 
					"        ");
			mvwprintw(tbc.screen, tbc.format.height - 3, PROMPT_LEFT, 
					(char*)is);

		} else if (c == KB_CLEAR) { /* ^U */
			guessbin_clearprompt(&size);
	
		} else if (c > 47 && c < 59 && size < 8) { /* number or ':' */
			is[size] = c;
			is[size+1] = 0;
			size++;
			wbkgdset(tbc.screen, COLOR_PAIR(TEXT_DEFAULT));
			mvwprintw(tbc.screen, tbc.format.height - 4, QUESTION_LEFT, 
					COMMENT_BLANK);
			mvwprintw(tbc.screen, tbc.format.height - 3, PROMPT_LEFT, 
					(char*)is);
		}

		refresh();

		usleep(TICK);
	}

	/* show score */
	tbc_clear();
	guessbin_score();

}

