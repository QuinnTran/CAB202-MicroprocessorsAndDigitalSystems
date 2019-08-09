// Include the AVR IO library
#include <avr/io.h>

// Include the CPU Speed information
#include "cpu_speed.h"
#include "macros.h"

void setup() {
	set_clock_speed(CPU_8MHz);

	// Output to left LED
	SET_BIT(DDRB, 2);

	// Input from the left and right buttons
	CLEAR_BIT(DDRF, 5);
	CLEAR_BIT(DDRF, 6);
}

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
	setup();

	for ( ;; ) {
		process();
	}
}