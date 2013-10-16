/*
 * main.h
 *
 *  Created on: 2013-10-01
 *      Author: EECE381 Group 11
 */

#ifndef MAIN_H_
#define MAIN_H_

#include <stdio.h>
#include <unistd.h>

#include "system.h"
#include "sys/alt_irq.h"
#include "priv/alt_legacy_irq.h"
#include "altera_up_sd_card_avalon_interface.h"
#include "altera_avalon_pio_regs.h"

#include "screen_mover.h"
#include "ball_mover.h"
#include "vga_display.h"

#include "sd_card_controller.h"
#include "sound_manager.h"

#include "lcd_controller.h"
#include "keyboard_controller.h"

//Positions for VGA module to reference
#define SCREEN_X 320
#define SCREEN_X_PLAY 240
#define SCREEN_Y 240
#define WALL_GAP 40

//Menu selections
#define MENU_PLAY 2
#define MENU_SCORE 1

//methods
void initializeGame();
void initializeGameBall();

void menu_controller();
void selectMenu();
void stopGame();
void waitForInput();
void gameOverInput();

void switchPlayMode();

//game ball
typedef struct
{
	int nw_x, nw_y;
	int w_x, w_y;
	int sw_x, sw_y;

	int ne_x, ne_y;
	int e_x, e_y;
	int se_x, se_y;

	int color;
}Ball;

//game states
typedef enum
{
	MAIN_MENU,
	PLAYING,
	HIGH_SCORE,
	SUBMIT_SCORE,
	GAME_OVER
}GameStatus;

extern Ball gameBall;

#endif /* MAIN_H_ */
