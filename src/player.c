#include "player.h"
#include "allegro/gfx.h"
#include "allegro/inline/draw.inl"
#include "allegro/text.h"
#include "book.h"
#include "dat_manager.h"
#include "game.h"
#include "helpers.h"
#include "object.h"
#include "piece.h"
#include "platform.h"
#include "player.h"
#include "sinking.h"

#include "statics.h"
#include "tiles.h"
#include <allegro.h>
#include <math.h>
#include <stdio.h>

#define CAR_WIDTH 105
#define MARTIN_WIDTH 24
#define LEVEL_ID_INTRO 1
#define LEVEL_ID_RUNNING_START 3

#define PLAYER_DEFAULT_ENERGY 6
#define PLAYER_DEFAULT_LIVES 3
#define PLAYER_HURT_COOLDOWN_FRAMES 60
#define PLAYER_BLINK_INTERVAL 2
#define ALMANAC_DIALOG_X 64
#define ALMANAC_DIALOG_Y 84
#define ALMANAC_DIALOG_W 192
#define ALMANAC_DIALOG_H 32
#define ALMANAC_DIALOG_TEXT_X 72
#define ALMANAC_DIALOG_TEXT_Y 96

#define LEVEL5_WALL_X_LEFT 1551
#define LEVEL5_WALL_X_RIGHT 1576
#define LEVEL5_WALL_Y_TOP 80
#define LEVEL5_WALL_Y_BOTTOM 118

#define PLAYER_ACCEL 1
#define CLIMBING_SPEED 1
#define PLATFORM_SUPPORT_SNAP_PIXELS 2
#define JUMP_CUT_VY (-2)

struct playerType player;

// BITMAP* sp_coche[COCHE_FRAMES];
// BITMAP* sp_martin[MARTIN_FRAMES];
PlayerData coche = { 0, 0, 0, { NULL } };  // static data for car player type
PlayerData martin = { 0, 0, 0, { NULL } }; // static data for martin player type

FlowEventType pending_flow_event = { PLAYER_FLOW_NONE, 0 }; // struct version with optional data field for extra info when needed (e.g., tmx_id for room to enter)
static int almanac_tile_trigger_available = TRUE;

static animeItem car_animations[23] = {
    { 0, { 0 }, 0, -1 },     // NONE
    { 1, { 0 }, 1, 60 },     // STOP
    { 12, { 0, 1 }, 2, 2 },  // MOVE_LEFT
    { 12, { 0, 1 }, 2, 2 },  // MOVE_RIGHT
    { 16, { 2 }, 1, 2 },     // BREAKING
    { 3, { 0, 1 }, 2, 1 },   // JUMP_UP
    { 60, { 0, 1 }, 2, 1 },  // JUMP_DOWN
    { 16, { 0, 1 }, 2, 1 },  // JUMP_HIT
    { 1, { 0 }, 1, 1 },      // FALL
    { 1, { 0 }, 1, 1 },      // FALL2
    { 150, { 0, 3 }, 2, 5 }, // DEAD
    { 60, { 0 }, 1, 60 },    // FALL_END
    { 70, { 0 }, 1, 70 },    // DEAD_END
    { 20, { 0, 2 }, 2, 30 }, // BOUNCING
    { 0, {}, 0, 0 },         // CROUCHING (cars don't crouch)
    { 0, {}, 0, 0 },         // THROWING (cars don't throw)
    { 0, {}, 0, 0 },         // FALL_TO_FLOOR (cars don't fall to floor)
    { 0, {}, 0, 0 },         // KICKING (cars don't kick)
    { 0, {}, 0, 0 },         // RUNNING (cars don't run)
    { 0, {}, 0, 0 },         // RUNNING_JUMP (cars don't run)
    { 0, {}, 0, 0 },         // RUNNING_CROUCH (cars don't run)
    { 0, {}, 0, 0 },         // RUNNING_JUMP_DOWN (cars don't run)
    { 0, {}, 0, 0 },         // CLIMBING (cars don't climb)
};

static animeItem martin_animations[23] = {
    { 0, { 0 }, 0, -1 },                                        // NONE
    { 1, { 0 }, 1, 60 },                                        // STOP
    { 4, { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 }, 13, 5 }, // MOVE_LEFT
    { 4, { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 }, 13, 5 }, // MOVE_RIGHT
    { 4, { 20 }, 1, 2 },                                        // BREAKING
    { 4, { 13 }, 1, 1 },                                        // JUMP_UP
    { 12, { 13 }, 1, 1 },                                       // JUMP_DOWN
    { 16, { 13 }, 1, 1 },                                       // JUMP_HIT
    { 1, { 13 }, 1, 1 },                                        // FALL
    { 1, { 13 }, 1, 1 },                                        // FALL2
    { 150, { 0, 17 }, 2, 5 },                                   // DEAD
    { 60, { 0, 17 }, 2, 5 },                                    // FALL_END (play dead animation)
    { 70, { 0 }, 1, 70 },                                       // DEAD_END
    { 20, { 0, 2 }, 2, 30 },                                    // BOUNCING
    { 1, { 14 }, 1, 30 },                                       // CROUCHING
    { 5, { 15 }, 0, 0 },                                        // THROWING OBJECT
    { 5, { 13 }, 0, 0 },                                        // FALL_TO_FLOOR
    { 10, { 16 }, 0, 0 },                                       // KICKING
    { 12, { 0, 2, 6, 8 }, 4, 5 },                               // RUN RIGHT
    { 20, { 13 }, 1, 1 },                                       // RUNNING_JUMP
    { 8, { 18, 19 }, 2, 5 },                                    // RUNNING_CROUCH
    { 12, { 13 }, 1, 1 },                                       // RUNNING_JUMP_DOWN
    { 8, { 21, 22 }, 2, 8 },                                    // CLIMBING (ladder)
};

