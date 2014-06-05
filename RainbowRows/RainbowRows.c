/*
 * Pretesting.c
 *
 * Created: 5/21/2014 11:36:36 AM
 *  Author: Randy
 */ 


#include <avr/io.h>
#include "timer.h"
#include <stdio.h>
#include <stdlib.h>
#include <avr/interrupt.h>

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

enum NES_states {NES_Init,
				 A1, A0,
				 B1, B0,
				 Select1, Select0,
				 Start1, Start0, 
				 Up1, Up0, 
				 Down1, Down0, 
				 Left1, Left0, 
				 Right1, Right0} NES_state;

unsigned short button = 0x00;

const unsigned short A_Button = 0x02;
const unsigned short B_Button = 0x04;
const unsigned short Select_Button = 0x08;
const unsigned short Start_Button = 0x10;
const unsigned short Up_Button = 0x20;
const unsigned short Down_Button = 0x40;
const unsigned short Left_Button = 0x80;
const unsigned short Right_Button = 0x01;

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

// ====================
// SM1: DEMO LED matrix
// ====================
enum SM1_States {sm1_display};
int SM1_Tick(int state) {

	// === Local Variables ===
	static unsigned char column_val = 0x01; // sets the pattern displayed on columns
	static unsigned char column_sel = 0x7F; // grounds column to display pattern
	
	// === Transitions ===
	switch (state) {
		case sm1_display:    break;
		default:   	        state = sm1_display;
		break;
	}
	
	// === Actions ===
	switch (state) {
		PORTC = 0x00;
		PORTA = 0x00;
		case sm1_display:   // If illuminated LED in bottom right corner
		if (column_sel == 0xFE && column_val == 0x80) {
			column_sel = 0x7F; // display far left column
			column_val = 0x01; // pattern illuminates top row
		}
		// else if far right column was last to display (grounded)
		else if (column_sel == 0xFE) {
			column_sel = 0x7F; // resets display column to far left column
			column_val = column_val << 1; // shift down illuminated LED one row
		}
		// else Shift displayed column one to the right
		else {
			column_sel = (column_sel >> 1) | 0x80;
		}
		break;
		default:   	        break;
	}
	
	PORTC = column_val; // PORTA displays column pattern
	PORTA = column_sel; // PORTB selects column to display pattern

	return state;
}

//This variable starts the entire process of randomizing for the LED matrix.
//The seed would be used with the rand() function to choose Red, Green, or Blue for the color.
signed short seed_randomize = 1;
const unsigned char RED_LIGHT = 0;
const unsigned char GREEN_LIGHT = 1;
const unsigned char BLUE_LIGHT = 2;

signed long col_states[8] = {0x01FFFFFF, 0x02FFFFFF, 0x04FFFFFF, 0x08FFFFFF, 0x10FFFFFF, 0x20FFFFFF, 0x40FFFFFF, 0x80FFFFFF};
void Init_LED()
{
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
	signed long red_mask = 0x01FFFFFE;
	signed long green_mask = 0x01FEFFFF;
	signed long blue_mask = 0x01FFFEFF;
	signed short rand_num = 0;
	
	for (unsigned char i = 0; i < 8; ++i)
	{
		for (unsigned char j = 0; j < 8; ++j)
		{
			//var_init_LED
			if (seed_randomize >= 1000)
			{
				seed_randomize = 1;
			}
			else
			{
				++seed_randomize;
			}
			
			rand_num = rand() % 3;
			if (rand_num == RED_LIGHT)
			{
				col_states[i] = var_init_LED & (col_states[i] & red_mask);
			}
			else if (rand_num == GREEN_LIGHT)
			{
				col_states[i] = var_init_LED & (col_states[i] & green_mask);
			}
			else if (rand_num == BLUE_LIGHT)
			{
				col_states[i] = var_init_LED & (col_states[i] & blue_mask);
			}

			red_mask = (var_init_LED & 0xFF000000) | (((red_mask << 1) | 0x00000001) & 0x00FFFFFF);
			green_mask = (var_init_LED & 0xFF000000) | (((green_mask << 1) | 0x00000001) & 0x00FFFFFF);
			blue_mask = (var_init_LED & 0xFF000000) | (((blue_mask << 1) | 0x00000001) & 0x00FFFFFF);
		}
		var_init_LED = ((var_init_LED & 0xFF000000) << 1) | 0x00FFFFFF;
		red_mask = (var_init_LED & 0xFF000000) | 0x00FFFFFE;
		green_mask = (var_init_LED & 0xFF000000) | 0x00FEFFFF;
		blue_mask = (var_init_LED & 0xFF000000) | 0x00FFFEFF;
	}
}

