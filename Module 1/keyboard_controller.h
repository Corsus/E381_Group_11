/*
 * keyboard_controller.h
 *
 *  Created on: 2013-10-10
 *      Author: EECE381 Group 11
 */

#ifndef KEYBOARD_CONTROLLER_H_
#define KEYBOARD_CONTROLLER_H_

#include <string.h>

#include "altera_up_avalon_ps2.h"
#include "altera_up_avalon_ps2_regs.h"
#include "altera_up_ps2_keyboard.h"

void initializeKeyboardController();
void readFromKeyboard();

#endif /* KEYBOARD_CONTROLLER_H_ */
