/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * dialog.h
 * Copyright (C) Ilario Pierbattista 2011 <ulven101@gmail.com>
 * 
 * znake is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * znake is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//zsnake.js
//#include <ncurses.h>
#include <curses.h>

WINDOW *wstart, *wover, *whelp, *wpause;

static void start(int color);      		//print countdown
static void pause(char text[100], int color); //print pause window
static void help();					    //print help
static void over(char text[100]);		//print game over window

static void start(int color) {
	int c,i;
	wstart=newwin(3,60,(LINES-3)/2, (COLS-60)/2);
	wtimeout(wstart,1000);
	wattron(wstart,COLOR_PAIR(color));
	box(wstart,ACS_VLINE,ACS_HLINE);
	for (i=3; i>0; i--) {
		mvwprintw(wstart,1,23,"STARTING IN %d",i);
		wrefresh(wstart);
		c=wgetch(wstart);
		if (c!=ERR) { break; }
	}
	delwin(wstart);
	erase();
	refresh();
}

static void pause(char text[100], int color) {
	int len=0;
	while (text[len]!='\0') {
		len++;
	}
	wpause=newwin(3,(len+4),(LINES-3)/2,(COLS-(len+4))/2);
	wattron(wpause,COLOR_PAIR(color));
	box(wpause,ACS_VLINE,ACS_HLINE);
	mvwprintw(wpause,1,2,"%s", text);
	wrefresh(wpause);
	getchar();
	delwin(wpause);
	erase();
	refresh();
}

static void help() {
	erase();
	refresh();
	whelp=newwin(14,40,(LINES-14)/2,(COLS-40)/2);
	wattron(whelp,COLOR_PAIR(1));
	box(whelp,ACS_VLINE,ACS_HLINE);
	mvwprintw(whelp,0,16," HELP ");
	mvwprintw(whelp,2,3,"MOVE:");
	mvwprintw(whelp,3,4,"KEY_UP TO MOVE UP");
	mvwprintw(whelp,4,4,"KEY_DOWN TO MOVE DOWN");
	mvwprintw(whelp,5,4,"KEY_LEFT TO MOVE LEFT");
	mvwprintw(whelp,6,4,"KEY_RIGHT TO MOVE RIGHT");
	mvwprintw(whelp,8,3,"KEY H TO PRINT THIS HELP");
	mvwprintw(whelp,9,3,"KEY M TO GO TO MENU");
	mvwprintw(whelp,10,3,"ESC KEY TO QUIT GAME");
	mvwprintw(whelp,12,3,"PRESS ANY KEY TO RESUME THE GAME");
	wrefresh(whelp);
	getchar();
	delwin(whelp);
	erase();
	refresh();
}

static void over(char text[100]) {
	int pch, len=0;
	while (text[len]!='\0') {
		len++;
	}
	erase();
	wover=newwin(3,(len+4),(LINES-3)/2,(COLS-(len+4))/2);
	keypad(wover, TRUE);
	do {
		wattron(wover,COLOR_PAIR(2));
		box(wover,ACS_VLINE,ACS_HLINE);
		mvwprintw(wover,1,2,"%s",text);
		wrefresh(wover);
		pch=wgetch(wover);
	} while ((pch==KEY_UP) ||
			(pch==KEY_DOWN) ||
			(pch==KEY_LEFT) ||
			(pch==KEY_RIGHT) );
}
