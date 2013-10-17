/*
 * screen_mover.h
 *
 *  Created on: 2013-09-30
 *      Author: EECE381 Group 11
 */

#ifndef SCREEN_MOVER_H_
#define SCREEN_MOVER_H_

#include <math.h>
#include <stdlib.h>
#include <time.h>

#include "altera_avalon_timer.h"
#include "altera_avalon_timer_regs.h"

#include "main.h"
#include "vga_display.h"

// vertical movement speed of the screen
#define SCREEN_SPEED 2

// declare the Line structure
typedef struct
{
	int y_pos;

	int start_x1;
	int end_x1;
	int start_x2;
	int end_x2;

	int on_screen;
	unsigned long int lineColor;
}Line;

//methods
void initializeScreenMover();
void initialize_screen_irq();
void initializeScreenObjects();
void screen_isr(void* context, alt_u32 id);
void pushScreenUp();
void updateCounters();
void generateRandomLine(Line *theLine);

void adjustDifficulty();


extern Line line1;
extern Line line2;

extern int game_score;
extern int difficulty_counter;

#endif /* SCREEN_MOVER_H_ */
