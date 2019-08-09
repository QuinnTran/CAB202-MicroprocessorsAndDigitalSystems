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
void setup_uart(void){
	//	(a.a) Set the data transmission rate to 2400 bits per second,
	//	using the formula provided in the Topic 10 Technical Notes.
	int baud = 2400;
	UBRR1 = (F_CPU/4/baud - 1)/2;
	//	(a.b) Set the UART speed to double speed.
	UCSR1A |= (1<<U2X1);
	//	(a.c) Enable receive and transmit, but do not enable any UART-related
	//	interrupts.
	UCSR1B |= (1<<TXEN1) | (1<<RXEN1); 
	//	(a.d) Set the character size to 7 bits, with odd parity
	//	and 2 stop bits.
	UCSR1C |= (1<<UCSZ11) | (1<<UPM11) | (1<<UPM10) | (1<<USBS1);
//	(b) End of function setup_uart
}


int main(void) {
	setup_uart();

	for ( ;; ) {}
}
