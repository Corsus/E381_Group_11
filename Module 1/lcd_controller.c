/*
 * lcd_controller.c
 *
 *  Created on: 2013-10-10
 *      Author: EECE381 Group 11
 */

#include "lcd_controller.h"

const char intro_1[] = "FALL DOWN 381";
const char intro_2[] = "Main Menu";
const char high_score_menu_string[] = "High Scores";
const char game_over_string[] = "Game over";
const char sub_score_string_1[] = "Congrats!";
const char sub_score_string_2[] = "New Highscore!";

const char play_mode_string_1[] = "Normal";
const char play_mode_string_2[] = "Reversed";

const char game_status_string_1[] = "Let's Begin";
const char game_status_string_2[] = "Owning!";
const char game_status_string_3[] = "Dominating!!";
const char game_status_string_4[] = "RAMPAGE!!!";
const char game_status_string_5[] = "G O D L I K E";


alt_up_character_lcd_dev* lcd;

/*
 * Initialize the LCD
 */
void initializeLCD()
{
	// open lcd
	lcd = alt_up_character_lcd_open_dev(CHARACTER_LCD_NAME);
	if (lcd == NULL)
	{
		printf("Error: could not open device\n");
	}
	else
	{
		printf("Opened LCD\n");
	}

	alt_up_character_lcd_init(lcd);
}

/*
 * Draw to the first row of the LCD
 * Takes in the string to draw
 */
void drawStatus(const char* string)
{
	clearRow(0);
	alt_up_character_lcd_set_cursor_pos(lcd, 0, 0);
	alt_up_character_lcd_string(lcd, string);
}

/*
 * Draw to the second row of the LCD
 * Takes in a string to draw
 */
void drawMode(const char* string)
{
	clearRow(1);
	alt_up_character_lcd_set_cursor_pos(lcd, 0, 1);
	alt_up_character_lcd_string(lcd, string);
}

/*
 * "Clears" a row of the LCD by writing spaces
 * Takes in the row to clear (0 or 1)
 */
void clearRow(int row)
{
	alt_up_character_lcd_set_cursor_pos(lcd, 0, row);
	alt_up_character_lcd_string(lcd, "                ");
}

