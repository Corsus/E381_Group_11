/*
 * keyboard_controller.c
 *
 *  Created on: 2013-10-10
 *      Author: EECE381 Group 11
 */

#include "keyboard_controller.h"

alt_up_ps2_dev* keyboard;

void initializeKeyboardController()
{
	printf("Initialize keyboard...\n");
	keyboard = alt_up_ps2_open_dev(KEYBOARD_CONTROLLER_NAME);
	if (keyboard == NULL)
	{
		printf("Error: could not open device\n");
	}
	else
	{
		printf("Opened Keyboard\n");
	}

	alt_up_ps2_init(keyboard);
	alt_up_ps2_clear_fifo(keyboard);
	alt_up_ps2_write_data_byte(keyboard, 0xff);

	printf("Done keyboard initialization...\n");
}

// make code = press
// break code = release

int readGameControl()
{
	unsigned char make_code = 0;
	int return_value = 0;
	make_code = IORD_8DIRECT(KEYBOARD_CONTROLLER_BASE, ALT_UP_PS2_PORT_DATA);

	if (make_code == 0x6B)
	{
		// move left
		//printf("Left.\n");
		return_value = LEFT_BUTTON;
	}
	else if (make_code == 0x74)
	{
		// move right
		//printf("Right.\n");
		return_value = RIGHT_BUTTON;
	}

	return return_value;
}

int readMenuControl()
{
	unsigned char make_code = 0;
	int return_value = 0;
	alt_up_ps2_read_data_byte(keyboard, &make_code);

	if (make_code == 0x6B)
	{
		// move left
		//printf("Left.\n");
		return_value = LEFT_BUTTON;
	}
	else if (make_code == 0x74)
	{
		// move right
		//printf("Right.\n");
		return_value = RIGHT_BUTTON;
	}
	else if (make_code == 0x5A)
	{
		// enter button
		return_value = ENTER_BUTTON;
	}
	else if (make_code == 0xF0)
	{
		// break code
		alt_up_ps2_read_data_byte(keyboard, &make_code);
	}

	return return_value;
}

void readUserNameInput()
{
	int loop_counter = 0;

	unsigned char make_code = 0;

	char player_name[3];
	char* temp_char;

	//set_keyboard_rate(keyboard, );

	while (loop_counter < 1000)
	{
		//read from FIFO, store in temp_char
		alt_up_ps2_read_data_byte(keyboard, &make_code);

		if (make_code == 0xF0 || make_code == 0xE0)
		{
			// skip it
			printf("skipped\n");
			alt_up_ps2_read_data_byte(keyboard, &make_code);
		}
		else
		{
			translate_make_code(KB_ASCII_MAKE_CODE, make_code, temp_char);
			player_name[loop_counter] = temp_char[0];
			printf("%c\n", temp_char[0]);
			//draw the character on screen in correct position
			//updateSubmitScoreScreen(temp_char[0], loop_counter);

			//increment loop counter
			loop_counter++;
		}
	}
	//TODO: write to SD card
	//updateSubmitScoreScreen(0, 0);
}

