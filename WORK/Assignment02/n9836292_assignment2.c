/*
 n9836292
 CAB202 assignment2
 
 serial done?

 save settings
 
 
 DEBUG:
 loading time too much (entities positioning loop too much)
 respawn time too much
 need to attach adc.c?
 player movement speed
 
 */

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

//draw_char -  word height = 7

#define PI 3.14159265
#define FREQ 8000000.0
#define PRESCALE 256.0

#define MAP_X 126 //large:126
#define MAP_Y 72 //large:72

#define PLAYER_W 7
#define PLAYER_H 11
uint8_t player_image[] = {
    0b00101000,
    0b00111000,
    0b00101000,
    0b00111000,
    0b00000000,
    0b11111110,
    0b10101010,
    0b10111010,
    0b00101000,
    0b00101000,
    0b00101000,
};

#define MONSTER_W 5
#define MONSTER_H 6
uint8_t monster_image[] = {
    0b01110000,
    0b11111000,
    0b10101000,
    0b11111000,
    0b11111000,
    0b10101000,
};

#define BOMB_W 7
#define BOMB_H 6
uint8_t bomb_image[] = {
    0b00111000,
    0b01101100,
    0b11101110,
    0b11101110,
    0b01101100,
    0b00111000,
};

#define BOW_W 7
#define BOW_H 6
uint8_t bow_image[] = {
    0b00111110,
    0b01100100,
    0b11001000,
    0b10010000,
    0b10100000,
    0b11000000,
};

#define SHIELD_W 5
#define SHIELD_H 6
uint8_t shield_image[] = {
    0b10101000,
    0b11011000,
    0b11111000,
    0b11111000,
    0b01110000,
    0b00100000,
};

#define DOOR_W 12
#define DOOR_H 11
uint8_t door_image[] = {
    0b00001111, 0b00000000,
    0b00111111, 0b11000000,
    0b01110000, 0b11100000,
    0b01000000, 0b00100000,
    0b11000000, 0b00110000,
    0b11000000, 0b00110000,
    0b11000001, 0b10110000,
    0b11000001, 0b10110000,
    0b11000000, 0b00110000,
    0b11000000, 0b00110000,
    0b11000000, 0b00110000,
};

#define TOWER_W 50
#define TOWER_H (int)((MAP_Y + DOOR_H) / 2)
#define TOWER_X (int)((MAP_X - TOWER_W) / 2)
#define TOWER_Y 0


#define KEY_W 7
#define KEY_H 3
uint8_t key_image[] = {
    0b11100000,
    0b10111110,
    0b11101010,
};

#define TREASURE_W 5
#define TREASURE_H 3
uint8_t treasure_image[] = {
    0b00100000,
    0b01110000,
    0b11111000,
};

#define ARROW_W 2
#define ARROW_H 2
#define MAX_ARROW 5
uint8_t arrow_image[] = {
    0b11000000,
    0b11000000,
};

#define VWALL_W 3
#define VWALL_H (int)(MAP_Y * 0.2) + 1  // 15
uint8_t vwall_image[] = {
    0b11100000,
    0b11100000,
    0b11100000,
    0b11100000,
    0b11100000,
    0b11100000,
    0b11100000,
    0b11100000,
    0b11100000,
    0b11100000,
    0b11100000,
    0b11100000,
    0b11100000,
    0b11100000,
    0b11100000,
};

#define HWALL_W (int)(MAP_X * 0.2) + 1  // 25
#define HWALL_H 3
uint8_t hwall_image[] = {
    0b11111111, 0b11111111, 0b11111111, 0b11111111,
    0b11111111, 0b11111111, 0b11111111, 0b11111111,
    0b11111111, 0b11111111, 0b11111111, 0b11111111,
};

//------------------------------global var-------------------------------//
float PLAYER_SPEED = 1; //default 0.5

//bool
uint8_t paused = 0, key_taken = 0, shield_provided, bomb_provided, bow_provided, counting_down, flash;

char str[20];

double start_time = 0, aim_angle, count_start_time;
uint8_t player_direction = 1; // 1 = top, 2 = bottom, 3 = left, 4 = right
uint8_t lives, level;
uint8_t boundary_w = 10, boundary_h = 15;
int score;
int shown_mapx, shown_mapy;
uint8_t monster_on_level, treasure_on_level, vwall_on_level, hwall_on_level;
uint8_t defensive_collected; // 1 = shield, 2 = bomb, 3 = bow
uint8_t arrow_left;

Sprite player, monster[5], door, key, treasure[5], shield, bomb, bow, arrow[5];
Sprite vwall[5], hwall[5];


//-----------------------------------Utilities------------------------------//

void usb_serial_send(char * message) {
    // Cast to avoid "error: pointer targets in passing argument 1
    //  of 'usb_serial_write' differ in signedness"
    
    
    //-------------------usb_serial_write((uint8_t *) message, strlen(message));
    return;
}


uint16_t adc_read(uint8_t channel) {
    // Select AVcc voltage reference and pin combination.
    // Low 5 bits of channel spec go in ADMUX(MUX4:0)
    // 5th bit of channel spec goes in ADCSRB(MUX5).
    ADMUX = (channel & ((1 << 5) - 1)) | (1 << REFS0);
    ADCSRB = (channel & (1 << 5));
    
    // Start single conversion by setting ADSC bit in ADCSRA
    ADCSRA |= (1 << ADSC);
    
    // Wait for ADSC bit to clear, signalling conversion complete.
    while ( ADCSRA & (1 << ADSC) ) {}
    
    // Result now available.
    return ADC;
}

double sind(double degree){
    return sin(degree * PI / 180);
}

void sprite_draw_on_map(Sprite *target);
void show_debug(){
    clear_screen();
    draw_string(10, 10, str, FG_COLOUR);
    
    show_screen();
    //while (!bit_is_set(PINF, 6)){
        /*if (bit_is_set(PINF, 5)){
            sprintf(str, "monster: %d", monster_on_level);
            draw_string(10, 25, str, FG_COLOUR);
            sprintf(str, "treasure: %d", treasure_on_level);
            draw_string(10, 33, str, FG_COLOUR);
            show_screen();
        }*/
    //}
    draw_string(20, 17, "ok", FG_COLOUR);
    show_screen();
    //_delay_ms(200);
}

