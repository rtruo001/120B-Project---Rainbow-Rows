/*
 * Pretesting.c
 *
 * Created: 5/21/2014 11:36:36 AM
 *  Author: Randy Truong
 *  Custom lab Proposal: Rainbow Rows (Connect 8)
 */ 


#include <avr/io.h>
#include "timer.h"
#include <stdio.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include "io.c"

/*************************************************VARIABLES*************************************************************************/
/*************************************************VARIABLES*************************************************************************/
/*************************************************VARIABLES*************************************************************************/
//States for the function to use.
enum To_Start_Game_State {Game_Init, Game_Wait, Game_LCD, Game_Start, Ending, Game_Reset} Game_State;	
//The states of the LED matrix, displaying a column at a time.
enum LED_States{LED_Init,
	col0, col0_off,
	col1, col1_off,
	col2, col2_off,
	col3, col3_off,
	col4, col4_off,
	col5, col5_off,
	col6, col6_off,
	col7, col7_off
} LED_State;
//States of the cursor
enum Cursor_States{Cursor_Init, Wait_For_Buttons, Up_State, Down_State, Left_State, Right_State} Cursor_State;
//States of the blinking
enum Blink_States {Blink_Init, Light_On, Light_Off} Blink_State;

signed long Timer_to_End = 24000;

//This variable starts the entire process of randomizing for the LED matrix.
//The seed would be used with the rand() function to choose Red, Green, or Blue for the color.
signed short seed_randomize = 1;

//The Red mask would be used if the red color was used in the game.
//const unsigned char RED_LIGHT = 0;
const unsigned char GREEN_LIGHT = 0;
const unsigned char BLUE_LIGHT = 1;

//The bytes of each column.
signed long col_states[8] = {0x01FFFFFF, 0x02FFFFFF, 0x04FFFFFF, 0x08FFFFFF, 0x10FFFFFF, 0x20FFFFFF, 0x40FFFFFF, 0x80FFFFFF};

//Amount of full rows and columns
signed char points = 0;

//This variable would start the entire process of the actual game when it changes to 1
//It will only change to 1 if the user pushes start at the beginning.
unsigned short state_of_game = 0;

//This variable would choose which column the LED Matrix would display
signed long display_lights = 0x00000000;

unsigned char x = 0;
unsigned short y = 0x01;

//Used for manipulation of the cursor and moving it.
signed long cursor_green_mask = 0x01010000;
signed long	cursor_blue_mask = 0x01000100;
signed long row_green_mask = 0x00010000;
signed long row_blue_mask = 0x00000100;
signed long to_check_swap = 0xFFFCFCFF;
signed long swap_green_to_blue = 0x00010200;
signed long swap_blue_to_green = 0x00020100;
unsigned char cursor_color = -1;

//Timing set for the blinking.
unsigned short BLINK_TIME = 150;
unsigned short blink_count = 150;

/*************************************************VARIABLES*************************************************************************/
/*************************************************VARIABLES*************************************************************************/
/*************************************************VARIABLES*************************************************************************/

/***********************************************************************************************************************************/

//Reinitializes all the variables for reset.
void Reinitialization()
{
	Game_State = Game_Init;
	LED_State = LED_Init;
	Cursor_State = Cursor_Init;
	Blink_State = Blink_Init;
	
	Timer_to_End = 24000;

	//This variable starts the entire process of randomizing for the LED matrix.
	//The seed would be used with the rand() function to choose Red, Green, or Blue for the color.
	seed_randomize = 1;

	//The bytes of each column.
	col_states[0] = 0x01FFFFFF;
	col_states[1] = 0x02FFFFFF;
	col_states[2] = 0x04FFFFFF;
	col_states[3] = 0x08FFFFFF;
	col_states[4] = 0x10FFFFFF;
	col_states[5] = 0x20FFFFFF;
	col_states[6] = 0x40FFFFFF;
	col_states[7] = 0x80FFFFFF;

	//Amount of full rows and columns
	points = 0;

	//This variable would start the entire process of the actual game when it changes to 1
	//It will only change to 1 if the user pushes start at the beginning.
	state_of_game = 0;

	//This variable would choose which column the LED Matrix would display
	display_lights = 0x00000000;

	x = 0;
	y = 0x01;

	//Used for manipulation of the cursor and moving it.
	cursor_green_mask = 0x01010000;
	cursor_blue_mask = 0x01000100;
	row_green_mask = 0x00010000;
	row_blue_mask = 0x00000100;
	to_check_swap = 0xFFFCFCFF;
	swap_green_to_blue = 0x00010200;
	swap_blue_to_green = 0x00020100;
	cursor_color = -1;

	//Timing set for the blinking.
	BLINK_TIME = 150;
	blink_count = 150;	
}