static uint8_t space_was_pressed = 0;
static int space_key_freed() {
    if (!space_was_pressed && key[KEY_SPACE]) {
        space_was_pressed = 1;
        return TRUE;
    } else if (space_was_pressed && !key[KEY_SPACE]) {
        space_was_pressed = 0;
    }
    return FALSE;
}

static uint8_t jump_was_pressed = 0;
static int jump_key_freed() {
    if (!jump_was_pressed && key[KEY_UP]) {
        jump_was_pressed = 1;
        return TRUE;
    } else if (jump_was_pressed && !key[KEY_UP]) {
        jump_was_pressed = 0;
    }
    return FALSE;
}

static uint8_t action_was_pressed = 0;
static int action_key_freed() {
    if (!action_was_pressed && (key[KEY_RCONTROL] || key[KEY_LCONTROL])) {
        action_was_pressed = 1;
        return TRUE;
    } else if (action_was_pressed && !key[KEY_RCONTROL] && !key[KEY_LCONTROL]) {
        action_was_pressed = 0;
    }
    return FALSE;
}

static uint8_t kick_was_pressed = 0;
static int kick_key_freed() {
    if (!kick_was_pressed && (key[KEY_ALT] || key[KEY_ALTGR])) {
        kick_was_pressed = 1;
        return TRUE;
    } else if (kick_was_pressed && !key[KEY_ALT] && !key[KEY_ALTGR]) {
        kick_was_pressed = 0;
    }
    return FALSE;
}

static void player_clamp_to_map_bounds(int current_level);

void player_new_game() {
    player.energy = PLAYER_DEFAULT_ENERGY;
    player.lives = PLAYER_DEFAULT_LIVES;
    player.has_almanac = FALSE;
    pending_flow_event = (FlowEventType){ PLAYER_FLOW_NONE, 0 };
}

void player_init(int x, int y, int current_level, int max_vx, int jump_vy) {
    player.pos.x = x;
    player.pos.y = y;
    player.vx = 0;
    player.vy = 0;
    player.jump_vy = jump_vy;
    player.prev_state = 0;
    player.anime_count = 0;
    player.anime_index = 0;
    player.flip = FALSE;
    player.boss_mode = FALSE;
    player.hurt_cooldown = 0;
    player.max_vx = max_vx;
    player.sprite_index = 0;
    player.animations = current_level == LEVEL_ID_INTRO ? car_animations : martin_animations;
    player.data = current_level == LEVEL_ID_INTRO ? &coche : &martin;
    player.type = current_level == LEVEL_ID_INTRO ? CAR_TYPE : MARTIN_TYPE;
    player.state = current_level == LEVEL_ID_RUNNING_START ? RUNNING : STOP;
    player.move_count = player.animations[player.state].move_count;
    player.riding_platform_idx = -1;
}

void player_took_almanac() {
    player.has_almanac = TRUE;
}

/**
@brief shows the reminder of buying the almanac when stepping on the almanac tile without having it,
 */
static void player_show_almanac_dialog() {
    rectfill(
        screen,
        ALMANAC_DIALOG_X,
        ALMANAC_DIALOG_Y,
        ALMANAC_DIALOG_X + ALMANAC_DIALOG_W,
        ALMANAC_DIALOG_Y + ALMANAC_DIALOG_H,
        makecol(255, 255, 255));
    rect(
        screen,
        ALMANAC_DIALOG_X,
        ALMANAC_DIALOG_Y,
        ALMANAC_DIALOG_X + ALMANAC_DIALOG_W,
        ALMANAC_DIALOG_Y + ALMANAC_DIALOG_H,
        makecol(0, 0, 0));
    print_at(
        ALMANAC_DIALOG_TEXT_X,
        ALMANAC_DIALOG_TEXT_Y,
        game_text(TXT_ROOM_05),
        makecol(0, 0, 0),
        makecol(255, 255, 255));

    clear_keybuf();
    do {
    } while (!key[KEY_SPACE]);
    do {
    } while (key[KEY_SPACE]);
}

