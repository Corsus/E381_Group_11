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

//constant strings

extern const char intro_1[];
extern const char intro_2[];
extern const char high_score_menu_string[];
extern const char game_over_string[];
extern const char sub_score_string_1[];
extern const char sub_score_string_2[];

extern const char play_mode_string_1[];
extern const char play_mode_string_2[];

extern const char game_status_string_1[];
extern const char game_status_string_2[];
extern const char game_status_string_3[];
extern const char game_status_string_4[];
extern const char game_status_string_5[];


//methods
void initializeLCD();
void drawStatus(const char* string);
void drawMode(const char* string);
void clearRow(int row);

#endif /* LCD_CONTROLLER_H_ */