//Counts the points by counting how many rows the user made. Only vertical orbs of 3 or more of the same color would count as rows.
void Count_Points()
{
	signed long green_point_mask = 0x00800000;
	signed long blue_point_mask = 0x00008000;
	
	for (unsigned char i = 0; i < 8; ++i)
	{
		if ((col_states[i] & 0x00FF0000) == 0x00000000 || (col_states[i] & 0x0000FF00) == 0x00000000)
		{
			++points;
		}
	}

	for (unsigned char j = 0; j < 8; j++)
	{
		if ((col_states[0] & green_point_mask) == 0 &&
		(col_states[1] & green_point_mask) == 0 &&
		(col_states[2] & green_point_mask) == 0 &&
		(col_states[3] & green_point_mask) == 0 &&
		(col_states[4] & green_point_mask) == 0 &&
		(col_states[5] & green_point_mask) == 0 &&
		(col_states[6] & green_point_mask) == 0 &&
		(col_states[7] & green_point_mask) == 0)
		{
			++points;
		}
		else if ((col_states[0] & blue_point_mask) == 0 &&
		(col_states[1] & blue_point_mask) == 0 &&
		(col_states[2] & blue_point_mask) == 0 &&
		(col_states[3] & blue_point_mask) == 0 &&
		(col_states[4] & blue_point_mask) == 0 &&
		(col_states[5] & blue_point_mask) == 0 &&
		(col_states[6] & blue_point_mask) == 0 &&
		(col_states[7] & blue_point_mask) == 0)
		{
			++points;
		}
		green_point_mask = green_point_mask >> 1;
		blue_point_mask = blue_point_mask >> 1;
	}
}

/*******************************************NES CONTROLLER**************************************************/
/*enum NES_states {NES_Init,
				 A1, A0,
				 B1, B0,
				 Select1, Select0,
				 Start1, Start0, 
				 Up1, Up0, 
				 Down1, Down0, 
				 Left1, Left0, 
				 Right1, Right0} NES_state;
*/
/*
button would be used throughout the entire program.
Would be the source in describing which button is pressed.

Goes like this:

Button: Left Down Up  Start Select B    A  Right
Bits:    7    6	   5    4     3    2    1    0
*/
unsigned short button = 0x00;

//Constant variables for each button and their representations by their bits.
const unsigned short A_Button = 0x02;
const unsigned short B_Button = 0x04;
const unsigned short Select_Button = 0x08;
const unsigned short Start_Button = 0x10;
const unsigned short Up_Button = 0x20;
const unsigned short Down_Button = 0x40;
const unsigned short Left_Button = 0x80;
const unsigned short Right_Button = 0x01;

/************************************************************************/
//How it works is that it has 2 inputs and 1 output, the 2 inputs would receive the latch which would send information
//its shift register, therefore every pulse would generate the different buttons from the controller, being outputted
//from the Data pin.
void NES_Controller()
{	
	unsigned char NES_latch = 0;
	unsigned char NES_pulse = 0;
	button = 0;
	
	for (unsigned char i = 8; i > 0; i--)
	{
		if (i == 8)
		{
			NES_latch = 0x02;
			PORTD |= (NES_latch & 0x02);
			if (PIND & 0x01){
				button |= 0x80;
			}
			else
			{
				button &= (~0x80) & 0xFF;
			}
			PORTD = 0x00;
		}
		else
		{
			button = button >> 1;
			NES_latch = 0x00;
			NES_pulse = 0x04;
			PORTD |= (NES_pulse & 0x04);
			if (PIND & 0x01){
				button |= 0x80;
			}
			else
			{
				button &= (~0x80) & 0xFF;
			}
			PORTD = 0x00;
		}
	}
}

