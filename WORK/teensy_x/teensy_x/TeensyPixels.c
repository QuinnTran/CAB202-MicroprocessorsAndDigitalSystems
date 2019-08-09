#include <stdint.h>
#include <stdlib.h>
#include <avr/io.h> 
#include <avr/interrupt.h>
#include <util/delay.h>
#include <cpu_speed.h>

#include <graphics.h>
#include <macros.h>

#define N 200
int x[N];
int y[N];
int current_pixel = 0;

void init_pixels(void) {
	for ( int i = 0; i < N; i++ ) {
		x[i] = rand() % LCD_X;
		y[i] = rand() % LCD_Y;
	}
}

void draw_pixels(void) {
	for ( int i = 0; i < N; i++ ) {
		draw_pixel(x[i], y[i], FG_COLOUR);
	}
}

void update_pixels() {
	x[current_pixel] = rand() % LCD_X;
	y[current_pixel] = rand() % LCD_Y;
	current_pixel = (current_pixel + 1) % N;
}

void setup(void) {
	set_clock_speed(CPU_8MHz);
	lcd_init(LCD_DEFAULT_CONTRAST);
	clear_screen();
	init_pixels();
	draw_pixels();
	show_screen();
}

void process(void) {
	clear_screen();
	update_pixels();
	draw_pixels();
	show_screen();
}

int main(void) {
	setup();

	for ( ;; ) {
		process();
		_delay_ms(10);
	}
}
