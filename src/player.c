#include "player.h"
#include "allegro/gfx.h"
#include "dat_manager.h"
#include "game.h"
#include "object.h"
#include "player.h"
#include "statics.h"
#include <allegro.h>
#include <math.h>

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

#define JUMP_VY -8

#define FINISHED 1
#define NOT_FINISHED 0

#define PLAYER_ACCEL 1

struct playerType player;

// BITMAP* sp_coche[COCHE_FRAMES];
// BITMAP* sp_martin[MARTIN_FRAMES];
PlayerData coche;
PlayerData martin;

animeItem car_animations[15] = {
    { 0, { 0 }, 0, -1 },     // NONE
    { 1, { 0 }, 1, 60 },     // STOP
    { 12, { 0, 1 }, 2, 2 },  // MOVE_LEFT
    { 12, { 0, 1 }, 2, 2 },  // MOVE_RIGHT
    { 16, { 2 }, 1, 2 },     // BREAKING
    { 60, { 0, 1 }, 2, 1 },  // JUMP_UP
    { 60, { 0, 1 }, 2, 1 },  // JUMP_DOWN
    { 16, { 0, 1 }, 2, 1 },  // JUMP_HIT
    { 1, { 0 }, 1, 1 },      // FALL
    { 1, { 0 }, 1, 1 },      // FALL2
    { 30, { 0 }, 1, 30 },    // DEAD
    { 60, { 0 }, 1, 60 },    // FALL_END
    { 70, { 0 }, 1, 70 },    // DEAD_END
    { 20, { 0, 2 }, 2, 30 }, // BOUNCING
    { 0, {}, 0, 0 }          // CROUCHING (cars don't crouch)
};

animeItem martin_animations[15] = {
    { 0, { 0 }, 0, -1 },                                         // NONE
    { 1, { 0 }, 1, 60 },                                         // STOP
    { 12, { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 }, 13, 5 }, // MOVE_LEFT
    { 12, { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 }, 13, 5 }, // MOVE_RIGHT
    { 16, { 9 }, 1, 2 },                                         // BREAKING
    { 60, { 13 }, 1, 1 },                                        // JUMP_UP
    { 60, { 13 }, 1, 1 },                                        // JUMP_DOWN
    { 16, { 13 }, 1, 1 },                                        // JUMP_HIT
    { 1, { 13 }, 1, 1 },                                         // FALL
    { 1, { 13 }, 1, 1 },                                         // FALL2
    { 30, { 0 }, 1, 30 },                                        // DEAD
    { 60, { 14 }, 1, 60 },                                       // FALL_END
    { 70, { 0 }, 1, 70 },                                        // DEAD_END
    { 20, { 0, 2 }, 2, 30 },                                     // BOUNCING
    { 1, { 14 }, 1, 30 },                                        // CROUCHING
};

void player_init(int x, int y, int current_level, int max_vx) {
    player.pos.x = x;
    player.pos.y = y;
    player.vx = 0;
    player.vy = 0;
    player.state = STOP;
    player.prev_state = 0;
    player.anime_count = 0;
    player.anime_index = 0;
    player.flip = FALSE;
    player.move_count = 0;
    player.max_vx = max_vx;
    // Ensure size/sprite indices are initialized to safe defaults. Width/height
    // are normally set when loading the spritesheet; initialize to 0 to
    // detect misuse before they contain garbage.
    player.sprite_index = 0;
    player.lives = 3;
    player.animations = current_level == 1 ? car_animations : martin_animations;
    player.data = current_level == 1 ? &coche : &martin;
}

void load_coche_spritesheet() {
    BITMAP* coche_spritesheet = dat_file[COCHE_SPRITESHEET_BMP].dat;
    int frame_width = (int)coche_spritesheet->w / COCHE_FRAMES;
    coche.width = frame_width;
    coche.height = coche_spritesheet->h;
    coche.total_frames = COCHE_FRAMES;
    // player.current_sprite = create_video_bitmap(player.width, player.height);

    for (int i = 0; i < COCHE_FRAMES; i++) {
        coche.sprites[i] = create_sub_bitmap(coche_spritesheet, i * frame_width, 0, frame_width, coche_spritesheet->h);
    }
}

void destroy_coche_spritesheet() {
    BITMAP* coche_spritesheet = dat_file[COCHE_SPRITESHEET_BMP].dat;
    int frame_width = (int)coche_spritesheet->w / COCHE_FRAMES;
    for (int i = 0; i < COCHE_FRAMES; i++) {
        destroy_bitmap(coche.sprites[i]);
    }
}

void load_martin_spritesheet() {
    BITMAP* martin_spritesheet = dat_file[MARTIN_SPRITESHEET_BMP].dat;
    int frame_width = (int)martin_spritesheet->w / MARTIN_FRAMES;
    martin.width = frame_width;
    martin.height = martin_spritesheet->h;
    martin.total_frames = MARTIN_FRAMES;

    for (int i = 0; i < MARTIN_FRAMES; i++) {
        martin.sprites[i] = create_sub_bitmap(martin_spritesheet, i * frame_width, 0, frame_width, martin_spritesheet->h);
    }
}

void destroy_martin_spritesheet() {
    BITMAP* martin_spritesheet = dat_file[MARTIN_SPRITESHEET_BMP].dat;
    int frame_width = (int)martin_spritesheet->w / MARTIN_FRAMES;
    for (int i = 0; i < MARTIN_FRAMES; i++) {
        destroy_bitmap(martin.sprites[i]);
    }
}