void load_coche_spritesheet() {
    BITMAP* coche_spritesheet = dat_file[COCHE_SPRITESHEET_BMP].dat;
    int frame_width = CAR_WIDTH;
    coche.width = frame_width;
    coche.height = coche_spritesheet->h;
    coche.total_frames = COCHE_FRAMES;
    // player.current_sprite = create_video_bitmap(player.width, player.height);
    int offset = 0;
    for (int i = 0; i < COCHE_FRAMES; i++) {
        coche.sprites[i] = create_sub_bitmap(coche_spritesheet, offset, 0, frame_width, coche_spritesheet->h);
        offset += frame_width;
    }
}

void destroy_coche_spritesheet() {
    BITMAP* coche_spritesheet = dat_file[COCHE_SPRITESHEET_BMP].dat;
    for (int i = 0; i < COCHE_FRAMES; i++) {
        destroy_bitmap(coche.sprites[i]);
    }
}

void load_martin_spritesheet() {
    BITMAP* martin_spritesheet = dat_file[MARTIN_SPRITESHEET_BMP].dat;
    int frame_width = MARTIN_WIDTH;
    martin.width = frame_width;
    martin.height = martin_spritesheet->h;
    martin.total_frames = MARTIN_FRAMES;
    int offset = 0;
    for (int i = 0; i < MARTIN_FRAMES; i++) {
        martin.sprites[i] = create_sub_bitmap(martin_spritesheet, offset, 0, frame_width, martin_spritesheet->h);
        offset += frame_width;
    }
}

void destroy_martin_spritesheet() {
    BITMAP* martin_spritesheet = dat_file[MARTIN_SPRITESHEET_BMP].dat;
    for (int i = 0; i < MARTIN_FRAMES; i++) {
        destroy_bitmap(martin.sprites[i]);
    }
}

static void player_change_state(unsigned int state) {
    player.prev_state = player.state;
    player.state = state;
    player.move_count = player.animations[player.state].move_count;
}
void player_energy_up() {
    player.energy = PLAYER_DEFAULT_ENERGY;
}

void player_life_up() {
    player.lives++;
}

void player_throwable_up() {
    player.throwable_count++;
}

void player_on_hit() {
    if (megahit_mode == 1) {
        return; // in megahit mode, player is invincible
    }
    if (player.state == DEAD || player.state == FALL_END || player.state == DEAD_END) {
        return; // already in dying/dead state, ignore further hits
    }

    if (player.hurt_cooldown > 0) {
        return;
    }

    player.energy--;

    if (player.energy <= 0) {
        player_change_state(DEAD);
        player.vx = 0;
    } else {
        player.hurt_cooldown = PLAYER_HURT_COOLDOWN_FRAMES;
    }
}

/**
 * @brief Applies a force (vy or vx) over player
 *
 * @param vx horz velocity
 * @param vy vert velocity
 *
 */
static void player_affect_force(int vx, int vy) {
    player.vy += vy;
    player.vx += vx;
}

/**
 * @brief Checks if player is over an object
 *
 * @return TRUE if player is in top of object

static int player_is_on_obj() {
    // returns true if sprite is over a walkable tile
    // todo foot_area collision
    if (player.pos.y > GROUND_Y) {
        return TRUE;
    }
    if (player.type == CAR_TYPE) {
        collisionType f1 = rear_wheels_area();
        if (checkOverObj(f1)) {
            return TRUE;
        }
        collisionType f2 = front_wheels_area();
        return checkOverObj(f2);
    }
    else {
        collisionType f1 = player_foot_area();
        return checkOverObj(f1);
    }
    return FALSE;
} */

/**
 * @brief defines the area of the rear car wheels for collision detection
 */
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

/**
 * @brief defines the area of the front car wheels for collision detection
 */
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

/**
 * @brief defines the area of the main player feet for collision detection
 */
inline collisionType player_foot_area() {
    if (player.data == NULL || player.data->height == 0 || player.data->width == 0) {
        return (collisionType){ 0, 0, 0, 0 };
    }
    int y1 = player.pos.y + player.data->height - 3;
    // player.pos.x - scroll_x +4, y1, player.pos.x + player.width - scroll_x-2, y1 + 5

    collisionType ret = {
        .x = player.pos.x + 6,
        .y = y1,
        .w = 12,
        .h = 4
    };
    return ret;
}

collisionType player_leg_aabb() {
    collisionType box = {
        .x = player.pos.x,
        .y = player.pos.y + 20,
        .w = 24,
        .h = 15
    };
    if (player.flip == TRUE) { // facing left: extend the left edge
        box.x -= 5;
        box.w += 5;
    } else { // facing right: extend the right edge
        box.w += 5;
    }
    return box;
}

