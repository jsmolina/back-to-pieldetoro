#include <math.h>
#include <allegro.h>
#include "player.h"
#include "game.h"
#include "dat_manager.h"
#include "statics.h"
#include "player.h"



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

#define JUMP_VY -8

#define FINISHED 1
#define NOT_FINISHED 0


struct playerType player;

struct animeItem {
    int move_count;
    int frames[6];
    uint8_t length;
    int frame_interval;
};

BITMAP* sp_coche[COCHE_FRAMES];

void player_init(int x, int y) {
    player.pos.x = x;
    player.pos.y = y;
    player.vx = 0;
    player.vy = 0;
    player.state = STOP;
    player.prev_state = 0;
}

void load_coche_spritesheet() {
    BITMAP* coche_spritesheet = dat_file[COCHE_SPRITESHEET_BMP].dat;
    int frame_width = (int)coche_spritesheet->w / COCHE_FRAMES;
    for (int i = 0; i < COCHE_FRAMES; i++) {
        sp_coche[i] = create_sub_bitmap(coche_spritesheet, i * frame_width, 0, frame_width, coche_spritesheet->h);
    }
}


void destroy_coche_spritesheet() {
    BITMAP* coche_spritesheet = dat_file[COCHE_SPRITESHEET_BMP].dat;
    int frame_width = (int)coche_spritesheet->w / COCHE_FRAMES;
    for (int i = 0; i < COCHE_FRAMES; i++) {
        destroy_bitmap(sp_coche[i]);
    }
}

struct animeItem player_animations[13] = {
    { 0, {0},0, -1 }, // NONE
    { 1, {0},1, 60 }, // STOP 
    { 12, {0,1},2, 2 }, // MOVE_LEFT
    { 12, {0,1},2, 2 }, // MOVE_RIGHT
    { 16, {2},1, 2 }, // BREAKING
    { 60, {0,1},2, 1 }, // JUMP_UP
    { 60, {0,1},2, 1 }, // JUMP_DOWN
    { 16, {0,1},2, 1 }, // JUMP_HIT
    { 1, {0},1, 1 }, // FALL
    { 1, {0},1, 1 }, // FALL2
    { 30, {0},1, 30 }, // DEAD
    { 60, {0},1, 60 }, // FALL_END
    { 70, {0},1, 70 }, // FALL_END
};

void player_change_state(unsigned int state) {
    player.prev_state = player.state;
    player.state = state;
    player.move_count = player_animations[player.state].move_count;
}

uint8_t space_was_pressed = 0;
int jump_key_freed() {
    if (!space_was_pressed && key[KEY_SPACE]) {
        space_was_pressed = 1;
        return TRUE;
    } else if(space_was_pressed && !key[KEY_SPACE]) {
        space_was_pressed = 0;
    }
    return FALSE;
}

/**
 * @brief Player performs a jump, that could be diagonal
 */
void player_do_jump() {
    if (key[KEY_LEFT]) {
        player.vx = -3;
        player.flip = 1;
    }
    if (key[KEY_RIGHT]) {
        player.vx = 3;
        player.flip = 0;
    }
    player.vy = JUMP_VY;
    player_change_state(JUMP_UP);
}

/**
 * @brief Applies a force (vy or vx) over player
 *
 * @param vx horz velocity
 * @param vy vert velocity
 *
 */
void player_affect_force(int vx, int vy) {
    player.vy += vy;
    player.vx += vx;
}

/**
 * @brief Checks if player is over an object
 *
 * @return TRUE if player is in top of object
 */
int is_on_obj() {
    // returns true if sprite is over a walkable tile
    return player.checkOverObj();
}

/**
 * @brief Checks vy for hits
 */
void check_vy() {
    if (player.vy > 0) {
        if (is_on_obj()) {
            player.vy = 0;
        }
    }
}

/**
 * @brief Checks vx for hits
 */
void check_vx() {
    if (player.vx != 0) {
        if (player.checkHitObj()) {
            player.vx = -player.vx;
        }
    }
}

/**
 * @brief Updates player position based on velocities
 */
void player_update_position() {
    check_vx();
    check_vy();
    player.pos.x = round(player.pos.x + player.vx);
    player.pos.y = round(player.pos.y + player.vy);
}

/**
 * @brief Counts how much movement is done per action
 *
 * @param dx 
 * @param dy 
 *
 * @return FINISHED/NOT_FINISHED
*/
unsigned int player_count_move(int dx, int dy) {
    player.move_count--;
    if (player.move_count < 0) {
        return FINISHED;
    }
    return NOT_FINISHED;
}

void player_move_left() {
    player.vx = -1;
    player_change_state(MOVE_LEFT);
    player.flip = TRUE;
}

void player_move_right() {
    player.vx = 1;
    player_change_state(MOVE_RIGHT);
    player.flip = FALSE;
}

void player_stop() {
    player.vx = 0;
    player.vy = 0;
    player_change_state(STOP);
}

// ACTIONS
void player_action_fall() {
    if (player.vy == 0) {
        if (player.vx == 0) {
            player_change_state(STOP);
        } else if (player.vx > 0 && key[KEY_RIGHT]) {
            player_change_state(MOVE_RIGHT);
        } else if (player.vx < 0 && key[KEY_LEFT]) {
            player_change_state(MOVE_LEFT);
        } else {
            player_change_state(BREAKING);
        }
    }
}

