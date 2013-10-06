#include "ball_mover.h"

//push button ISR edge capture
volatile int edge_capture;

//initialization function called by main
void initializeBallMover()
{
	initialize_ball_irq();
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
	alt_ic_isr_register(BALL_TIMER_IRQ_INTERRUPT_CONTROLLER_ID, BALL_TIMER_IRQ,
			ball_isr, 0x0, 0x0);

	// enable interrupt
	alt_ic_irq_enable(BALL_TIMER_IRQ_INTERRUPT_CONTROLLER_ID, BALL_TIMER_IRQ);

	//start timer
	IOWR_ALTERA_AVALON_TIMER_CONTROL(BALL_TIMER_BASE, 0x7); //stop-start-cont-ito 0111

	printf("Ball timer started...\n");
}

//helper function for pushbutton_isr
void handleControllerInput()
{
	usleep(5000);
	if (!((*((volatile char*)KEY_BASE)) & 0x4))
	{
		// move right
		if (gameBall.e_x < SCREEN_X_PLAY - HORIZONTAL_SPEED)
		{
			undrawBall();
			updateBallPosition(HORIZONTAL_MOVE_RIGHT);
			drawBall();
		}
		printf("Right %d\n", gameBall.nw_x + 1);
	}
	else if (!((*((volatile char*)KEY_BASE)) & 0x8))
	{
		// move left
		if (gameBall.w_x > ((SCREEN_X_PLAY/2) - 2) % 3 )
		{
			undrawBall();
			updateBallPosition(HORIZONTAL_MOVE_LEFT);
			drawBall();
		}
		printf("Left %d\n", gameBall.nw_x + 1);
	}
	usleep(5000);
	if (!((*((volatile char*)KEY_BASE)) & 0x4) || !((*((volatile char*)KEY_BASE)) & 0x8))
	{
		handleControllerInput();
	}
}

//handler for fall down interrupt
void ball_isr(void* context)
{
	undrawBall();
	if (gameBall.sw_y < SCREEN_Y - 1)
	{
		updateBallPosition(VERTICAL_MOVE);
	}
	detectCollision(VERTICAL_MOVE);
	drawBall();

	//printf("Ball Y-position: %d\n", gameBall.nw_y + 1);

	//clear timeout bit
	IOWR_ALTERA_AVALON_TIMER_STATUS(BALL_TIMER_BASE, 0x0);

}

// movement parameter: 0 = falling, 1 = horizontal
void detectCollision(int movement)
{
	if (movement == VERTICAL_MOVE)
	{
		if (line1.y_pos >= gameBall.nw_y && line1.y_pos <= gameBall.sw_y)
		{
			if (line1.end_x1 >= gameBall.w_x || line1.start_x2 <= gameBall.e_x)
			{
				handleCollision(VERTICAL_MOVE, 1);
			}
		}
		else if (line2.y_pos >= gameBall.nw_y && line2.y_pos <= gameBall.sw_y)
		{
			if (line2.end_x1 >= gameBall.w_x || line2.start_x2 <= gameBall.e_x)
			{
				handleCollision(VERTICAL_MOVE, 2);
			}
		}
	}
}

void handleCollision(int movement, int line_number)
{
	if (movement == VERTICAL_MOVE)
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