collisionType player_fist_aabb() {
    return (collisionType){
        .x = player.pos.x,
        .y = player.pos.y + 10,
        .w = 24,
        .h = 10
    };
}
// 84x16
inline collisionType player_aabb() {
    if (player.data == NULL || player.data->height == 0 || player.data->width == 0) {
        return (collisionType){ 0, 0, 0, 0 };
    }
    int y1 = player.pos.y + player.data->height - 3;
    // player.pos.x - scroll_x +4, y1, player.pos.x + player.width - scroll_x-2, y1 + 5

    if (player.state == CROUCHING) {
        return (collisionType){
            .x = player.pos.x,
            .y = player.pos.y + 14,
            .w = 20,
            .h = 26
        };
    } else if (player.state == KICKING) {
        return (collisionType){
            .x = player.flip == TRUE ? player.pos.x - 10 : player.pos.x,
            .y = player.pos.y,
            .w = player.flip == FALSE ? 24 : 18,
            .h = 40
        };
    } else if (player.state == RUNNING_CROUCH) {
        return (collisionType){
            .x = player.pos.x,
            .y = player.pos.y + 10,
            .w = 20,
            .h = 36
        };
    } else if (player.state == THROWING) {
        return (collisionType){
            .x = player.flip == TRUE ? player.pos.x - 10 : player.pos.x +5,
            .y = player.pos.y + 20,
            .w = player.flip == FALSE ? 24 : 18,
            .h = 15
        };
    } else {
        return (collisionType){
            .x = player.pos.x,
            .y = player.pos.y,
            .w = 18,
            .h = 40
        };
    }
}

/**
 * @brief handles special player behavior in level 4
 *     special treatment for level 4, where boss appears when all pieces taken
 *     once taken, player can go beyond the right edge of the map to reach the boss
 *     and cannot return left to the normal map to fight the boss
 */
static inline void player_in_level4() {
    int leftside = map_pixel_width - SCREEN_W;
    if (player.boss_mode == TRUE) {
        if (player.pos.x < leftside) {
            player.pos.x += 1;
            // player.pos.x = leftside;
            if (player.vx < 0) {
                player.vx = 0;
            }
        }
    } else {
        if (almanac_tile_x >= 0 && piece_get_remaining() == 0) {
            if (player.pos.x >= almanac_tile_x) {
                player.pos.x = almanac_tile_x - 1;
                if (player.vx > 0) {
                    player.vx = 0;
                    player.boss_mode = TRUE;
                }
            }
        }
    }
}

/**
 * @brief invisible wall in level 5: blocks rightward passage through the
 *        rectangle x [LEVEL5_WALL_X_LEFT, LEVEL5_WALL_X_RIGHT], y [80, 118].

static inline void player_in_level5() {
    if (player.pos.y >= LEVEL5_WALL_Y_TOP && player.pos.y <= LEVEL5_WALL_Y_BOTTOM &&
        player.pos.x >= LEVEL5_WALL_X_LEFT && player.pos.x <= LEVEL5_WALL_X_RIGHT) {
        player.pos.x = LEVEL5_WALL_X_LEFT - 1;
        if (player.vx > 0) {
            player.vx = 0;
        }
    }
}*/

static void player_clamp_to_map_bounds(int current_level) {
    if (player.pos.x < 2) {
        player.pos.x = 2;
        if (player.vx < 0) {
            player.vx = 0;
        }
    }

    if (current_level == LEVEL_MOUNTAIN) {
        player_in_level4();
    }

    /*if (current_level == 5) {
        player_in_level5();
    }*/

    if (player.data != NULL && map_pixel_width > 0) {
        int max_x = map_pixel_width - player.data->width;
        if (max_x < 2) {
            max_x = 2;
        }

        if (player.pos.x > max_x) {
            player.pos.x = max_x;
        }
        if (player.pos.x >= max_x && player.vx > 0) {
            player.vx = 0;
            player.pos.x = max_x;
        }
    }
}

static void player_clear_platform_support() {
    player.riding_platform_idx = -1;
}

static int player_update_platform_support() {
    int platform_index = -1;
    int platform_top = 0;

    if (player.type != MARTIN_TYPE || player.data == NULL) {
        player_clear_platform_support();
        return FALSE;
    }

    collisionType foot = player_foot_area();
    if (platform_find_support(foot, PLATFORM_SUPPORT_SNAP_PIXELS, &platform_index, &platform_top)) {
        player.riding_platform_idx = platform_index;
        return TRUE;
    }

    player_clear_platform_support();
    return FALSE;
}

static void player_apply_platform_carry(int current_level) {
    int dx = 0;
    int dy = 0;

    if (player.type != MARTIN_TYPE || player.riding_platform_idx < 0) {
        return;
    }

    if (!platform_get_delta(player.riding_platform_idx, &dx, &dy)) {
        player_clear_platform_support();
        return;
    }

    player.pos.x += dx;
    player.pos.y += dy;
    // a bit overkill to check this player_clamp_to_map_bounds(current_level);
}

/**
 * @brief Checks player speed
 *
 */
static void player_check_vy() {
    /*if (player.state == DEAD) {
        return;
    }*/

    if (player.state == JUMP_HIT || player.state == DEAD || player.state == FALL_END) {
        player.vy = 0;
        return;
    }

    if (player.vy > 0) {
        if (player.type == MARTIN_TYPE && martin_is_on_obj()) {
            player_clear_platform_support();
            player.vy = 0;
        }
        if (player.type == CAR_TYPE && car_is_on_obj()) {
            player.vy = 0;
        }
    }
}