/************************************SHIFT REGISTER FUNCTION**********************************
 *Summary: Converts the pins of a microcontroller and forms it into a 32 bit port. This allows 
  the usage of the 8x8 LED RGB Matrix. This uses 4 shift registers, each shift register gives 
  an extra 8 bits.
*********************************************************************************************/
void transmit_data(signed long data) {
	int i;
	for (i = 0; i < 32 ; ++i) {
		// Sets SRCLR to 1 allowing data to be set
		// Also clears SRCLK in preparation of sending data
		PORTB = 0x08;
		// set SER = next bit of data to be sent.
		PORTB |= ((data >> i) & 0x01);
		// set SRCLK = 1. Rising edge shifts next bit of data into the shift register
		PORTB |= 0x02;
	}
	// set RCLK = 1. Rising edge copies data from “Shift” register to “Storage” register
	PORTB |= 0x04;
	// clears all lines in preparation of a new transmission
	PORTB = 0x00;
}

//This function randomizes the LED at the very beginning.
void Init_LED()
{
	//Restarts and initializes all the columns to their basic state.
	col_states[0] = 0x01FFFFFF;
	col_states[1] = 0x02FFFFFF;
	col_states[2] = 0x04FFFFFF;
	col_states[3] = 0x08FFFFFF;
	col_states[4] = 0x10FFFFFF;
	col_states[5] = 0x20FFFFFF;
	col_states[6] = 0x40FFFFFF;
	col_states[7] = 0x80FFFFFF;
	//This variable is initialized as an LED with all columns and rows turned off except the first column.
	signed long var_init_LED = 0x01FFFFFF;
	//The following variables are initialized to which bits would be Red, Green, or Blue.
//	signed long red_mask = 0x01FFFFFE;
	signed long green_mask = 0x01FEFFFF;
	signed long blue_mask = 0x01FFFEFF;
	signed short rand_num = 0;
	
	for (unsigned char i = 0; i < 8; ++i)
	{
		for (unsigned char j = 0; j < 8; ++j)
		{
			//var_init_LED
			if (seed_randomize >= 10000)
			{
				seed_randomize = 1;
			}
			else
			{
				++seed_randomize;
			}
			
			//The random number generated would either be 0 or 1 
			//0 - green 
			//1 - blue.
			//If red was used, 0 1 and 2 would be generated. 
			//0 - red
			//1 - green
			//2 - blue
			rand_num = rand() % 2;
		
		//The Red mask would be used if the red color was used in the game.
		//	if (rand_num == RED_LIGHT)
		//	{
		//		col_states[i] = var_init_LED & (col_states[i] & red_mask);
		//	}
			if (rand_num == GREEN_LIGHT)
			{
				col_states[i] = var_init_LED & (col_states[i] & green_mask);
			}
			else if (rand_num == BLUE_LIGHT)
			{
				col_states[i] = var_init_LED & (col_states[i] & blue_mask);
			}
			
		//The Red mask would be used if the red color was used in the game.
		//	red_mask = (var_init_LED & 0xFF000000) | (((red_mask << 1) | 0x00000001) & 0x00FFFFFF);
			green_mask = (var_init_LED & 0xFF000000) | (((green_mask << 1) | 0x00000001) & 0x00FFFFFF);
			blue_mask = (var_init_LED & 0xFF000000) | (((blue_mask << 1) | 0x00000001) & 0x00FFFFFF);
		}
		var_init_LED = ((var_init_LED & 0xFF000000) << 1) | 0x00FFFFFF;
		
	//The Red mask would be used if the red color was used in the game.
	//	red_mask = (var_init_LED & 0xFF000000) | 0x00FFFFFE;
		green_mask = (var_init_LED & 0xFF000000) | 0x00FEFFFF;
		blue_mask = (var_init_LED & 0xFF000000) | 0x00FFFEFF;
	}
}

