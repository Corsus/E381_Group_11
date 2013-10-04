#include "ball_mover.h"

//push button ISR edge capture
volatile int edge_capture;

//initialization function called by main
void initializeBallMover()
{
	initialize_button_irq();
	initialize_ball_irq();
}

//initialize irq for the fall down timer
void initialize_ball_irq()
{
	printf("Initializing Ball Timer IRQ...\n");
	// setting up timer (Continuous, Interrupt enabled, Stopped)
	IOWR_ALTERA_AVALON_TIMER_CONTROL(BALL_TIMER_BASE, 0xb); //stop-start-cont-ito 1011

	IOWR_ALTERA_AVALON_TIMER_PERIODL(BALL_TIMER_BASE, 0x25A0);
	IOWR_ALTERA_AVALON_TIMER_PERIODH(BALL_TIMER_BASE, 0x0026);

	// register isr
	alt_ic_isr_register(BALL_TIMER_IRQ_INTERRUPT_CONTROLLER_ID, BALL_TIMER_IRQ,
			ball_isr, 0x0, 0x0);

	// enable interrupt
	alt_ic_irq_enable(BALL_TIMER_IRQ_INTERRUPT_CONTROLLER_ID, BALL_TIMER_IRQ);

	//start timer
	IOWR_ALTERA_AVALON_TIMER_CONTROL(BALL_TIMER_BASE, 0x7); //stop-start-cont-ito 0111

	printf("Ball timer started...\n");
}

//initialize irq for ball controls
void initialize_button_irq()
{
	printf("Initializing Button IRQ...\n");

	void* edge_capture_ptr = (void*) &edge_capture;

	IOWR_ALTERA_AVALON_PIO_IRQ_MASK(KEY_BASE, 0xf);

	IOWR_ALTERA_AVALON_PIO_EDGE_CAP(KEY_BASE, 0x0);

	alt_ic_isr_register(KEY_IRQ_INTERRUPT_CONTROLLER_ID, KEY_IRQ,
			pushbutton_isr, edge_capture_ptr, 0x0);
	printf("Initialization completed...\n");
}

//handler for ball control interrupt
void pushbutton_isr(void* context)
{
	alt_u32 interruptible = alt_irq_interruptible(KEY_IRQ);
	volatile int* edge_capture_ptr = (volatile int*) context;

	*edge_capture_ptr = IORD_ALTERA_AVALON_PIO_EDGE_CAP(KEY_BASE);

	IOWR_ALTERA_AVALON_PIO_EDGE_CAP(KEY_BASE, 0);

	adjustPosition_isrHelper();
	alt_irq_non_interruptible(interruptible);
}

//helper function for pushbutton_isr
void adjustPosition_isrHelper()
{
/*	if (!((*((volatile char*)KEY_BASE)) & 0x4))
	{
		// move right
		if (detectCollision(gameBall.x_pos + 1, gameBall.y_pos) == 0)
		{
			if (gameBall.x_pos < SCREEN_X - 1)
			{
				undrawBall();
				gameBall.x_pos += 2;
				drawBall();
			}
		}
		printf("Right %d\n", gameBall.x_pos);
	}
	else if (!((*((volatile char*)KEY_BASE)) & 0x8))
	{
		// move left
		if (detectCollision(gameBall.x_pos - 1, gameBall.y_pos) == 0)
		{
			if (gameBall.x_pos > 0)
			{
				undrawBall();
				gameBall.x_pos -= 2;
				drawBall();
			}
		}
		printf("Left %d\n", gameBall.x_pos);
	}*/

	if (!((*((volatile char*)KEY_BASE)) & 0x4))
	{
		// move right
		undrawBall();
		gameBall.x_pos += 2;
		drawBall();
		printf("Right %d\n", gameBall.x_pos);
	}
	else if (!((*((volatile char*)KEY_BASE)) & 0x8))
	{
		// move left
		undrawBall();
		gameBall.x_pos -= 2;
		drawBall();
		printf("Left %d\n", gameBall.x_pos);
	}

	usleep(20000);
	// from here i can see, inside the interrupt, i can check again to see if it is pressed
	if (!((*((volatile char*)KEY_BASE)) & 0x4) || !((*((volatile char*)KEY_BASE)) & 0x8))
	{
		adjustPosition_isrHelper();
	}
}

//handler for fall down interrupt
void ball_isr(void* context)
{
/*	if (detectCollision(gameBall.x_pos, gameBall.y_pos + 1) == 1)
	{
		//TODO: collision detected
		undrawBall();
		gameBall.y_pos--;
		drawBall();
	}
	else
	{
		if (gameBall.y_pos < SCREEN_Y - 1)
		{
			undrawBall();
			gameBall.y_pos++;
			drawBall();
		}
	}*/

	undrawBall();
	if (gameBall.y_pos < SCREEN_Y - 2)
	{
		gameBall.y_pos++;
	}
	drawBall();

	printf("Ball Y-position: %d\n", gameBall.y_pos);

	//clear timeout bit
	IOWR_ALTERA_AVALON_TIMER_STATUS(BALL_TIMER_BASE, 0x0);

}

//TODO: Collision Detection
// Returns 0 if no collision, 1 if collision detected
int detectCollision(int next_x, int next_y)
{
	int result = 0;

	if (next_y == line1.y_pos)
	{
		if (!(next_x > line1.end_x1 && next_x < line1.start_x2))
		{
			result = 1;
		}
	}
	else if (next_y == line2.y_pos)
	{
		if (!(next_x > line2.end_x1 && next_x < line2.start_x2))
		{
			result = 1;
		}
	}
	else if (next_y == line3.y_pos)
	{
		if (!(next_x > line3.end_x1 && next_x < line3.start_x2))
		{
			result = 1;
		}
	}
	return result;
}
