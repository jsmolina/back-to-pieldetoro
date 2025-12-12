#ifndef PLAYER_H
#define PLAYER_H
#include <allegro.h>

#define COCHE_FRAMES 3

typedef int (*CheckHitFn)();


struct coordsType {
    int x;
    int y;
};

struct playerType {
    struct coordsType pos;
    int vx;
    int vy;
    int width;
    int height;
    int flip;
    int lives;
    int move_count;
    int anime_count;
    int anime_index;
    int sprite_index;
    unsigned int state;
    unsigned int prev_state;
    CheckHitFn checkHitObj; // should be injected during player initialization
    CheckHitFn checkOverObj;
};

/**
* @brief __init__ :) 
*/
void player_init(int x, int y);

/**
 * @brief Apply a force over player
 * @param vx Horizontal force
 * @param vy Vertical force
 * 
*/
void player_affect_force(int vx, int vy);

/**
 * @brief Does a jump for the player
 * 
*/
void player_do_jump();

/**
 * @brief Changes player state machine
 *
 * @param state new state
*/
void player_change_state(unsigned int state);


/**
 * @brief Checks if player is over an object
 *
 * @param vy player velocity
 * @return True
*/
int is_on_obj();

/**
 * @brief Checks player speed
 *
 * @param vy player velocity
 */
void check_vy();

/**
 * @brief Updates player position checking collisions
 *
 */
void update_position();


/**
 * @brief Updates player from main loop
 *
*/
void player_update();

/**
* @brief returns TRUE if player is deading
* @return TRUE if dead or falling
*/
int player_is_deading();


void load_coche_spritesheet();
void destroy_coche_spritesheet();
void player_killed();
/**
 * @brief Gets the collision area for the player's foot
 * 
 * @return collisionType struct representing the foot collision area
 */
struct collisionType foot_area();
struct collisionType foot_area2();

extern struct playerType player;
extern BITMAP *sp_coche[COCHE_FRAMES];
extern struct playerType player;


#endif