void cal_mapxy(){
    shown_mapx = player.x + PLAYER_W / 2.0 - 0.5 - LCD_X / 2.0;
    if (shown_mapx < 0){
        shown_mapx = 0;
    }
    if (shown_mapx + LCD_X > MAP_X){
        shown_mapx = MAP_X - LCD_X;
    }
    shown_mapy = player.y + PLAYER_H / 2.0 - 0.5 - LCD_Y / 2.0;
    if (shown_mapy < 0){
        shown_mapy = 0;
    }
    if (shown_mapy + LCD_Y > MAP_Y){
        shown_mapy = MAP_Y - LCD_Y;
    }
}

void cal_shown_border(int *top, int *bottom, int *left, int *right){
    cal_mapxy();
    
    if (shown_mapy==0){
        *top = 2;
    }else
        if (shown_mapy==1){
            *top = 1;
        } else {
            *top = 0;
        }
    if (shown_mapy + LCD_Y==MAP_Y){
        *bottom = 2;
    }else
        if (shown_mapy + LCD_Y - 1 == MAP_Y - 2){
            *bottom = 1;
        } else {
            *bottom = 0;
        }
    if (shown_mapx==0){
        *left = 2;
    }else
        if (shown_mapx==1){
            *left = 1;
        } else {
            *left = 0;
        }
    if (shown_mapx + LCD_X == MAP_X){
        *right = 2;
    }else
        if (shown_mapx + LCD_X - 1 == MAP_X - 2){
            *right = 1;
        } else {
            *right = 0;
        }
}

int onscreen(Sprite *target){
    
    int top = 0, bottom = 0, left = 0, right = 0;
    
    cal_shown_border(&top, &bottom, &left, &right);
    
    if (target->x + target->width - 1 > shown_mapx + left &&
        target->x < shown_mapx + LCD_X - right &&
        target->y + target->height - 1 > shown_mapy + top &&
        target->y < shown_mapy + LCD_Y - bottom){ // 2border considered
        return 1;
    }else{
        return 0;
    }
}

void sprite_draw_on_map(Sprite *target){
    double tempx = target->x, tempy = target->y;
    
    target->x -= shown_mapx;
    target->y -= shown_mapy;
    
    sprite_draw(&*target);
    
    target->x = tempx;
    target->y = tempy;
}

void draw_string_centre(int y, char *message, colour_t c){
    int x;
    x = LCD_X / 2 - (strlen(message) / 2) * 5;
    draw_string(x, y, message, c);
}

void draw_border(int top, int bottom, int left, int right, colour_t c){
    int i;
    for (i = 0; i < top; i++){
        draw_line(0, i, LCD_X - 1, i, c);
    }
    
    for (i = 0; i < bottom; i++){
        draw_line(0, LCD_Y - 1 - i, LCD_X - 1, LCD_Y - 1 - i, c);
    }
    
    for (i = 0; i < left; i++){
        draw_line(i, 0, i, LCD_Y - 1, c);
    }
    
    for (i = 0; i < right; i++){
        draw_line(LCD_X - 1 - i, 0, LCD_X - 1 - i, LCD_Y - 1, c);
    }
}

void draw_border_on_map(){
    int top = 0, bottom = 0, left = 0, right = 0;
    
    cal_shown_border(&top, &bottom, &left, &right);
    
    draw_border(top, bottom, left, right, FG_COLOUR);
}


void draw_tower(){
    
    draw_line(TOWER_X - shown_mapx, TOWER_Y, TOWER_X - shown_mapx, TOWER_Y + TOWER_H - 1 - shown_mapy, FG_COLOUR);
    draw_line((TOWER_X - shown_mapx < 0)?0:(TOWER_X - shown_mapx), TOWER_Y + TOWER_H - 1 - shown_mapy, TOWER_X + TOWER_W - 1 - shown_mapx, TOWER_Y + TOWER_H - 1 - shown_mapy, FG_COLOUR);
    draw_line(TOWER_X + TOWER_W - 1 - shown_mapx, TOWER_Y, TOWER_X + TOWER_W - 1 - shown_mapx, TOWER_Y + TOWER_H - 1 - shown_mapy, FG_COLOUR);
}



volatile uint32_t overflow_count = 0;

double current_time(){
    return ( overflow_count * 65536.0 + TCNT3 ) * PRESCALE  / FREQ;
}

ISR(TIMER3_OVF_vect) {
    overflow_count ++;
}

int gen_rand_int(int range){
    srand((unsigned)current_time() + rand() % 100);
    return rand() % range;
}


//-----------------------------------SCREENS---------------------------//
//--------------------------report to serial---//
void serial_report(){
    sprintf(str, "Time: %.3f\r\n", current_time());
    usb_serial_send( str);
    sprintf(str, "Score: %d\r\n", score);
    usb_serial_send( str);
    sprintf(str, "Floor: %d\r\n", level);
    usb_serial_send( str);
    sprintf(str, "x: %d  y: %d\r\n", (int)player.x, (int)player.y);
    usb_serial_send( str);
    sprintf(str, "Lives: %d\r\n\n", lives);
    usb_serial_send( str);
}

//----------------------------wait serial---//
void wait_for_serial(){
    
    clear_screen();
    
    draw_border(2, 2, 2, 2, FG_COLOUR);
    
    draw_string_centre(LCD_Y/2 - 8, "Waiting for", FG_COLOUR);
    draw_string_centre(LCD_Y/2, "USB SERIAL", FG_COLOUR);
    
    show_screen();
}

//----------------------------status-------//
void show_status(){
    
    char temp[] = "Floor: ";
    int temp_y = 3;
    int temp_t = (int)(current_time() - start_time);
    int min = temp_t / 60, sec = temp_t % 60;
    
    clear_screen();
    
    draw_border(2, 2, 2, 2, FG_COLOUR);
    
    draw_string_centre(temp_y, "P a u s e d", FG_COLOUR);
    temp_y += 10;
    
    sprintf(str, "%d", level);
    draw_string(13, temp_y, strcat(temp, str), FG_COLOUR);
    temp_y += 8;
    
    strcpy(temp, "Time: ");
    if (min > 9){
        if (sec > 9){
            sprintf(str, "%d:%d", min, sec);
        } else {
            sprintf(str, "%d:0%d", min, sec);
        }
    } else {
        if (sec > 9){
            sprintf(str, "0%d:%d", min, sec);
        } else {
            sprintf(str, "0%d:0%d", min, sec);
        }
    }
    draw_string(13, temp_y, strcat(temp, str), FG_COLOUR);
    temp_y += 8;
    
    strcpy(temp, "Score: ");
    sprintf(str, "%d", score);
    draw_string(13, temp_y, strcat(temp, str), FG_COLOUR);
    temp_y += 8;
    
    strcpy(temp, "Lives: ");
    sprintf(str, "%d", lives);
    draw_string(13, temp_y, strcat(temp, str), FG_COLOUR);
    
    show_screen();
}

