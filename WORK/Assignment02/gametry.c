#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <cpu_speed.h>
#include <sprite.h>
#include <graphics.h>
#include <lcd.h>
#include <macros.h>
#include <lcd_model.h>

#include "usb_serial.h"


#define FREQ     (8000000.0)
#define PRESCALE (1024.0)
//------------------------------characters-------------------------------//
//The player’s sprite is at least 3 pixels high and 3 pixels wide.
#define PLAYER_W 7
#define PLAYER_H 6
uint8_t player_jpg[] = {
    0b10111010,
    0b10101010,
    0b01101100,
    0b00111000,
    0b00101000,
    0b00101000,
};
uint8_t px, py;

// All blocks are:
// At least 2 pixels high, 10 pixels wide and are clearly distinguished from each other (through spacing 
// between sprites or other visual design),
#define BLOCK_W 10
#define BLOCK_H 2
uint8_t good_block[] = {
    0b11111111,0b11000000,
    0b11111111,0b11000000,
};

uint8_t bad_block[] = {
    0b10101010,0b10000000,
    0b01010101,0b01000000,
};

// There is a treasure sprite that:
// a. Is no larger than the player's sprite and has a clearly distinct image.
#define TREASURE_W 6
#define TREASURE_H 5
uint8_t treasure_jpg[] = {
    0b00111100,
    0b01111110,
    0b11111111,
    0b11111111,
    0b11111111,
};

#define ZOMBIE_W 4
#define ZOMBIE_H 4
uint8_t zombie_jpg[] = {
    0b11110000,
    0b00100000,
    0b01000000,
    0b11110000,
};

#define FOOD_W 3
#define FOOD_H 3
uint8_t food_jpg[] = {
    0b01000000,
    0b11100000,
    0b10100000,
};

//------------------------------global var-------------------------------//
int player_speed = 1;
float x_speed;
float y_speed;
float gravity = 0.001;

uint8_t paused = 0;
uint8_t isPressed = 0;

char str[20];

double start_time = 0;

int live = 10, score = 0;
int num_food = 5, num_zombies = 5;
int num_gblock = 7, num_bblock = 2;

Sprite player, treasure, block, starting_block, goodBlock[30], badBlock[10];
Sprite zombie[5], food[5];
//-------------------------------------Assignment 2 code------------------------------------------//
void setup(void){
    set_clock_speed(CPU_8MHz);
	lcd_init(LCD_DEFAULT_CONTRAST);

    //Timer
    TCCR3A = 0;
    TCCR3B = 4;
    TIMSK3 = 1;
    sei();
    //LED
    SET_BIT(DDRB, 2); //LED0
	SET_BIT(DDRB, 3); //LED1
    CLEAR_BIT(PORTB, 2);
    CLEAR_BIT(PORTB, 3);
    //Joystick
	CLEAR_BIT(DDRB, 1); //left
	CLEAR_BIT(DDRD, 0); //right
    CLEAR_BIT(DDRD, 1); //up
	CLEAR_BIT(DDRB, 7); //down
    CLEAR_BIT(DDRB, 0); //center
    //Button
    CLEAR_BIT(DDRF, 6); //sw1-left
    CLEAR_BIT(DDRF, 5); //sw2-right
}

void intro_screen(){
    clear_screen();
    draw_string(15, 10, "Quinn",FG_COLOUR);
    draw_string(15, 20, "n9808281",FG_COLOUR);
    show_screen();
}

volatile uint32_t overflow_count = 0;

double timer(){
    return ( overflow_count * 65536.0 + TCNT3 ) * PRESCALE  / FREQ;
}

ISR(TIMER3_OVF_vect) {
    overflow_count ++;
}

//get random integer
int rand_int(int range){
    srand((unsigned)timer() + rand() % 100);
    return rand() % range;
}

void game_pauses(){
    int gametime = (int)(timer() - start_time);
    int min = gametime / 60, sec = gametime % 60;

    clear_screen();

    draw_string(10, 0, "P A U S E D",FG_COLOUR);
    //   Lives remaining
    sprintf(str, "Live: %d",live);
    draw_string(10, 8, str, FG_COLOUR);
    //   Current score
    sprintf(str, "Score: %d",score);
    draw_string(10, 16, str, FG_COLOUR);
    //   Game time in mm:ss format
    sprintf(str, "Time: %d:%d", min, sec);
    draw_string(10, 24, str, FG_COLOUR);
    //   Number of Zombies on screen.
    sprintf(str, "Zombies: %d",num_zombies);
    draw_string(10, 32, str, FG_COLOUR);
    //   Number of Food in inventory.
    sprintf(str, "Inventory: %d",num_food);
    draw_string(10, 40, str, FG_COLOUR);

    show_screen();
}

