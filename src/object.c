#include "object.h"
#include "game.h"
#include "player.h"
#include "tiles.h"

// simple AABB collision detection
int collision(struct collisionType obj1, struct collisionType obj2) {
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
    tiles_at_pos[1] = get_tile_at_position(player.pos.x + player.width, player.pos.y);                     // Superior derecha
    tiles_at_pos[2] = get_tile_at_position(player.pos.x, player.pos.y + player.height - 4);                // Inferior izquierda
    tiles_at_pos[3] = get_tile_at_position(player.pos.x + player.width, player.pos.y + player.height - 4); // Inferior derecha
}

inline int is_harmful_tile(int id) {
    return id == HARMFUL_TILE_1 || id == HARMFUL_TILE_2;
}

inline int is_back_in_time_tile(int id) {
    return id == 876;
}

static int rect_over_harmful_tiles(struct collisionType r) {
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
        }
    }
    return ROAD;
}

/* Returns true if one of wheels is over (foot_area/foot_area2)
   a an special tile, FALSE otherwise. */
int wheels_on_tiles() {
    int result;

    struct collisionType f1 = foot_area();
    result = rect_over_harmful_tiles(f1);
    if (result != ROAD) {
        return result;
    }

    struct collisionType f2 = foot_area2();
    result = rect_over_harmful_tiles(f2);
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
    return FALSE;
}

int checkHitObj() {
    return FALSE;
}
