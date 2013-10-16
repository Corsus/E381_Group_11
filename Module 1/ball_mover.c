/*
 * ball_mover.c
 *
 *  Created on: 2013-09-30
 *      Author: EECE381 Group 11
 */

#include "ball_mover.h"

// values of the push buttons
int right_move = RIGHT_BUTTON;
int left_move = LEFT_BUTTON;

/*
 * Initialize the ball mover module
 */
void initializeBallMover()
{
	initialize_ball_irq();
	right_move = RIGHT_BUTTON;
	left_move = LEFT_BUTTON;
}

/*
 * Initialize the ball falling timer irq
 */
void initialize_ball_irq()
{
	printf("Initializing Ball Timer IRQ...\n");
	// setting up timer (Continuous, Interrupt enabled, Stopped)
	IOWR_ALTERA_AVALON_TIMER_CONTROL(BALL_TIMER_BASE, 0xb); //stop-start-cont-ito 1011

	IOWR_ALTERA_AVALON_TIMER_PERIODL(BALL_TIMER_BASE, 0x625A);
	IOWR_ALTERA_AVALON_TIMER_PERIODH(BALL_TIMER_BASE, 0x0002);

	// register isr
	alt_irq_register(BALL_TIMER_IRQ, 0x0, ball_isr);

	// enable interrupt
	alt_irq_enable(BALL_TIMER_IRQ);

	//start timer
	IOWR_ALTERA_AVALON_TIMER_CONTROL(BALL_TIMER_BASE, 0x7); //stop-start-cont-ito 0111

	printf("Ball timer started...\n");
}

/*
 * Checks the push button input and performs horizontal movement
 */
void handleControllerInput()
{
	usleep(5000);

	// check if game is over
	if (gameBall.nw_y <= 0)
	{
		stopGame();
		return;
	}

	int direction = readGameControl();

	if (direction == right_move)
	{
		// move right
		if (gameBall.e_x < SCREEN_X_PLAY - HORIZONTAL_SPEED)
		{
			disableTimerInterrupt();
			undrawBall();
			updateBallPosition(HORIZONTAL_MOVE_RIGHT);
			drawBall();
			enableTimerInterrupt();
		}
		//printf("Right %d\n", gameBall.nw_x + 1);
	}
	else if (direction == left_move)
	{
		// move left
		if (gameBall.w_x > ((SCREEN_X_PLAY/2) - 2) % 3 )
		{
			disableTimerInterrupt();
			undrawBall();
			updateBallPosition(HORIZONTAL_MOVE_LEFT);
			drawBall();
			enableTimerInterrupt();
		}
		//printf("Left %d\n", gameBall.nw_x + 1);
	}

	usleep(5000);
}

/*
 * Method to swap the values of the left and right buttons
 */
void reverseControllerInput()
{
	int temp = left_move;
	left_move = right_move;
	right_move = temp;
}

/*
 * Handler for the ball fall timer
 * Makes the ball fall
 */
void ball_isr(void* context, alt_u32 id)
{
	alt_u32 interruptible = alt_irq_interruptible(BALL_TIMER_IRQ);
	undrawBall();
	if (gameBall.sw_y < SCREEN_Y - 1)
	{
		updateBallPosition(VERTICAL_MOVE);
	}
	detectCollision();
	drawBall();

	//printf("Ball Y-position: %d\n", gameBall.nw_y + 1);

	//clear timeout bit
	IOWR_ALTERA_AVALON_TIMER_STATUS(BALL_TIMER_BASE, 0x0);
	alt_irq_non_interruptible(interruptible);

}

/*
 * Method to disable the ball fall timer
 * Used by the horizontal movement method to make sure horizontal movement doesn't
 * get preempted by the vertical movement interrupt
 */
void disableTimerInterrupt()
{
	// disable the interrupt, stops the timer
	IOWR_ALTERA_AVALON_TIMER_CONTROL(BALL_TIMER_BASE, 0xA);
}

/*
 * Method to re-enable the ball fall timer after it was disabled
 * Used in conjunction with the disableTimerInterrupt method
 */
void enableTimerInterrupt()
{
	// enable the interrupt, starts the timer
	IOWR_ALTERA_AVALON_TIMER_CONTROL(BALL_TIMER_BASE, 0x7);
}

/*
 * Method to detect collision between Ball and Line
 * Handles the collision if detected
 */
void detectCollision()
{
	if (line1.y_pos >= gameBall.nw_y && line1.y_pos <= gameBall.sw_y)
	{
		if (line1.end_x1 >= gameBall.w_x || line1.start_x2 <= gameBall.e_x)
		{
			handleCollision(1);
		}
	}
	else if (line2.y_pos >= gameBall.nw_y && line2.y_pos <= gameBall.sw_y)
	{
		if (line2.end_x1 >= gameBall.w_x || line2.start_x2 <= gameBall.e_x)
		{
			handleCollision(2);
		}
	}

}

/*
 * Method to handle collision once it is detected.
 * Helper function for detectCollision
 */
void handleCollision(int line_number)
{
	switch (line_number)
	{
		case 1:
			gameBall.sw_y = gameBall.se_y = line1.y_pos - SCREEN_SPEED;
			gameBall.w_y = gameBall.e_y = line1.y_pos - SCREEN_SPEED - 1;
			gameBall.nw_y = gameBall.ne_y = line1.y_pos - SCREEN_SPEED - 2;
			break;
		case 2:
			gameBall.sw_y = gameBall.se_y = line2.y_pos - SCREEN_SPEED;
			gameBall.w_y = gameBall.e_y = line2.y_pos - SCREEN_SPEED - 1;
			gameBall.nw_y = gameBall.ne_y = line2.y_pos - SCREEN_SPEED - 2;
			break;
	}

}

/*
 * Method to update the ball's position based on the type of movement
 */
void updateBallPosition(int movement)
{
	if (movement == VERTICAL_MOVE)
	{
		gameBall.nw_y += VERTICAL_SPEED;
		gameBall.w_y += VERTICAL_SPEED;
		gameBall.sw_y += VERTICAL_SPEED;
		gameBall.ne_y += VERTICAL_SPEED;
		gameBall.e_y += VERTICAL_SPEED;
		gameBall.se_y += VERTICAL_SPEED;
	}
	else if (movement == HORIZONTAL_MOVE_LEFT)
	{
		gameBall.nw_x -= HORIZONTAL_SPEED;
		gameBall.w_x -= HORIZONTAL_SPEED;
		gameBall.sw_x -= HORIZONTAL_SPEED;
		gameBall.ne_x -= HORIZONTAL_SPEED;
		gameBall.e_x -= HORIZONTAL_SPEED;
		gameBall.se_x -= HORIZONTAL_SPEED;
	}
	else if (movement == HORIZONTAL_MOVE_RIGHT)
	{
		gameBall.nw_x += HORIZONTAL_SPEED;
		gameBall.w_x += HORIZONTAL_SPEED;
		gameBall.sw_x += HORIZONTAL_SPEED;
		gameBall.ne_x += HORIZONTAL_SPEED;
		gameBall.e_x += HORIZONTAL_SPEED;
		gameBall.se_x += HORIZONTAL_SPEED;
	}
}

