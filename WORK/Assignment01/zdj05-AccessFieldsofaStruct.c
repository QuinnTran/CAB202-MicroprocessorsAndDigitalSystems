#include <stdio.h>
#include <math.h>
#include <cab202_graphics.h>
#include <cab202_sprites.h>
#include <cab202_timers.h>

void get_screen_pos(sprite_id sprite, int *x, int *y);

// Accessing fields of a struct directly
//
// Uses a Sprite struct instead of a sprite_id.
// Initialises the fields of the Sprite explicitly.
// This lengthy initialisation is inherently more error-prone than use of sprite_create.
// Uses the address-of operator to send the sprite address to sprite_step, sprite_turn, sprite_draw, etc.
// Expands sprite_step in place to show what the code would look like if that function did not exist.
int main(void) {
    setup_screen();

    Sprite zombie;
    zombie.x = 10;
    zombie.y = 10;
    zombie.width = 1;
    zombie.height = 1;
    zombie.bitmap = "Z";
    zombie.dx = 0.2;
    zombie.dy = 0;
    zombie.is_visible = true;

    for ( ;; ) {
        clear_screen();

        //sprite_step(&zombie);
        zombie.x += zombie.dx;
        zombie.y += zombie.dy;

        sprite_turn(&zombie, -1);
        sprite_draw(&zombie);

        int screen_x, screen_y;
        get_screen_pos(&zombie, &screen_x, &screen_y);
        draw_formatted( 0, 0, "zombie is now at (%03d,%03d)", screen_x, screen_y );

        show_screen();
        timer_pause(10);
    }

    cleanup_screen();
}
// Accessing fields of a struct through a pointer
//
// Uses a Sprite struct instead of a sprite_id.
// Uses an alternative function sprite_init to initialise the fields of the Sprite.
// Must use the address-of operator to send the sprite address to sprite_step, sprite_turn, sprite_draw, etc.
// get_screen_pos now uses the arrow operator to get the contents of x and y.
int main(void) {
    setup_screen();

    Sprite zombie;
    sprite_init(&zombie, 10, 10, 1, 1, "Z");
    sprite_turn_to(&zombie, 0.2, 0);

    for ( ;; ) {
        clear_screen();
        sprite_step(&zombie);
        sprite_turn(&zombie, -1);
        sprite_draw(&zombie);

        int screen_x, screen_y;
        get_screen_pos(&zombie, &screen_x, &screen_y);
        draw_formatted( 0, 0, "zombie is now at (%03d,%03d)", screen_x, screen_y );

        show_screen();
        timer_pause(10);
    }

    cleanup_screen();
}

void get_screen_pos(sprite_id sprite, int *x, int *y) {
    *x = round(sprite_x(sprite));
    *y = round(sprite_y(sprite));