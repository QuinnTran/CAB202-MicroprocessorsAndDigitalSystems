/*
**	TimerOverflow0.c
**
**	Demonstrates how to set up a Timer Overflow ISR
**	for Timer 0, and uses it to implement a digital 
**	time counter.
**
**	Lawrence Buckingham, QUT, September 2017.
**	(C) Queensland University of Technology.
*/
#include <stdint.h>
#include <stdio.h>
#include <avr/io.h> 
#include <avr/interrupt.h>
#include <util/delay.h>
#include <cpu_speed.h>

#include <graphics.h>
#include <macros.h>
#include "lcd_model.h"

#define FREQ     (8000000.0)
#define PRESCALE (1024.0)

void draw_double(uint8_t x, uint8_t y, double value, colour_t colour);

void setup(void) {
	set_clock_speed(CPU_8MHz);
	lcd_init(LCD_DEFAULT_CONTRAST);
	lcd_clear();

	// Timer 0 in normal mode, with pre-scaler 1024 ==> ~30Hz overflow.
	// Timer overflow on.
	TCCR0A = 0;
	TCCR0B = 5; 
	TIMSK0 = 1; 

	/*
	Alternatively:
		CLEAR_BIT(TCCR0B,WGM02);
		SET_BIT(TCCR0B,CS02);
		CLEAR_BIT(TCCR0B,CS01);
		SET_BIT(TCCR0B,CS00);
		SET_BIT(TIMSK0, TOIE0);
	*/

	// Enable timer overflow, and turn on interrupts.
	sei();
}

char buffer[20];

volatile int overflow_counter = 0;

ISR(TIMER0_OVF_vect) {
	overflow_counter ++;
}

void process(void) {
	double time = ( overflow_counter * 256.0 + TCNT0 ) * PRESCALE  / FREQ;
	clear_screen();
	draw_string(0, 0, "Time = ", FG_COLOUR);
	draw_double(10, 10, time, FG_COLOUR);
	show_screen();
}

int main(void) {
	setup();

	for ( ;; ) {
		process();
	}
}
// -------------------------------------------------
// Helper functions.
// -------------------------------------------------

void draw_double(uint8_t x, uint8_t y, double value, colour_t colour) {
	snprintf(buffer, sizeof(buffer), "%f", value);
	draw_string(x, y, buffer, colour);
}
