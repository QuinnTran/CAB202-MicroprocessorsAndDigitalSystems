#include <stdlib.h>
#include <cab202_graphics.h>

//  (a) Begin the definition a function called fill_rect that returns nothing, 
//      but requires the following arguments:
//      left - an integer that specifies the absolute horizontal location 
//          of the left hand side of the rectangle.
//      top - an integer that specifies the absolute vertical location 
//          of the upper edge of the rectangle.
//      right - an integer that specifies the absolute horizontal location 
//          of the right hand side of the rectangle.
//      bottom - an integer that specifies the absolute vertical location 
//          of the lower edge of the rectangle.
//      ch = a character value that will be used to draw the
//          display the rectangle.
void fill_rect(int left, int top, int right, int bottom, char ch){
    
    int width = right-left;
	int height = bottom-top;
	if (width < 0 || height < 0){
		
	}else{
    //  (c) Use statements of your own choice to fill the region defined by the
    //      first four arguments with the designated character value.
    //      There a many ways to achieve this, but any one of the following algorithms
    //      should suffice.
    
    // Option 1: Nested for loop; draw chars.
    //           FOR x = left to right
    //              FOR y = top to bottom
    //                 Draw char at (x,y)
    //
    // Option 2: Single loop; vertical lines.
    //           FOR x = left to right
    //              Draw line from (x,top) to (x,bottom)
    //
    // Option 3: Single loop; horizontal lines.
    //           FOR y = top to bottom
    //              Draw line from (left,y) to (right,y)
        for(int y = top; y <= bottom; y++){
		    draw_line(left, y , right, y, ch);
        }
    }
}


int main( void ) {
	int l, t, r, b;
	char c;

	printf( "Please enter the horizontal location of the left edge of the rectangle: " );
	scanf( "%d", &l );

	printf( "Please enter the vertical location of the top edge of the rectangle: " );
	scanf( "%d", &t );

	printf( "Please enter the horizontal location of the right edge of the rectangle: " );
	scanf( "%d", &r );

	printf( "Please enter the vertical location of the bottom edge of the rectangle: " );
	scanf( "%d", &b );

	printf( "Please enter the character used to draw the rectangle? " );
	scanf( " %c", &c );

	setup_screen();
	fill_rect( l, t, r, b, c );
	show_screen();
	wait_char();

	return 0;
}
