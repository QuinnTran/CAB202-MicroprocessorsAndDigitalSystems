#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <cab202_graphics.h>
#include <cab202_sprites.h>
#include <cab202_timers.h>


// Configuration
#define DELAY (10) /* Millisecond delay between game updates */

#define PLAYER_WIDTH (3)
#define PLAYER_HEIGHT (4)

#define BLOCK_WIDTH (7)
#define BLOCK_HEIGHT (2)
#define MAX_GOOD_BLOCK (160)
#define MAX_BAD_BLOCK (40)

#define TREASURE_WIDTH (3)
#define TREASURE_HEIGHT (3)

// Game state.
bool game_over = false; /* Set this to true when game is over */
bool update_screen = false; /* Set to false to prevent screen update. */
bool paused = false;

sprite_id player;
int lives = 10;
double player_speed = 1;

sprite_id treasure;

sprite_id starting_block;
sprite_id good_block[MAX_GOOD_BLOCK];
sprite_id bad_block[MAX_BAD_BLOCK];

//The player’s sprite is at least 3 characters high and 3 characters wide.
char * player_image =
/**/	" o "
/**/	"/|\\"
/**/	" | "
/**/	"/ \\";

char * player_none_image =
/**/	"   "
/**/	"   "
/**/	"   "
/**/	"   ";

char * left_player_image =
/**/	 " o "
/**/	"\\| "
/**/	 " | "
/**/	 "/ \\";

char * right_player_image =
/**/	" o "
/**/	" |/"
/**/	" | "
/**/	"/ \\";

char * jump_player_image =
/**/	 " o "
/**/	"\\|/"
/**/	 " | "
/**/	 "/ \\";

char * fall_player_image =
/**/	"\\ /"
/**/	 " | "
/**/	"\\|/"
/**/	 " o ";

char * good_block_image =
/**/	"======="
/**/	"=======";

char * bad_block_image =
/**/	"xxxxxxx"
/**/	"xxxxxxx";

char * treasure_image =
/**/	"$$$"
/**/	"$$$"
/**/	"$$$";

char * treasure2_image =
/**/	"   "
/**/	"$$$"
/**/	"$$$ ";

sprite_id border;
char *border_image=
/**/	"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~";

// The display screen is 5 pixels high, located at the top of the screen, 
// is entirely visible at all times (i.e. never occluded in any way) and displays:
//     a. Student number,
//     b. Lives remaining,
//     c. Playing time in minutes and seconds format as mm:ss,
//     d. Score,
//     e. play screen from the game area.Border that separates the dis

int score = 0;
int time_start = 0;

void display_border()
{
    int game_time = (int) get_current_time() - time_start;
    draw_line( 0, 0, screen_width(), 0, '~' );
    draw_string(screen_width() -14, 3, "n9808281");
    draw_formatted(4, 3,"Lives: %d", lives); //The player starts with 10 lives.
    draw_formatted(16, 3, "Time: %d",(game_time/60));
    draw_formatted(24, 3, ": %d",(game_time % 60));
    draw_formatted(28, 3, "Score: %d", score);
}

// Columns are calculated as visible screen width / max block width + horizontal spacing 
// (if used to distinguish between sprites) and there are:
//  a. The number of blocks on screen is proportional to the number of columns, 
//     to a maximum of 160 safe blocks and 40 forbidden blocks,
int columns()
{
    int columns = screen_width() / (BLOCK_WIDTH + 2); //number of columns
    int num_blocks = columns;
    return num_blocks;
}
int rows(){
    int rows = screen_height() / (BLOCK_HEIGHT + 10);
    int num_rows = rows;
    printf("%d", num_rows);
    return num_rows;
}

//  b. At least 1 safe block per column denoted by '=' on the screen at all times,
//  c. At least 2 forbidden block on the screen denoted by 'x' on the screen at all times.

// Blocks have no consistent observable pattern and each block:
// Appears in a randomly selected row,
// Appears in a randomly selected column (after there is 1 safe block per column),
// Does not overlap other blocks or the display screen.
// store blocks positions
// check if columns have at least 1 good block and arr has at least 2 bad block
//int arr[4][21];


