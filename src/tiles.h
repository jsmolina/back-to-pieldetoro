#ifndef TILES_H
#define TILES_H

#include <allegro.h>
#include "statics.h"

#define TILES_SIZE 8
#define MAX_HORIZ_TILES 80
#define MAX_VERT_TILES 25


struct coords {
    int x;
    int y;
};

/**
Tiles image
*/
extern BITMAP *tiles;
extern PALETTE palette;
// dirty tiles array
extern char dirty_tiles[25][MAX_HORIZ_TILES];
extern int curr_tiles_width;
extern int map_width;
/**
Preloads all tiles as a bitmap to blit them to screen
*/
inline void load_tiles();
BITMAP * load_background(int id, int screen_w);

/**
Returns x and y positions from a tile number
*/
struct coords get_tile_coords(int tile_number);


#endif