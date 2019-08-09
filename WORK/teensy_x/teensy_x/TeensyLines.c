#include <stdint.h>
#include <avr/io.h> 
#include <avr/interrupt.h>
#include <util/delay.h>
#include <cpu_speed.h>

#include <graphics.h>
#include <macros.h>

void setup(void) {
	
	set_clock_speed(CPU_8MHz);
	lcd_init(LCD_DEFAULT_CONTRAST);
	clear_screen();
	show_screen();
}

void process(void) {
	static double fraction = 0.0;
	clear_screen();

	int x1 = LCD_X * fraction;
	int x2 = LCD_X - x1;
	int y1 = LCD_Y * fraction;
	int y2 = LCD_Y - y1;
	
	

	fraction += 0.01;

	if ( fraction >= 0.5 ) fraction = 0.0;

	draw_line(x1, y1, x2, y1, BG_COLOUR);
    draw_line(x1, y2, x2, y2, BG_COLOUR);
	draw_line(x1, y1, x1, y2, BG_COLOUR);
	draw_line(x2, y1, x2, y2, BG_COLOUR);

	show_screen();
}

int main(void) {
	setup();

	for ( ;; ) {
		process();
		 _delay_ms(10);
	}
}
