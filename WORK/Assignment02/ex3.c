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

	TCCR0A = 0;
	TCCR0B = 4; 
	TIMSK0 = 1; 
	sei();
	CLEAR_BIT(DDRF, 5);
	draw_string(16, 26, "n9808281", FG_COLOUR);

	show_screen();
}

volatile uint32_t bit_counter = 0;
volatile uint32_t is_pressed = 0;
ISR(TIMER0_OVF_vect) {
	uint32_t mask = 0b00000111;
	bit_counter = ((bit_counter << 1) & mask) | BIT_VALUE(PINF, 5);
	if(bit_counter == mask){
		is_pressed = 1;
	}
	else if(bit_counter == 0){
		is_pressed = 0;
	}
}

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
