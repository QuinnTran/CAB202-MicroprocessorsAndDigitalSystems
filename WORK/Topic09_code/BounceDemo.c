/*
**	BounceDemo.c
**
**	Demonstrates bouncing effect in tactile switch.
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

void draw_all(void);
void draw_int( uint8_t x, uint8_t y, int value, colour_t colour );

//keep track of click counts
uint16_t counter = 0;
//used to store and display click count
char buffer[10];

void setup(void) {
	set_clock_speed(CPU_8MHz);
	lcd_init(LCD_DEFAULT_CONTRAST);
	draw_all(); // display text/strings with information
	CLEAR_BIT(DDRD, 1); // up joystick as input
	CLEAR_BIT(DDRF, 6); // left_button as input
}



void process(void) {
	// Detect a Click on left button
	if ( BIT_IS_SET(PINF, 6) ) {
		while ( BIT_IS_SET(PINF, 6) ) {
			// Block until button released.
		}
		// Button has now been pressed and released...
		counter ++;
	}

	// Display and wait if joystick up.
	if ( BIT_IS_SET(PIND, 1) ) {
		draw_all();

		while ( BIT_IS_SET(PIND, 1) ) {
			// Block until joystick released.
		}
	}
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
void draw_all( void ) {
	clear_screen();
	draw_string( 0, 0, "BounceDemo", FG_COLOUR );
	draw_string( 0, 10, "Clk lft to count,", FG_COLOUR );
	draw_string( 0, 20, "Joystk up to view", FG_COLOUR );
	draw_int(10, 30, counter, FG_COLOUR);
	show_screen();
}

void draw_int(uint8_t x, uint8_t y, int value, colour_t colour) {
	snprintf(buffer, sizeof(buffer), "%d", value );
	draw_string(x, y, buffer, colour);
}