//----------------------------Home---------//
void home_screen(){
    clear_screen();
    
    draw_string_centre(5, "ANSI TOWER", FG_COLOUR);
    
    draw_string_centre(13, "09836292", FG_COLOUR);
    
    draw_string_centre(21, "Wesley Lam", FG_COLOUR);
    
    draw_string_centre(30, "Ready?", FG_COLOUR);
    
    show_screen();
}

//--------------------------count down-----//
void count_down_screen(){
    char count[3][14] = {"countdown: 3", "countdown: 2", "countdown: 1"};
    for (int i = 0; i < 3; i++){
        clear_screen();
        draw_string_centre(12, "GET READY!", FG_COLOUR);
        draw_string(13, 22, count[i], FG_COLOUR);
        show_screen();
        _delay_ms(343);
    }
    clear_screen();
}

//---------------------------loading--------//
void loading_screen(){
    char temp[] = "Floor: ";
    int temp_y = 12;
    
    draw_border(2, 2, 2, 2, FG_COLOUR);
    
    draw_string_centre(temp_y, "LOADING ...", FG_COLOUR);
    temp_y += 10;
    
    sprintf(str, "%d", level);
    draw_string(13, temp_y, strcat(temp, str), FG_COLOUR);
    temp_y += 8;
    
    strcpy(temp, "Score: ");
    sprintf(str, "%d", score);
    draw_string(13, temp_y, strcat(temp, str), FG_COLOUR);
    
    show_screen();
    _delay_ms(1500);
    clear_screen();
}

//---------------------------gameover-------//
void gameover_screen(){
    char temp[] = "Final Floor:";
    int temp_y = 5;
    
    clear_screen();
    
    draw_border(2, 2, 2, 2, FG_COLOUR);
    
    draw_string_centre(temp_y, "GAME OVER", FG_COLOUR);
    temp_y += 10;
    
    sprintf(str, "%d", level);
    draw_string(10, temp_y, strcat(temp, str), FG_COLOUR);
    temp_y += 8;
    
    strcpy(temp, "Score: ");
    sprintf(str, "%d", score);
    draw_string(10, temp_y, strcat(temp, str), FG_COLOUR);
    
    draw_string_centre(LCD_Y - 10, "continue", FG_COLOUR);
    
    show_screen();
    while (!bit_is_set(PINF, 6) && !bit_is_set(PINF, 5)) {}
    
    _delay_ms(200);
    
    clear_screen();
}
//----------------------------------END SCREENS---------------------------//


//--------------------------------collisions-------------------------------//


int collision(Sprite a, Sprite b){
    int ax = round(a.x), bx = round(b.x);
    int ay = round(a.y), by = round(b.y);
    
    
    if (ax > bx + b.width - 1 || bx > ax + a.width - 1 || ay > by + b.height - 1 || by > ay + a.height - 1){
        return 0;
    } else{
        return 1;
    }
}

int multiple_collision(Sprite target, Sprite s_list[], int list_size){
    for (int i = 0; i < list_size; i++){
        if (collision(s_list[i], target)) {
            return 1;
        }
    }
    return 0;
}

int cross_collision(Sprite s_list[], int list_size){
    
    for (int i = 0; i < list_size; i++){
        for (int j = i + 1; j< list_size; j++){
            if (collision(s_list[i], s_list[j])){
                return 1;
            }
        }
    }
    return 0;
}

int border_collision_react(Sprite *target){
    int collide = 0;
    int tx = round(target->x);
    int ty = round(target->y);
    int mid = TOWER_X + TOWER_W / 2;
    
    while (level == 0 && tx <= TOWER_X + TOWER_W - 1 && tx + target->width - 1 >= TOWER_X && ty <= TOWER_Y + TOWER_H - 1){
        if (tx < mid && tx + target->width - 1 == TOWER_X && ty <= TOWER_Y + TOWER_H - 1){ // left
            target->x = --tx;
        }
        if (tx > mid && tx == TOWER_X + TOWER_W - 1 && ty <= TOWER_Y + TOWER_H - 1) { // right
            target->x = ++tx;
        }
        if (tx + target->width - 1 >= TOWER_X && tx <= TOWER_X + TOWER_W - 1 && ty <= TOWER_Y + TOWER_H - 1){ // bottom
            target->y = ++ty;
        }
        sprintf(str, "%d, %d", TOWER_X, tx);
        show_debug();
    }
    
    while (ty < 2) {
        target->y = ++ty;
        collide = 1;
    }
    while (ty + target->height - 1 >= MAP_Y - 2) {
        target->y = --ty;
        collide = 1;
    }
    
    while (tx < 2) {
        target->x = ++tx;
        collide = 1;
    }
    while (tx + target->width - 1 >= MAP_X - 2) {
        target->x = --tx;
        collide = 1;
    }
    return collide;
}

uint8_t wall_collision_react(Sprite *target){
    uint8_t i, collide = 0 ;
    
    for (i = 0; i < vwall_on_level; i++){
        while (collision(*target, vwall[i] ) && round(target->y) + target->height - 1 == vwall[i].y) {
            target->y = target->y - 1;
            collide = 1;
        }
        while (collision(*target, vwall[i] ) && round(target->y) == vwall[i].y + round(vwall[i].height) - 1){
            target->y = target->y + 1;
            collide = 1;
        }
        while (collision(*target, vwall[i] ) && round(target->x) + target->width - 1 == vwall[i].x) {
            target->x = target->x - 1;
            collide = 1;
        }
        while (collision(*target, vwall[i] )){// && target->x == vwall[i].x + 2){
            target->x = target->x + 1;
            collide = 1;
        }
    }
    for (i = 0; i < hwall_on_level; i++){
        while (collision(*target, hwall[i] ) && round(target->x) + target->width - 1 == hwall[i].x) {
            target->x = target->x - 1;
            collide = 1;
        }
        while (collision(*target, hwall[i] ) && round(target->x) == hwall[i].x + round(hwall[i].width) - 1){
            target->x = target->x + 1;
            collide = 1;
        }
        while (collision(*target, hwall[i] ) && round(target->y) + target->height - 1 == hwall[i].y) {
            target->y = target->y - 1;
            collide = 1;
        }
        while (collision(*target, hwall[i] )){// && target->x == vwall[i].x + 2){
            target->y = target->y + 1;
            collide = 1;
        }
    }
    
    return collide;
}

