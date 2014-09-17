/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * soviet.c
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
 *
 *
 * Soviet Russia Mode module
 *
 *  SOVIET  RUSSIA 
 *    ===CCCP===
 * 
 *    WHERE FOOD
 *     EATS YOU
 *         
 *        ##
 *       #### 
 *      ######
 * ################
 *   ############
 *     ########
 *    ####  ####
 *   ##        ##
 * 
 * the game
 * 
 */

#define lines LINES
#define locCols (COLS-20)

#define CHAR '#'

//zsnake.js
//#include <ncurses.h>
#include <curses.h>

#include <stdlib.h>
#include "windows.h"

typedef struct coordinates {
	unsigned int l;
	unsigned int c;
	} coordinates;

//variables
WINDOW *wCCCP, *wsoviet;
int i,j, nfood=0, s_cicle, s_ch, s_old, s_score, s_len;
coordinates s_snake[9000], s_food[9000];
char *trolling_over[2] = {	"YOU'VE JUST LOST THE GAME! :O",
							"GAME OVER! U MAD?"};
char *trolling_pause[2] = {	"WHAT'S UP? CAN'T YOU WIN THE GAME?",
							"PROBLEMS?"};
char *trolling_text;

//functions
static void CCCP();
static void s_snake_move (int direction);
extern coordinates randomfood (int nlines, int ncols);
extern int loopcollision (struct coordinates *snake, int nlines, int ncols, int len);
extern int avoid_retrofront(int direction, int old_direction);

void soviet() {
	//clean screen
	erase();
	refresh();
	start(2);
	CCCP();
	//soviet window
	wsoviet=newwin(lines,locCols,0,20);
	keypad(wsoviet,TRUE);
	//initialize soviet snake as Lenin wants!
	s_cicle=TRUE;
	s_score=0;
	s_len=20;
	s_old=KEY_UP;
	for (i=0; i<s_len; i++) {
		s_snake[i].l=lines/2;
		s_snake[i].c=locCols/2-10+i;
		}
	for (i=0; i<(sizeof(s_food)/sizeof(coordinates)); i++) {
		s_food[i].l=lines; 
		s_food[i].c=locCols;
	}
	wattron(wsoviet,COLOR_PAIR(1));
	s_food[nfood]=randomfood(lines, locCols);
	//Main loop
	while (s_cicle) {
		wtimeout(wsoviet,170-(15*(s_score/100))); //set timeout
		wattron(wsoviet,COLOR_PAIR(2)); //set color red
		box(wsoviet,ACS_VLINE,ACS_HLINE); 
		//score, length and LPS
		mvwprintw(wsoviet,0,4," SCORE: %4d ",s_score);
		mvwprintw(wsoviet,0,20," LENGHT: %3d ",s_len);
		mvwprintw(wsoviet,lines-1,4," LPS x: %3d y: %3d ",s_snake[0].c,s_snake[0].l);
		//
		wattron(wsoviet,COLOR_PAIR(1)); //set color green
		for (i=0; i<s_len; i++) { //write the snake
			mvwprintw(wsoviet,s_snake[i].l,s_snake[i].c,"%c", CHAR); 
		} 
		wrefresh(wsoviet); //refresh
		s_ch=wgetch(wsoviet); //GET THE KEY PRESSED
		for (i=0; i<s_len; i++) { //cancel the snake
			mvwprintw(wsoviet, s_snake[i].l, s_snake[i].c, " ");
		} 
		if ((s_ch==KEY_UP)||
		    (s_ch==KEY_DOWN)||
		    (s_ch==KEY_LEFT)||
		    (s_ch==KEY_RIGHT)) {
		    	if (avoid_retrofront(s_ch, s_old)) {
					s_old=s_ch;
					s_snake_move(s_ch);
				}
		}
		if(s_ch==104) { //H (help) code
			help(); 
			CCCP(); 
		} 
		if(s_ch==112) { //P (pause) code
			srand(time(NULL));
			trolling_text = trolling_pause[ rand()%2 ];
			pause(trolling_text, 2); 
			CCCP();
		} 
		if(s_ch==109) { //M (menu) code
			erase(); 
			s_cicle=TRUE;
			s_score=0;
			s_old=KEY_UP;
			s_len=20; 
			break;
		} 
		if (s_ch==27) { //ESC
			endwin(); 
			exit(0);
		}
		if(s_ch==ERR) { //NO KEY PRESSED
			s_snake_move(s_old); 
		}
		//Write food
		for (j=0; j<nfood; j++) {
			wattron(wsoviet,COLOR_PAIR(2));
			mvwprintw(wsoviet,s_food[j].l,s_food[j].c,"%c", CHAR);
		} 
		//food eats you!!! D: <--IMPORTANT!!!
		for (j=0; j<nfood; j++) {
			if ((s_snake[0].l==s_food[j].l)&&
			    (s_snake[0].c==s_food[j].c)) {
					s_len-=1;
					s_food[j].l=lines;
					s_food[j].c=locCols;
					break;
			}
		}
		nfood+=1; //each cicle increases food num
		s_score+=1; //each cicle increases score
		s_food[nfood]=randomfood(lines, locCols); //each cicle gets new food
		// Look not to die :D
		s_cicle=loopcollision (s_snake, lines, locCols, s_len); //collisions
		if (s_len<=0) { //end of the snake
			s_cicle=FALSE; 
		}
		if (s_cicle==FALSE) {
			srand(time(NULL));
			trolling_text = trolling_over[ rand()%2 ];
			over(trolling_text);
		}
	}
	delwin(wsoviet);
	erase();
	refresh();	
}

static void CCCP() {
	wCCCP=newwin(LINES,20,0,0);
	wattron(wCCCP, COLOR_PAIR(2));
	box(wCCCP,ACS_VLINE,ACS_HLINE);
	mvwprintw(wCCCP,2,2," SOVIET  RUSSIA ");
	mvwprintw(wCCCP,3,2,"   ===CCCP===   ");
	mvwprintw(wCCCP,5,2,"   WHERE FOOD   ");
	mvwprintw(wCCCP,6,2,"    EATS YOU    ");
	mvwprintw(wCCCP,8,2,"       ##       ");
	mvwprintw(wCCCP,9,2,"      ####      ");
	mvwprintw(wCCCP,10,2,"     ######     ");
	mvwprintw(wCCCP,11,2,"################");
	mvwprintw(wCCCP,12,2,"  ############  ");
	mvwprintw(wCCCP,13,2,"    ########    ");
	mvwprintw(wCCCP,14,2,"   ####  ####   ");
	mvwprintw(wCCCP,15,2,"  ##        ##  ");
	wrefresh(wCCCP);
}

static void s_snake_move(int direction) {
	for (i=s_len;i>0;--i){
		s_snake[i]=s_snake[i-1];
		}
	if (direction==KEY_UP) { s_snake[0].l-=1; }
	if (direction==KEY_DOWN) { s_snake[0].l+=1; }
	if (direction==KEY_LEFT) { s_snake[0].c-=1; }
	if (direction==KEY_RIGHT) { s_snake[0].c+=1;}
}