void gameover_screen(){
    clear_screen();
    draw_string(5, 0, "GAME OVER", FG_COLOUR);

    sprintf(str, "Score: %d", score);
    draw_string(5, 8, str, FG_COLOUR);

    sprintf(str, "Live: %d", live);
    draw_string(5, 16, str, FG_COLOUR);

    sprintf(str, "Time: %.2f", timer());
    draw_string(5, 24, str, FG_COLOUR);

    sprintf(str, "Position: x-%d, y-%d", (int)player.x, (int)player.y);
    draw_string(5, 32, str, FG_COLOUR);

    draw_string(5, 40, "Continue?", FG_COLOUR);
    
    show_screen();
    _delay_ms(200);
}

int border_collide(Sprite *s){
    int sx = round(s->x);
    int sy = round(s->y);
    
    while (sy < 2) {
        s->y = ++sy;
    }
    while (sy + s->height - 1 >= LCD_Y - 1) {
        s->y = --sy;
    }
    
    while (sx < 2) {
        s->x = ++sx;
    }
    while (sx + s->width - 1 >= LCD_X - 1) {
        s->x = --sx;
    }
    return 0;
}
//--------------------------------Assignment 1 code------------------------------------------//
int sprite_collide(Sprite s1, Sprite s2){
    int s1x = round(s1.x), s2x = round(s2.x);
    int s1y = round(s1.y), s2y = round(s2.y);
    if (s1x > s2x + s2.width - 1 || s2x > s1x + s1.width - 1 || s1y > s2y + s2.height - 1 || s2y > s1y + s1.height - 1){
        return 0;
    } else{
        return 1;
    }
}

// int sprites_collide(Sprite s, Sprite ss[], int n){
//     for (int i = 0; i < n; i++){
//         if (sprite_collide(ss[i], s)) {
//             return 1;
//         }
//     }
//     return 0;
// }

int sprites_collide_any( Sprite s, Sprite sprites[], int n ){
    for ( int i = 0; i < n; i++ ){
        if ( sprite_collide( s, sprites[i] ) ){
            return 1;
        }
    }
    return 0;
}

void draw_sprites( Sprite sids[], int n ){
    for ( int i = 0; i < n; i++ ){
        sprite_draw( &sids[i] ); 
    }
}

int column(){
    int num_column = LCD_X / BLOCK_W +2;
    return num_column;
}

int row(){
    int num_row = (LCD_Y - PLAYER_H - TREASURE_H) / (BLOCK_H + PLAYER_H + 2);
    return num_row;
}

void auto_move( Sprite sid ){
    // Get screen location of block.
    int bx = round( sid.x);
    int by = round( sid.y );
    // Get the displacement vector of the block.
    double bdx = sid.dx;
    double bdy = sid.dy;
    // Test to see if the block hit the left or right border.
    if ( bx == 0 || bx == LCD_X - sid.width ){
        bdx = -bdx;
    }
    // Test to see if the block hit the top or bottom border.
    if ( by == 0 || by == LCD_Y- sid.height ){
        bdy = -bdy;
    }
    // Test to see if the block needs to step back and change direction.
    if ( bdx != sid.dx || bdy != sid.dy ){
        sid.x -= sid.dx;
        sid.dx -= sid.dx;
    }
}

void auto_move_all( Sprite sprites[], int n )
{
    for ( int i = 0; i < n; i++ )
    {
        auto_move( sprites[i] );
    }
}

Sprite setup_block (int good, int row, int col){
    int bx = col * (BLOCK_W + 2); 
    int by = (row * (BLOCK_H + PLAYER_H + 2)) + PLAYER_H+1;

    Sprite new_block;

    if(good == 1){
        sprite_init( &new_block, bx, by, BLOCK_W, BLOCK_H, good_block );
    }
    else{
        sprite_init( &new_block, bx, by, BLOCK_W, BLOCK_H, bad_block );
    }
    return new_block;
}

void distribute_block(){
    for(int i = 0; i < num_gblock ; i++){
       int rand_row = rand_int(row());
       int rand_col = rand_int(column());
       goodBlock[i] = setup_block(1,rand_row, rand_col);
       
    }
    for(int j = 0; j < num_bblock ; j++){
       int rand_row = rand_int(row());
       int rand_col = rand_int(column());
       badBlock[j] = setup_block(0,rand_row, rand_col);
    }

}

// There is a treasure sprite that:
// b. Does not overlap any of the blocks.
// c. Spawns in the bottom half of the screen and moves back and forward, changing horizontal direction when it reaches the edges of the screen.
// e. Disappears when the player collides with it and gives the player 2 more lives and returns the player to the 'starting block'.
// f. Does not reappear again unless the game is restarted.
Sprite setup_treasure(){
    Sprite new_treasure;
    // Set up the treasure at the bottom of the screen.
    sprite_init(&new_treasure, rand_int(LCD_X), LCD_Y - 1 - TREASURE_H, TREASURE_W, TREASURE_H, treasure_jpg); 

    // Set the treasure in motion.
    new_treasure.x = new_treasure.x * -1;

    return new_treasure;
}
//-------------------------------------------------------------------------------------------------------//
Sprite setup_food(int col, int row){
    int fx = rand_int(LCD_X);
    int fy = rand_int(LCD_Y);

    Sprite foods;
    sprite_init(&foods, fx, fy, FOOD_W, FOOD_H, food_jpg);

    return foods;
}

