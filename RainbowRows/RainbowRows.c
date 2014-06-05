/*
 * Pretesting.c
 *
 * Created: 5/21/2014 11:36:36 AM
 *  Author: Randy
 */ 


#include <avr/io.h>
#include "timer.h"
#include <stdlib.h>

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

enum To_Start_Game_State {Game_Init, Game_Wait, Game_Start} Game_State;
signed short seed_randomize = 1;
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
			if (seed_randomize >= RAND_MAX)
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

signed long col_states[8] = {0, 0, 0, 0, 0, 0, 0, 0}; 
void Init_LED()
{
		
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
enum LED_States{LED_Init, col0, col1, col2, col3, col4, col5, col6, col7} LED_State;
	
void LED_Matrix()
{
	//Transitions
	switch(LED_State)
	{
		case LED_Init:
			LED_State = col0;
			break;
		case col0:
			display_lights = 0x01000000;
			LED_State = col1;
			break;
		case col1:
			display_lights = 0x02000000;
			LED_State = col2;
			break;
		case col2:
			display_lights = 0x04000000;
			LED_State = col3;
			break;
		case col3:
			display_lights = 0x08000000;
			LED_State = col4;
			break;
		case col4:
			display_lights = 0x10000000;
			LED_State = col5;
			break;
		case col5:
			display_lights = 0x20000000;
			LED_State = col6;
			break;
		case col6:
			display_lights = 0x40000000;
			LED_State = col7;
			break;
		case col7:
			display_lights = 0x80000000;
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
			break;
		case col1:
			break;
		case col2:
			break;
		case col3:
			break;
		case col4:
			break;
		case col5:
			break;
		case col6:
			break;
		case col7:
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
		if (seed_randomize >= RAND_MAX)
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
	
	
	signed long lights = 0x0067BBDC;
	//unsigned char add = -1;
	//unsigned char LED = 0xFF;
	
	TimerSet(2);
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
			lights |= (display_lights & 0xFFFFFFFF);
			transmit_data(lights & 0xFFFFFFFF);
			lights &= 0x00FFFFFF;
		}
		while (!TimerFlag);
		TimerFlag = 0;
		
	}
}