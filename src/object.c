#include "object.h"
#include "tiles.h"
#include "player.h"
#include "game.h"

// simple AABB collision detection
int collision(struct collisionType obj1, struct collisionType obj2) {
    int flg =  obj1.x >= obj2.x + obj2.w
        || obj2.x >= obj1.x + obj1.w
        || obj1.y >= obj2.y + obj2.h
        || obj2.y >= obj1.y + obj1.h;
    return !flg;
}

// + 16 ruedas touches ground?
int tiles_at_positions[4];
void check_tiles_around_player(int* tiles_at_pos) {
    // Obtén los 4 tiles de las esquinas del jugador
    tiles_at_pos[0] = get_tile_at_position(player.pos.x, player.pos.y);                                // Superior izquierda
    tiles_at_pos[1] = get_tile_at_position(player.pos.x + player.width, player.pos.y);                 // Superior derecha
    tiles_at_pos[2] = get_tile_at_position(player.pos.x, player.pos.y + player.height - 4);                // Inferior izquierda
    tiles_at_pos[3] = get_tile_at_position(player.pos.x + player.width, player.pos.y + player.height - 4); // Inferior derecha
}

void check_harming_tiles() {
    // 874 & 875 are harming tiles
    // Obtén los 4 tiles de las esquinas del jugador
    
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