void distribute_food(){
    for(int i = 0; i <= num_food; i++){
       int rand_col = rand_int(column());
       int rand_row = rand_int(row());
       food[i] = setup_food(rand_col, rand_row);
    }
}

Sprite setup_zombie(int good, int col, int speed){
    int zx = rand_int(LCD_X);
    int zy = LCD_Y - 49;

    Sprite new_zombie;
    sprite_init(&new_zombie, zx, zy, ZOMBIE_W, ZOMBIE_H, zombie_jpg);

    return new_zombie;
}

void distribute_zombies(){
    y_speed += gravity;
    
    for(int i = 0; i < num_zombies; i++){
       int rand_col = rand_int(column());
       zombie[i] = setup_zombie(1, rand_col, y_speed);
    }
}
// Pressing the joystick LEFT or RIGHT has the following effect:
// a. Pressing the joystick left or joystick right moves the character one step left or right, respectively,
// b. If the player is not supported by a block, it will move vertically toward the bottom of the screen. 
//    For Basic Functionality, realistic physics such as acceleration and horizontal momentum, are not required.
// c. The joystick has no effect on the player's motion when the player is not supported by a block.
// d. If the falling player collides with a safe block from above, then the player lands on the block. 
//    The player will then immediately stop moving.
// e. Under no circumstances may the visible pixels of the player sprite overlap with the visible pixels of a block.
void move_player(){
    //y_speed += gravity; 
    //BIT_IS_SET(PINB, 1)

    if (BIT_IS_SET(PINB, 1)){ //left
        player.x -= player_speed;
    }
    //BIT_IS_SET(PIND, 0)
    if (BIT_IS_SET(PIND, 0)){ //right
        player.x += player_speed;
    }

    if(BIT_IS_SET(PINB, 7)){
        //drop food
    }

    // Pressing the joystick UP while the player is supported by a block causes the player to jump.
    // After UP is pressed, the player should commence to move upwards. Any horizontal motion should continue, 
    //      and the acceleration provision of 10(e) will take effect.
    // Once UP is pressed, the joystick has no effect until the player lands on a block or dies.
    // Immediately upon landing on a block, the player's velocity changes to match that of the block so it is carried along.
    // If the player jumps off screen, the player dies.
    // The initial vertical velocity should be sufficient to allow the player to jump through gaps 
    //      between blocks and land on blocks on the row above.
    if (BIT_IS_SET(PIND, 1)){ //up
        y_speed = y_speed - 0.1;
    }
    
}
//-------------------------------report to serial-----------------------------------//
void usb_serial_send(char * message){
    return;
}

void serial_report(){
    // - Game starts - name of event, player x position, player y position.
    sprintf(str, "Game starts - player_x: %d  player_y: %d\n\n", (int)player.x, (int)player.y);
    usb_serial_send( str);

    // - Player dies - name of event, reason for death, lives after death, score, game time.
    sprintf(str, "Player dies - Live: %d , Score: %d, Time: %.2f\r\n",live, score, timer());
    usb_serial_send( str);

    // - Player respawns - name of event, player x, player y.
    sprintf(str, "Player respawn - Player_x: %d,  Player_y: %d\n\n", (int)player.x, (int)player.y);
    usb_serial_send( str);

    // - Zombies appear - name of event, number of zombies, game time, player lives, player score.
        sprintf(str, "Zombie appear - No. of zombies: %d, Time: %.2f, Live: %d, Score: %d", num_zombies, timer(), live, score);
        usb_serial_send( str);

    // - Zombies collide with Food - name of event, number of zombies on screen after collision,
    //   number of Food in inventory after collision, game time.
    //     sprintf(str, "", );
    //     usb_serial_send( str);

    // - Player collides with treasure - name of event, score, lives, game time,
    //   position of player after returning to the top row,
    sprintf(str, "Player collides with treasure - Score:%d, Live:%d, Time:%.2f\r\n", score, live, timer());
    usb_serial_send( str);

    // - Pause button pressed - name of event, player lives, player score, game time,
    //   number of zombies on screen, number of Food in inventory.
    sprintf(str, "Game Pause - Live:%d, Score:%d, Time:%.2f\r\n", live, score, timer());
    usb_serial_send( str);

    // - Game over - name of event, player lives, player score, game time, total number of zombies fed.
    sprintf(str, "GameOver - Live:%d, Score:%d, Time:.2%f",live, score, timer() );
    usb_serial_send( str);
}

