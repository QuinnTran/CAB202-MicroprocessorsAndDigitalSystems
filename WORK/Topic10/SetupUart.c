#include <stdint.h>
#include <stdio.h>
#include <avr/io.h> 
#include <avr/interrupt.h>
#include <util/delay.h>
#include <cpu_speed.h>

#include <graphics.h>
#include <macros.h>
#include "lcd_model.h"


//	(a) Implement a function called setup_uart which requires no arguments and
//	returns no value.
void setup_uart(){
	//	(a.a) Set the data transmission rate to 2400 bits per second,
	//	using the formula provided in the Topic 10 Technical Notes.
	// uart_init(2400);
	//cli();
	int BAUD = 2400;
	UBRR1 = (F_CPU/4/BAUD -1)/2;
	//	(a.b) Set the UART speed to double speed.
	UCSR1A |= (1<<U2X1);
	//	(a.c) Enable receive and transmit, but do not enable any UART-related
	//	interrupts.
	UCSR1B |= (1<<TXEN1) | (1<<RXEN1); 
	//	(a.d) Set the character size to 6 bits, with no parity
	//	and 1 stop bit.
	UCSR1C |= (1<<UCSZ10);
//	(b) End of function setup_uart
	//sei();
}


int main(void) {
	setup_uart();

	for ( ;; ) {}
}