/**
 * @brief Checks vx for hits
 */
static void player_check_vx(int current_level) {
    player_clamp_to_map_bounds(current_level);

    if (player.vx != 0) {
        if (checkHitObj()) {
            player.vx = 0; // wall ahead: stop, do not cross
        }
    }
}

/**
 * @brief Updates player position based on velocities
 */
static void player_move_y_substeps() {
    if (player.vy == 0) {
        player_update_platform_support();
        return;
    }

    int step_dir = (player.vy > 0) ? 1 : -1;
    int steps = (player.vy > 0) ? player.vy : -player.vy;
    if (steps > 10)
        steps = 10;

    for (int i = 0; i < steps; i++) {
        player.pos.y += step_dir;

        if (step_dir > 0) {
            if (player.type == MARTIN_TYPE && martin_is_on_obj()) {
                collisionType foot = player_foot_area();
                if (player.data != NULL && foot.h > 0) {
                    int foot_bottom = foot.y + foot.h - 1;
                    // optimized as tiles are 8x8
                    int tile_row = foot_bottom >> 3;
                    int tile_top = tile_row << 3;
                    player.pos.y = tile_top - player.data->height;
                }
                player_clear_platform_support();
                player.vy = 0;
                break;
            } else if (player.type == MARTIN_TYPE) {
                int platform_index = -1;
                int platform_top = 0;
                collisionType foot = player_foot_area();
                if (platform_find_support(foot, PLATFORM_SUPPORT_SNAP_PIXELS, &platform_index, &platform_top)) {
                    if (player.data != NULL) {
                        player.pos.y = platform_top - player.data->height;
                    }
                    player.riding_platform_idx = platform_index;
                    player.vy = 0;
                    break;
                }
                // sinking platforms give footing only while solid; no carry (they don't move).
                // pool is empty outside LEVEL_CITY (reset_sinking), so this is a no-op there.
                if (sinking_find_support(foot, PLATFORM_SUPPORT_SNAP_PIXELS, &platform_index, &platform_top)) {
                    if (player.data != NULL) {
                        player.pos.y = platform_top - player.data->height;
                    }
                    player.vy = 0;
                    break;
                }
            } else if (player.type == CAR_TYPE && car_is_on_obj()) {
                collisionType rear = rear_wheels_area();
                if (player.data != NULL && rear.h > 0) {
                    int support_bottom = rear.y + rear.h - 1;
                    // optimized as tiles are 8x8
                    int tile_row = support_bottom >> 3;
                    int tile_top = tile_row << 3;
                    int support_offset_y = rear.y - player.pos.y;
                    player.pos.y = tile_top - support_offset_y - rear.h;
                }
                
                player.vy = 0;
                break;
            }
        }
    }
}

static void player_update_position(int current_level) {
    player_apply_platform_carry(current_level);

    player_check_vx(current_level);
    player_check_vy();
    player.pos.x = round(player.pos.x + player.vx);

    player_clamp_to_map_bounds(current_level);

    player_move_y_substeps();

    if (player.pos.y > 130 && player.state != FALL_END && player.state != DEAD && player.state != DEAD_END) {
        player_change_state(FALL_END);
        player.vx = 0;
        player.vy = 0;
    }
}

/**
 * @brief Counts how much movement is done per action
 *
 * @param dx
 * @param dy
 *
 * @return FINISHED/NOT_FINISHED
 */
static unsigned int player_count_move(int dx, int dy) {
    if (player.move_count >= 0) {
        player.move_count--;
    }

    if (player.move_count < 0) {
        return FINISHED;
    }
    return NOT_FINISHED;
}

// DOERS: CALLED ON KEY PRESS TO CHANGE STATE
static void player_do_move_left() {
    player.vx = -1;
    player_change_state(MOVE_LEFT);
    player.flip = TRUE;
}

static void player_do_move_right() {
    player.vx = 1;
    player_change_state(MOVE_RIGHT);
    player.flip = FALSE;
}

static inline void player_do_stop() {
    player.vx = 0;
    player.vy = 0;
    player_change_state(STOP);
}

/**
 * @brief called on throw key press, changes player state to THROWING
 */
static inline void player_do_throw() {
    player_change_state(THROWING);
    player.vx = 0;
    // throw logic, create throwable object (book), set its position and velocity based on player state and direction
    init_book(
        player.pos.x + (player.flip ? -6 : 6),
        player.pos.y + 3,
        player.flip);
}

static inline void player_do_kick() {
    player_change_state(KICKING);
    player.vx = 0;
}

static inline void player_do_open() {
    // only for martin, coche doesn't open
    int result = martin_is_over_door();
    if (result != -1) {
        pending_flow_event.type = PLAYER_ENTER_ROOM;
        pending_flow_event.data = result;
    }
    // same Space/AABB interaction as doors: drop carried TNT onto a BOX (level 6)
    tnt_place_on_box_if_over();
}

/**
 * @brief Player performs a jump, that could be diagonal
 */