// Setup many blocks
sprite_id setup_block(bool good, int row, int col)
{
    /*int random_r, random_col;
    while((random_r == 0)&&(random_col == 0)){
        int r = rand() % rows();
        int c = rand() % columns();
        random_r = r;
        random_col = c;
    }*/
    //int bx = random_col * (BLOCK_WIDTH + 2); //get random columns
    //int by = random_r * (BLOCK_HEIGHT + 6);// get randome rows
    int bx = col * (BLOCK_WIDTH + 2); 
    int by = row * (BLOCK_HEIGHT + 8);


    sprite_id new_block;

    if(good){
        new_block = sprite_create( bx, by, BLOCK_WIDTH, BLOCK_HEIGHT, good_block_image );
    }
    else{
        new_block = sprite_create( bx, by, BLOCK_WIDTH, BLOCK_HEIGHT, bad_block_image );
    }
    return new_block;
}

void setup_blocks()
{
    int goodIncremeter = 0;
    int badIncrementer = 0;

    for(int rowCount = 2; rowCount < rows()+2 ; rowCount++){
       for(int colCount = 0; colCount < columns(); colCount++){
           
           int decider = rand() % 20;
           bool deciderB;

           if (decider >=0 && decider <= 5){
               deciderB = true;
           } else if (decider <= 12){
               deciderB = false;
           }

           sprite_id holder = setup_block((decider < 10),rowCount,colCount);
           
           if (deciderB == true){
               good_block[goodIncremeter] = holder;
               goodIncremeter++;
           } else{
               bad_block[badIncrementer] = holder;
               badIncrementer++;
           } 
       }
    }   
}

void auto_move( sprite_id sid, int keyCode )
{
    if ( keyCode == 'z' || keyCode < 0 )
    {
        sprite_step( sid );

        // (s) Get screen location of block.
        int bx = round( sprite_x( sid ) );
        int by = round( sprite_y( sid ) );

        // (t) Get the displacement vector of the block.
        double bdx = sprite_dx( sid );
        double bdy = sprite_dy( sid );

        // (u) Test to see if the block hit the left or right border.
        if ( bx == 0 || bx == screen_width() - sprite_width( sid ) )
        {
            bdx = -bdx;
        }

        // (v) Test to see if the block hit the top or bottom border.
        if ( by == 0 || by == screen_height() - sprite_height( sid ) )
        {
            bdy = -bdy;
        }

        // (w) Test to see if the block needs to step back and change direction.
        if ( bdx != sprite_dx( sid ) || bdy != sprite_dy( sid ) )
        {
            sprite_back( sid );
            sprite_turn_to( sid, bdx, bdy );
        }
    }
}

void auto_move_all( sprite_id sprites[], int n, int key )
{
    for ( int i = 0; i < n; i++ )
    {
        auto_move( sprites[i], key );
    }
}

void draw_sprites( sprite_id sids[], int n )
{
    for ( int i = 0; i < n; i++ )
    {
        sprite_draw( sids[i] ); 
    }
}

// Player movement behaves such that:
//  c. When off a block, the player free falls downwards at an appropriate speed,
//  d. No lateral motion occurs during downward motion (unless Advanced Functionality is implemented).

// The player initially appears on a 'starting block' in the top row. 
// Pressing the ‘a’ or ‘d’ key moves the character left and right respectively,
float x_speed = 0;
float y_speed = 0;
float gravity = 0.001;

void move_player( int key )
{
    y_speed += gravity; 
    x_speed = 0;

    int px = round( sprite_x( player ) );
    // int hy = round( sprite_y( player ) );

    switch(key){
        case'd':
        if(x_speed < player_speed && px < ( screen_width() - 1 - sprite_width( player ) )){
            x_speed = x_speed + player_speed;
            if(x_speed > 0){
                sprite_set_image(player, right_player_image);
            }else{
                sprite_set_image(player, player_image);
            }
        }
        
        break;
        case'a':
        if(x_speed > -player_speed && px > 1){
            x_speed = x_speed - player_speed;
            if(x_speed < 0){
                sprite_set_image(player, left_player_image);
            }else{
                sprite_set_image(player, player_image);
            }
        }

        break;
        case'w':
        y_speed = y_speed - 0.1;
        if(y_speed < 0){
            sprite_set_image(player, jump_player_image);
        }else{
            sprite_set_image(player, player_image);
        }
        break;
    }
    sprite_turn_to(player, x_speed, y_speed);
    sprite_step(player);
}