const unsigned char press_start[] = "Press Start";
/***********************************************************************
 *Function void Starting_Game()
 *The state machine in where you start the game by pushing start
 *If don't push start, will continue to wait until user push starts
************************************************************************/
void Starting_Game()
{
	//Transitions
	switch(Game_State)
	{
		case Game_Init:
			Reinitialization();
			LCD_DisplayString(1, press_start);
			Game_State = Game_Wait;
			break;
		case Game_Wait:
			if (seed_randomize >= 10000)
			{
				seed_randomize = 1;
			}
			else
			{
				++seed_randomize;
			}
			if ((button & Start_Button) == 0x00)
			{
				//Here comes the exciting part
				state_of_game = 1;
				//Initializes the seed for the random.
				srand(seed_randomize);
				Init_LED();
				Game_State = Game_LCD;
			}
			else
			{
				Game_State = Game_Wait;
			}
			break;
		case Game_LCD:
			Game_State = Game_Start;
			break;
		case Game_Start:
			if (Timer_to_End <= 0)
			{
				Game_State = Ending;
				state_of_game = 0;
				Count_Points();
			}
			else
			{
				Game_State = Game_Start;
			}
			--Timer_to_End;
			break;
		case Ending:
			Game_State = Game_Reset;
			break;
		case Game_Reset:
			if ((button & A_Button) == 0x00)
			{
				Game_State = Game_Init;
			}
			else
			{
				Game_State = Game_Reset;
			}
			break;
		default:
			Game_State = Game_Init;
			break;
	}
	
	//Actions
	switch(Game_State)
	{
		case Game_Init:
			break;
		case Game_Wait:
			//Just the intro lighting sequence, green on top, blue on the bottom.
			transmit_data(0xFF7FFEFF);
			break;
		case Game_LCD:
			LCD_ClearScreen();
			unsigned char game_message[] = "Move the orbs!      Make rows";
			LCD_DisplayString(1, game_message);
			break;
		case Game_Start:
			break;
		case Ending:
			transmit_data(0);
			transmit_data(0xFFFFFF00);
			LCD_ClearScreen();
			unsigned char message[] = "Total Rows:     Reset: Press A";
			LCD_DisplayString(1, message);
			LCD_Cursor(12);
			LCD_WriteData(points + 48);	
			break;
		case Game_Reset:
			break;
		default:
			break;
	}
}

/*************************************************************************************************
 *Function: void LED_MATRIX() 
 *Summary: Would show each column of the LED matrix, with the timer set at 1
  the LED matrix would look like it is displaying everything at once
  but the moving rows would be quick to the invisible eye. 
 *FSM: 17 states including the init state, traversing the columns of an 8x8 RGB LED matrix.
  Does this by setting display_lights to the column it will display. Then turning off the columns
  to prevent trailing lights. 
 *************************************************************************************************/
void LED_Matrix()
{
	//Transitions
	//Goes from state column on, to column off, to the next column on. The cycle continues.
	switch(LED_State)
	{
		case LED_Init:
			if (state_of_game == 0)
			{
				LED_State = LED_Init;
			}
			else
			{
				LED_State = col0;
			}
			break;
		case col0:
			LED_State = col0_off;
			break;
		case col0_off:
			LED_State = col1;
			break;
		case col1:
			LED_State = col1_off;
			break;
		case col1_off:
			LED_State = col2;
			break;
		case col2:
			LED_State = col2_off;
			break;
		case col2_off:
			LED_State = col3;
			break;
		case col3:
			LED_State = col3_off;
			break;
		case col3_off:
			LED_State = col4;
			break;
		case col4:
			LED_State = col4_off;
			break;
		case col4_off:
			LED_State = col5;
			break;
		case col5:
			LED_State = col5_off;
			break;
		case col5_off:
			LED_State = col6;
			break;
		case col6:
			LED_State = col6_off;
			break;
		case col6_off:
			LED_State = col7;
			break;
		case col7:
			LED_State = col7_off;
			break;
		case col7_off:
			LED_State = col0;
			break;
		default:
			LED_State = LED_Init;
			break;
	}
	
	//Actions
	//The actions sets the column state to the specific colors.
	//After a column is displayed, the next state would turn off the LED's of the column, preventing trailing lights.
	switch(LED_State)
	{
		case LED_Init:
			break;
		case col0:
			display_lights = col_states[0];
			break;
		case col1:
			display_lights = col_states[1];
			break;
		case col2:
			display_lights = col_states[2];
			break;
		case col3:
			display_lights = col_states[3];
			break;
		case col4:
			display_lights = col_states[4];
			break;
		case col5:
			display_lights = col_states[5];
			break;
		case col6:
			display_lights = col_states[6];
			break;
		case col7:
			display_lights = col_states[7];
			break;
			
		//Turns column of the LED's off.
		case col0_off:
			display_lights = 0x00FFFFFFFF;
			break;
		case col1_off:
			display_lights = 0x00FFFFFFFF;
			break;
		case col2_off:
			display_lights = 0x00FFFFFFFF;
			break;
		case col3_off:
			display_lights = 0x00FFFFFFFF;
			break;
		case col4_off:
			display_lights = 0x00FFFFFFFF;
			break;
		case col5_off:
			display_lights = 0x00FFFFFFFF;
			break;
		case col6_off:
			display_lights = 0x00FFFFFFFF;
			break;
		case col7_off:
			display_lights = 0x00FFFFFFFF;
			break;
		default:
			break;
	}
}

