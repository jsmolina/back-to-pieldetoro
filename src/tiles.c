#include "tiles.h"
#include "allegro/datafile.h"
#include "allegro/gfx.h"
#include "dat_manager.h"
#include "errors.h"
#include "statics.h"
#include <allegro.h>
#include <stdio.h>
#include <stdlib.h>

PALETTE palette;
BITMAP* tiles;
// width="80" height="25"
// char dirty_tiles[MAX_VERT_TILES][MAX_HORIZ_TILES] = { 0 };
int tiles_values[MAX_VERT_TILES][MAX_HORIZ_TILES] = { 0 };
BITMAP* numbers_sprites[10] = { NULL }; // spritesheet for numbers used in score display
// defines the current width in tiles of the loaded background (e.g., 80 for 640px)
int curr_tiles_width = 0;
int map_width = 0;
int map_pixel_width = 0;

struct coords get_tile_coords(int tile_number) {
    struct coords result;
    result.x = ((tile_number - 1) & 31) << 3;
    result.y = ((tile_number - 1) >> 5) << 3;

    return result;
}

int get_tile_at_position(int x, int y) {
    int tile_x = x >> 3;
    int tile_y = y >> 3;

    // Validar límites
    if (tile_x < 0 || tile_x >= curr_tiles_width || tile_y < 0 || tile_y >= MAX_VERT_TILES) {
        return -1; // Fuera de límites
    }

    return tiles_values[tile_y][tile_x];
}

inline void load_tiles() {
    tiles = dat_file[TILES_BMP].dat;

    if (!tiles) {
        die("cannot load tiles");
    }
}

void load_numbers_spritesheet() {
    BITMAP* numbers_spritesheet = dat_file[NUMBERS_BMP].dat;
    int frame_width = 4; // each number sprite is 4 pixels wide
    int curr_x = 0;
    for (int i = 0; i < 10; i++) {
        numbers_sprites[i] = create_sub_bitmap(numbers_spritesheet, curr_x, 0, frame_width, numbers_spritesheet->h);
        curr_x += frame_width;
    }
}

BITMAP* load_shop_bg(int id) {
    char* in_file = dat_file[id].dat;
    if (in_file == NULL) {
        die("cannot load shop background %d", id);
    }

    unsigned char current;
    int start_csv = 0;
    int i = 0;
    int local_tiles_width = 0;

    do {
        current = (unsigned char)in_file[i++];
        if (current == '>') {
            int header_start = i - 1;
            while (header_start > 0 && in_file[header_start] != '<')
                header_start--;
            char header_line[256] = { 0 };
            int hlen = 0;
            while (in_file[header_start + hlen] != '>' && hlen < 255) {
                header_line[hlen] = in_file[header_start + hlen];
                hlen++;
            }
            header_line[hlen] = '\0';
            const char* wptr = strstr(header_line, "width=\"");
            if (wptr) {
                wptr += 7;
                char numbuf[16] = { 0 };
                int ni = 0;
                while (wptr[ni] && wptr[ni] != '"' && ni < 15) {
                    numbuf[ni] = wptr[ni];
                    ni++;
                }
                numbuf[ni] = '\0';
                local_tiles_width = atoi(numbuf);
            }
            start_csv += 1;
        }
    } while (start_csv < 5 && current != '\0');

    if (local_tiles_width <= 0) {
        die("invalid shop tiles width: %d", local_tiles_width);
    }

    int local_pixel_width = local_tiles_width << 3;
    BITMAP* background = create_bitmap(local_pixel_width, SCREEN_H);
    if (!background) {
        die("cannot create shop background bitmap");
    }

    char current_tile[16] = { 0 };
    int charpos = 0;
    char* output;
    struct coords screen_coords;
    screen_coords.x = screen_coords.y = 0;

    do {
        current = in_file[i++];

        if (current == ',' || current == '<') {
            if (current == '<')
                start_csv = -1;
            current_tile[charpos] = '\0';
            charpos = 0;
            int tile_number = strtol(current_tile, &output, 10);
            struct coords coordinates = get_tile_coords(tile_number);

            blit(tiles, background, coordinates.x, coordinates.y,
                screen_coords.x, screen_coords.y, TILES_SIZE, TILES_SIZE);

            screen_coords.x += 8;
            if (screen_coords.x >= local_pixel_width - 1) {
                screen_coords.y += 8;
                screen_coords.x = 0;
            }
        } else if (current != '\0') {
            current_tile[charpos++] = current;
        }
    } while (current != '\0' && start_csv != -1);

    return background;
}

