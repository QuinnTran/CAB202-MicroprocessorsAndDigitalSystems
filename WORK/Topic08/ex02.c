#include <avr/io.h>
#include <util/delay.h>
#include <cpu_speed.h>
#include <macros.h>
#include <graphics.h>
#include <lcd.h>
#include "lcd_model.h"

void setup( void ) {
	set_clock_speed(CPU_8MHz);

	//	(a) Enable input from the joystick-up switch and joystick-down switch.
	CLEAR_BIT(DDRD, 1);
	CLEAR_BIT(DDRB, 7);
	//	(b) Initialise the LCD display using the default contrast setting.
	lcd_init(LCD_DEFAULT_CONTRAST);	
	//	(c) Use one of the functions declared in "graphics.h" to display 
	//	your student number, "n9808281", using the foreground colour, 
	//	positioning the left edge of the text at 14 and the nominal top 
	//	of the text at 10.
	draw_string(14, 10, "n9808281", FG_COLOUR);
	//	(d) Use one of the functions declared in "graphics.h" to copy the contents 
	//	of the screen buffer to the LCD.
	show_screen();
}

//	(e) Declare a global variable of type int named Contrast and 
//	initialise it to the default LCD contrast.
int Contrast = LCD_DEFAULT_CONTRAST;

void process(void) {
	//	(f) Test pin corresponding to joystick-down switch. If closed,
	//	decrement Contrast by 7. If the result is less than zero,
	//	set Contrast to the maximum permitted LCD contrast value.
	if (BIT_IS_SET(PINB, 7)) {
		Contrast -=7;
	}
	if(Contrast < 0) Contrast = 127;
	//	(g) Test pin corresponding to joystick-up switch. If closed,
	//	increment Contrast by 7. If the result is greater
	//	then the maximum permitted LCD contrast value, set Contrast 
	//	to zero.
	if (BIT_IS_SET(PIND, 1)) {
		Contrast +=7;
	}
	if ( Contrast > 127 ) Contrast = 0;
	//	(h) Send a sequence of commands to the LCD to enable extended
	//	instructions, set contrast to the current value of Contrast, 
	//	and finally retore the LCD to basic instruction mode.

	LCD_CMD( lcd_set_function, lcd_instr_extended );
	LCD_CMD( lcd_set_contrast, Contrast );
	LCD_CMD( lcd_set_function, lcd_instr_basic );
}


int main(void) {
	setup();

	for ( ;; ) {
		process();
		_delay_ms(100);
	}

	return 0;
}
