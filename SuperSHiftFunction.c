#include <avr/io.h>
#include <stdio.h>

char array[10] = "ERROR";



void transmit_data(unsigned char  section, unsigned char data) 
{
	unsigned char i;
	if (section == 1)
	{
		for (i = 0; i < 8 ; ++i) {
			// Sets SRCLR to 1 allowing data to be set
			// Also clears SRCLK in preparation of sending data
			PORTA &= 0xF8;
			PORTA |= 0x08;
			// set SER = next bit of data to be sent.
			PORTA |= ((data >> i) & 0x01);
			// set SRCLK = 1. Rising edge shifts next bit of data into the shift register
			PORTA |= 0x02;
		}
		// set RCLK = 1. Rising edge copies data from “Shift” register to “Storage” register
		PORTA |= 0x04;
		// clears all lines in preparation of a new transmission
		PORTA &= 0xF0;
	}	
	else if(section == 2)
	{
		for (i = 0; i < 8 ; ++i) {
			// Sets SRCLR to 1 allowing data to be set
			// Also clears SRCLK in preparation of sending data
			PORTA &= 0x8F;
			PORTA |= 0x80;
			// set SER = next bit of data to be sent.
			PORTA |= ((data >> i) & 0x01)<< 4;
			// set SRCLK = 1. Rising edge shifts next bit of data into the shift register
			PORTA |= 0x20;
		}
		// set RCLK = 1. Rising edge copies data from “Shift” register to “Storage” register
		PORTA |= 0x40;
		// clears all lines in preparation of a new transmission
		PORTA &= 0x0F;
	}
	else if(section == 3)
	{
		for (i = 0; i < 8 ; ++i) {
			// Sets SRCLR to 1 allowing data to be set
			// Also clears SRCLK in preparation of sending data
			PORTB &= 0xF8;
			PORTB |= 0x08;
			// set SER = next bit of data to be sent.
			PORTB |= ((data >> i) & 0x01);
			// set SRCLK = 1. Rising edge shifts next bit of data into the shift register
			PORTB |= 0x02;
		}
		// set RCLK = 1. Rising edge copies data from “Shift” register to “Storage” register
		PORTB |= 0x04;
		// clears all lines in preparation of a new transmission
		PORTB &= 0xF0;
		
	}	
	else if(section == 4)
	{
		for (i = 0; i < 8 ; ++i) {
			// Sets SRCLR to 1 allowing data to be set
			// Also clears SRCLK in preparation of sending data
			PORTB &= 0x8F;
			PORTB |= 0x80;
			// set SER = next bit of data to be sent.
			PORTB |= ((data >> i) & 0x01)<< 4;
			// set SRCLK = 1. Rising edge shifts next bit of data into the shift register
			PORTB |= 0x20;
		}
		// set RCLK = 1. Rising edge copies data from “Shift” register to “Storage” register
		PORTB |= 0x40;
		// clears all lines in preparation of a new transmission
		PORTB &= 0x0F;
	}	
	return;
}