BITMAP* load_background(int id) {
    char* in_file = dat_file[id].dat;
    if (in_file == NULL) {
        die("cannot load %s", id);
    }
    // BITMAP * background = create_bitmap(screen_w, SCREEN_H);
    // rectfill(background, 0, 0, SCREEN_W, SCREEN_H, makecol(16, 16, 16));
    unsigned char current;

    // skip xml data
    int start_csv = 0;
    int i = 0;

    curr_tiles_width = 0;

    // Find the first '>' and extract width before skipping header
    do {
        current = (unsigned char)in_file[i++];
        if (current == '>') {
            // Read whole header line
            int header_start = i - 1;
            while (header_start > 0 && in_file[header_start] != '<')
                header_start--;
            char header_line[256] = { 0 };
            int hlen = 0;
            while (in_file[header_start + hlen] != '>' && hlen < 255) {
                header_line[hlen] = in_file[header_start + hlen];
                hlen++;
            }
            header_line[hlen] = '\0';
            // search for width="
            const char* wptr = strstr(header_line, "width=\"");
            if (wptr) {
                // skip width=": we just want the number
                wptr += 7;
                char numbuf[16] = { 0 };
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
    if (curr_tiles_width <= 0) {
        die("invalid tiles width: %d", curr_tiles_width);
    }

    map_width = (curr_tiles_width << 3) - SCREEN_W;
    map_pixel_width = curr_tiles_width << 3;
    if (map_pixel_width <= 0) {
        die("invalid map pixel width: %d", map_pixel_width);
    }
    BITMAP* background = create_bitmap(map_pixel_width, SCREEN_H);
    if (!background) {
        die("cannot create background bitmap");
    }

    // temporal data for csv
    char current_tile[16] = { 0 };
    int charpos = 0;
    char* output;
    int iterations = 0;
    // current screen position
    struct coords screen_coords;
    screen_coords.x = screen_coords.y = 0;
    int tiles_x = 0, tiles_y = 0;

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
            // store tile value
            tiles_values[tiles_y][tiles_x] = tile_number;

            screen_coords.x += 8;
            tiles_x++;
            if (screen_coords.x >= map_pixel_width - 1) {
                iterations = 0;
                screen_coords.y += 8;
                screen_coords.x = 0;
                // store the current tiles values
                tiles_x = 0;
                tiles_y++;
            }

        } else if (current != '\0') { // a number
            current_tile[charpos++] = current;
        }

        iterations += 1;
        // if (iterations == 200) start_csv = -1; // Uncomment if needed
    } while (current != '\0' && start_csv != -1);

    return background;
}
/*
void mark_dirty_tiles(int x, int y, int width, int height) {
    int sx, sy, ex, ey;

    sx = x / TILES_SIZE;
    sy = y / TILES_SIZE;
    ex = (x + width - 1) / TILES_SIZE;
    ey = (y + height - 1) / TILES_SIZE;

    if (sx < 0)
        sx = 0;
    if (sy < 0)
        sy = 0;
    if (ex >= MAX_HORIZ_TILES)
        ex = MAX_HORIZ_TILES - 1;
    if (ey >= MAX_VERT_TILES)
        ey = MAX_VERT_TILES - 1;

    for (int ty = sy; ty <= ey; ++ty) {
        for (int tx = sx; tx <= ex; ++tx) {
            dirty_tiles[ty][tx] = 1;
        }
    }
}

// Clear all dirty tiles
void clear_dirty_tiles() {
    memset(dirty_tiles, 0, sizeof(dirty_tiles));
}*/