/*
void item_collision_react(Sprite *target){
    uint8_t collide_with;
    
    do{
        collide_with = wall_collision_react(target);
        switch (collide_with) {
            case 1:
                target->x++;
                break;
                
            case 2:
                target->y++;
                break;
                
            default:
                break;
        }
    } while(collide_with);
}*/

//--------------------------------Entities actions------------------------------//
void player_action(){
    if (bit_is_set(PINB, 0)){ //joy centre - status screen
        show_status();
        paused = 1;
        return;
    } else {
        paused = 0;
    }
    
    if (bit_is_set(PINB, 1)){ //joy left
        player.x -= PLAYER_SPEED;
        player_direction = 3;
    }
    
    if (bit_is_set(PINB, 7)){ //joy down
        player.y += PLAYER_SPEED;
        player_direction = 2;
    }
    
    if (bit_is_set(PIND, 0)){ //joy right
        player.x += PLAYER_SPEED;
        player_direction = 4;
    }
    
    if (bit_is_set(PIND, 1)){ //joy up
        player.y -= PLAYER_SPEED;
        player_direction = 1;
    }
}

void update_monster(uint8_t i) {
    
    float distancex = player.x - monster[i].x;
    float distancey = player.y - monster[i].y;
    float diagonaldis = sqrt(distancex * distancex + distancey * distancey);
    
    float time = diagonaldis / 0.04;
    float dx = distancex / time;
    float dy = distancey / time;
    
    monster[i].x += dx;
    monster[i].y += dy;
    
    
    
}

void arrow_action(){
    for (int i = 0; i < MAX_ARROW; i++){
        if (arrow[i].is_visible){
            arrow[i].x += arrow[i].dx;
            arrow[i].y += arrow[i].dy;
            if (border_collision_react(&arrow[i]) || wall_collision_react(&arrow[i])){
                arrow[i].is_visible = 0;
            }
            
            for (int j = 0; j < monster_on_level; j++){
                if (collision(arrow[i], monster[j]) && monster[j].is_visible){
                    score += 10;
                    arrow[i].is_visible = 0;
                    arrow[i].x = MAP_X +1;
                    arrow[i].y = MAP_Y +1;
                    monster[j].is_visible = 0;
                    //Report to serial
                    usb_serial_send("Player killed a monster!\r\n");
                }
            }
        }
    }
}

//-------------------------------process--------------------------//
void place_defensive(Sprite *target){
    switch (player_direction) {
        case 1: //top
            target->x = player.x;
            if (key_taken){
                target->y = player.y + PLAYER_H + 1 + KEY_H + 1;
            } else {
                target->y = player.y + PLAYER_H + 1;
            }
            break;
            
        case 2: //bottom
            target->x = player.x;
            if (key_taken){
                target->y = player.y - KEY_H - 1 - target->height - 1;
            } else {
                target->y = player.y - KEY_H - 1;
            }
            break;
            
        case 3: //left
            target->x = player.x + PLAYER_W + 1;
            if (key_taken){
                target->y = player.y + PLAYER_H - KEY_H - 1 - target->height;
            } else {
                target->y = player.y;
            }
            break;
            
        case 4: //right
            target->x = player.x - target->width - 1;
            if (key_taken){
                target->y = player.y + PLAYER_H - KEY_H - 1 - target->height;
            } else {
                target->y = player.y;
            }
            
            break;
            
        default:
            break;
    }
}

void defensive_item_follow(){
    switch (defensive_collected) {
        case 1:
            place_defensive(&shield);
            break;
            
        case 2:
            place_defensive(&bomb);
            break;
            
        case 3:
            place_defensive(&bow);
            break;
            
        default:
            break;
    }
}
void defensive_item_action(){
    
    int right_p;
    int x, y;
    int x1, y1;
    int outery, outerx;
    float temp_angle = 0, radius = 10.0;
    
    //arrow action
    arrow_action();
    
    //bomb action
    if (counting_down){
        if (current_time() - count_start_time >= 2){
            // explode
            bomb.is_visible = 0;
            counting_down = 0;
            for (int i = 0; i < monster_on_level; i++){
                if (onscreen(&monster[i]) && monster[i].is_visible){
                    monster[i].is_visible = 0;
                    score += 10;
                    //Report to serial
                    usb_serial_send("Player killed a monster!\r\n");
                }
            }
            //LED flash
            flash = 1;
        }
    }
    if (flash > 0){
        flash++;
        if (bit_is_set(PORTB, 3)){
            CLEAR_BIT(PORTB, 3);
            CLEAR_BIT(PORTB, 2);
        } else {
            SET_BIT(PORTB, 3);
            SET_BIT(PORTB, 2);
        }
        if (flash >= 5){
            CLEAR_BIT(PORTB, 3);
            CLEAR_BIT(PORTB, 2);
            flash = 0;
        }
    }
    
    //draw aim
    if (defensive_collected == 2 || defensive_collected == 3){
        // AIMING
        right_p = adc_read(1);
        aim_angle = (right_p / 1024.0) * 720.0;
        if (aim_angle > 359){
            aim_angle -= 360;
        }
        temp_angle = aim_angle;
        
        y = (radius/sind(90)) * sind(-temp_angle);
        x = (radius/sind(90)) * sind(-90 - temp_angle);
        y1 = (radius + 5/sind(90)) * sind(-temp_angle);
        x1 = (radius + 5/sind(90)) * sind(-90 - temp_angle);
        
        outerx = player.x + player.width/2 + x1;
        outery = player.y + player.height/2 + y1;
        
        if (!(outerx < 2 || outery < 2)){
            draw_line(player.x + player.width/2 + x - shown_mapx, player.y + player.height/2 + y - shown_mapy, outerx - shown_mapx, outery - shown_mapy, FG_COLOUR);
        }
        
    }
    
    switch (defensive_collected) {
        case 2:
            counting_down = 0;
            if (bit_is_set(PINF, 6) || bit_is_set(PINF, 5)){
                //throw bomb
                //Report to serial
                usb_serial_send("Player used the bomb!\r\n");
                
                y = (radius + 3/sind(90)) * sind(-temp_angle);
                x = (radius + 3/sind(90)) * sind(-90 - temp_angle);
                
                x1 = bomb.x;
                y1 = bomb.y;
                bomb.x = player.x + player.width/2 + x - BOMB_W/2;
                bomb.y = player.y + player.height/2 + y - BOMB_H/2;
                
                if (border_collision_react(&bomb) || wall_collision_react(&bomb) ){
                    bomb.x = x1;
                    bomb.y = y1;
                } else{
                    counting_down = 1;
                    defensive_collected = 0;
                    count_start_time = current_time();
                }
            }
            break;
            
        case 3:
            if ((bit_is_set(PINF, 6) || bit_is_set(PINF, 5)) && (current_time() - count_start_time >= 0.5)){
                // shoot arrow
                //Report to serial
                usb_serial_send("Player shoots an arrow!\r\n");
                count_start_time = current_time();
                arrow_left--;
                
                for (int i = 0; i < MAX_ARROW; i++){
                    if (!arrow[i].is_visible){
                        arrow[i].is_visible = 1;
                        arrow[i].y = player.y + player.height/2 + (radius/sind(90) * sind(-temp_angle));
                        arrow[i].x = player.x + player.width/2 + (radius/sind(90) * sind(-90 - temp_angle));
                        arrow[i].dy = (PLAYER_SPEED * 2) * sind(-temp_angle);
                        arrow[i].dx = (PLAYER_SPEED * 2) * sind(- 90 - temp_angle);
                        i = MAX_ARROW;
                    }
                }
                
                if (arrow_left == 0){
                    defensive_collected = 0;
                    arrow_left = 0;
                    bow.is_visible = 0;
                }
            }
            break;
            
        default:
            break;
    }
}

