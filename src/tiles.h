#ifndef TILES_H
#define TILES_H

#include <allegro.h>
#include "statics.h"

#define TILES_SIZE 8
#define MAX_HORIZ_TILES 325
#define MAX_VERT_TILES 25
#define SCREEN_VIRTUAL 640


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
extern int tiles_values[MAX_VERT_TILES][MAX_HORIZ_TILES];
extern int curr_tiles_width;
extern int map_width;
extern int map_pixel_width;
/**
Preloads all tiles as a bitmap to blit them to screen
*/
inline void load_tiles();
BITMAP * load_background(int id, int screen_w);

/**
Returns x and y positions from a tile number
*/
struct coords get_tile_coords(int tile_number);

/** gets tile number at position x,y
@param x position in pixels
@param y position in pixels
@return tile number
*/
int get_tile_at_position(int x, int y);


#endif