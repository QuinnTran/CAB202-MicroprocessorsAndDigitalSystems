/*
**	ReadTimer0.c
**
**	Demonstrates how to set up Timer0 in normal mode and
**	read the Timer/Counter. Effectively, displays 8-bit 
**	numbers at lightning speed.
**
**	See OverflowTimer0.c for use of the Timer Overflow
**	interrupt.
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
	TCCR0A = 0;
	TCCR0B = 5; 

	/*
	Alternatively:
		CLEAR_BIT(TCCR0B,WGM02);
		SET_BIT(TCCR0B,CS02);
		CLEAR_BIT(TCCR0B,CS01);
		SET_BIT(TCCR0B,CS00);
	*/
}

char buffer[20];

void process(void) {
	double time = TCNT0 * PRESCALE / FREQ;
	clear_screen();
	draw_string(0, 0, "TCNT0 = ", FG_COLOUR);
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
