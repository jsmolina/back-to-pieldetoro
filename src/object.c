#include "object.h"
#include "book.h"
#include "coin.h"
#include "door.h"
#include "enemy.h"
#include "enemy_throw.h"
#include "game.h"
#include "helpers.h"
#include "piece.h"
#include "player.h"
#include "tiles.h"
#include "tnt.h"

#define CAR_PLATFORM_SIZE 9
#define ALMANAC_TILE_ID 1040
#define ADVANCE_TILE_ID 1071
#define BOMB_JUMP_CLEARANCE_MARGIN 5

// simple AABB collision detection
int collision(collisionType obj1, collisionType obj2) {
    int flg = obj1.x >= obj2.x + obj2.w
        || obj2.x >= obj1.x + obj1.w
        || obj1.y >= obj2.y + obj2.h
        || obj2.y >= obj1.y + obj1.h;
    return !flg;
}

// + 16 ruedas touches ground?
int tiles_at_positions[4];
void check_tiles_around_player(int* tiles_at_pos) {
    // Obtén los 4 tiles de las esquinas del jugador
    tiles_at_pos[0] = get_tile_at_position(player.pos.x, player.pos.y);                                                // Superior izquierda
    tiles_at_pos[1] = get_tile_at_position(player.pos.x + player.data->width, player.pos.y);                           // Superior derecha
    tiles_at_pos[2] = get_tile_at_position(player.pos.x, player.pos.y + player.data->height - 4);                      // Inferior izquierda
    tiles_at_pos[3] = get_tile_at_position(player.pos.x + player.data->width, player.pos.y + player.data->height - 4); // Inferior derecha
}

static inline int is_harmful_tile(int id) {
    return id == HARMFUL_TILE_1 || id == HARMFUL_TILE_2;
}

static inline int is_back_in_time_tile(int id) {
    return id == 876 || id == 1040;
}
/*
static const int platform_ids1[GAME_PLATFORMS_SIZE1] = {
    39, 45, 46, 47, 48, 49, 109, 137, 141,
    174, 175, 176, 177,
    178, 137, 135, 253,
    254, 255
};
static const int platform_ids2[GAME_PLATFORMS_SIZE2] = {
    296, 297, 298, 299, 630, 631,
    906, 907, 908, 920, 921, 922,
    1016, 1017, 1018
};*/

int is_a_platform(int id) {
    // two rows of tiles are considered platforms, this check is faster
    // than looping through an array of platform ids and works because platforms are grouped together in the tileset
    if (id > 1023 && id < 1088) {
        return TRUE;
    }
    return FALSE;
}

// static const int car_platform_ids[CAR_PLATFORM_SIZE] = { 906, 907, 908, 909, 865, 866, 864, 867, 832 };
/*
static inline int is_a_car_platform(int id) {
    for (int i = 0; i < CAR_PLATFORM_SIZE; i++) {
        if (car_platform_ids[i] == id) {
            return TRUE;
        }
    }
    return FALSE;
}*/

static int rect_over_tile_types(collisionType r, int is_wheel) {
    if (r.w <= 0 || r.h <= 0)
        return 0;

    int sx = r.x >> 3;
    int ex = (r.x + r.w - 1) >> 3;
    /*int sy = r.y >> 3;
    int ey = (r.y + r.h - 1) >> 3;*/
    int sy = (r.y + r.h - 1) >> 3; // only inferior row for platform check
    int ey = sy;

    if (sx < 0)
        sx = 0;
    if (sy < 0)
        sy = 0;
    if (ex >= curr_tiles_width)
        ex = curr_tiles_width - 1;
    if (ey >= MAX_VERT_TILES)
        ey = MAX_VERT_TILES - 1;

    for (int ty = sy; ty <= ey; ++ty) {
        for (int tx = sx; tx <= ex; ++tx) {
            int tile_x = tx << 3;            // tile left edge
            int tile_bottom = (ty + 1) << 3; // tile bottom edge

            // Calculate vertical overlap (how deep into the tile)
            int overlap = tile_bottom - r.y; // distance from foot to tile bottom

            if (overlap < 4) // margin of 2px
                continue;    // ignore shallow collisions

            int tile_id = tiles_values[ty][tx] - 1;

            if (is_wheel) {
                if (is_harmful_tile(tile_id))
                    return HARMFUL;
                if (is_back_in_time_tile(tile_id))
                    return BACK_IN_TIME;
                if (is_a_platform(tile_id))
                    return PLATFORM;
            } else {
                if (tile_id == ALMANAC_TILE_ID)
                    return ALMANAC;
                if (tile_id == ADVANCE_TILE_ID)
                    return ADVANCE;
                if (is_a_platform(tile_id))
                    return PLATFORM;
                if (is_back_in_time_tile(tile_id))
                    return BACK_IN_TIME;
                if (tile_id == WALL_TILE)
                    return WALL;
            }
        }
    }
    return ROAD;
}

