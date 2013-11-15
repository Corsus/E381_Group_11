/*
 * main.h
 *
 *  Created on: 2013-11-13
 *      Author: htang
 */

#ifndef MAIN_H_
#define MAIN_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "Communication.h"

#define GRID_ROWS 7
#define GRID_COLS 6

typedef enum
{
	EMPTY,
	SHIP,
	FIRE_MISS,
	FIRE_HIT,
}GridContent;

typedef enum
{
	HORIZONTAL,
	VERTICAL
}Orientation;

void initializeGridContents(GridContent grid[GRID_COLS][GRID_ROWS]);
void insertShipInGrid(int player, char* shipInfo);
int main();

void testGridContents(GridContent grid[GRID_COLS][GRID_ROWS]);

extern GridContent playerOneGrid[GRID_COLS][GRID_ROWS];
extern GridContent playerTwoGrid[GRID_COLS][GRID_ROWS];

#endif /* MAIN_H_ */