void start_floor_init(int respawn);

int process(){
    
    clear_screen();
    
    player_action();
    if (paused){
        return 0 ;
    }
    
    
    if (collision(door, player) && key_taken){
        level++;
        score += 100;
        
        //Report to serial
        usb_serial_send("Player processed to the next floor!\r\n");
        //display loading
        loading_screen();
        return 1;
    }
    border_collision_react(&player);
    wall_collision_react(&player);
    
    for (int i = 0; i < monster_on_level; i++){
        if (onscreen(&monster[i]) && monster[i].is_visible){
            update_monster(i);
            border_collision_react(&monster[i]);
            wall_collision_react(&monster[i]);
        }
    }
    
    
    for (int i = 0; i < monster_on_level; i++){
        if (collision(monster[i], player) && monster[i].is_visible){
            if (defensive_collected == 1){
                monster[i].is_visible = 0;
                shield.is_visible = 0;
                //Report to serial
                usb_serial_send("Player used the shield!\r\n");
            } else{
                i = monster_on_level;
                lives--;
                //Report to serial
                usb_serial_send("Player was killed by a monster!\r\n");
                if (lives == 0){
                    return 0;
                }
                key_taken = 0;
                border_collision_react(&key);
                wall_collision_react(&key);
                switch (defensive_collected) {
                    case 2:
                        border_collision_react(&bomb);
                        wall_collision_react(&bomb);
                        break;
                        
                    case 3:
                        border_collision_react(&bow);
                        wall_collision_react(&bow);
                        break;
                        
                    default:
                        break;
                }
                //respawn
                start_floor_init(1);
            }
            defensive_collected = 0;
        }
        
    }
    
    for (int i = 0; i < treasure_on_level; i++){
        if (collision(treasure[i], player)){
            treasure[i].x = -TREASURE_W - 2;
            treasure[i].y = -TREASURE_H - 2;
            treasure[i].is_visible = 0;
            score += 10;
            //Report to serial
            usb_serial_send("Player picked up a treasure!\r\n");
        }
    }
    
    if (collision(player, key)){
        if (key_taken == 0){
            key_taken = 1;
            //Report to serial
            usb_serial_send("Player picked up the key!\r\n");
        }
    }
    
    if (key_taken){
        switch (player_direction) {
            case 1: //top
                key.x = player.x;
                key.y = player.y + PLAYER_H + 1;
                break;
                
            case 2: //bottom
                key.x = player.x;
                key.y = player.y - KEY_H - 1;
                break;
                
            case 3: //left
                key.x = player.x + PLAYER_W + 1;
                key.y = player.y + PLAYER_H - KEY_H;
                break;
                
            case 4: //right
                key.x = player.x - KEY_W - 1;
                key.y = player.y + PLAYER_H - KEY_H;
                break;
                
            default:
                break;
        }
    }
    
    if (collision(player, shield) && shield.is_visible && defensive_collected != 1){
        defensive_collected = 1;
        //Report to serial
        usb_serial_send("Player picked up the shield!\r\n");
    }
    if (collision(player, bomb) && bomb.is_visible && !counting_down && defensive_collected != 2){
        defensive_collected = 2;
        //Report to serial
        usb_serial_send("Player picked up the bomb!\r\n");
    }
    if (collision(player, bow) && bow.is_visible && defensive_collected != 3){
        defensive_collected = 3;
        //Report to serial
        usb_serial_send("Player picked up the bow!\r\n");
    }
    defensive_item_follow();
    cal_mapxy();
    defensive_item_action();
    
    
    
    draw_border_on_map();
    if (level == 0){
        draw_tower();
    }
    
    sprite_draw_on_map(&door);
    sprite_draw_on_map(&key);
    sprite_draw_on_map(&player);
    sprite_draw_on_map(&monster[0]);
    if (level != 0){
        for (int i = 0; i < treasure_on_level; i++){
            sprite_draw_on_map(&treasure[i]);
        }
        for (int i = 1; i < monster_on_level; i++){
            sprite_draw_on_map(&monster[i]);
        }
        if (shield_provided){
            sprite_draw_on_map(&shield);
        }
        if (bomb_provided){
            sprite_draw_on_map(&bomb);
        }
        if (bow_provided){
            sprite_draw_on_map(&bow);
            for (int i = 0; i < MAX_ARROW; i++){
                if (arrow[i].is_visible){
                    sprite_draw_on_map(&arrow[i]);
                }
            }
        }
        for (int i = 0; i < vwall_on_level; i++){
            sprite_draw_on_map(&vwall[i]);
        }
        for (int i = 0; i < hwall_on_level; i++){
            sprite_draw_on_map(&hwall[i]);
        }
        
    }
    show_screen();
    if (bit_is_set(PINF, 6)){
        level++;
        return 1;
    }
    
    return 0;
}

//-------------------------------start floor init---------------------//

int walls_too_close(Sprite wallA, Sprite wallB){
    if (wallA.x > wallB.x + wallB.width - 1 + boundary_w || wallB.x > wallA.x + wallA.width - 1 + boundary_w || wallA.y > wallB.y + wallB.height - 1 + boundary_h || wallB.y > wallA.y + wallA.height - 1 + boundary_h ){
        return 0;
    } else {
        return 1;
    }
}

