#include "object.h"
#include "game.h"
#include "player.h"
#include "tiles.h"

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
    tiles_at_pos[0] = get_tile_at_position(player.pos.x, player.pos.y);                                    // Superior izquierda
    tiles_at_pos[1] = get_tile_at_position(player.pos.x + player.data->width, player.pos.y);                     // Superior derecha
    tiles_at_pos[2] = get_tile_at_position(player.pos.x, player.pos.y + player.data->height - 4);                // Inferior izquierda
    tiles_at_pos[3] = get_tile_at_position(player.pos.x + player.data->width, player.pos.y + player.data->height - 4); // Inferior derecha
}

static inline int is_harmful_tile(int id) {
    return id == HARMFUL_TILE_1 || id == HARMFUL_TILE_2;
}

static inline int is_back_in_time_tile(int id) {
    return id == 876;
}

static const int platform_ids[] = {920,921,922,906,907,908,253,254,255, 1016};

static inline int is_a_platform(int id) {
    for (int i = 0; i < sizeof(platform_ids) / sizeof(platform_ids[0]); i++) {
        if (platform_ids[i] == id) {
            return TRUE;
        }
    }
    return FALSE;
}

static int rect_over_tile_types(collisionType r) {
    if (r.w <= 0 || r.h <= 0)
        return 0;

    int sx = r.x / TILES_SIZE;
    int ex = (r.x + r.w - 1) / TILES_SIZE;
    int sy = r.y / TILES_SIZE;
    int ey = (r.y + r.h - 1) / TILES_SIZE;

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
            if (is_harmful_tile(tile_id))
                return HARMFUL;
            if (is_back_in_time_tile(tile_id))
                return BACK_IN_TIME;
            if (is_a_platform(tile_id))
                return PLATFORM;
        }
    }
    return ROAD;
}

/* Returns true if one of wheels is over (foot_area/foot_area2)
   a an special tile, FALSE otherwise. */
int wheels_on_tiles() {
    int result;

    collisionType f1 = rear_wheels_area();
    result = rect_over_tile_types(f1);
    if (result != ROAD) {
        return result;
    }

    collisionType f2 = front_wheels_area();
    result = rect_over_tile_types(f2);
    if (result != ROAD) {
        return result;
    }

    return ROAD;
}

// will check if player is over a walkable thing
int checkOverObj() {
    // find tile from player.pos.x to player.pos.x + player.width, at player.pos.y + player.height +1
    /*check_tiles_around_player(tiles_at_positions);
    // 874 is skewers, 1 is hole, 875 is oil
    // > 832 is ground (in general)
    if (tiles_at_positions[2] >= 832 && tiles_at_positions[3] >= 832) {
        return TRUE;
    }
    return FALSE;*/
    if (player.pos.y > GROUND_Y) {
        return TRUE;
    }

    collisionType f1 = player_foot_area();
    int result = rect_over_tile_types(f1);
    if (result == PLATFORM) {
        return TRUE;
    }

    return FALSE;
}

int checkHitObj() {
    return FALSE;
}