static inline void player_do_jump() {
    player_clear_platform_support();

    if (key[KEY_LEFT]) {
        player.flip = TRUE;
        if (player.vx > -player.max_vx) {
            player.vx -= PLAYER_ACCEL;
            if (player.vx < -player.max_vx)
                player.vx = -player.max_vx;
        } else {
            player.vx = -player.max_vx;
        }
    }
    if (key[KEY_RIGHT]) {
        player.flip = FALSE;
        if (player.vx < player.max_vx) {
            player.vx += PLAYER_ACCEL;
            if (player.vx > player.max_vx)
                player.vx = player.max_vx;
        } else {
            player.vx = player.max_vx;
        }
    }
    player.vy = player.jump_vy;
    player_change_state(JUMP_UP);
}

// ACTIONS
static void player_action_fall() {
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
    } else if (player.pos.y > 130) {
        player_change_state(FALL_END);
        player.vx = 0;
    }

    if (action_key_freed()) {
        player_do_throw();
    } else if (kick_key_freed()) {
        player_do_kick();
    }
}

void player_killed() {
    player_change_state(DEAD);
}

static void player_traveling() {
    player_change_state(BOUNCING);
}

/**
 * @brief called on crouch key press, changes player state to CROUCHING
 */
static inline void player_do_crouch() {
    player_change_state(CROUCHING);
}

/**
 * @brief If the player presses up/down while over a ladder tile, grab it:
 *        snap to the ladder column, kill velocity, enter CLIMBING.
 * @return TRUE if the ladder was grabbed this frame.
 */
static int player_try_enter_ladder() {
    if (player.type != MARTIN_TYPE || player.state == CLIMBING) {
        return FALSE;
    }
    // UP grabs when the body center is over the shaft (climbing up from below);
    // DOWN grabs when the feet are over a ladder tile (descending from the top,
    // where the center is still above the ladder). Never auto-grab while falling.
    int grab_up = key[KEY_UP] && player_is_over_ladder();
    int grab_down = key[KEY_DOWN] && player_foot_over_ladder();
    if (!grab_up && !grab_down) {
        return FALSE;
    }
    // center the player on the ladder tile column (shifts only, no division)
    int col = (player.pos.x + (player.data->width >> 1)) >> 3;
    player.pos.x = (col << 3) + 4 - (player.data->width >> 1);
    player.vx = 0;
    player.vy = 0;
    // entering from the top: nudge down into the shaft so the center probe
    // catches the ladder and we clear the solid top tile
    if (grab_down && !player_is_over_ladder()) {
        player.pos.y += CLIMBING_SPEED;
    }
    player_change_state(CLIMBING);
    return TRUE;
}

/**
 * @brief Ladder climbing FSM action.
 *        UP/DOWN climb (DOWN stops at the ladder foot), LEFT/RIGHT walk off the
 *        side, and leaving the ladder returns to STOP so gravity resumes.
 *        Note: UP is also the jump key, so there is no jump-to-dismount.
 */
static void player_action_climbing() {
    // stay on the ladder while either the body center or the feet overlap it,
    // so climbing up continues until the feet clear the top onto the platform
    if (!player_is_over_ladder() && !player_foot_over_ladder()) { // off the top or walked off the side
        player_change_state(STOP);
        player.vy = 0;
        return;
    }

    int moving = FALSE;

    if (key[KEY_UP]) {
        player.pos.y -= CLIMBING_SPEED;
        moving = TRUE;
    } else if (key[KEY_DOWN]) {
        if (player_foot_over_ladder()) { // keep footing on the ladder
            player.pos.y += CLIMBING_SPEED;
            moving = TRUE;
        } else { // reached the bottom of the ladder, stand on the ground
            player_change_state(STOP);
            player.vy = 0;
            return;
        }
    }

    if (key[KEY_LEFT]) {
        player.pos.x -= CLIMBING_SPEED;
        player.flip = TRUE;
        moving = TRUE;
    } else if (key[KEY_RIGHT]) {
        player.pos.x += CLIMBING_SPEED;
        player.flip = FALSE;
        moving = TRUE;
    }

    if (moving) {
        player_count_move(0, 0);
    } else {
        // idle on the ladder: freeze on frame 21 (reset the anime timer so
        // player_anime_update does not tick it forward to frame 22)
        player.anime_index = 0;
        player.anime_count = 0;
    }
}

// ACTIONS: called on update loop to perform current action and transitions

/**
 * @brief moves player to the left, with possible jump if up is pressed. Called on update loop when state is MOVE_LEFT
 */
static void player_action_move_left() {
    if (jump_key_freed()) {
        player_do_jump();
        return;
    } else if (action_key_freed()) {
        player_do_throw();
        return;
    } else if (kick_key_freed()) {
        player_do_kick();
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
        player_do_move_left();
    } else if (player.prev_state == STOP) {
        player_do_stop();
    } else {
        player.vx = -1;
        player_change_state(BREAKING);
    }
}

