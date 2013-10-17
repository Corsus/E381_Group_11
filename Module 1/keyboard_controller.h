/*
 * keyboard_controller.h
 *
 *  Created on: 2013-10-10
 *      Author: EECE381 Group 11
 */

#ifndef KEYBOARD_CONTROLLER_H_
#define KEYBOARD_CONTROLLER_H_

#include <string.h>

#include "main.h"

#include "altera_up_avalon_ps2.h"
#include "altera_up_avalon_ps2_regs.h"
#include "altera_up_ps2_keyboard.h"

#define NAME_SIZE 3
#define LEFT_BUTTON 1
#define RIGHT_BUTTON 2
#define ENTER_BUTTON 3


void initializeKeyboardController();
int readGameControl();
int readMenuControl();

#endif /* KEYBOARD_CONTROLLER_H_ */