void player_action_move_left() {
    if (jump_key_freed()) {
        player_do_jump();
        return;
    }

    if (player_count_move(-1, 0) == NOT_FINISHED) {
        return;
    }

    if (player.vy > 0) {
        if (player.vx == 0) {
            player_change_state(FALL);
        } else {
            player_change_state(FALL2);
        }
    } else if (key[KEY_LEFT]) {
        player_move_left();
    } else if (player.prev_state == STOP) {
        player_stop();
    } else { 
        player.vx = -1;
        player_change_state(BREAKING);
    }
}

void player_action_move_right() {
    if (jump_key_freed()) {
        player_do_jump();
        return;
    }
    if (player_count_move(1, 0) == NOT_FINISHED) {
        return;
    }

    if (player.vy > 0) {
        if (player.vx == 0)
            player_change_state(FALL);
        else 
            player_change_state(FALL2);
    } else if (key[KEY_RIGHT]) {
        player_move_right();
    } else if (player.prev_state == STOP) {
        player_stop();
    } else {
        player.vx = 1;
        player_change_state(BREAKING);
    }
}

void player_action_stop() {
    if (player.vy > 0) {
        if (player.vx == 0) {
            player_change_state(FALL);
        } else  {
            player_change_state(FALL2);
        }
    } else if (key[KEY_LEFT]) {
        player_move_left();
    } else if (key[KEY_RIGHT]) {
        player_move_right();
    } else if (jump_key_freed()) {
        player_do_jump();
    } else {
        player_count_move(0,0);
    }
}

void player_action_jump_up() {
    if (player.vy < 0) {
        // collides on up
        //int ht = player.pushUpObj();

        /*if (ht) {
            player.vy = 0;
            player_change_state(JUMP_HIT);
            return;
        }*/
        if (player.vx == 0) {
            if (key[KEY_LEFT]) {
                player.flip = TRUE;
                player.vx = -1;
            } else if (key[KEY_RIGHT]) {
                player.flip = FALSE;
                player.vx = 1;
            }
        }
    }
    if (player.vy > 0) {
        player_change_state(JUMP_DOWN);
        return;
    }
    player_count_move(player.vx, 0);
}

void player_action_jump_down() {
    if (player.vy != 0) {
        player_count_move(player.vx, 0);
        return;
    }
    if (player.vx > 0) {
        if (key[KEY_RIGHT]) {
            player_change_state(MOVE_RIGHT);
        } else {
            player_change_state(BREAKING);
        }
    } else if (player.vx < 0) {
        if (key[KEY_LEFT]) {
            player_change_state(MOVE_LEFT);            
        } else {
            player_change_state(BREAKING);
        }
    } else {
        player_change_state(STOP);
    }
}

void action_jump_hit() {
    if (player_count_move(player.vx, -player.vy)== FINISHED) {
        player_change_state(JUMP_DOWN);
    }
}

void player_action_breaking() {
    if (player_count_move(player.vx, 0) == NOT_FINISHED) {
        return;
    }
    player_stop();
}

void player_action_dead() {
    if (player_count_move(0,0) == FINISHED) {
        player_change_state(FALL_END);
    }
}
    
void player_action_fall_end() {
}

// conditional
int player_is_deading() {
    if (player.state == DEAD || player.state == FALL_END) {
        return TRUE;
    } else {
        return FALSE;
    }
}

// ANIMATION
/*
STOP: {move_count: 1, frames: [0], frame_interval: 60},
MOVE_LEFT: {move_count: 12, frames: [1,2,3,1,5,4], frame_interval: 2},
MOVE_RIGHT: {move_count: 12, frames: [1,2,3,1,5,4], frame_interval: 2},
TURN_LEFT: {move_count: 8, frames: [0], frame_interval: 2},
TURN_RIGHT: {move_count: 8, frames: [0], frame_interval: 2},
BREAKING: {move_count: 16, frames: [7,10,11], frame_interval: 2},
JUMP_UP: {move_count: 60, frames: [6], frame_interval: 1},
JUMP_DOWN: {move_count: 60, frames: [6], frame_interval: 1},
JUMP_HIT: {move_count: 16, frames: [6], frame_interval: 1},
FALL: {move_count: 1, frames: [3], frame_interval: 1},
FALL2: {move_count: 1, frames: [7], frame_interval: 1},
DEAD: {move_count: 30, frames: [8], frame_interval: 30},
FALL_END: {move_count: 60, frames: [9], frame_interval: 60},
REBORN: {move_count: 70, frames: [0], frame_interval: 70},
 */

void player_anime_update() {
        // player_animations

        int * frames = player_animations[player.state].frames;
        int frame_interval = player_animations[player.state].frame_interval;

        if (player.anime_count >= frame_interval) {
            player.anime_index++;
            player.anime_count = 0;
        }

        if (player.anime_index >= player_animations[player.state].length) {
            player.anime_index = 0;
        }

        player.sprite_index = frames[player.anime_index];
        player.anime_count++;
}

// LOOP
void player_update() {
    if (game_pause) {
        return;
    }
    player_update_position();

    switch(player.state) {
        case MOVE_LEFT:
            player_action_move_left();
        break;
        case MOVE_RIGHT:
            player_action_move_right();
        break;
        case FALL:
        case FALL2:
            player_action_fall();
        break;
        case STOP:
            player_action_stop();
        break;
        case JUMP_UP:
            player_action_jump_up();
        break;
        case JUMP_DOWN:
            player_action_jump_down();
        break;
        case BREAKING:
            player_action_breaking();
        break;
        case DEAD:
            player_action_dead();
        break;
        case FALL_END:
            player_action_fall_end();
        break;

    }
    player_anime_update();
    
    /**
        if (this.pause) 
            return;
        this.update_position();
        const action_func = `action_${this.state.toLowerCase()}`;
        this[action_func]();
        this.anime_update();

     */
}
