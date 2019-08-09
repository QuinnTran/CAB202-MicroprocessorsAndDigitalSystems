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
float y_speed = -1;
float gravity = 0.01;

uint8_t paused = 0;

char str[20];

int level_start_time;
double start_time = 0;
int end_game = 0;

int live = 10, score = 0;
int num_food = 5, num_zombies = 5;
int num_gblock = 7, num_bblock = 2;

Sprite player, treasure, block, starting_block, goodBlock[30], badBlock[10];
Sprite zombie[5], food;
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

int gametime;
void game_pauses(){
    gametime = (int)(timer() - start_time);
    clear_screen();

    draw_string(10, 0, "P A U S E D",FG_COLOUR);
    //   Lives remaining
    sprintf(str, "Live: %d",live);
    draw_string(10, 8, str, FG_COLOUR);
    //   Current score
    sprintf(str, "Score: %d",score);
    draw_string(10, 16, str, FG_COLOUR);
    //   Game time in mm:ss format
    sprintf(str, "Time: %d:%d", gametime / 60, gametime % 60);
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
    sprintf(str, "Time: %d", gametime);
    draw_string(5, 24, str, FG_COLOUR);
    sprintf(str, "Position:(%d,%d)", (int)player.x, (int)player.y);
    draw_string(5, 32, str, FG_COLOUR);
    draw_string(5, 40, "Continue?", FG_COLOUR);
    show_screen();
}


//--------------------------------Assignment 1 code------------------------------------------//

int border_collide( Sprite s){
    int xpos = (int)round(s.x);
    int xsize = xpos + s.width - 1;

    int ypos = (int)round(s.y);
    int ysize = ypos + s.height - 1;

    if(xpos < 0 || xsize >= LCD_X){
        return 1;
    }
    if(ypos < 0 || ysize >= LCD_Y){
        return 1;
    } else {
        return 0;
    }
}

int sprite_collide(Sprite s1, Sprite s2){
    int s1x = round(s1.x), s2x = round(s2.x);
    int s1y = round(s1.y), s2y = round(s2.y);
    if (s1x > s2x + s2.width - 1 || s2x > s1x + s1.width - 1 || s1y > s2y + s2.height - 1 || s2y > s1y + s1.height - 1){
        return 0;
    } else{
        return 1;
    }
}

int sprites_collide_any( Sprite s, Sprite sprites[], int n ){
    for ( int i = 0; i < n; i++ ){
        if ( sprite_collide( s, sprites[i] ) ){
            return 1;
        }
    }
    return 0;
}

int sprite_collide_count(Sprite s, Sprite sprites[], int n ){
    int count = 0;

    for ( int i = 0; i < n; i++ ){
        if ( sprite_collide( s, sprites[i] ) ){
            count++;
        }
    }
    return count;
}

void draw_sprites( Sprite sids[], int n ){
    for ( int i = 0; i < n; i++ ){
        sprite_draw( &sids[i] ); 
    }
}

int column(){
    int num_column = LCD_X / (BLOCK_W +2);
    return num_column;
}

int row(){
    int num_row = (LCD_Y - PLAYER_H - TREASURE_H) / (BLOCK_H + PLAYER_H + 2);
    return num_row;
}

