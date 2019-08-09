#include <stdlib.h>
#include <cab202_graphics.h>
#include <cab202_timers.h>

//  (a) Declare four global integer variables, as follows:
//      left - the horizontal location of the left edge of the rectangle
//      top - the vertical location of the top edge of the rectangle 
//      width - the width of the rectangle.
//      height - the height of the rectangle.
int left = 0;
int top = 0;
int width;
int height;

//  (b) Declare a global variable of type char called ch.
//      This is the character that is to be used to render the rectangle.
char ch;

void draw_rect(void) {
    //  (c) Insert code to draw the outline of the rectangle defined by the global variables.
    //      If either of the width or height is less than or equal to zero,
    //      the function must not draw anything.
    draw_line(left, top, screen_width()-1, top, '*');
    draw_line(left, height, screen_width()-1, screen_height()-1, '*');
    draw_line(left, top, left, screen_height()-1, '*');
    draw_line(screen_width()-1, top, screen_width()-1, screen_height()-1, '*');
}

int main(void) {
    setup_screen();

    // draw a box.
    left = rand() % screen_width() / 2;
    top = rand() % screen_height() / 2;
    width = 1 + rand() % (screen_width() - left - 1);
    height = 1 + rand() % (screen_height() - top - 1);
    ch = '@';
    draw_rect();
    show_screen();

    // draw a box.
    left = rand() % screen_width() / 2;
    top = rand() % screen_height() / 2;
    width = 1 + rand() % (screen_width() - left - 1);
    height = 1 + rand() % (screen_height() - top - 1);
    ch = '&';
    draw_rect();
    show_screen();

    // draw a box with zero width.
    left = rand() % screen_width() / 2;
    top = rand() % screen_height() / 2;
    width = 0;
    height = 1 + rand() % (screen_height() - top - 1);
    ch = '*';
    draw_rect();
    show_screen();

    // draw a box.
    left = rand() % screen_width() / 2;
    top = rand() % screen_height() / 2;
    width = 1 + rand() % (screen_width() - left - 1);
    height = 1 + rand() % (screen_height() - top - 1);
    ch = '#';
    draw_rect();
    show_screen();

    // draw a box with negative width.
    left = rand() % screen_width() / 2;
    top = rand() % screen_height() / 2;
    width = -rand() % screen_width();
    height = 1 + rand() % (screen_height() - top - 1);
    ch = '!';
    draw_rect();
    show_screen();

    // draw a box.
    left = rand() % screen_width() / 2;
    top = rand() % screen_height() / 2;
    width = 1 + rand() % (screen_width() - left - 1);
    height = 1 + rand() % (screen_height() - top - 1);
    ch = '+';
    draw_rect();
    show_screen();

    // draw a box with zero height.
    left = rand() % screen_width() / 2;
    top = rand() % screen_height() / 2;
    width = 1 + rand() % (screen_width() - left - 1);
    height = 0;
    ch = 'a';
    draw_rect();
    show_screen();

    // draw a box.
    left = rand() % screen_width() / 2;
    top = rand() % screen_height() / 2;
    width = 1 + rand() % (screen_width() - left - 1);
    height = 1 + rand() % (screen_height() - top - 1);
    ch = 'b';
    draw_rect();
    show_screen();

    // draw a box with negative width.
    left = rand() % screen_width() / 2;
    top = rand() % screen_height() / 2;
    width = -rand() % screen_width();
    height = 1 + rand() % (screen_height() - top - 1);
    ch = 'c';
    draw_rect();
    show_screen();

    // draw a box.
    left = rand() % screen_width() / 2;
    top = rand() % screen_height() / 2;
    width = 1 + rand() % (screen_width() - left - 1);
    height = 1 + rand() % (screen_height() - top - 1);
    ch = 'd';
    draw_rect();
    show_screen();

    timer_pause(5000);
    cleanup_screen();
    return 0;
}
