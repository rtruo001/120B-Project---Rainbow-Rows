/*
 * Pretesting.c
 *
 * Created: 5/21/2014 11:36:36 AM
 *  Author: Randy
 */ 


#include <avr/io.h>
#include "timer.h"

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

unsigned char NES_latch = 0;
unsigned char NES_pulse = 0;
unsigned short button = 0x00;

void NES_Controller()
{	/*
	//Transitions
	switch (NES_state)
	{
		case NES_Init:
			NES_state = A1;
			break;
		case A1:
			NES_latch = 1;
			NES_pulse = 0;
			NES_state = A0;
		//	PORTC = 0x01;
			break;
		case A0:
			NES_latch = 0;
			NES_pulse = 0;
			NES_state = B1;
			break;
		case B1:
			NES_latch = 0;
			NES_pulse = 1;
			NES_state = B0;
		//	PORTC = 0x00;
			break;
		case B0:
			NES_latch = 0;
			NES_pulse = 0;
			NES_state = Select1;
			break;
		case Select1:
			NES_latch = 0;
			NES_pulse = 1;
			NES_state = Select0;
		//	PORTC = 0x01;
			break;
		case Select0:
			NES_latch = 0;
			NES_pulse = 0;
			NES_state = Start1;
			break;
		case Start1:
			NES_latch = 0;
			NES_pulse = 1;
			NES_state = Start0;
		//	PORTC = 0x00;
			break;
		case Start0:
			NES_latch = 0;
			NES_pulse = 0;
			NES_state = Up1;
			break;
		case Up1:
			NES_latch = 0;
			NES_pulse = 1;
			NES_state = Up0;
			//			PORTC = 0x01;
			break;
		case Up0:
			NES_latch = 0;
			NES_pulse = 0;
			NES_state = Down1;
			break;
		case Down1:
			NES_latch = 0;
			NES_pulse = 1;
			NES_state = Down0;
					//	PORTC = 0x00;
			break;
		case Down0:
			NES_latch = 0;
			NES_pulse = 0;
			NES_state = Left1;
			break;
		case Left1:
			NES_latch = 0;
			NES_pulse = 1;
			NES_state = Left0;
					//	PORTC = 0x01;
			break;
		case Left0:
			NES_latch = 0;
			NES_pulse = 0;
			NES_state = Right1;
			break;
		case Right1:
			NES_latch = 0;
			NES_pulse = 1;
			NES_state = Right0;
				//		PORTC = 0x00;
			break;
		case Right0:
			NES_latch = 0;
			NES_pulse = 0;
			NES_state = A1;
			break;
		default:
			NES_state = NES_Init;
			break;
	}
	
	//Setting the Ports of the controller appropriately.
	PORTD |= ((PORTD & 0x02) | (NES_latch << 1)) | ((PORTD & 0x04) | (NES_pulse << 2));
	//PORTD |= (PORTD & 0x04) | (NES_pulse << 2);
	
	unsigned char Data_in = PIND;
	
	//ACTIONS
	switch(NES_state)
	{
		case NES_Init:
			break;
		case A1:
			if (Data_in & 0x01){
				button |= 0x01;
			//	PORTC = 0x01;
			}
			else
			{
				button &= (~0x01) & 0xFF;
			}
			break;
		case A0:
			break;
		case B1:
			if (Data_in & 0x01){
				button |= 0x02;
			//	PORTC = 0x02;
			}
			else
			{
				button &= (~0x02) & 0xFF;
			}
		case B0:
			break;
		case Select1:
			if (Data_in & 0x01){
				button |= 0x04;
			//	PORTC = 0x04;
			}
			else
			{
				button &= (~0x04) & 0xFF;
			}
			break;
		case Select0:
			break;
		case Start1:
			if (Data_in & 0x01){
				button |= 0x08;
			//	PORTC = 0x08;
			}
			else
			{
				button &= (~0x08) & 0xFF;
			}
			break;
		case Start0:
			break;
		case Up1:
			if (Data_in & 0x01){
				button |= 0x10;
			//	PORTC = 0x10;
			}
			else
			{
				button &= (~0x10) & 0xFF;
			}
			break;
		case Up0:
			break;
		case Down1:
			if (Data_in & 0x01){
				button |= 0x20;
			//	PORTC = 0x20;
			}
			else
			{
				button &= (~0x20) & 0xFF;
			}
			break;
		case Down0:
			break;
		case Left1:
			if (Data_in & 0x01){
				button |= 0x40;
			//	PORTC = 0x40;
			}
			else
			{
				button &= (~0x40) & 0xFF;
			}
			break;
		case Left0:
			break;
		case Right1:
			if (Data_in & 0x01){
				button |= 0x80;
			//	PORTC = 0x80;
			}
			else
			{
				button &= (~0x80) & 0xFF;
			}
			break;
		case Right0:
			break;
		default:
			break;
	}*/
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
	
	if (button == 0xFF)
	{
		PORTC = 0x00;
	}
	else
	{
		if ((button & 0x01) == 0x00)
		{
			PORTC |= 0x01;
		}
		else
		{
			PORTC &= (~0x01) & 0xFF;
		}
		if ((button & 0x02) == 0x00)
		{
			PORTC |= 0x02;
		}
		else
		{
			PORTC &= (~0x02) & 0xFF;
		}
		if ((button & 0x04) == 0x00)
		{
			PORTC |= 0x04;
		}
		else
		{
			PORTC &= (~0x04) & 0xFF;
		}
		if ((button & 0x08) == 0x00)
		{
			PORTC |= 0x08;
		}
		else
		{
			PORTC &= (~0x08) & 0xFF;
		}
		if ((button & 0x10) == 0x00)
		{
			PORTC |= 0x10;
		}
		else
		{
			PORTC &= (~0x10) & 0xFF;
		}
		if ((button & 0x20) == 0x00)
		{
			PORTC |= 0x20;
		}
		else
		{
			PORTC &= (~0x20) & 0xFF;
		}
		if ((button & 0x40) == 0x00)
		{
			PORTC |= 0x40;
		}
		else
		{
			PORTC &= (~0x40) & 0xFF;
		}
		if ((button & 0x80) == 0x00)
		{
			PORTC |= 0x80;
		}
		else
		{
			PORTC &= (~0x80) & 0xFF;
		}
		button = 0x00;
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

int main(void)
{
	//DDRA = 0xFF; PORTA = 0x00;
	DDRB = 0xFF; PORTB = 0x00;
	//DDRD = 0x00; PORTD = 0xFF;
	//DDRC = 0xFF; PORTC = 0x00;
	//DDRD = 0xFE; PORTD = 0x01;
	
	signed long lights = 0xFFFF00FF;
	//unsigned char add = -1;
	//unsigned char LED = 0xFF;
	
	TimerSet(100);
	TimerOn();
	//int state = sm1_display;
    while(1)
    {
		//NES_Controller();
		//SM1_Tick(state);
		transmit_data(lights & 0xFFFFFFFF);
		while (!TimerFlag);
		TimerFlag = 0;
		
	}
}