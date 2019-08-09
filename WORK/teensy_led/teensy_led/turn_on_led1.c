/* File: turn_on_led_solution1a.c
 * Description: C program for the ATMEL AVR microcontroller (ATmega32 chip)
 * Turn on Teensy LED 
 
 * Teensy PewPew
 
 * Includes (pretty much compulsory for using the Teensy this semester)
 * 	- avr/io.h: port and pin definitions (i.e. DDRB, PORTB, PB1, etc)
 *
 *	- cpu_speed.h: macros for forcing the CPU speed to 8MHz (nothing else should ever be used!)
 */
 
 //THIS IS A VARIATION OF THE turn_on_led_solution1.c
 //USING FUNCTIONS. CONSIDER ADOPTING THIS PRACTICE.
 
#include <avr/io.h>

// Include the CPU Speed information
#include "cpu_speed.h"
#include "macros.h"


//perform all the operation to setup registers, configure pins, 
//and setup timers
void setup() {
	set_clock_speed(CPU_8MHz);

	// Enable left LED for output
	SET_BIT(DDRB, 2);

	// Turn the LED on
	SET_BIT(PORTB, 2);
}

//this is the fucntion that is executed every loop iteration and contains 
//the main operation the program is intended to do
void process() {
	// Do nothing this time. since we anly want to turn the LED once, and this 
	// method is called all the time form the for loop
}

int main(void) {

	//setup avr
	setup();
	
	//main loop
	for ( ;; ) {
		process();
	}
}
