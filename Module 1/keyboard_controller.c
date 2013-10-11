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
	keyboard = alt_up_ps2_open_dev(KEYBOARD_CONTROLLER_NAME);
	alt_up_ps2_init(keyboard);
	//set_keyboard_rate(keyboard, )
}

// make code = press
// break code = release

void readFromKeyboard()
{
	unsigned char* make_code;
	char* translation;

	//read from FIFO
	decode_scancode(keyboard, KB_LONG_BINARY_MAKE_CODE, make_code, 0);
	//translate reading
	translate_make_code(KB_LONG_BINARY_MAKE_CODE, make_code, translation);

	if (strcmp(translation, "L ARROW") == 0)
	{
		// move left
		printf("Left.\n");
	}
	else if (strcmp(translation, "R ARROW") == 0)
	{
		// move right
		printf("Right.\n");
	}

/*	For KB_LONG_BINARY_MAKE_CODE and KB_BREAK_CODE , only
	the second byte is returned. For KB_LONG_BREAK_CODE , only the
	third byte is returned.*/
}
