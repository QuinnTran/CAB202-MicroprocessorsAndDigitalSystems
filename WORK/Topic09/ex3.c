#include <stdint.h>
#include <stdio.h>
#include <avr/io.h> 
#include <avr/interrupt.h>
#include <util/delay.h>
#include <cpu_speed.h>

#include <graphics.h>
#include <macros.h>
#include "lcd_model.h"

void setup(void) {
	set_clock_speed(CPU_8MHz);
	lcd_init(LCD_DEFAULT_CONTRAST);
	lcd_clear();

	//	(a) Initialise Timer 0 in normal mode so that it overflows 
	//	with a period of approximately 0.008 seconds.
	//	Hint: use the table you completed in a previous exercise.
	TCCR0A = 0;
	TCCR0B = 4; 
	//	(b) Enable timer overflow interrupt for Timer 0.
	TIMSK0 = 1; 
	//	(c) Turn on interrupts.
	sei();
	//	(d) Enable the right button switch for digital input.
	CLEAR_BIT(DDRF, 5);
	//	(e) Display your student number, "n9808281", with nominal
	//	top-left corner at screen location (16,26).
	draw_string(16, 26, "n9808281", FG_COLOUR);
	// Keep the next instruction intact.
	show_screen();
}
//	(f) Create a volatile global variable called bit_counter.
//	The variable should be an 8-bit unsigned integer. 
//	Initialise the variable to 0.
volatile uint32_t bit_counter = 0;
//	(g) Define a volatile global 8-bit unsigned global variable 
//	called is_pressed which will store the current state of the switch.
volatile uint32_t is_pressed = 0;
//	(h) Define an interrupt service routine to process timer overflow
//	interrupts for Timer 0. Every time the interrupt service
//	routine is called, bit_counter should:
ISR(TIMER0_OVF_vect) {
//	(h.a) Left-shift bit_counter one place;
//	(h.b) Bitwise AND with a mask in which the 3 bits on the right
//	are 1 and the others are 0.
	uint32_t mask = 0b00000111;
//	(h.c) Use bitwise OR to add the current open/closed value of the 
//		right button switch to the history.
	bit_counter = ((bit_counter << 1) & mask) | BIT_VALUE(PINF, 5);
//	(h.d) If bit_counter is equal to the bit mask, then the switch has been 
//		observed 3 times in a row to be closed. Assign the value 1 to 
//		is_pressed, indicating that the switch should now be considered to be
//		officially "closed".
	if(bit_counter == mask){
		is_pressed = 1;
	}
//	(h.e) If bit_counter is equal to 0, then the switch has been observed 
//		to be open at least 3 in a row, so store 0 in is_pressed, 
//		indicating that the switch should now be considered to be officially "closed".
	else if(bit_counter == 0){
		is_pressed = 0;
	}
}



// -------------------------------------------------
// Test driver.
// -------------------------------------------------

void process(void) {
	static uint8_t prevState = 0;
	if ( is_pressed != prevState ) {
		prevState = is_pressed;
		draw_string( 30, 40, prevState ? "closed" : "open  ", FG_COLOUR);
		show_screen();
	}
}

int main(void) {
	setup();

	for ( ;; ) {
		process();
	}
}
