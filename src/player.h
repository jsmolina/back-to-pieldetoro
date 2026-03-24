#ifndef PLAYER_H
#define PLAYER_H
#include "helpers.h"
#include "object.h"
#include <allegro.h>

#define STOP 1
#define MOVE_LEFT 2
#define MOVE_RIGHT 3
#define BREAKING 4
#define JUMP_UP 5
#define JUMP_DOWN 6
#define JUMP_HIT 7
#define FALL 8
#define FALL2 9
#define DEAD 10
#define FALL_END 11
#define DEAD_END 12
#define BOUNCING 13
#define CROUCHING 14
#define THROWING 15
#define FALL_TO_FLOOR 16
#define KICKING 17

#define COCHE_FRAMES 3
#define MARTIN_FRAMES 17

typedef enum {
    CAR_TYPE = 0,
    MARTIN_TYPE = 1,
} playerEnum;

typedef int (*CheckHitFn)();

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
    int energy;
    int hurt_cooldown;
    int move_count;
    int anime_count;
    int anime_index;
    int sprite_index;
    int type;
    unsigned int state;
    unsigned int prev_state;
    PlayerData* data;      // pointer to static data for current player type (car or martin)
    animeItem* animations; // pointer to current level animations, should be injected during player initialization
};

/**
 * @brief __init__ :)
 */
void player_init(int x, int y, int current_level, int max_vx);

/**
 * @brief Updates player from main loop
 *
 */
void player_update();

/** @brief Draws the player on the screen
 *
 * @param scroll_x The current horizontal scroll amount for camera offset
 */
inline void player_draw(int scroll_x);

/**
 * @brief returns TRUE if player is deading
 * @return TRUE if dead or falling
 */
int player_is_deading();
/** @brief Loads the spritesheet for the car player
 *
 */
void load_coche_spritesheet();
void destroy_coche_spritesheet();
/** @brief Loads the spritesheet for the martin player
 *
 */
void load_martin_spritesheet();
void destroy_martin_spritesheet();

/** @brief Handles player death
 *
 */
void player_killed();

/**
 * @brief Gets the collision area for the player's foot
 *
 * @return collisionType struct representing the foot collision area
 */
collisionType front_wheels_area();
/** @brief Gets the collision area for the player's rear wheels
 *
 * @return collisionType struct representing the rear wheels collision area
 */
collisionType rear_wheels_area();

/** @brief Gets the collision area for the player's foot
 *
 * @return collisionType struct representing the foot collision area
 */
collisionType player_foot_area();

/** @brief Gets the axis-aligned bounding box for the player
 *
 * @return collisionType struct representing the AABB
 */
collisionType player_aabb();
void player_on_hit();

extern struct playerType player;

#endif
