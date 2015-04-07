/*
 * SPEED_RACER4.c
 *
 * Created: 3/11/2015 9:25:37 PM
 *  Author: Chet
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include "SuperSHiftFunction.c"
#include "io.h"
#include "io.c"

//automatic flag
volatile unsigned char TimerFlag = 0;

//global variables for timing
unsigned long ul_avr_timer_M = 1;  // 1 ms
unsigned long ul_avr_timer_cntcurr = 0; // Current internal count of 1ms ticks

void TimerOn() {
	// AVR timer/counter controller register TCCR1
	// bit3 = 0: CTC mode (clear timer on compare)
	// bit2bit1bit0=011: pre-scaler /64
	// 00001011: 0x0B
	// SO, 8 MHz clock or 8,000,000 /64 = 125,000 ticks/s
	// Thus, TCNT1 register will count at 125,000 ticks/s
	TCCR1B = 0x0B;
	// AVR output compare register OCR1A.
	// Timer interrupt will be generated when TCNT1==OCR1A
	// We want a 1 ms tick. 0.001 s * 125,000 ticks/s = 125
	// So when TCNT1 register equals 125,
	// 1 ms has passed. Thus, we compare to 125.
	OCR1A = 125;// AVR timer interrupt mask register
	// bit1: OCIE1A -- enables compare match interrupt
	TIMSK1 = 0x02;
	//Initialize avr counter
	TCNT1=0;
	// TimerISR will be called every _avr_timer_cntcurr milliseconds
	ul_avr_timer_cntcurr = ul_avr_timer_M;
	//Enable global interrupts: 0x80: 1000000
	SREG |= 0x80;
}
void TimerOff() {
	// bit3bit1bit0=000: timer off
	TCCR1B = 0x00;
}

ISR(TIMER1_COMPA_vect) {
	// CPU automatically calls when TCNT1 == OCR1
	// (every 1 ms per TimerOn settings)
	// Count down to 0 rather than up to TOP (results in a more efficient comparison)
	ul_avr_timer_cntcurr--;
	if (ul_avr_timer_cntcurr == 0) {
		// Call the ISR that the user uses
		TimerISR();
		ul_avr_timer_cntcurr = ul_avr_timer_M;
	}
}

// Set TimerISR() to tick every M ms
void TimerSet(unsigned long M) {
	ul_avr_timer_M = M;
	ul_avr_timer_cntcurr = ul_avr_timer_M;
}

// Bit-access functions
unsigned char SetBit(unsigned char x, unsigned char k, unsigned char b) {
	return (b ? x | (0x01 << k) : x & ~(0x01 << k));
}
unsigned char GetBit(unsigned char x, unsigned char k) {
	return ((x & (0x01 << k)) != 0);
}

typedef struct task
{
	int state;      // for the current state in task
	unsigned long period;   // length of task
	unsigned long elapsedTime; // current time taken
	int (*TickFct) (int);  // tick for task
}task;

int tasksNum = 5;
task tasks[5];
const unsigned long tasksPeriodGCD = 2;
const unsigned long periodMatrix = 2;
const unsigned long periodKeypad = 100;
const unsigned long periodLevel = 120;
const unsigned long periodLCD = 500;

unsigned char BLUE = 0x00;	//Section 1
unsigned char RED = 0x00;	//Section 4
unsigned char GREEN = 0x00;	//Section 3
unsigned char SELECT = 0x00;	//Section 2

void TimerISR() {
	unsigned char i;
	for (i = 0; i < tasksNum; ++i) { // Heart of the scheduler code
		if ( tasks[i].elapsedTime >= tasks[i].period ) { // Ready
			tasks[i].state = tasks[i].TickFct(tasks[i].state);
			tasks[i].elapsedTime = 0;
		}
		tasks[i].elapsedTime += tasksPeriodGCD;
	}
}
unsigned char GetKeypadKey() {
	PORTC = 0xEF;
	asm("nop");
	if (GetBit(PINC,0)==0) { return('1'); }
	if (GetBit(PINC,1)==0) { return('4'); }
	if (GetBit(PINC,2)==0) { return('7'); }
	if (GetBit(PINC,3)==0) { return('*'); }
	PORTC = 0xDF;
	asm("nop");
	if (GetBit(PINC,0)==0) { return('2'); }
	if (GetBit(PINC,1)==0) { return('5'); }
	if (GetBit(PINC,2)==0) { return('8'); }
	if (GetBit(PINC,3)==0) { return('0'); }
	PORTC = 0xBF;
	asm("nop");
	if (GetBit(PINC,0)==0) { return('3'); }
	if (GetBit(PINC,1)==0) { return('6'); }
	if (GetBit(PINC,2)==0) { return('9'); }
	if (GetBit(PINC,3)==0) { return('#'); }
	PORTC = 0x7F;
	asm("nop");
	if (GetBit(PINC,0)==0) { return('A'); }
	if (GetBit(PINC,1)==0) { return('B'); }
	if (GetBit(PINC,2)==0) { return('C'); }
	if (GetBit(PINC,3)==0) { return('D'); }
	return('\0'); // default value
}

unsigned char select_pos = 0x00;
unsigned char you_lost = 0x00;
unsigned char turn_up = 0x00;
unsigned char random_num = 0x00;
unsigned long random_maker = 0x00;
unsigned char score = 0x00;
unsigned char high_score = 0x00;
unsigned char temp_score = 0x00;

unsigned char tail[8] = {
	0b10000,
	0b11000,
	0b11111,
	0b11111,
	0b01111,
	0b11111,
	0b00000,
	0b00000
};
unsigned char middle[8] = {
	0b00000,
	0b00000,
	0b10000,
	0b11111,
	0b11111,
	0b11111,
	0b00000,
	0b00000
};
unsigned char nose[8] = {
	0b00000,
	0b00000,
	0b00000,
	0b11110,
	0b11111,
	0b11000,
	0b00000,
	0b00000
};

unsigned char greens[8] =	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08};
unsigned char blues[8] =	{0xE3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	
unsigned char prompt[12] = {0x54, 0x55, 0x4E, 0x4E, 0x45, 0x4C, 0x20, 0x52, 0x41, 0x43, 0x45, 0x52};
unsigned char lose_prompt[6] =	{0x43, 0x52, 0x41, 0x53, 0x48, 0x21};
unsigned char score_prompt[6] = {0x53, 0x63, 0x6F, 0x72, 0x65, 0x3A};

void LCD_Custom(unsigned char location, unsigned char *string)
{
	unsigned char i;
	if(location<8){
		LCD_WriteCommand(0x40+(location*8));
		for(i=0;i<8;i++)
		LCD_WriteData(string[ i ]);
	}
}

enum KP_States { GetKey };
int TickFct_Keypad(int state)
{
	switch (state)
	{
		unsigned char x = '\0';
		case  GetKey:
		x = GetKeypadKey();
		switch (x) {
			// All 5 LEDs on
			case '\0': PORTB = 0x1F; break;
			// hex equivalent
			case '1': PORTB = 0x01; break;
			case '2': PORTB = 0x02; break;
			case '3': PORTB = 0x03; break;
			case '4': PORTB = 0x04; break;
			case '5': PORTB = 0x05; break;
			case '6': PORTB = 0x06; break;
			case '7': PORTB = 0x07; break;
			case '8': PORTB = 0x08; break;
			case '9': PORTB = 0x09; break;
			case 'A': PORTB = 0x0A; you_lost = 1; break;
			case 'B': PORTB = 0x0B; turn_up = 1; break;
			case 'C': PORTB = 0x0C; break;
			case 'D': PORTB = 0x0D; 
			if (greens[7] != 0x01)
			{
				greens[7] = greens[7] >> 1;
			}
			break;
			case '*': PORTB = 0x0E;
			if (greens[7] != 0x80)
			{
				greens[7] = greens[7] << 1;
			}
			break;
			case '0': PORTB = 0x00;  break;
			case '#': PORTB = 0x0F; break;
			// Should never occur. Middle LED off.
			default: PORTB = 0x1B; break;
		}
		break;
		
		default:
		state = GetKey;
		break;
	}
	return state;
}

enum Matrix_States2 { Init_Matrix2, Wait2 };
int TickFct_Matrix2(int state)
{
	switch(state)
	{
		case Init_Matrix2:
		state = Wait2;
		break;
		
		case Wait2:
		state = Wait2;
		break;
		
		default:
		state = Init_Matrix2;
		break;
	}
	switch(state)
	{
		case Init_Matrix2:
		break;
		
		case Wait2:
		asm("nop");
		BLUE = blues[select_pos];
		GREEN = greens[select_pos];
		SELECT = 0x01 << select_pos;
			
		if (select_pos < 0x07) {select_pos++;}
		else {select_pos = 0x00;}
	
		break;
		
		default:
		state = Init_Matrix2;
		break;
	}
	return state;
}

enum Matrix_States { Init_Matrix, Wait };
int TickFct_Matrix(int state)
{
	switch(state)
	{
		case Init_Matrix:
		if (turn_up != 0)
		{
			turn_up = 0;
			srand(random_maker);
			
			state = Wait;
		}
		else
		{
			state = Init_Matrix;
		}
		break;
		
		case Wait:
		if (you_lost != 0)
		{
			you_lost = 1;
			state = Init_Matrix;
		}
		else{state = Wait;}
		break;
		
		default:
		state = Init_Matrix;
		break;
	}
	switch(state)
	{
		case Init_Matrix:
		
		if(random_maker == 214748364)
		{
			random_maker = 0;
		}
		random_maker++;
		
		transmit_data(2, SELECT); //SELECT
		transmit_data(3, (~GREEN)); //GREEN
		transmit_data(1, (~0x00)); //BLUE
		
		break;
		
		case Wait:
		
		transmit_data(2, SELECT); //SELECT
		transmit_data(1, (~BLUE)); //BLUE		
		transmit_data(3, (~GREEN)); //GREEN
		
		if ((blues[7] & greens[7]) != 0)
		{
			you_lost = 1;
		}
		break;
		
		default:
		break;
	}
	return state;
}

enum LCD_States { Init_LCD, Choose_Dif };
int TickFct_LCD(int state)
{
	switch(state)
	{	
		case Init_LCD:
			LCD_init();
			
			LCD_Custom(0, tail);
			LCD_Cursor(14);
			LCD_WriteData(0);
			
			LCD_Custom(1, middle);
			LCD_Cursor(15);
			LCD_WriteData(1);
			
			LCD_Custom(2, nose);
			LCD_Cursor(16);
			LCD_WriteData(2);
			
			LCD_Cursor(1);
			for (unsigned char dat = 0; dat < 12; dat++)
			{
				LCD_WriteData(prompt[dat]);
			}
		state = Choose_Dif;
		break;
		
		case Choose_Dif:
		if (you_lost != 0)
		{	
			you_lost = 0;
			state = Init_LCD;
		}
		else{state = Choose_Dif;}
		break;
		
		default:
		state = Init_LCD;
		break;
	}
	switch(state)
	{
		case Init_LCD:
		break;
		
		case Choose_Dif:
		break;
		
		default:
		break;
	}
	return state;
}

enum Level_Gen { Init_Gen, Start_Level };
int TickFct_LevelGen(int state)
{
	unsigned char temps[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
		
	switch(state)
	{
		case Init_Gen:
		
		LCD_Cursor(17);
		for (unsigned char dat = 0; dat < 6; dat++)
		{
			LCD_WriteData(score_prompt[dat]);
		}
		
		if (score > high_score)
		{
			high_score = score;
		}
		
		LCD_WriteData(high_score + 0x30);
		
		if (turn_up != 0)
		{
			score = 0;
			for (unsigned char dat = 17; dat < 32; dat++)
			{
				LCD_Cursor(dat);
				LCD_WriteData(0x20);
			}
			LCD_Cursor(17);
			state = Start_Level;
		}
		//else {state = Init_Gen;}
		break;
		
		case Start_Level:
		if (you_lost != 0)
		{
			LCD_Cursor(27);
			for (unsigned char dat = 0; dat < 13; dat++)
			{
				LCD_WriteData(lose_prompt[dat]);
			}
			state = Init_Gen;	
		}
		else {state = Start_Level;}
		break;
		
		default:
		state = Init_Gen;
		break;
	}
	switch(state)
	{
		case Init_Gen:
		blues[0] = 0xE3;
		blues[1] = 0x00;
		blues[2] = 0x00;
		blues[3] = 0x00;
		blues[4] = 0x00;
		blues[5] = 0x00;
		blues[6] = 0x00;
		blues[7] = 0x00;
		break;
		
		case Start_Level:
		
		for (unsigned char i = 0; i < 0x07; i++)
		{
			temps[i] = blues[i];
		}
		for (unsigned char i = 0; i < 0x07; i++)
		{
			blues[i+1] = temps[i];
		}
		
		random_num = rand() % 3;
		
		if (score < 6)
		{
			if (random_num == 0)
			{
				if (blues[0] != 0x1F)
				{
					blues[0] = (blues[0] << 1) | 0x01;
				}
			}
			if (random_num == 1)
			{
				if (blues[0] != 0xF8)
				{
					blues[0] = (blues[0] >> 1) | 0x80;
				}
			}
		}
		if (score >= 6)
		{
			if (random_num == 0)
			{
				if (blues[0] != 0x1F)
				{
					blues[0] = (blues[0] << 1) | 0x01;
				}
				else if (blues[0] == 0x1F)
				{
					blues[0] = (blues[0] >> 1) | 0x80;
				}
			}
			if (random_num == 1)
			{
				if (blues[0] != 0xF8)
				{
					blues[0] = (blues[0] >> 1) | 0x80;
				}
				else if (blues[0] == 0xF8)
				{
					blues[0] = (blues[0] << 1) | 0x01;
				}
			}
		}

		if (temp_score < 20)
		{
			temp_score++;
		}
		else
		{
			score++;
			temp_score = 0; 
			LCD_WriteData(0x3E);
		}
		
		break;
		
		default:
		break;
	}
	return state;
}

int main(void)
{
	// PORTB set to output, outputs init 0s
	DDRB = 0xFF; PORTB = 0x00;
	// PC7..4 outputs init 0s, PC3..0 inputs init 1s
	DDRC = 0xF0; PORTC = 0x0F;
	DDRA = 0xFF; PORTA = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
	
	unsigned char i = 0;
	
	tasks[i].state = GetKey;
	tasks[i].period = periodKeypad;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &TickFct_Keypad;
	
	i++;
	
	tasks[i].state = Init_Matrix;
	tasks[i].period = periodMatrix;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &TickFct_Matrix;
	
	i++;
	
	tasks[i].state = Init_Matrix2;
	tasks[i].period = periodMatrix;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &TickFct_Matrix2;
	
	i++;
	
	tasks[i].state = Init_Gen;
	tasks[i].period = periodLevel;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &TickFct_LevelGen;
	TimerSet(tasksPeriodGCD);
	TimerOn();
	
	i++;
	
	tasks[i].state = Init_LCD;
	tasks[i].period = periodLCD;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &TickFct_LCD;
	
	TimerSet(tasksPeriodGCD);
	TimerOn();
    while(1)
    {}
}
