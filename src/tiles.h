#ifndef TILES_H
#define TILES_H

#include "statics.h"
#include <allegro.h>

#define TILES_SIZE 8
#define MAX_HORIZ_TILES 410
#define MAX_VERT_TILES 25
#define SCREEN_VIRTUAL 320

struct coords {
    int x;
    int y;
};

/**
Tiles image
*/
extern BITMAP* tiles;
extern PALETTE palette;
// dirty tiles array
extern char dirty_tiles[25][MAX_HORIZ_TILES];
extern int tiles_values[MAX_VERT_TILES][MAX_HORIZ_TILES];
extern int curr_tiles_width;
extern int map_width;
extern int map_pixel_width;
extern int almanac_tile_x;
/**
Preloads all tiles as a bitmap to blit them to screen
*/
inline void load_tiles();
void load_numbers_spritesheet();
BITMAP* load_background(int id);

/** @brief Loads a background from a TMX datafile entry as a BITMAP without
 *         touching any global state (tiles_values, curr_tiles_width, etc.).
 *         Suitable for sub-rooms that should not affect the main level state.
 *  @param id Datafile index of the TMX asset.
 *  @return Newly allocated BITMAP*; caller must destroy_bitmap() it when done.
 */
BITMAP* load_shop_bg(int id);

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