/* Returns true if one of wheels is over (foot_area/foot_area2)
   a an special tile, FALSE otherwise. */
int wheels_on_tiles() {
    int result;

    collisionType f1 = rear_wheels_area();
    result = rect_over_tile_types(f1, TRUE);
    if (result != PLATFORM) {
        return result;
    }

    collisionType f2 = front_wheels_area();
    result = rect_over_tile_types(f2, TRUE);
    if (result != PLATFORM) {
        return result;
    }

    return ROAD;
}

// will check if player is over a walkable thing
int checkOverObj(collisionType area) {
    int result = rect_over_tile_types(area, FALSE);
    if (result == PLATFORM || result == ALMANAC || result == ADVANCE) {
        return TRUE;
    }

    return FALSE;
}

// TRUE if the player is walking into a solid wall tile (blocks both directions).
int checkHitObj() {
    if (player.vx == 0) {
        return FALSE;
    }
    collisionType foot = player_foot_area();
    if (foot.w <= 0) {
        return FALSE;
    }
    // probe just past the leading edge of the foot, at the floor-tile row
    int probe_x = (player.vx > 0) ? (foot.x + foot.w) : (foot.x - 1);
    int probe_y = player.pos.y > 81 ? 161: 81;
    int tile_id = get_tile_at_position(probe_x, probe_y) - 1;
    return tile_id == WALL_TILE_2;
}

void collision_check_throwable_vs_enemy() {
    collisionType boxes[MAX_THROWABLE_OBJECTS];
    collisionType enemies[MAX_ACTIVE_ENEMIES];
    book_get_all_aabb(boxes);
    enemy_get_all_aabb(enemies);
    for (int throw_id = 0; throw_id < MAX_THROWABLE_OBJECTS; throw_id++) {
        if (boxes[throw_id].w == 0)
            continue;

        for (int enemy_id = 0; enemy_id < MAX_ACTIVE_ENEMIES; enemy_id++) {
            if (enemies[enemy_id].w == 0)
                continue;

            if (enemies[enemy_id].meta != ENEMY_DOG && collision(boxes[throw_id], enemies[enemy_id])) {
                book_on_hit(throw_id);
                enemy_on_hit(enemy_id);
            }
        }
    }
}

void collision_check_enemy_throwable_vs_player() {
    collisionType boxes[MAX_ENEMY_THROWABLE_OBJECTS];
    collisionType player_area = player_aabb();
    enemy_throwable_get_all_aabb(boxes);
    for (int throw_id = 0; throw_id < MAX_ENEMY_THROWABLE_OBJECTS; throw_id++) {
        if (boxes[throw_id].w == 0)
            continue;

        // Check for collision
        if (collision(player_area, boxes[throw_id])) {            
            player_on_hit();            
        }        
    }
}

/** @brief Checks for collisions between enemies and the player.
 * If an enemy collides with the player, it marks the enemy as killed and removes a life from the player.
 *
 * TODO: Add damage cooldown to prevent multiple hits in successive frames.
 */
static int _kick_connects(collisionType enemy) {
    collisionType leg = player_leg_aabb();
    if (!collision(leg, enemy))
        return FALSE;
    return player.flip ? enemy.x < player.pos.x : enemy.x > player.pos.x;
}

static int _punch_connects(collisionType enemy) {
    collisionType leg = player_leg_aabb();
    if (!collision(leg, enemy))
        return FALSE;
    return player.flip ? enemy.x < player.pos.x : enemy.x > player.pos.x;
}

void collision_check_enemy_vs_player(int scroll_x) {
    // Get player collision area (using foot area for main body collision)
    collisionType player_area = player_aabb();
    // rect(screen, player_area.x - scroll_x, player_area.y, player_area.x + player_area.w - scroll_x, player_area.y + player_area.h, makecol(0, 255, 0)); // Debug: draw player collision box
    // Get all enemy bounding boxes
    collisionType enemies[MAX_ACTIVE_ENEMIES];
    enemy_get_all_aabb(enemies);

    // Check collision between player and each enemy
    for (int enemy_id = 0; enemy_id < MAX_ACTIVE_ENEMIES; enemy_id++) {
        // Skip inactive enemies
        if (enemies[enemy_id].w == 0)
            continue;

        // Check for collision
        if (collision(player_area, enemies[enemy_id])) {
            if (enemies[enemy_id].meta == ENEMY_BOMB) {
                int player_bottom = player_area.y + player_area.h;
                int bomb_top = enemies[enemy_id].y;

                if (player_bottom <= bomb_top + BOMB_JUMP_CLEARANCE_MARGIN) {
                    continue;
                }
            }

            if (player.state == THROWING ) {
                enemy_on_hit(enemy_id);
            } else if (player.state == KICKING ) {
                enemy_on_hit(enemy_id);
            } else {
                player_on_hit();
                continue;
            }
        }
    }
}