void wall_gen(){
    uint8_t temp_vwall = 0, temp_hwall = 0, i, single_is_v, invalid_wall;
    int x = -1, y = -1, hx, hy, vx, vy;
    Sprite temp_wall;
    int count;
    
    
    // combined three walls
    if (vwall_on_level == 4 || ((hwall_on_level != 4) && gen_rand_int(2))){
        // side way segment
        sprite_init(&hwall[temp_hwall], gen_rand_int(MAP_X - 4 - HWALL_W - boundary_w * 2) + boundary_w + 2, gen_rand_int(MAP_Y - 4 - HWALL_H - (boundary_h + VWALL_H) * 2) + boundary_h + VWALL_H + 2, HWALL_W, HWALL_H, hwall_image);
        temp_hwall++;
        
        for (i = 0; i < 2; i++){
            do{
                if (gen_rand_int(2)){ //bottom
                    y = hwall[0].y + 3;
                } else { //top
                    y = hwall[0].y - VWALL_H;
                }
                if (gen_rand_int(2)){ //left
                    x = hwall[0].x - 3;
                } else { //right
                    x = hwall[0].x + HWALL_W;
                }
            }while (i == 1 && vwall[0].x == x && vwall[0].y == y);
                
            sprite_init(&vwall[temp_vwall], x, y, VWALL_W, VWALL_H, vwall_image);
            temp_vwall++;
        }
        
    } else {
        // segment faceup
        sprite_init(&vwall[temp_vwall], gen_rand_int(MAP_X - 4 - VWALL_W - (boundary_w + HWALL_W) * 2) + boundary_w + HWALL_W + 2, gen_rand_int((MAP_Y - 4 - VWALL_H - boundary_h * 2)) + boundary_h + 2, VWALL_W, VWALL_H, vwall_image);
        temp_vwall++;
        
        for (i = 0; i < 2; i++){
            do{
                if (gen_rand_int(2)){ //bottom
                    y = vwall[0].y + VWALL_H;
                } else { //top
                    y = vwall[0].y - 3;
                }
                if (gen_rand_int(2)){ //left
                    x = vwall[0].x - HWALL_W;
                } else { //right
                    x = vwall[0].x + 3;
                }
            }while (i == 1 && hwall[0].x == x && hwall[0].y == y);
            
            sprite_init(&hwall[temp_hwall], x, y, HWALL_W, HWALL_H, hwall_image);
            temp_hwall++;
        }
    }
    
    
    // L section
    do{
        if(gen_rand_int(2)){ // generate Vertical wall first
            vx = gen_rand_int(MAP_X - 4 - boundary_w * 2) + boundary_w;
            if (vx - HWALL_W < boundary_w + 2){
                // right
                hx = vx + 3;
            } else if (vx + 3 + HWALL_W  > MAP_X - 2 - boundary_w) {
                // left
                hx = vx - HWALL_W;
            } else {
                if (gen_rand_int(2)){
                    // left
                    hx = vx - HWALL_W;
                } else {
                    // right
                    hx = vx + 3;
                }
            }
            
            vy = gen_rand_int(MAP_Y - 4) + 2;
            if (vy < boundary_h + 2){
                // bottom
                vy = 2;
                hy = vy + VWALL_H;
            } else if (vy + VWALL_H - 1 > MAP_Y - 2 - boundary_h) {
                // top
                vy = MAP_Y - 2 - VWALL_H + 1;
                hy = vy - 3;
            } else {
                if (gen_rand_int(2)){
                    // top
                    hy = vy - 3;
                } else {
                    // bottom
                    hy = vy + VWALL_H;
                }
            }
        } else { // generate Horizontal wall first
            hy = gen_rand_int(MAP_Y - 4 - boundary_h * 2) + boundary_h;
            if (hy - VWALL_H < boundary_h + 2){
                // bottom
                vy = hy + 3;
            } else if (hy + 3 + VWALL_H > MAP_Y - 2 - boundary_h){
                // top
                vy = hy - VWALL_H;
            } else {
                if (gen_rand_int(2)){
                    // bottom
                    vy = hy + 3;
                } else {
                    // top
                    vy = hy - VWALL_H;
                }
            }
            
            hx = gen_rand_int(MAP_X - 4) + 2;
            if (hx < boundary_w + 2){
                // right
                hx = 2;
                vx = hx + HWALL_W;
            } else if (hx + HWALL_W - 1 > MAP_X - 2 - boundary_w){
                // left
                hx = MAP_X - 2 - HWALL_W + 1;
                vx = hx - 3;
            } else {
                if (gen_rand_int(2)){
                    // left
                    vx = hx - 3;
                } else {
                    // right
                    vx = hx + HWALL_W;
                }
            }
        }
        sprite_init(&hwall[temp_hwall], hx, hy, HWALL_W, HWALL_H, hwall_image);
        sprite_init(&vwall[temp_vwall], vx, vy, VWALL_W, VWALL_H, vwall_image);
        
        //check for invalid walls
        invalid_wall = 0;
        for (int i = 0; i < temp_vwall ;i++){
            if (walls_too_close(hwall[temp_hwall], vwall[i])){
                invalid_wall = 1;
                break;
            }
        }
        
        if (!invalid_wall){
            for (int i = 0; i < temp_hwall ;i++){
                if (walls_too_close(hwall[temp_hwall], hwall[i])){
                    invalid_wall = 1;
                    break;
                }
            }
        }
        
        if (!invalid_wall){
            for (int i = 0; i < temp_vwall ;i++){
                if (walls_too_close(vwall[temp_vwall], vwall[i])){
                    invalid_wall = 1;
                    break;
                }
            }
        }
        
        if (!invalid_wall){
            for (int i = 0; i < temp_hwall ;i++){
                if (walls_too_close(vwall[temp_vwall], hwall[i])){
                    invalid_wall = 1;
                    break;
                }
            }
         }
        strcpy(str, "Lshapewallloop");
        show_debug();
        
    }while (invalid_wall);
    temp_hwall++;
    temp_vwall++;
    
    // single wall
    count = 0;
    //changed = 0;
    do {
        count++;
        if (temp_hwall == hwall_on_level) {
            // init vwall
            sprite_init(&vwall[temp_vwall], gen_rand_int(MAP_X - 4 - boundary_w * 2 - VWALL_W) + 2 + boundary_w, gen_rand_int(MAP_Y - 4 - VWALL_H) + 2, VWALL_W, VWALL_H, vwall_image);
            if (vwall[temp_vwall].y < 2 + boundary_h){
                vwall[temp_vwall].y = 2;
            }
            if (vwall[temp_vwall].y + VWALL_H - 1 > MAP_Y - 2 - boundary_h){
                vwall[temp_vwall].y = MAP_Y - 3 - VWALL_H;
            }
            single_is_v = 1;
            temp_wall = vwall[temp_vwall];
        } else {
            // init hwall
            sprite_init(&hwall[temp_hwall], gen_rand_int(MAP_X - 4 - HWALL_W) + 2, gen_rand_int(MAP_Y - 4 - boundary_h * 2 - HWALL_H) + 2 + boundary_h, HWALL_W, HWALL_H, hwall_image);
            if (hwall[temp_hwall].x < 2 + boundary_w){
                hwall[temp_hwall].x = 2;
            }
            if (hwall[temp_hwall].x + HWALL_W - 1 > MAP_X - 2 - boundary_w){
                hwall[temp_hwall].x = MAP_X - 3 - HWALL_W;
            }
            single_is_v = 0;
            temp_wall = hwall[temp_hwall];
        }
        
        
        invalid_wall = 0;
        for (int i = 0; i < temp_vwall ;i++){
            if (walls_too_close(temp_wall, vwall[i])){
                invalid_wall = 1;
                break;
            }
        }
        
        if (!invalid_wall){
        for (int i = 0; i < temp_hwall ;i++){
            if (walls_too_close(temp_wall, hwall[i])){
                invalid_wall = 1;
                break;
            }
        }
        }
    
        sprintf(str, "singlewall, %d", count);
        show_debug();
        if (count == 150){
            if (single_is_v){
                sprite_init(&vwall[temp_vwall], MAP_X + 1, MAP_Y + 1, VWALL_W, VWALL_H, vwall_image);
                vwall_on_level -= 1;
                hwall_on_level += 1;
                single_is_v = 0;
            } else {
                sprite_init(&hwall[temp_hwall], MAP_X + 1, MAP_Y + 1, HWALL_W, HWALL_H, hwall_image);
                hwall_on_level -= 1;
                vwall_on_level += 1;
                single_is_v = 1;
            }
            
            /*if (single_is_v && !changed){
                vwall_on_level -= 1;
                hwall_on_level += 1;
                single_is_v = 0;
            }
            if (!single_is_v && !changed){
                hwall_on_level -= 1;
                vwall_on_level += 1;
                single_is_v = 1;
            }*/
        }
        
    } while (invalid_wall);
    boundary_w = 10, boundary_h = 15;
    if (single_is_v) {
        temp_vwall++;
    } else {
        temp_hwall++;
    }
    
    while (temp_hwall != hwall_on_level || temp_vwall != vwall_on_level){
        strcpy(str, "incorrect count");
        show_debug();
    }
    
}