void player_change_state(unsigned int state) {
    player.prev_state = player.state;
    player.state = state;
    player.move_count = player.animations[player.state].move_count;
}

uint8_t space_was_pressed = 0;
int jump_key_freed() {
    if (!space_was_pressed && key[KEY_SPACE]) {
        space_was_pressed = 1;
        return TRUE;
    } else if (space_was_pressed && !key[KEY_SPACE]) {
        space_was_pressed = 0;
    }
    return FALSE;
}

/**
 * @brief Player performs a jump, that could be diagonal
 */
void player_do_jump() {
    if (key[KEY_LEFT]) {
        player.vx = -4;
        player.flip = 1;
    }
    if (key[KEY_RIGHT]) {
        player.vx = 4;
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
    // todo foot_area collision
    return checkOverObj();
}

collisionType rear_wheels_area() {
    if (!player.data) {
        return (collisionType){ 0, 0, 0, 0 };
    }
    // should consist in wheel area
    int y = player.pos.y + player.data->height - 13;

    collisionType ret = {
        .x = player.pos.x + 16,
        .y = y,
        .w = 12,
        .h = 12
    };
    return ret;
}

collisionType front_wheels_area() {
    if (!player.data) {
        return (collisionType){ 0, 0, 0, 0 };
    }
    // should consist in wheel area
    int y = player.pos.y + player.data->height - 13;

    collisionType ret = {
        .x = player.pos.x + player.data->width - 26,
        .y = y,
        .w = 12,
        .h = 12
    };
    return ret;
}

inline collisionType player_foot_area() {
    if (player.data == NULL || player.data->height == 0 || player.data->width == 0
        || player.pos.x == 0 || player.pos.y == 0) {
        return (collisionType){ 0, 0, 0, 0 };
    }
    int y1 = player.pos.y + player.data->height - 5;
    // player.pos.x - scroll_x +4, y1, player.pos.x + player.width - scroll_x-2, y1 + 5

    collisionType ret = {
        .x = player.pos.x + 4,
        .y = y1,
        .w = 16,
        .h = 5
    };
    return ret;
}

/**
 * @brief Checks vy for hits
 */
void check_vy() {
    if (player.state == DEAD) {
        return;
    }

    if (player.state == JUMP_HIT || player.state == DEAD) {
        player.vy = 0;
        return;
    }

    if (player.state == FALL_END)
        return;

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
        if (checkHitObj()) {
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
    if (player.move_count >= 0) {
        player.move_count--;
    }

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
    } else if (player.pos.y > SCREEN_H - player.data->height) {
        player_change_state(FALL_END);
    }
}

void player_killed() {
    player_change_state(DEAD);
}

void player_traveling() {
    player_change_state(BOUNCING);
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
        // accelerate up to MAX_PLAYER_VX
        if (player.vx < player.max_vx) {
            player.vx += PLAYER_ACCEL;
            if (player.vx > player.max_vx)
                player.vx = player.max_vx;
        } else {
            player.vx = player.max_vx;
        }
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

void player_do_crouch() {
    player_change_state(CROUCHING);
}

void player_action_stop() {
    if (player.vy > 0) {
        if (player.vx == 0) {
            player_change_state(FALL);
        } else {
            player_change_state(FALL2);
        }
    } else if (key[KEY_LEFT]) {
        player_move_left();
    } else if (key[KEY_RIGHT]) {
        player_move_right();
    } else if (jump_key_freed()) {
        player_do_jump();
    } else if (key[KEY_DOWN]) {
        player_do_crouch();
    } else {
        player_count_move(0, 0);
    }
}

void player_crouch() {
    if (!key[KEY_DOWN]) {
        player_change_state(STOP);
    } else {
        player_count_move(0, 0);
    }
}

void player_action_jump_up() {
    if (player.vy < 0) {
        // collides on up
        // int ht = player.pushUpObj();

        /*if (ht) {
            player.vy = 0;
            player_change_state(JUMP_HIT);
            return;
        }*/
        if (player.vx < 0) {
            player.flip = TRUE;
        } else if (player.vx > 0) {
            player.flip = FALSE;
        } else {
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
    if (player.vy == 0) {
        player_change_state(JUMP_DOWN);
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
    if (player_count_move(player.vx, -player.vy) == FINISHED) {
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
    if (player_count_move(0, 0) == FINISHED) {
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
    animeItem* anim = &player.animations[player.state];
    int* frames = anim->frames;
    int frame_interval = anim->frame_interval;

    if (player.anime_count >= frame_interval) {
        player.anime_index++;
        player.anime_count = 0;
    }

    if (anim->length == 0) {
        player.anime_index = 0;
    } else if (player.anime_index >= anim->length) {
        player.anime_index = 0;
    }

    int sprite_index = frames[player.anime_index];
    if (player.data != NULL && sprite_index >= 0 && sprite_index < player.data->total_frames) {
        player.sprite_index = sprite_index;
    } else {
        player.sprite_index = 0;
    }
    player.anime_count++;
}

// LOOP
void player_update() {
    if (game_pause) {
        return;
    }
    player_update_position();

    switch (player.state) {
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
    case CROUCHING:
        player_crouch();
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
