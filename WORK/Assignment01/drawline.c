/**
 *  draw_formatted_lines.h, by Lawrence Buckingham.
 *  (C) 2017 Queensland University of Technology
 *  
 *  A utility function to render text across multiple lines in a ZDK(TM)
 *  program.
 */
#pragma once

/**
 *  Draws one or more lines of formatted text starting at the specified
 *  location. Total length of text must be less than 1000 characters.
 *  Parameters:
 *      x - the horizontal position at which the text will start.
 *      y - the vertical position at which the text will start.
 *      format - a standard C format string, consistent with 
 *          printf/fprintf/sprintf.
 *      ... - a variable length argument list which will provide further 
 *          arguments as required.
 */

void draw_formatted_lines(int x, int y, const char * format, ...);
The definition of the function is placed in draw_formatted_lines.c.

/**
 *	draw_formatted_lines.c, by Lawrence Buckingham.
 *	(C) 2017 Queensland University of Technology
 *
 *	A utility function to render text across multiple lines in a ZDK(TM)
 *	program.
 * 
 *	To compile the code to produce a stand-alone test driver, define symbol DFL_GENERATE_MAIN:
 *		gcc draw_formatted_lines.c -DDFL_GENERATE_MAIN -std=gnu99 -Wall -Werror -I${ZDK_FOLDER}  -L${ZDK_FOLDER} -lzdk -lncurses -g -o test_formatted_lines
 *		where ${ZDK_FOLDER} is the location of your ZDK folder.
 *
 *	To compile this function into a program without the test driver:
 *		gcc your_file_1.c your_file_2.c ... draw_formatted_lines.c ...
 *
 *	Programs that use the function should place draw_formatted_lines.h somewhere in the include
 *	path and #include it.
 */
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <cab202_graphics.h>
#include "draw_formatted_lines.h"

/**
 *  Draw one or more lines of formatted text starting at the specified
 *  location. Total length of text must be less than 1000 characters.
 *  Parameters:
 *      x0 - the horizontal position at which the text will start.
 *      y0 - the vertical position at which the text will start.
 *      format - a standard C format string, consistent with
 *          printf/fprintf/sprintf.
 *      ... - a variable length argument list which will provide further
 *          arguments as required.
 */

void draw_formatted_lines(int x0, int y0, const char * format, ...) {
    #define BUFF_SIZE 1000
    char buffer[BUFF_SIZE];

    // Get access to the variable argument list
    va_list args;
    va_start(args, format);

    // Do formatted output into the buffer.
    vsnprintf(buffer, BUFF_SIZE, format, args);

    // (x,y) is the location at which the next character will be written.
    int x = x0;
    int y = y0;

    // For each character in the formatted part of the buffer 
    for ( int i = 0; (i < BUFF_SIZE) && (buffer[i] != 0); i++ ) {
        // Get the current character
        char ch = buffer[i];

        if ( ch == '\n' ) {
            // Linefeed: advance to start of next line.
            x = x0;
            y++;
        }
        else {
            // Draw a normal char
            draw_char(x, y, ch);
            x++;
        }
    }
}

#if defined(DFL_GENERATE_MAIN) 
/**
 *	Helper function that waits for a key press
 */
void pause() {
    char * message = "Press a key to continue...";
    int x = (screen_width() - strlen(message)) / 2;
    int y = screen_height() - 1;
    draw_string(x, y, message);
    show_screen();
    wait_char();
}

/**
 *	Test driver for draw_formatted_lines.
 */
int main() {
    setup_screen();

    int test_num = 1;
    int y = screen_height() / 3;

    clear_screen();
    draw_formatted(0, 0, "Test %d: %s", test_num++, "Draw a single character on the left");
    draw_formatted_lines(0, y, "%c", '@');
    pause();

    clear_screen();
    draw_formatted(0, 0, "Test %d: %s", test_num++, "Draw a single character on the left);
    draw_formatted_lines(0, y, "%c\n\n\n%c", '@', '@');
    pause();

    clear_screen();
    draw_formatted(0, 0, "Test %d: %s", test_num++, "Draw a right-justified list of numbers in the middle of the screen.");
    draw_formatted_lines((screen_width() - 6) / 2, y, "%6.3f\n%6.3f\n%6.3f\n%6.3f\n%6.3f\n", 1.0, sqrt(2), sqrt(3), sqrt(4), sqrt(5));
    pause();

    cleanup_screen();
    return 0;
}
#endif