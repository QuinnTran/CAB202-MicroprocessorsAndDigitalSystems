/* File: turn_on_led_solution.c
 * Description: C program for the ATMEL AVR microcontroller (ATmega32 chip)
 * Turn on Teensy LED with button
 
 * Teensy PewPew
 
 * Includes (pretty much compulsory for using the Teensy this semester)
 * 	- avr/io.h: port and pin definitions (i.e. DDRB, PORTB, PB1, etc)
 *
 *	- cpu_speed.h: macros for forcing the CPU speed to 8MHz (nothing else should ever be used!)
 */
#include <avr/io.h>

// Include the CPU Speed information
#include "cpu_speed.h"
#include "macros.h"

//perform all the operation to setup registers, configure pins, 
//and setup timers
void setup() {
	set_clock_speed(CPU_8MHz);

	// Output to left LED
	SET_BIT(DDRB, 2);

	// Input from the left and right buttons
	CLEAR_BIT(DDRF, 5);
	CLEAR_BIT(DDRF, 6);
}

//this is the fucntion that is executed every loop iteration and contains 
//the main operation the program is intended to do
void process() {
	// If left button is pressed, Turn on left LED.
	if ( BIT_IS_SET(PINF, 6) ) {
		SET_BIT(PORTB, 2);
	}

	// Otherwise, if right button is pressed, turn off left LED.
	else if ( BIT_IS_SET(PINF, 5) ) {
		CLEAR_BIT(PORTB, 2);
	}
}

int main(void) {

	//setup avr
	setup();

	//main loop 
	for ( ;; ) {
		process();
	}
}
