/*
 * lcd_controller.h
 *
 *  Created on: 2013-10-10
 *      Author: EECE381 Group 11
 */

#ifndef LCD_CONTROLLER_H_
#define LCD_CONTROLLER_H_

#include "altera_up_avalon_character_lcd.h"
#include "main.h"

//size of LCD row
#define LCD_WIDTH 16

//methods
void initializeLCD();
void drawStatus(const char* string);
void drawMode(const char* string);
void clearRow(int row);

#endif /* LCD_CONTROLLER_H_ */