/**
 * @brief moves player to the right, with possible jump if up is pressed. Called on update loop when state is MOVE_RIGHT
 */
static void player_action_move_right() {
    if (jump_key_freed()) {
        player_do_jump();
        return;
    } else if (action_key_freed()) {
        player_do_throw();
        return;
    } else if (kick_key_freed()) {
        player_do_kick();
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
        player_do_move_right();
    } else if (player.prev_state == STOP) {
        player_do_stop();
    } else {
        player.vx = 1;
        player_change_state(BREAKING);
    }
}

static void player_action_stop() {

    if (player.vy > 0) {
        if (player.vx == 0) {
            player_change_state(FALL);
        } else {
            player_change_state(FALL2);
        }
    } else if (key[KEY_LEFT]) {
        player_do_move_left();
    } else if (key[KEY_RIGHT]) {
        player_do_move_right();
    } else if (jump_key_freed()) {
        player_do_jump();
    } else if (key[KEY_DOWN]) {
        player_do_crouch();
    } else if (action_key_freed()) {
        player_do_throw();
    } else if (kick_key_freed()) {
        player_do_kick();
    } else if (space_key_freed()) {
        player_do_open();

    } else {
        player_count_move(0, 0);
    }
}
// special cases for auto-running
static void player_action_running() {
    player.vx = 1;
    if (jump_key_freed()) {
        player_clear_platform_support();
        player.vy = player.jump_vy; // extra boost for running jumps
        player_change_state(RUNNING_JUMP);
    } else if (key[KEY_DOWN]) {
        player_change_state(RUNNING_CROUCH);
    }
}

static void player_action_running_jump() {
    player.vx = 1;
    player.flip = FALSE;

    if (player.vy > 0) {
        player_change_state(RUNNING_JUMP_DOWN);
        return;
    }
    if (player.vy == 0) {
        player_change_state(RUNNING_JUMP_DOWN);
    }

    if (player_count_move(player.vx, 0) == FINISHED) {
        player_change_state(RUNNING_JUMP_DOWN);
    }
}

static void player_action_running_crouch() {
    player.vx = 1;
    player.flip = FALSE;

    if (jump_key_freed()) {
        player_clear_platform_support();
        player.vy = player.jump_vy;
        player_change_state(RUNNING_JUMP);
        return;
    }

    if (!key[KEY_DOWN]) {
        player_change_state(RUNNING);
        return;
    }

    player_count_move(player.vx, 0);
}

static void player_action_running_jump_down() {
    if (player.vy != 0) {
        player_count_move(player.vx, 0);
        return;
    }
    player_change_state(RUNNING);
}

static void player_action_crouch() {
    if ((key[KEY_RCONTROL] || key[KEY_LCONTROL])) {
        if (player.pos.y < GROUND_Y) {
            player_change_state(FALL_TO_FLOOR);
            player.vy = 1;
        }
    }

    if (!key[KEY_DOWN]) {
        player_change_state(STOP);
    } else {
        player_count_move(0, 0);
    }
}

static void player_action_jump_up() {
    // variable jump height: release early to cut ascent
    if (player.vy < JUMP_CUT_VY && !key[KEY_UP]) {
        player.vy = JUMP_CUT_VY;
    }

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
        }
        // allow move in air
        if (key[KEY_LEFT]) {
            player.flip = TRUE;
            if (player.vx < 0) {
                player.vx -= PLAYER_ACCEL;
                if (player.vx < -player.max_vx)
                    player.vx = -player.max_vx;
            } else {
                player.vx = -1;
            }
        } else if (key[KEY_RIGHT]) {
            player.flip = FALSE;
            if (player.vx > 0) {
                player.vx += PLAYER_ACCEL;
                if (player.vx > player.max_vx)
                    player.vx = player.max_vx;
            } else {
                player.vx = 1;
            }
        }
    }
    // arc driven by gravity: transition when vy reaches 0 or positive
    if (player.vy >= 0) {
        player_change_state(JUMP_DOWN);
        return;
    }

    if (action_key_freed()) {
        player_do_throw();
    } else if (kick_key_freed()) {
        player_do_kick();
    }
}

static void player_action_jump_down() {
    if (player.vy != 0) {
        if (player_count_move(player.vx, 0) == FINISHED) {
            player.vx = 0;
        }
        return;
    } else {
        if (player.vx > 0 && key[KEY_RIGHT]) {
            player_change_state(MOVE_RIGHT);
        } else if (player.vx < 0 && key[KEY_LEFT]) {
            player_change_state(MOVE_LEFT);
        } else {
            player_do_stop();
        }
    }
}

static void action_jump_hit() {
    if (player_count_move(player.vx, -player.vy) == FINISHED) {
        player_change_state(JUMP_DOWN);
    }
}

static void player_action_breaking() {
    if (player_count_move(player.vx, 0) == NOT_FINISHED) {
        return;
    }
    player_do_stop();
}