// There is a treasure sprite that:
//  a. Is no larger than the player's sprite and has a clearly distinct image,
//  b. Spawns at the bottom of the screen and moves back and forward along the bottom, 
//     changing horizontal direction when it reaches the edges of the screen,
sprite_id setup_treasure() 
{
    // Set up the treasure at the bottom of the screen.
    int tx = rand() % screen_width() ;
    int ty = screen_height() - 4;
    sprite_id new_treasure = sprite_create(tx, ty, TREASURE_WIDTH, TREASURE_HEIGHT, treasure_image);

    // Set the treasure in motion.
    sprite_turn_to( new_treasure, 0.1, 0.0 );

    return new_treasure;
}

bool sprites_collide( sprite_id s1, sprite_id s2 )
{
    int l1 = round( sprite_x( s1 ) );
    int l2 = round( sprite_x( s2 ) );
    int t1 = round( sprite_y( s1 ) );
    int t2 = round( sprite_y( s2 ) );
    int r1 = l1 + sprite_width( s1 ) - 1;
    int r2 = l2 + sprite_width( s2 ) - 1;
    int b1 = t1 + sprite_height( s1 ) - 1;
    int b2 = t2 + sprite_height( s2 ) - 1;

    if ( l1 > r2 )
        return false;
    if ( l2 > r1 )
        return false;
    if ( t1 > b2 )
        return false;
    if ( t2 > b1 )
        return false;

    return true;
}

sprite_id sprites_collide_any( sprite_id s, sprite_id sprites[], int n )
{
    sprite_id result = NULL;

    for ( int i = 0; i < n; i++ )
    {
        if ( sprites_collide( s, sprites[i] ) )
        {
            result = sprites[i];
            break;
        }
    }

    return result;
}

bool onland(sprite_id s){
    if((sprite_y(s) + sprite_height(s) >= sprite_y(starting_block) - 0.5) 
    && (sprite_y(s) + sprite_height(s) <= sprite_y(starting_block) + 0.49 )
    && (sprite_x(s) + sprite_width(s) -1 >= sprite_x(starting_block))
    && (sprite_x(s) <= sprite_x(starting_block) + sprite_width(starting_block) - 1) ){
        return true;
    }

    for(int i = 0; i < 16; i++){
        if((sprite_y(s) + sprite_height(s) >= sprite_y(good_block[i]) - 0.5) 
    && (sprite_y(s) + sprite_height(s) <= sprite_y(good_block[i]) + 0.49 )
    && (sprite_x(s) + sprite_width(s) -1 >= sprite_x(good_block[i]))
    && (sprite_x(s) <= sprite_x(good_block[i]) + sprite_width(good_block[i]) - 1) ){
            return true;
        }
    }

    return false;

}
 
void end_game()
{
    game_over = true;

    static char *msg_text = "Game over!Thank you for playing !"
    "Score:"
    "Time:"
    "Press Q to Exit";
    int msg_width = strlen( msg_text ) / 2;
    int msg_height = 4;
    int msg_x = ( screen_width() - msg_width ) / 2;
    int msg_y = ( screen_height() - msg_height ) / 2;
    sprite_id msg = sprite_create( msg_x, msg_y, msg_width, msg_height, msg_text );
    clear_screen();
    sprite_draw( msg );
    show_screen();

    timer_pause( 1000 );

    while ( get_char() >= 0 )
    {
        // Do nothing
    }

    wait_char();
}

// Clean up game
void cleanup( void )
{
    // STATEMENTS
}

void draw_all() {
    clear_screen();
    sprite_draw(player);
    sprite_draw(starting_block);
    sprite_draw(treasure);
    draw_sprites(good_block, 16);
    draw_sprites(bad_block, 5);
    sprite_draw(border);
    display_border();
    show_screen();
}

//Setup game

