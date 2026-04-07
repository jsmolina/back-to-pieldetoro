#include "object.h"
#include "book.h"
#include "enemy.h"
#include "helpers.h"
#include "player.h"
#include "tiles.h"

#define GAME_PLATFORMS_SIZE1 19
#define GAME_PLATFORMS_SIZE2 15
#define CAR_PLATFORM_SIZE 9

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
    return id == 876;
}

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
};

static inline int is_a_platform(int id) {
    if (id < 296) {
        for (int i = 0; i < GAME_PLATFORMS_SIZE1; i++) {
            if (platform_ids1[i] == id) {
                return TRUE;
            }
        }
    } else {
        for (int i = 0; i < GAME_PLATFORMS_SIZE2; i++) {
            if (platform_ids2[i] == id) {
                return TRUE;
            }
        }
        return FALSE;
    }
    return FALSE;
}

static const int car_platform_ids[CAR_PLATFORM_SIZE] = { 906, 907, 908, 909, 865, 866, 864, 867, 832 };

static inline int is_a_car_platform(int id) {
    for (int i = 0; i < CAR_PLATFORM_SIZE; i++) {
        if (car_platform_ids[i] == id) {
            return TRUE;
        }
    }
    return FALSE;
}

static int rect_over_tile_types(collisionType r, int is_wheel) {
    if (r.w <= 0 || r.h <= 0)
        return 0;

    int sx = r.x / TILES_SIZE;
    int ex = (r.x + r.w - 1) / TILES_SIZE;
    /*int sy = r.y / TILES_SIZE;
    int ey = (r.y + r.h - 1) / TILES_SIZE;*/
    int sy = (r.y + r.h - 1) / TILES_SIZE; // only inferior row for platform check
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
            int tile_id = tiles_values[ty][tx] - 1;
            if (is_wheel) {
                if (is_harmful_tile(tile_id))
                    return HARMFUL;
                if (is_back_in_time_tile(tile_id))
                    return BACK_IN_TIME;
                if (is_a_car_platform(tile_id))
                    return PLATFORM;
            } else {
                if (is_a_platform(tile_id))
                    return PLATFORM;
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
    if (result != ROAD) {
        return result;
    }

    collisionType f2 = front_wheels_area();
    result = rect_over_tile_types(f2, TRUE);
    if (result != ROAD) {
        return result;
    }

    return ROAD;
}

// will check if player is over a walkable thing
int checkOverObj(collisionType area) {
    int result = rect_over_tile_types(area, FALSE);
    if (result == PLATFORM) {
        return TRUE;
    }

    return FALSE;
}

int checkHitObj() {
    return FALSE;
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

            if (collision(boxes[throw_id], enemies[enemy_id])) {
                book_on_hit(throw_id);
                enemy_on_hit(enemy_id);
            }
        }
    }
}

/** @brief Checks for collisions between enemies and the player.
 * If an enemy collides with the player, it marks the enemy as killed and removes a life from the player.
 *
 * TODO: Add damage cooldown to prevent multiple hits in successive frames.
 */
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
            if (player.state == KICKING) {
                enemy_on_hit(enemy_id);
            } else {
                player_on_hit();
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
    return checkOverObj(rear) || checkOverObj(front);
}

static inline int is_a_door_tile(int id) {
    return id == DOOR_TILE_1 || id == DOOR_TILE_2 || id == DOOR_TILE_3;
}

int martin_is_over_room_door() {
    if (player.data == NULL)
        return FALSE;

    int tile_id = get_tile_at_position(player.pos.x + 10, player.pos.y+ 10); 
    if (is_a_door_tile(tile_id)) {
        return TRUE;
    }
    return FALSE;

    /*int sx = player.pos.x / TILES_SIZE;
    int ex = (player.pos.x + player.data->width - 1) / TILES_SIZE;
    int sy = player.pos.y / TILES_SIZE;
    int ey = (player.pos.y + player.data->height - 1) / TILES_SIZE;

    if (sx < 0) sx = 0;
    if (sy < 0) sy = 0;
    if (ex >= curr_tiles_width) ex = curr_tiles_width - 1;
    if (ey >= MAX_VERT_TILES) ey = MAX_VERT_TILES - 1;

    for (int ty = sy; ty <= ey; ty++) {
        for (int tx = sx; tx <= ex; tx++) {
            int tile_id = tiles_values[ty][tx] - 1;
            if (is_a_door_tile(tile_id)) {
                return TRUE;
            }
        }
    }
    return FALSE;*/
}
