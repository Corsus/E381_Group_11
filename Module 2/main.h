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

#define GRID_ROWS 6
#define GRID_COLS 6

typedef enum
{
	HORIZONTAL,
	VERTICAL
}Orientation;

void initializeGridContents(int grid[GRID_COLS][GRID_ROWS]);
void insertShipInGrid(int player, char* shipInfo);
int isGameOver(int gridToCheck[GRID_COLS][GRID_ROWS]);
int main();

void testGridContents(int grid[GRID_COLS][GRID_ROWS]);

extern int playerOneGrid[GRID_COLS][GRID_ROWS];
extern int playerTwoGrid[GRID_COLS][GRID_ROWS];

extern int playerOneShips[4];
extern int playerTwoShips[4];

extern int clientOneID;
extern int clientTwoID;

#endif /* MAIN_H_ */
