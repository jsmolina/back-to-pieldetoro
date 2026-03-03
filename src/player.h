#ifndef PLAYER_H
#define PLAYER_H
#include <allegro.h>
#include "object.h"


#define COCHE_FRAMES 3
#define MARTIN_FRAMES 16
#define MAX_FRAMES 13

typedef int (*CheckHitFn)();

typedef struct {
    int move_count;
    int frames[MAX_FRAMES];
    uint8_t length;
    int frame_interval;
} animeItem;

typedef struct {
    int width;
    int height;
    int total_frames;
    BITMAP* sprites[MARTIN_FRAMES];
} PlayerData;

struct playerType {
    coordsType pos;
    int vx;
    int vy;
    int max_vx;
    int flip;
    int lives;
    int move_count;
    int anime_count;
    int anime_index;
    int sprite_index;
    unsigned int state;
    unsigned int prev_state;
    PlayerData * data; // pointer to static data for current player type (car or martin)
    animeItem * animations; // pointer to current level animations, should be injected during player initialization
};



/**
 * @brief __init__ :)
 */
void player_init(int x, int y, int current_level, int max_vx);

/**
 * @brief Does a jump for the player
 *
 */
void player_do_jump();


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

void load_coche_spritesheet(); void destroy_coche_spritesheet();
void load_martin_spritesheet(); void destroy_martin_spritesheet();
void player_killed();
/**
 * @brief Gets the collision area for the player's foot
 *
 * @return collisionType struct representing the foot collision area
 */
collisionType front_wheels_area();
collisionType rear_wheels_area();
collisionType player_foot_area();

extern struct playerType player;

#endif