void setup(void)
{
    player = sprite_create((screen_width() - PLAYER_WIDTH) - 4, (screen_height() - PLAYER_HEIGHT) - 47, 
             PLAYER_WIDTH, PLAYER_HEIGHT, player_image);
    starting_block = sprite_create((screen_width() - BLOCK_WIDTH) - 2, (screen_height() - BLOCK_HEIGHT) - 45, 
             BLOCK_WIDTH, BLOCK_HEIGHT, good_block_image);
    border = sprite_create(0, 5, 200, 1, border_image);
    srand(get_current_time());
    treasure = setup_treasure();
    setup_blocks();
    draw_all();
}

// Stops moving when 't' is pressed and starts moving again if 't' is pressed again 
// (i.e. motion toggles on and off when 't' is pressed),
int read_char()
{
    int key_code = paused ? wait_char() : get_char();

    if ('t' == key_code)
    {
        paused = !paused;
    }

    return key_code;
}

// The following game mechanics are present:
void process( void )
{
    int keyCode = read_char();
    //  e. The game over screen allows the player to restart by pressing ‘r’ (score, lives, time, and 
    //     screen all reset) or quit with the ‘q’ key which ends the game gracefully.
    if ( keyCode == 'q' )
    {
        end_game();
    }
    
    // restart the game
    else if (keyCode == 'r')
    {
        setup();
    }

    move_player( keyCode );
    auto_move( treasure, keyCode );

    //not collide with display border
    if(sprites_collide(player, border)){
        sprite_move(player, 0, 1);
        y_speed = 0;
    }

    // When the player loses all their lives, the game over screen is displayed 
    // which displays a game over message, total score, and game play time.
    if(lives == 0){
        game_over = true;
    }
    //treasure animation
    if(rand() % 2 == 0){
        sprite_set_image(treasure,treasure_image);
    }else{
        sprite_set_image(treasure,treasure2_image);
    }

    // Disappears when the player collides with it and gives the player 2 more lives and 
    // returns the player to the 'starting block'.
    if(sprites_collide(player, treasure))
    {
        lives += 2;
        player = sprite_create((screen_width() - PLAYER_WIDTH) - 2, (screen_height() - PLAYER_HEIGHT) - 45, 
                 PLAYER_WIDTH, PLAYER_HEIGHT, player_image); 
        treasure = setup_treasure();
    }

    //  c. The player dies when they fall off the bottom of the screen or hit a forbidden block. 
    //     On death, the player starts from the ‘starting block’ (unless Advanced Functionality is implemented).
    if(sprite_y(player) > screen_height()){
        setup();
        // int n = 3;
        // for(int i = 0; i < n; i++){
        //     sprite_set_image(player,player_none_image);
        //     sprite_set_image(player, player_image);
        // }
    }

    //collide with starting block
    if ( sprites_collide( player, starting_block ))
    {
        sprite_move(player, 0, -1);
        y_speed = 0;
    }

    // A point is scored every time the player lands on a safe block.
    if ( sprites_collide_any( player, good_block, 16 ) )
    {
        sprite_move(player, 0, -1);
        y_speed = 0;
        score += 1;
    }

    //collide with bad block will reduce a live
    if(sprites_collide_any(player, bad_block, 5))
    {
        lives -= 1;
        setup();
    }

    //falling animation
        if(!onland(player)){
            if (y_speed > 0){
                sprite_set_image(player, fall_player_image);
            }else{
                sprite_set_image(player, jump_player_image);
            }
        }else if(x_speed < 0){
            sprite_set_image(player, left_player_image);
        }else if(x_speed > 0){
            sprite_set_image(player, left_player_image);
        }else{
            sprite_set_image(player, player_image);
        }

    draw_all();
}

// Program entry point.
int main( int argc, char *argv[] )
{
    for ( int i = 0; i < argc; i++ )
    {
        printf( "Argv[%d] = %s\n", i, argv[i] );
    }

    timer_pause(5000);

    // if ( argc > 1 )
    // {
    //     int n_zombies;
    //     int vals_converted = sscanf( argv[1], "%d", &n_zombies );

    //     if ( vals_converted == 1 && n_zombies >= 0 && n_zombies <= MAX_ZOMBIES )
    //     {
    //         num_zombies = n_zombies;
    //     }
    // }

    setup_screen();
    setup();
    show_screen();
    time_start = get_current_time();
    while ( !game_over )
    {
        process();

        if ( update_screen )
        {
            show_screen();
        }

        timer_pause( DELAY );
    }

    cleanup();

    return 0;
}