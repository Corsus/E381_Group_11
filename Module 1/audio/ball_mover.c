#include "ball_mover.h"

int right_button_mask = 0x4;
int left_button_mask = 0x8;

//initialization function called by main
void initializeBallMover()
{
	initialize_ball_irq();
	right_button_mask = 0x4;
	left_button_mask = 0x8;
}

//initialize irq for the fall down timer
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

//helper function for pushbutton_isr
void handleControllerInput()
{
	usleep(4000);

	// check if game is over
	if (gameBall.nw_y <= 0)
	{
		stopGame();
		return;
	}

	if (!(IORD_ALTERA_AVALON_PIO_DATA(KEY_BASE) & right_button_mask))
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
	else if (!(IORD_ALTERA_AVALON_PIO_DATA(KEY_BASE) & left_button_mask))
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

	usleep(4000);
}

void reverseControllerInput()
{
	int temp = left_button_mask;
	left_button_mask = right_button_mask;
	right_button_mask = temp;
}

//handler for fall down interrupt
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

void disableTimerInterrupt()
{
	// disable the interrupt, stops the timer
	IOWR_ALTERA_AVALON_TIMER_CONTROL(BALL_TIMER_BASE, 0xA);
}

void enableTimerInterrupt()
{
	// enable the interrupt, starts the timer
	IOWR_ALTERA_AVALON_TIMER_CONTROL(BALL_TIMER_BASE, 0x7);
}

// movement parameter: 0 = falling, 1 = horizontal
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