// Your program uses USB serial communication to control the Teensy Game. All requirements specifying joystick and buttons now also apply to the corresponding keyboard keys specified below:
void usb_serial_communication(){
    int key_code = -1;

	// if ( BIT_IS_SET(PINB, 7) ) key_code = 's';// 's' drops the Food where the player stands,
	if ( BIT_IS_SET(PIND, 1) ) key_code = 'w';// 'w' lets the character jump,
	if ( BIT_IS_SET(PINB, 1) ) key_code = 'a';// 'a' moves the character left,
	if ( BIT_IS_SET(PIND, 0) ) key_code = 'd';// 'd' moves the character right,

	// if() key_code = 'q';// 'q' takes the game to the student number screen after game over.
	// if() key_code = 's';// 's' starts the game from the intro screen,
	// if (  ) key_code = 't';// 't' stops and starts the treasure movement,
	// if ( ) key_code = 'p';// 'p' pauses the game and shows the game information,
	// if (  ) key_code = 'r';// 'r' restarts the game after game over,

	if ( key_code > 0 ) {
		draw_string(0, 10, "TX:", FG_COLOUR);
		draw_char(3 * 5, 10, key_code, FG_COLOUR);
		usb_serial_putchar(key_code);
	}

	show_screen();
}

//-------------------------------------Setup game------------------------------------------//

void draw_all() {
    clear_screen();
    sprite_draw(&player);
    sprite_draw(&starting_block);
    sprite_draw(&treasure);

    draw_sprites(zombie, num_zombies);
    draw_sprites(food, num_food);

    draw_sprites(goodBlock, num_gblock);
    draw_sprites(badBlock, num_bblock);
    sprite_draw(&block);
    show_screen();
}


void game_setup(){
    // The player initially appears on a 'starting block' in the top row. 
    sprite_init(&player,5, 1,PLAYER_W, PLAYER_H, player_jpg);

    //draw starting block
    sprite_init(&block, 5, 1+PLAYER_H, BLOCK_W, BLOCK_H, good_block);

    //draw blocks
    distribute_block();
    for (int i = 0; i < 40;i++){
        if (sprites_collide_any(goodBlock[i], &block, num_gblock)){
            distribute_block();
        }
        if (sprites_collide_any(badBlock[i], &block, num_bblock)){
            distribute_block();
        }
    }
    // distribute_block();
    //draw treasure
    treasure = setup_treasure();
    // distribute_food();

    draw_all();
}


// The following game mechanics are present:
// a. The player starts with 10 lives.
// b. A point is scored every time the player lands on a safe block.
// c. The player dies if any part of the player sprite moves off the screen in any direction or manner, or if it collides with a forbidden block.
// d. On death, the player respawns on the 'starting block'.
void process(void){
    clear_screen();

    move_player();
    
    // e. When the player loses all their lives, the game over screen is displayed which displays 
    //a game over message, total score, and game play time.
    if ( live == 0){
        gameover_screen();
        // f. The game over screen allows the player to restart by pressing SW3 (right button) and 
        //score, lives, time, and player position all reset or 
        if(BIT_IS_SET(PINF, 5)){
            clear_screen();
            game_setup();
            show_screen();
        }
        //end the game by pressing SW2 (left button) which clears everything and just displays student number on the screen.
        if ( BIT_IS_SET(PINF, 6)){
            clear_screen();
            draw_string(20, 20, "n9808281", FG_COLOUR);
            show_screen();
        }
    }

    if (BIT_IS_SET( PORTB,0)){
        if (paused == 1){
            paused = 0;
        } else {
            paused = 1;
            game_pauses();
        }
    }

    // Treasure stops moving when SW3 (right button) is pressed and 
    // starts moving again if SW3 (right button) is pressed again.
    // while(paused == 0){
    //     if ( BIT_IS_SET(PINF, 5)){
    //         //treasure pause/
    //     }
    // }

    if(sprite_collide(player, treasure)){
        live +=2;
        sprite_init(&player,LCD_X/2+10, LCD_Y+1,PLAYER_W, PLAYER_H, player_jpg);
        sprite_draw(&player);
        setup_treasure();
    }
    
    // if(sprites_collide)
    // Both LED0 and LED1 flash at approximately 4Hz (synchronously or asynchronously) before 
    // the zombies spawn and continue to flash until all the zombies have landed or fallen off screen.
    draw_all();
}

int main(void) {
    setup();

    while(!BIT_IS_SET(PINF, 6)){
        intro_screen();
        show_screen();
    }
    game_setup();
    //game 
    for(;;) {
        
        process();
        //initialised
        start_time = timer();

        // if(timer() >= 3){
        //     distribute_zombies();
        // }


        _delay_ms(100);
    }
    
}