/****************************CURSOR FUNCTIONS**************************************
 *We can say the column is the x axis
 *Row can be the y axis.
 **********************************************************************************/
	/********************Diagram example*******************************************
	
	row7  B   G		...
	row6  B   B		...
	row5  G   G		...
	row4  G   B		...
y	row3  B   B		...
	row2  B   G		...
	row1  G   B		...
	row0  B*  G		...
		col0 col1 col2 col3 col4 col5 col6 col7   
						 x
	
	the * symbolizes where the cursor would be.
	so when the user pushes right, it will turn into
	
	
	row7  B    G	 ...
	row6  B    B	 ...
	row5  G    G	 ...
	row4  G    B	 ...
y	row3  B    B	 ...
	row2  B    G	 ...
	row1  G    B     ...
	row0  B    G*    ...
		col0 col1 col2 col3 col4 col5 col6 col7  
						x
						
	When the user pushes up or down, all the code would do is take the col_state[x]
	and then turn on and off the colors position by manipulating the bits with y.
	When user pushes left or right, it will just shift the col_state[x-1 or x+1].
	**********************************************************************************/
void Player_Cursor()
{
	//Transitions
	switch(Cursor_State)
	{
		case Cursor_Init:
			if (state_of_game == 0)
			{
				Cursor_State = Cursor_Init;
			}
			else
			{
				if ((col_states[x] & cursor_green_mask) == 0x01000000)
				{
					cursor_color = GREEN_LIGHT;
				}
				else if ((col_states[x] & cursor_blue_mask) == 0x01000000)
				{
					cursor_color = BLUE_LIGHT;
				}
				Cursor_State = Wait_For_Buttons;
			}
			break;
		case Wait_For_Buttons:
			if ((button & Left_Button) == 0x00)
			{
				if (x > 0)
				{
					Cursor_State = Left_State;
					--x;
					if (cursor_color == GREEN_LIGHT)
					{
						if ((col_states[x] & row_green_mask) != 0)
						{
							col_states[x+1] = (col_states[x+1] | row_green_mask) & ~row_blue_mask;
							col_states[x] = (col_states[x] | row_blue_mask) & ~row_green_mask;
						}
					}
					else if (cursor_color == BLUE_LIGHT)
					{
						if ((col_states[x] & row_blue_mask) != 0)
						{
							col_states[x+1] = (col_states[x+1] | row_blue_mask) & ~row_green_mask;
							col_states[x] = (col_states[x] | row_green_mask) & ~row_blue_mask;
						}
					}
				}
			}
			else if ((button & Right_Button) == 0x00)
			{
				if (x < 7)
				{
					Cursor_State = Right_State;
					++x;
					if (cursor_color == GREEN_LIGHT)
					{
						if ((col_states[x] & row_green_mask) != 0)
						{
							col_states[x-1] = (col_states[x-1] | row_green_mask) & ~row_blue_mask;
							col_states[x] = (col_states[x] | row_blue_mask) & ~row_green_mask;
						}
					}
					else if (cursor_color == BLUE_LIGHT)
					{
						if ((col_states[x] & row_blue_mask) != 0)
						{
							col_states[x-1] = (col_states[x-1] | row_blue_mask) & ~row_green_mask;
							col_states[x] = (col_states[x] | row_green_mask) & ~row_blue_mask;
						}
					}
				}
			}
			else if ((button & Up_Button) == 0x00)
			{
				if (y < 0x80)
				{
					Cursor_State = Up_State;
					row_green_mask = row_green_mask << 1;
					row_blue_mask = row_blue_mask << 1;
					y = y << 1;	
					
					if (cursor_color == GREEN_LIGHT)
					{
						if ((col_states[x] & row_green_mask) != 0x00000000)
						{
							col_states[x] = (col_states[x] & to_check_swap) | swap_green_to_blue;
						}
					}
					else if (cursor_color == BLUE_LIGHT)
					{
						if ((col_states[x] & row_blue_mask) != 0x00000000)
						{
							col_states[x] = (col_states[x] & to_check_swap) | swap_blue_to_green;
						}
					}
					
					to_check_swap = (to_check_swap << 1) | 0x00000001;
					swap_blue_to_green = swap_blue_to_green << 1;
					swap_green_to_blue = swap_green_to_blue << 1;
					cursor_green_mask = (cursor_green_mask & 0xFF00FFFF) | (cursor_green_mask << 1);
					cursor_blue_mask = (cursor_blue_mask & 0xFFFF00FF) | (cursor_blue_mask << 1);
					
				}
			}
			else if ((button & Down_Button) == 0x00)
			{
				if (y > 0x01)
				{
					Cursor_State = Down_State;
					row_green_mask = row_green_mask >> 1;
					row_blue_mask = row_blue_mask >> 1;
					to_check_swap = (to_check_swap >> 1) | 0x80000000;
					swap_blue_to_green = swap_blue_to_green >> 1;
					swap_green_to_blue = swap_green_to_blue >> 1;
					y = y >> 1;
					
					if (cursor_color == GREEN_LIGHT)
					{
						if ((col_states[x] & row_green_mask) != 0x00000000)
						{
							col_states[x] = (col_states[x] & to_check_swap) | swap_blue_to_green;
						}
					}
					else if (cursor_color == BLUE_LIGHT)
					{
						if ((col_states[x] & row_blue_mask) != 0x00000000)
						{
							col_states[x] = (col_states[x] & to_check_swap) | swap_green_to_blue;
						}
					}
					
					cursor_green_mask = (cursor_green_mask & 0xFF00FFFF) | ((cursor_green_mask & 0x00FF0000) >> 1);
					cursor_blue_mask = (cursor_blue_mask & 0xFFFF00FF) | ((cursor_blue_mask & 0x0000FF00) >> 1);
				}
			}
			else
			{
				Cursor_State = Wait_For_Buttons;
			}
			break;
		case Left_State:
			if ((button & Left_Button) != 0x00)
			{
				Cursor_State = Wait_For_Buttons;
			}
			else
			{
				Cursor_State = Left_State;
			}
			break;
		case Right_State:
			if ((button & Right_Button) == 0x00)
			{
				Cursor_State = Right_State;
			}
			else
			{
				Cursor_State = Wait_For_Buttons;
			}
			break;
		case Up_State:
			if ((button & Up_Button) != 0x00)
			{
				Cursor_State = Wait_For_Buttons;
			}
			else
			{
				Cursor_State = Up_State;
			}
			break;
		case Down_State:
			if ((button & Down_Button) != 0x00)
			{
				Cursor_State = Wait_For_Buttons;
			}
			else
			{
				Cursor_State = Down_State;
			}
			break;
		default:
			Cursor_State = Cursor_Init;
			break;
	}
	
	//Actions
	switch(Cursor_State)
	{
		case Cursor_Init:
			break;
		case Wait_For_Buttons:
			if (seed_randomize >= 10000)
			{
				//Restarts the seed to 1 for randomizing
				seed_randomize = 1;
			}
			else
			{
				++seed_randomize;
			}
			break;
		case Left_State:
			break;
		case Right_State:
			break;
		case Up_State:
			break;
		case Down_State:
			break;
		default:
			break;
	}
}

