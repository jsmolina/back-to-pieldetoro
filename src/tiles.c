#include "tiles.h"
#include "errors.h"
#include "allegro/datafile.h"
#include <allegro.h>
#include <stdio.h>
#include <stdlib.h>
#include "dat_manager.h"
#include "statics.h"


PALETTE palette;
BITMAP *tiles;
// defines the current width in tiles of the loaded background (e.g., 80 for 640px)
int curr_tiles_width = 0;
int map_width = 0;

struct coords get_tile_coords(int tile_number) {
    struct coords result;
    result.x = 8 * ((tile_number - 1) % 32);
    result.y = 8 * ((tile_number - 1) >> 5);

    return result;
}

inline void load_tiles() {
    tiles = dat_file[TILES_BMP].dat;

    if(!tiles) {
        die("cannot load tiles");
    }
}



BITMAP * load_background(int id, int screen_w) {
    char *in_file = dat_file[id].dat;
    if (in_file == NULL) {
        die("cannot load %s", id);
    }
    BITMAP * background = create_bitmap(screen_w, SCREEN_H);
    rectfill(background, 0, 0, SCREEN_W, SCREEN_H, makecol(16, 16, 16));
    char current;

    // skip xml data
    int start_csv = 0;
    int i = 0;

    curr_tiles_width = 0;

    // Find the first '>' and extract width before skipping header
    do {
        current = in_file[i++];
        if (current == '>') {
            // Read whole header line
            int header_start = i - 1;
            while (header_start > 0 && in_file[header_start] != '<') header_start--;
            char header_line[256] = {0};
            int hlen = 0;
            while (in_file[header_start + hlen] != '>' && hlen < 255) {
                header_line[hlen] = in_file[header_start + hlen];
                hlen++;
            }
            header_line[hlen] = '\0';
            // search for width="
            const char *wptr = strstr(header_line, "width=\"");
            if (wptr) {
                // skip width=": we just want the number
                wptr += 7;
                char numbuf[16] = {0};
                int ni = 0;
                while (wptr[ni] && wptr[ni] != '"' && ni < 15) {
                    numbuf[ni] = wptr[ni];
                    ni++;
                }
                numbuf[ni] = '\0';
                curr_tiles_width = atoi(numbuf);
            }
            start_csv += 1;
        }
    } while (start_csv < 5 && current != '\0');
    map_width = curr_tiles_width * TILES_SIZE - SCREEN_W;

    // temporal data for csv
    char current_tile[5] = "     ";
    int charpos = 0;
    char * output;
    int iterations = 0;
    // current screen position
    struct coords screen_coords;
    screen_coords.x = screen_coords.y = 0;

    do {
        current = in_file[i++];

        if (current == ',' || current == '<') {
            if (current == '<') {
                // signal finish reading
                start_csv = -1;
            }
            current_tile[charpos] = '\0';
            charpos = 0;
            int tile_number = strtol(current_tile, &output, 10);
            struct coords coordinates = get_tile_coords(tile_number);

            // copies from tiles to background
            blit(tiles, background, coordinates.x, coordinates.y,
                 screen_coords.x, screen_coords.y, TILES_SIZE, TILES_SIZE);

            screen_coords.x += 8;
            if (screen_coords.x >= screen_w - 1) {
                iterations = 0;
                screen_coords.y += 8;
                screen_coords.x = 0;
            }

        } else if (current != '\0') { // a number
            current_tile[charpos++] = current;
        }

        iterations += 1;
        // if (iterations == 200) start_csv = -1; // Uncomment if needed
    } while (current != '\0' && start_csv != -1);


    return background;
}
//width="80" height="25"
char dirty_tiles[MAX_VERT_TILES][MAX_HORIZ_TILES] = {0};

void mark_dirty_tiles(int x, int y, int width, int height) {
    int sx, sy, ex, ey;

    sx = x / TILES_SIZE;
    sy = y / TILES_SIZE;
    ex = (x + width - 1) / TILES_SIZE;
    ex = (y + height - 1) / TILES_SIZE;

    for (int ty = sy; ty <= ey; ++ty) {
        for (int tx = sx; tx <= ex; ++tx) {
            if (x >= 0 && x < TILES_SIZE && y >= 0 && y < TILES_SIZE) {
                dirty_tiles[ty][tx] = 1;
            }
        }
    }
}

// Clear all dirty tiles
void clear_dirty_tiles() {
    memset(dirty_tiles, 0, sizeof(dirty_tiles));
}