int martin_is_on_obj() {
    collisionType f1 = player_foot_area();
    return checkOverObj(f1);
}

int car_is_on_obj() {
    collisionType rear = rear_wheels_area();
    collisionType front = front_wheels_area();
    return checkOverObj(rear) && checkOverObj(front);
}

int player_is_over_almanac_tile() {
    collisionType foot = player_foot_area();
    return rect_over_tile_types(foot, FALSE) == ALMANAC;
}

int player_is_over_wall_tile() {
    collisionType foot = player_foot_area();
    return rect_over_tile_types(foot, FALSE) == ALMANAC;
}

int player_is_over_advance_tile() {
    collisionType foot = player_foot_area();
    return rect_over_tile_types(foot, FALSE) == ADVANCE;
}

int player_is_over_ladder() {
    if (player.data == NULL) {
        return FALSE;
    }
    // check the player's body center so the ladder can be grabbed mid-climb
    int cx = player.pos.x + (player.data->width >> 1);
    int cy = player.pos.y + (player.data->height >> 1);
    int tile_id = get_tile_at_position(cx, cy) - 1;
    return tile_id == LADDER_TILE_1 || tile_id == LADDER_TILE_2 || tile_id == LADDER_TILE_3 || tile_id == LADDER_TILE_4 ;
}

int player_foot_over_ladder() {
    collisionType foot = player_foot_area();
    if (foot.w <= 0) {
        return FALSE;
    }
    int cx = foot.x + (foot.w >> 1);
    int cy = foot.y + foot.h - 1;
    int tile_id = get_tile_at_position(cx, cy) - 1;
    return tile_id == LADDER_TILE_1 || tile_id == LADDER_TILE_2 || tile_id == LADDER_TILE_3 || tile_id == LADDER_TILE_4 ;
}

int martin_is_over_door() {
    collisionType player_area = player_aabb();
    collisionType door_boxes[MAX_DOORS];
    door_get_all_aabb(door_boxes);

    for (int i = 0; i < MAX_DOORS; i++) {
        if (door_boxes[i].w == 0)
            continue;

        if (collision(player_area, door_boxes[i])) {
            return door_boxes[i].meta; // this is the destination tmx id stored in door static_id
            // can be used to trigger room change on next frame after confirming player wants to enter
            // return TRUE;
        }
    }
    return -1;
}

void collision_check_player_vs_coins() {
    collisionType player_area = player_aabb();
    collisionType coin_boxes[MAX_TOTAL_COINS];
    coin_get_all_aabb(coin_boxes);

    for (int i = 0; i < MAX_TOTAL_COINS; i++) {
        if (coin_boxes[i].w == 0)
            continue;

        if (collision(player_area, coin_boxes[i])) {
            coin_on_collect(i);
        }
    }
}

void collision_check_player_vs_pieces() {
    collisionType player_area = player_aabb();
    collisionType piece_boxes[MAX_PIECES];
    piece_get_all_aabb(piece_boxes);

    for (int i = 0; i < MAX_PIECES; i++) {
        if (piece_boxes[i].w == 0)
            continue;

        if (collision(player_area, piece_boxes[i])) {
            piece_on_collect(i);
        }
    }
}

void collision_check_player_vs_tnt() {
    collisionType player_area = player_aabb();
    collisionType tnt_boxes[MAX_TNT];
    tnt_get_all_aabb(tnt_boxes);

    for (int i = 0; i < MAX_TNT; i++) {
        if (tnt_boxes[i].w == 0)
            continue;

        if (collision(player_area, tnt_boxes[i])) {
            tnt_on_collect(i);
        }
    }
}

// door-style Space interaction: place carried TNT on the overlapped BOX
void tnt_place_on_box_if_over() {
    collisionType player_area = player_aabb();
    collisionType box_boxes[MAX_TNT];
    tnt_box_get_all_aabb(box_boxes);

    for (int i = 0; i < MAX_TNT; i++) {
        if (box_boxes[i].w == 0)
            continue;

        if (collision(player_area, box_boxes[i])) {
            tnt_place_on_box(i);
            enemy_spawn_init();
            enemy_pool_init(); // respawns the enemies again
            return; // one box per press
        }
    }
}