void start_floor_init(int respawn){
    int collide, monster_too_close, count, numofelement;
    srand((unsigned) current_time());
    
    if (level != 0 || respawn){
        if (!respawn){
            if (level != 0){
                monster_on_level = (gen_rand_int(5)) + 1;
                treasure_on_level = gen_rand_int(6);
            }else{
                monster_on_level = 1;
                treasure_on_level = 0;
            }
            
            if (gen_rand_int(3) == 1){
                shield_provided = 1;
            } else {
                shield_provided = 0;
                shield.is_visible = 0;
            }
            if (gen_rand_int(3) == 1){
                bomb_provided = 1;
            } else {
                bomb_provided = 0;
                bomb.is_visible = 0;
            }
            if (gen_rand_int(3) == 1){
                bow_provided = 1;
                arrow_left = 5;
            } else {
                bow_provided = 0;
                bow.is_visible = 0;
                arrow_left = 0;
            }
            for (int i = 0; i < MAX_ARROW; i++){
                sprite_init(&arrow[i], MAP_X + 1, MAP_Y + 1, ARROW_W, ARROW_H, arrow_image);
                arrow[i].is_visible = 0;
            }
            
            
            vwall_on_level = gen_rand_int(3) + 2;
            hwall_on_level = 6 - vwall_on_level;
            wall_gen();
            
            count_start_time = current_time();
            counting_down = 0;
        }
        defensive_collected = 0;
        numofelement = 3 + monster_on_level + treasure_on_level + shield_provided + bomb_provided + bow_provided + vwall_on_level + hwall_on_level ;//3:k/d/p +
        
        //key: monster
        //door: player, monster
        //monster: treasure
        Sprite list[numofelement];
        int x[numofelement], y[numofelement];
        
        
        do {
            collide = 0;
            monster_too_close = 0;
            count = 0;
            
            for (int i = 0; i < numofelement; i++) {
                x[i] = (rand() % (MAP_X - 4)) + 2; // (rand() % (MAP_X - 4))
                y[i] = (rand() % (MAP_Y - 4)) + 2;
            }
            
            sprite_init(&player, x[count], y[count], PLAYER_W, PLAYER_H, player_image);
            border_collision_react(&player);
            wall_collision_react(&player);
            count++;
            
            if (respawn != 1){ // generated all random positioned entity
                for (int i = 0; i < monster_on_level; i++) {
                    sprite_init(&monster[i], x[count], y[count], MONSTER_W, MONSTER_H, monster_image);
                    border_collision_react(&monster[i]);
                    wall_collision_react(&monster[i]);
                    count++;
                }
                
                sprite_init(&door, x[count], y[count], DOOR_W, DOOR_H, door_image);
                border_collision_react(&door);
                wall_collision_react(&door);
                count++;
                
                sprite_init(&key, x[count], y[count], KEY_W, KEY_H, key_image);
                border_collision_react(&key);
                wall_collision_react(&key);
                count++;
                
                for (int i = 0; i < treasure_on_level; i++) {
                    sprite_init(&treasure[i], x[count], y[count], TREASURE_W, TREASURE_H, treasure_image);
                    border_collision_react(&treasure[i]);
                    wall_collision_react(&treasure[i]);
                    count++;
                }
                
                if (shield_provided){
                    sprite_init(&shield, x[count], y[count], SHIELD_W, SHIELD_H, shield_image);
                    border_collision_react(&shield);
                    wall_collision_react(&shield);
                    count++;
                }
                if (bomb_provided){
                    sprite_init(&bomb, x[count], y[count], BOMB_W, BOMB_H, bomb_image);
                    border_collision_react(&bomb);
                    wall_collision_react(&bomb);
                    count++;
                }
                if (bow_provided){
                    sprite_init(&bow, x[count], y[count], BOW_W, BOW_H, bow_image);
                    border_collision_react(&bow);
                    wall_collision_react(&bow);
                    count++;
                }
                
                key_taken = 0;
            }
            
            
            // put every entity into list
            count = 0;
            if (respawn != 1){
                list[count] = player;
                count++;
            }
            for (int i = 0; i < monster_on_level; i++) {
                list[count] = monster[i];
                count++;
            }
            list[count] = door;
            count++;
            list[count] = key;
            count++;
            for (int i = 0; i < treasure_on_level; i++) {
                list[count] = treasure[i];
                count++;
            }
            if (shield_provided){
                list[count] = shield;
                count++;
            }
            if (bomb_provided){
                list[count] = bomb;
                count++;
            }
            
            if (bow_provided){
                list[count] = bow;
                count++;
            }
            
            
            //check if valid map/respawn generated
            if (!respawn){
                
                collide = cross_collision(list, count);
                
                for (int i = 0; i < vwall_on_level; i++) {
                    collide = collide || multiple_collision(vwall[i], list, count);
                }
                
                for (int i = 0; i < hwall_on_level; i++) {
                    collide = collide || multiple_collision(hwall[i], list, count);
                }
            
            
            }
            
            
            if (respawn){
                for (int i = 0; i < vwall_on_level; i++) {
                    list[count] = vwall[i];
                    count++;
                }
                
                for (int i = 0; i < hwall_on_level; i++) {
                    list[count] = hwall[i];
                    count++;
                }
                collide = multiple_collision(player, list, count);
            }
            
            for (int i = 0; i < monster_on_level; i++) {
                if ((player.x - monster[i].x + MONSTER_W - 1 < 5 && player.x + player.width - 1 - monster[i].x > -5) || (player.y - monster[i].y + MONSTER_H - 1 < 5 && player.y + PLAYER_H - 1 - monster[i].y > -5)){
                    monster_too_close = 1;
                }
            }
            
            strcpy(str, (monster_too_close || collide)?"WILL LOOP":"NO LOOP");
            show_debug();
            
        }while (monster_too_close || collide);
        
        
    } else {
        monster_on_level = 1;
        treasure_on_level = 0;
        // Ground floor
        sprite_init(&player, MAP_X / 2, MAP_Y - PLAYER_H - 10, PLAYER_W, PLAYER_H, player_image);
        sprite_init(&monster[0], MAP_X - 5 - MONSTER_W, 3, MONSTER_W, MONSTER_H, monster_image);
        sprite_init(&door, (MAP_X - DOOR_W) / 2, (MAP_Y - DOOR_H) / 2, DOOR_W, DOOR_H, door_image);
        sprite_init(&key, 5, 3, KEY_W, KEY_H, key_image);
    }
}