Sprite setup_block (int good, int row, int col){
    int bx = col * (BLOCK_W + 2); 
    int by = (row * (BLOCK_H + PLAYER_H + 2)) + PLAYER_H + 1;

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

Sprite setup_treasure(){
    Sprite new_treasure;
    // Set up the treasure at the bottom of the screen.
    sprite_init(&new_treasure, rand_int(LCD_X), LCD_Y - 1 - TREASURE_H, TREASURE_W, TREASURE_H, treasure_jpg); 

    // Set the treasure in motion.
    new_treasure.dx = -1;

    return new_treasure;
}

int stop_treasure = 0;
void move_treasure(){
    if (BIT_IS_SET( PINF,5)){
        if (stop_treasure){
            stop_treasure = 0;
        } else {
            stop_treasure = 1;
        }
    }
    
    if (stop_treasure){
        return;
    }

    treasure.x += treasure.dx;

    int tx = (int)round(treasure.x);
    int tw = tx + treasure.width - 1;

    if(tx < 0 || tw >= LCD_X){
        treasure.x -= treasure.dx;
        treasure.dx = -treasure.dx;
    }
}
//-------------------------------------------------------------------------------------------------------//
void setup_food(){
    sprite_init(&food, LCD_X, LCD_Y, FOOD_W, FOOD_H, food_jpg);
    //food.is_visible = 0;
}

Sprite setup_zombie(int good, int col, int speed){
    int zx = rand_int(LCD_X-10) + 5;
    int zy = 1;

    Sprite new_zombie;
    sprite_init(&new_zombie, zx, zy, ZOMBIE_W, ZOMBIE_H, zombie_jpg);

    return new_zombie;
}

int zombie_landed[5];
void distribute_zombies(){
    for(int i = 0; i < num_zombies; i++){
       int rand_col = rand_int(column());
       zombie[i] = setup_zombie(1, rand_col, y_speed);
        zombie_landed[i] = 0;
        zombie[i].dx = 1;
    }
}


void move_zombies(){
    for(int i = 0; i < num_zombies; i++){
         int onland = 0;
        if (sprite_collide(zombie[i], starting_block)){
            onland = 1;
        }
        if (sprites_collide_any(zombie[i], goodBlock, num_gblock)){
            onland = 1;
        }
        if (onland){
            zombie_landed[i] = 1;
            zombie[i].x += zombie[i].dx;
        }else {
            if (zombie_landed[i]){
                zombie[i].dx = -1 * zombie[i].dx;
                zombie[i].x += zombie[i].dx;
            } else {
                zombie[i].y -= y_speed;
            }
        }
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
    sprintf(str, "Player dies - Live: %d , Score: %d, Time: %d\r\n",live, score, gametime);
    usb_serial_send( str);

    // - Player respawns - name of event, player x, player y.
    sprintf(str, "Player respawn - Player_x: %d,  Player_y: %d\n\n", (int)player.x, (int)player.y);
    usb_serial_send( str);

    // - Zombies appear - name of event, number of zombies, game time, player lives, player score.
    sprintf(str, "Zombie appear - No.of Zombies: %d, Time: %d, Live: %d, Score: %d", num_zombies, gametime, live, score);
    usb_serial_send( str);

    // - Zombies collide with Food - name of event, number of zombies on screen after collision,
    //   number of Food in inventory after collision, game time.
    sprintf(str, "Zombie collide with Food - No.of Zombie:%d, Inventory:%d, Time:%d", num_zombies, num_food, gametime);
    usb_serial_send( str);

    // - Player collides with treasure - name of event, score, lives, game time,
    //   position of player after returning to the top row,
    sprintf(str, "Player collides with treasure - Score:%d, Live:%d, Time:%d\r\n", score, live, gametime);
    usb_serial_send( str);

    // - Pause button pressed - name of event, player lives, player score, game time,
    //   number of zombies on screen, number of Food in inventory.
    sprintf(str, "Game Pause - Live:%d, Score:%d, Time:%d\r\n", live, score, gametime);
    usb_serial_send( str);

    // - Game over - name of event, player lives, player score, game time, total number of zombies fed.
    sprintf(str, "GameOver - Live:%d, Score:%d, Time:%d",live, score, gametime );
    usb_serial_send( str);
}

// Your program uses USB serial communication to control the Teensy Game. All requirements specifying joystick and buttons now also apply to the corresponding keyboard keys specified below:
void usb_serial_communication(){
    int key_code = -1;

	if ( BIT_IS_SET(PINB, 7) ) key_code = 's';// 's' drops the Food where the player stands,
	if ( BIT_IS_SET(PIND, 1) ) key_code = 'w';// 'w' lets the character jump,
	if ( BIT_IS_SET(PINB, 1) ) key_code = 'a';// 'a' moves the character left,
	if ( BIT_IS_SET(PIND, 0) ) key_code = 'd';// 'd' moves the character right,
	if( BIT_IS_SET(PINF, 5) ) key_code = 'q';// 'q' takes the game to the student number screen after game over.
	if( BIT_IS_SET(PINF, 6) ) key_code = 's';// 's' starts the game from the intro screen,
	if ( BIT_IS_SET(PINF, 5) ) key_code = 't';// 't' stops and starts the treasure movement,
	if ( BIT_IS_SET(PINB, 7) ) key_code = 'p';// 'p' pauses the game and shows the game information,
	if ( BIT_IS_SET(PINF, 6) ) key_code = 'r';// 'r' restarts the game after game over,

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
    sprite_draw(&food);
    draw_sprites(zombie, num_zombies);
    draw_sprites(goodBlock, num_gblock);
    draw_sprites(badBlock, num_bblock);
    sprite_draw(&block);
    show_screen();

}

void game_setup(){
    sprite_init(&player,5, 1,PLAYER_W, PLAYER_H, player_jpg);//display player
    sprite_init(&starting_block, 5, 1+PLAYER_H, BLOCK_W, BLOCK_H, good_block);//display starting block
    int overlapping = 0;
    do {
        overlapping = 0;
        distribute_block();
        for (int i = 0; i < num_gblock;i++){

            if (sprites_collide_any(goodBlock[i], badBlock, num_bblock) 
                || (sprite_collide_count(goodBlock[i], goodBlock, num_bblock) > 1)){
                overlapping = 1;
            }
            
        }

        for (int i = 0; i < num_bblock;i++){
            if (sprites_collide_any(badBlock[i], goodBlock, num_bblock)
                || (sprite_collide_count(badBlock[i], badBlock, num_bblock) > 1)){
                overlapping = 1;
            }
        }
    }while (overlapping == 1);

    level_start_time = timer();
    distribute_zombies();

    treasure = setup_treasure();//display treasure
    setup_food();
}

void respawn(){
    game_setup();
    live -= 1;
}

void move_player(){
    if (border_collide(player) == 1){
        respawn();
    }
    if (BIT_IS_SET(PINB, 1) && !((border_collide(player)) == 1)){ //left
        player.x -= player_speed;
    }
    if (BIT_IS_SET(PIND, 0) && !((border_collide(player)) == 1)){ //right
        player.x += player_speed;
    }
    int onland = 0;
    if (sprite_collide(player, starting_block)){
        onland = 1;
    }
    if (sprites_collide_any(player, goodBlock, num_gblock)){
        onland = 1;
    }
    if (onland){
        // if (BIT_IS_SET(PIND, 1)){ //up, only works from on block
        //     y_speed = 2;
        // }
    }else {
        player.y -= y_speed;
    }
    //y_speed -= gravity;
}


// Game mechanics are present:
void process(void){

    if (BIT_IS_SET( PINB,0)){
        if (paused){
            paused = 0;
        } else {
            paused = 1;
        }
    }
    
    if (paused){
        game_pauses();
        return;
    }

    if ( live == 0){
        gameover_screen();
        // f. The game over screen allows the player to restart by pressing SW3 (right button) and 
        //score, lives, time, and player position all reset or 
        if(BIT_IS_SET(PINF, 5)){
            game_setup();
        }
        //end the game by pressing SW2 (left button) which clears everything and just displays student number on the screen.
        if ( BIT_IS_SET(PINF, 6)){
            end_game = 1;
        }
        return;
    }
    clear_screen();

    move_player();
    move_zombies();
    move_treasure();

    //score +1 on every good block
    for(int j = 0; j < num_gblock; j++){
        if(sprites_collide_any(player, goodBlock, num_gblock)){
            score +=1;
        }
    }
    
    if(BIT_IS_SET(PINB, 7)){//drop food
        food.x = player.x;
        food.y = player.y;
        food.is_visible = 1;
        num_food -= 1;
    }

    if(sprites_collide_any(player, zombie, num_zombies)){
        respawn();
    }
    for(int i = 0; i < num_zombies; i++){
        if (sprite_collide(food, zombie[i])){
            zombie[i].is_visible = 0;
            score = score + 10;
            setup_food();
            num_food = num_food + 1;
        }
    }

    if(sprite_collide(player, treasure)){
        treasure.is_visible = 0;
        respawn();
        live += 2;
    }

    if(sprites_collide_any(player, badBlock, num_bblock)){
        respawn();
        live -= 1;
    }
    draw_all();
}

int main(void) {
    setup();

    while(!BIT_IS_SET(PINF, 6)){
        intro_screen();
        show_screen();
    }
    game_setup();
    start_time = timer();
    //game 
    for(;;) {
        //initialised
        if (end_game == 1){
            clear_screen();
            draw_string(20, 20, "n9808281", FG_COLOUR);
            show_screen();
        }else {
            process();
        }
        _delay_ms(100);
    }
}