void Cursor_blinking()
{
	//Transitions
	switch(Blink_State)
	{
		case Blink_Init:
			if (state_of_game == 0)
			{
				Blink_State = Blink_Init;
			}
			else
			{
				Blink_State = Light_On;
			}
			break;
		case Light_On:
			if (blink_count <= 0)
			{
				blink_count = BLINK_TIME;
				Blink_State = Light_Off;
			}
			else
			{
				--blink_count;
				Blink_State = Light_On;
			}
			break;
		case Light_Off:
			if (blink_count <= 0)
			{
				blink_count = BLINK_TIME;
				Blink_State = Light_On;
			}
			else
			{
				--blink_count;
				Blink_State = Light_Off;
			}
			break;
		default:
			Blink_State = Blink_Init;
			break;
	}
	
	//Actions
	switch(Blink_State)
	{
		case Blink_Init:
			break;
		case Light_On:
			if (blink_count == BLINK_TIME)
			{
				if (cursor_color == GREEN_LIGHT)
				{
					col_states[x] = (col_states[x] & 0xFF000000) | (col_states[x] & (~cursor_green_mask & 0x00FFFFFF));
				}
				else if (cursor_color == BLUE_LIGHT)
				{
					col_states[x] = (col_states[x] & 0xFF000000) | (col_states[x] & (~cursor_blue_mask & 0x00FFFFFF));
				}
			}
			break;
		case Light_Off:
			if (blink_count == BLINK_TIME)
			{
				if (cursor_color == GREEN_LIGHT)
				{
					col_states[x] = (col_states[x] & 0xFF000000) | (col_states[x] | (cursor_green_mask & 0x00FFFFFF));
				}
				else if (cursor_color == BLUE_LIGHT)
				{
					col_states[x] = (col_states[x] & 0xFF000000) | (col_states[x] | (cursor_blue_mask & 0x00FFFFFF));
				}	
			}	
			//The following would have the cursor continiously blink even if the user holds down a button.
			if (((button & Left_Button) == 0x00 || 
				 (button & Right_Button) == 0x00 || 
				 (button & Up_Button) == 0x00 || 
				 (button & Down_Button) == 0x00) && Cursor_State == Wait_For_Buttons)
			{
				
				if (cursor_color == GREEN_LIGHT)
				{
					col_states[x] = (col_states[x] & 0xFF000000) | (col_states[x] & (~cursor_green_mask & 0x00FFFFFF));
				}
				else if (cursor_color == BLUE_LIGHT)
				{
					col_states[x] = (col_states[x] & 0xFF000000) | (col_states[x] & (~cursor_blue_mask & 0x00FFFFFF));
				}
			}
		default:
			break;
	}
}