//-----------------------------start_game_init--------------------------//
void start_game_init(){
    int outsidex = MAP_X + 1, outsidey = MAP_Y + 1, i;
    
    //reinit every global var
    start_time = 0;
    lives = 3;
    level = 0;
    score = 0;
    arrow_left = 0;
    paused = 0;
    key_taken = 0;
    bomb_provided = 0;
    shield_provided = 0;
    bow_provided = 0;
    defensive_collected = 0;
    monster_on_level = 0;
    treasure_on_level = 0;
    vwall_on_level = 0;
    hwall_on_level = 0;
    
    // init every sprite
    for (i = 0; i < 5; i++) {
        sprite_init(&monster[i], outsidex, outsidey, 1, 1, 0);
    }
    
    for (i = 0; i < 5; i++) {
        sprite_init(&treasure[i], outsidex, outsidey, 1, 1, 0);
    }
    
    for (i = 0; i < 5; i++) {
        sprite_init(&arrow[i], outsidex, outsidey, 1, 1, 0);
    }
    
    for (i = 0; i < 4; i++) {
        sprite_init(&vwall[i], outsidex, outsidey, 1, 1, 0);
    }
    for (i = 0; i < 4; i++) {
        sprite_init(&hwall[i], 130, 100, 1, 1, 0);
    }
    sprite_init(&shield, outsidex, outsidey, 1, 1, 0);
    sprite_init(&bomb, outsidex, outsidey, 1, 1, 0);
    sprite_init(&bow, outsidex, outsidey, 1, 1, 0);
}


//-------------------------------start setup-----------------------------//
void start_setup(void) {
    set_clock_speed(CPU_8MHz);
    lcd_init(LCD_DEFAULT_CONTRAST);
    
    //backlight
    SET_BIT(DDRC,7); SET_BIT(PORTC,7);
    
    //adc init
    // ADC Enable and pre-scaler of 128: ref table 24-5 in datasheet
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
    
    //Timer
    TCCR3A = 0;
    TCCR3B = 4;
    TIMSK3 = 1;
    sei();
    
    //set controllers
    // LED
    SET_BIT(DDRB, 2); // LED0 (left)
    SET_BIT(DDRB, 3); // LED1 (right)
    CLEAR_BIT(PORTB, 2);
    CLEAR_BIT(PORTB, 3);
    
    // joystick
    CLEAR_BIT(DDRB, 0); // Centre
    CLEAR_BIT(DDRB, 1); // Left
    CLEAR_BIT(DDRB, 7); // Down
    CLEAR_BIT(DDRD, 0); // Right
    CLEAR_BIT(DDRD, 1); // Up
    
    // Buttons
    CLEAR_BIT(DDRF, 6); // Left button
    CLEAR_BIT(DDRF, 5); // Right button
    
}

//----------------------------------MAIN-------------------------------//
int main(void) {
    int new_floor;
    int printed = 0;
    
    start_setup();
    
    //wait for usb serial
    //(what happen after usb disconnected)
    // Set up LCD and display message
    
    
    //---------------------usb_init();
    
    while (!bit_is_set(PINF, 6) && !bit_is_set(PINF, 5)) { //---------------------while ( !usb_configured() || !usb_serial_get_control()) {
        wait_for_serial();
        show_screen();
    }
    //continue after connected (two button replaced for now)
    usb_serial_send("Wellcome\r\n");
    _delay_ms(200);
    
    
    for ( ;; ) {
        //home screen
        home_screen();
        while (!bit_is_set(PINF, 6) && !bit_is_set(PINF, 5)) {}
        //wait until player is ready to play
        count_down_screen();
        
        //initalize
        new_floor = 1;
        start_game_init();
        start_time = current_time();
        
        do {
            if (new_floor){start_floor_init(0);}
            
            if (current_time() - printed >= 0.5){
                printed = current_time();
                serial_report();
            }
            
            new_floor = process();
            _delay_ms(10);
        } while (lives > 0);   // keep playing while not gameover
        //gameover
        gameover_screen();
    }
}

