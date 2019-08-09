#include <avr/io.h>
#include <util/delay.h>
#include <cpu_speed.h>
#include <macros.h>
#include <graphics.h>
#include <lcd.h>

// Jenna Riseley 2018 - LCD screen direct write example 

// This program demonstrates the way bytes are written directly to the LCD screen, 
// by a cursor starting at the top left that advances by 1 position every time a new byte is written.
// When it reaches the end of a bank, it goes to the next bank and starts at the left again. 

// INSTRUCTIONS:
// Build and program the teensy with this program.
// Press sw2 or sw3.

void setup( void ) {
	set_clock_speed(CPU_8MHz);

	//	(a) Initialise the LCD display using the default contrast setting
	lcd_init(LCD_DEFAULT_CONTRAST);

// setup up as input
	SET_BIT(DDRF,5);  // sw2
	SET_BIT(DDRF,6); // sw3

	//	(c) Use one of the functions declared in "graphics.h" to copy the contents 
	//	of the screen buffer to the LCD.
	show_screen();
}


void process(void){

	// Write a byte to the screen if button is pressed

	// TODO: try changing the byte that is written to the screen . Eg try 0b11111111, 0b10101010...
	if(BIT_IS_SET(PINF,5) || BIT_IS_SET(PINF,6)){
		lcd_write(1,0b11110000); 
	}

}


int main(void) {
	setup();

	for ( ;; ) {
		_delay_ms(20); // This is not an elegant way of sampling button presses - we will learn a better way in week 10 
		process();
	}

	return 0;
}