/**
@brief called when player energy reaches 0, performs death animation and transitions to DEAD_END or STOP based on lives left
*/
static void player_action_dead() {
    if (player_count_move(0, 0) == FINISHED) {
        player.energy = PLAYER_DEFAULT_ENERGY;
        reinit_book_stock();
        if (megahit_mode == 0) {
            player.lives--;
        }
        player_change_state(DEAD_END);
        if (player.lives <= 0) {
            pending_flow_event.type = PLAYER_FLOW_GAME_OVER;
            return;
        } else {
            pending_flow_event.type = PLAYER_FLOW_RESTART_STAGE;
        }

        // TODO restart level
        player.pos.y = GROUND_Y;
        player.vx = 0;
        player.vy = 0;

        return;
    }
}

FlowEventType player_consume_flow_event() {
    FlowEventType event = pending_flow_event;
    pending_flow_event = (FlowEventType){ PLAYER_FLOW_NONE, 0 };
    return event;
}

static void player_action_throw() {
    // TODO: implement throwing action and pass to stop
    if (player_count_move(0, 0) == FINISHED) {
        player_change_state(STOP);
    }
}

static void player_action_kick() {
    if (player_count_move(0, 0) == FINISHED) {
        player_change_state(STOP);
    }
}

void player_action_fall_end() {
    if (player_count_move(0, 0) == FINISHED) {
        player.lives--;
        player.energy = PLAYER_DEFAULT_ENERGY;
        player_change_state(DEAD_END);
        if (player.lives <= 0) {
            pending_flow_event.type = PLAYER_FLOW_GAME_OVER;
            return;
        } else {
            pending_flow_event.type = PLAYER_FLOW_RESTART_STAGE;
        }
    }
}

// conditional
int player_is_deading() {
    if (player.state == DEAD_END) {
        return TRUE;
    } else {
        return FALSE;
    }
}

/**
Anime update will take the current state and update the sprite index based on the animation defined for that state.
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
inline void player_draw(int scroll_x) {
    if (player.hurt_cooldown > 0 && ((player.hurt_cooldown >> 1) & 1) == 0) {
        return;
    }

    if (player.data && player.sprite_index >= 0 && player.sprite_index < player.data->total_frames && player.data->sprites[player.sprite_index] != NULL) {
        if (player.flip == TRUE && player.type != CAR_TYPE) { // cars don't flip
            draw_sprite_h_flip(current_screen, player.data->sprites[player.sprite_index], player.pos.x - scroll_x, player.pos.y);
        } else {
            draw_sprite(current_screen, player.data->sprites[player.sprite_index], player.pos.x - scroll_x, player.pos.y);
        }
    } else {
        textprintf_ex(current_screen, font, 10, 10, makecol(255, 0, 0), -1, "DEBUG: invalid sprite idx %d", player.sprite_index);
    }
}

// FSM LOOP for player, called on game loop when world_state is GAME_RUN
void player_update(int current_level) {
    if (game_pause) {
        return;
    }

    if (player.hurt_cooldown > 0) {
        player.hurt_cooldown--;
    }

    player_try_enter_ladder();

    // gravity always applied for smooth arc (except while climbing a ladder)
    if (player.state != CLIMBING) {
        player_affect_force(0, (player.anime_index & 1) == 0);
    }
    // extra gravity on descent for snappier fall (Mario-style)
    if ((current_level != 3) && (player.vy > 0 && (player.state == JUMP_UP || player.state == JUMP_DOWN || player.state == RUNNING_JUMP || player.state == RUNNING_JUMP_DOWN))) {
        player_affect_force(0, 1);
    }
    player_update_position(current_level);

    if (current_level == 2) {
        if (player_is_over_almanac_tile()) {
            if (almanac_tile_trigger_available == TRUE) {
                almanac_tile_trigger_available = FALSE;
                if (player.has_almanac) {
                    pending_flow_event.type = PLAYER_ADVANCE_STAGE;
                } else {
                    player_show_almanac_dialog();
                }
            }
        } else {
            almanac_tile_trigger_available = TRUE;
        }
    }

    if (player_is_over_advance_tile()) {
        pending_flow_event.type = PLAYER_ADVANCE_STAGE;
    }

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
    case RUNNING:
        player_action_running();
        break;
    case RUNNING_JUMP:
        player_action_running_jump();
        break;
    case RUNNING_CROUCH:
        player_action_running_crouch();
        break;
    case RUNNING_JUMP_DOWN:
        player_action_running_jump_down();
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
        player_action_crouch();
        break;
    case THROWING:
        player_action_throw();
        break;
    case KICKING:
        player_action_kick();
        break;
    case CLIMBING:
        player_action_climbing();
        break;
    case FALL_TO_FLOOR:
        player.pos.y += 1;
        if (player.pos.y >= GROUND_Y) {
            player.pos.y = GROUND_Y;
            player_do_stop();
        }
        break;
    }
    player_anime_update();
}

void player_has_all_tnt() {
    pending_flow_event.type = PLAYER_ADVANCE_STAGE;
}

void player_has_beaten_boss() {
    pending_flow_event.type = PLAYER_BEAT_BOSS;
}