void State_Machines()
{
	NES_Controller();
	Starting_Game();
	LED_Matrix();
	Cursor_blinking();
	Player_Cursor();
	transmit_data(display_lights & 0xFFFFFFFF);
}
/*******************************************************************************
NOTES:
Example of what bits represent what
 
0xFF123456 

FF -- The columns 

The rows:
12 -- Green
34 -- Blue
56 -- Red

The rows are active low, meaning 0 is on and 1 is off.
**********************************************************************************/
/*The following represents a small diagram of the placement of the matrix.

ROWS
bit 7 _
bit 6 _
bit 5 _
bit 4 _
bit 3 _
bit 2 _
bit 1 _
bit 0 _ L
        _  _  _  _  _  _  _  _
        0  1  2  3  4  5  6  7 COLS

The L would be on if it is 0x01000000
and if the 0, 8, or 16 bit is 0.

0 bit for the color red
8 bit for blue
16 bit for green.
Can have multiple of them be 0 for combination of colors.

PS: Green mostly overpowers the other colors.
************************************************************************************/
int main(void)
{
	DDRA = 0xFF; PORTA = 0x00;
	DDRB = 0xFF; PORTB = 0x00;
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFE; PORTD = 0x01;
	
	
	//signed long lights = 0x0067BBDC;
	//unsigned char add = -1;
	//unsigned char LED = 0xFF;
	
	LCD_init();
	
	TimerSet(1);
	TimerOn();
	//int state = sm1_display;
	Game_State = Game_Init;
	LED_State = LED_Init;
	Cursor_State = Cursor_Init;
	Blink_State = Blink_Init;
    while(1)
    {
		/*TODO
		
		IF GAME END
		state_of_game = 0;
		
		*/
		State_Machines();
		while (!TimerFlag);
		TimerFlag = 0;
		
	}
}