/***********************************************************************
 *Function void Starting_Game()
 *The state machine in where you start the game by pushing start
 *If don't push start, will continue to wait until user push starts
************************************************************************/
//States for the function to use.
enum To_Start_Game_State {Game_Init, Game_Wait, Game_Start} Game_State;
//This variable would start the entire process of the actual game when it changes to 1
//It will only change to 1 if the user pushes start at the beginning.
unsigned short state_of_game = 0;
void Starting_Game()
{
	//Transitions
	switch(Game_State)
	{
		case Game_Init:
			Game_State = Game_Wait;
			break;
		case Game_Wait:
			if (seed_randomize >= 10000)
			{
				seed_randomize = 1;
			}
			else
			{
				seed_randomize = (seed_randomize * 2) + 1;
			}
			if ((button & Start_Button) == 0x00)
			{
				//Here comes the exciting part
				state_of_game = 1;
				srand(seed_randomize);
				Init_LED();
				Game_State = Game_Start;
			}
			else
			{
				Game_State = Game_Wait;
			}
			break;
		case Game_Start:
			if (state_of_game == 0)
			{
				Game_State = Game_Init;
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
			break;
		case Game_Start:
			break;
		default:
			break;
	}
}

/*************************************************************************************************
 *Function: void LED_MATRIX() 
 *Summary: Would show each column of the LED matrix, with the timer set at 2
  the LED matrix would look like it is displaying everything at once
  but the moving rows would be quick to the invisible eye. 
 *FSM: 9 states including the init state, traversing the columns of an 8x8 RGB LED matrix.
  Does this by setting display_lights to the row it will display.
 *************************************************************************************************/
//This variable would choose which column the LED Matrix would display
signed long display_lights = 0x00000000;
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
	
void LED_Matrix()
{
	//Transitions
	switch(LED_State)
	{
		case LED_Init:
			LED_State = col0;
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

//counts down to when a
unsigned short blink_count = 400;
void Player_Cursor()
{
	if(1)
	{
		blink_count = 400;
	}
	else if(1)
	{
		blink_count = 400;
	}
	else
	{
		if (seed_randomize >= 1000)
		{
			//Restarts the seed to 1 for randomizing
			seed_randomize = 1;	
		}
		else
		{
			++seed_randomize;
		}
		blink_count--;
		if(blink_count <= 0)
		{
			
		}
	}
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
	//DDRA = 0xFF; PORTA = 0x00;
	DDRB = 0xFF; PORTB = 0x00;
	//DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFE; PORTD = 0x01;
	
	
	//signed long lights = 0x0067BBDC;
	//unsigned char add = -1;
	//unsigned char LED = 0xFF;
	
	TimerSet(1);
	TimerOn();
	//int state = sm1_display;
	LED_State = LED_Init;
    while(1)
    {
		/*TODO
		
		IF GAME END
		state_of_game = 0;
		
		*/
		NES_Controller();
		if (state_of_game == 0)
		{
			Starting_Game();
		}
		else
		{
			LED_Matrix();
			transmit_data(display_lights & 0xFFFFFFFF);
		}
		while (!TimerFlag);
		TimerFlag = 0;
		
	}
}