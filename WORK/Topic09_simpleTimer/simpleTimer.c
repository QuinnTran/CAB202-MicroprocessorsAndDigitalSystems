


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



void setup(){

 //set clock speed to 8MhZ
   	set_clock_speed(CPU_8MHz);
    
    lcd_init( LCD_DEFAULT_CONTRAST ); // initliase LCD screen
	clear_screen(); //clear screen

	
    DDRB |= 1 << PINB3; //PIN 3 PORTB for LED1 = output
    PORTB ^= 1 << PINB3; // 
    DDRB |= 1 << PINB2;  // //PIN 2 PORTB for LED2 = output
    PORTB |= 1 << PINB2; //
    
    
	
	
}


void TimerInit(void)
{
    //Set to Normal Timer Mode using TCCR0A and B
    TCCR0A = 0; 
    TCCR0B &= ~((1<<WGM02));
    
    //Set Prescaler using TCCR0B, using Clock Speed find timer speed = 1/(Clock Speed/Prescaler)
    //Prescaler = 1024
    //Timer Speed = 128 micro seconds
    //Timer Overflow Speed = 32678 micro seconds (Timer Speed * 256) - (256 since 8-bit timer)
    TCCR0B |= (1<<CS02)|(1<<CS00);
    TCCR0B &= ~((1<<CS01));
    
   
}

//temp buffer
char buffer[20];

void process(void){

		double curr_time =  TCNT0 * PRESCALE / FREQ;
    
        clear_screen();
        //be careful with the comparison value
        //led might flash too quick causing  health issues
        if ( curr_time > 0.03){
         PORTB ^= (1<<PINB3);        // toggle LED 1 at a frequency
         PORTB ^= (1<<PINB2);        // toggle LED 2 at a frequency
        }
        snprintf(buffer, sizeof(buffer), "%f", curr_time);
        draw_string(0, 10, "TCNT0 = ", FG_COLOUR);
        draw_string(42,10,buffer,FG_COLOUR);  // print value of the counter
        show_screen();
            
       // _delay_ms(100);//for visualization purposes

}

int main(void) {
    
    //initialise ports and LCD screen
	setup();
    
    //initialiase timer
    TimerInit();

    //initial mesasge
    clear_screen();
    draw_string(4,4,"Simple Timer",FG_COLOUR);
    show_screen();
    
    //stop for 2 sec
    _delay_ms(2000);
   
   for (;;){
    process();
   }
                

	return 0;
}	





