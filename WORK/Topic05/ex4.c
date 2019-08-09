#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <cab202_graphics.h>
#include <cab202_sprites.h>
#include <cab202_timers.h>


//  (Task 1) Define a function called pixel_level_collision which detects whether
//  two sprites s1 and s2 collide at pixel level. The function must be able 
//  to work correctly even if the ZDK screen has not been set up, and should 
//  make no attempt to use any function or variable declared in 
//  <cab202_graphics.h>, either directly or indirectly.
//
//  Parameters:
//      s1 - the address of a Sprite which has been initialised and placed at 
//      arbitrary position on the screen.
//
//      s2 - the address of a Sprite which has been initialised and placed at 
//      arbitrary position on the screen.
//
//  Returns:
//      A boolean value which is true if and only if there exists a point (x,y)
//      which is opaque in both s1 and s2.
// 
//      Definition: (x,y) is opaque in sprite s if and only if:
//      0 <= (sx = x - round(s->x)) < s->width, and
//      0 <= (sy = y - round(s->y)) < s->height, and
//      offset(s,x,y) = sy * s->width + sx, and
//      s->bitmap[offset(s,x,y)] != ' '.


bool pixel_level_collision ( sprite_id s1, sprite_id s2 )
{
    
    typedef struct coordinates_t{
        int x;
        int y;
    }coordinates_t;
    coordinates_t coords_s1[100];
    coordinates_t coords_s2[100];

    //get s1 opaque coords   
    int ctr1 = 0;
    for (int y = 0; y < s1->height; y++){
        for(int x = 0; x < s1->width; x++){

            int offset_s1 = y * s1->width + x;
            if(s1->bitmap[offset_s1] != ' '){
                coords_s1[ctr1].x = round(x + s1->x);
                coords_s1[ctr1].y = round(y + s1->y);
                ctr1++;
            }
        }
    }

    //get s2 opaque coords   
    int ctr2 = 0;
    for (int y = 0; y < s2->height; y++)
    {
        for(int x = 0; x < s2->width; x++)
        {
            int offset_s2 = y * s2->width + x;
            if(s2->bitmap[offset_s2] != ' ')
            {
                coords_s2[ctr2].x= round(x + s2->x);
                coords_s2[ctr2].y = round(y + s2->y);
                ctr2++;
            }
        }
    }

    for(int i = 0; i < ctr1; i++){
        for(int x = 0; x < ctr2; x++){
            if(coords_s1[i].x == coords_s2[x].x && coords_s1[i].y == coords_s2[x].y){
                return true;
            }
        }
    }
    return false;
}


char * cross = "y   y"
/**/           " y y "
/**/           "  y  "
/**/           " y y "
/**/           "y   y"
;
char * plus = "  z  "
/**/          "  z  "
/**/          "zzzzz"
/**/          "  z  "
/**/          "  z  "
;

int main( void )
{
    setup_screen();
    sprite_id s_cross = sprite_create( 20.9, 8.3, 5, 5, cross );
    sprite_id s_plus = sprite_create( 30.9, 17.9, 5, 5, plus );

    while ( true )
    {
        clear_screen();
        sprite_draw( s_cross );
        sprite_draw( s_plus );
        if ( pixel_level_collision( s_cross, s_plus ) )
        {
            draw_string(0, 0, "Pixel-level collision has been detected!");
        }
        show_screen();

        int key = wait_char();

        if ( key == '2' )
        {
            s_plus->y++;
        }
        else if ( key == '8' )
        {
            s_plus->y--;
        }
        else if ( key == '4' )
        {
            s_plus->x--;
        }
        else if ( key == '6' )
        {
            s_plus->x++;
        }
        else if ( key == 'q' ) {
            break;
        }
    }

    cleanup_screen();

    return 0;
}
