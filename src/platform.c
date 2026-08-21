#include "platform.h"
#include "dat_manager.h"
#include "errors.h"
#include "helpers.h"
#include "statics.h"
#include <allegro.h>
#include <stdio.h>
#include <string.h>

#define MOVING_PLATFORM_W 24
#define MOVING_PLATFORM_H 8

typedef struct {
    int x;
    int y;
    // current position
    coordsType pos;
    // previous position from the prior frame
    coordsType prev_pos;
    // platform type: 'L' for LR (horizontal), 'D' for DT (vertical)
    char type;
    // max displacement for movement
    int distance;
    // movement direction: +1 or -1
    int dir;
    int active;
    int move_tick;
    BITMAP * sp;
} Platform;

static Platform platforms[MAX_PLATFORMS];

static int platform_count = 0;


void reset_platforms() {
    for (int i = 0; i < MAX_PLATFORMS; i++) {
        platforms[i].x = 0;
        platforms[i].y = 0;
        platforms[i].pos.x = 0;
        platforms[i].pos.y = 0;
        platforms[i].prev_pos.x = 0;
        platforms[i].prev_pos.y = 0;
        platforms[i].type = 'L';
        platforms[i].distance = 0;
        platforms[i].dir = 1;
        platforms[i].active = FALSE;
    }
    platform_count = 0;
}

void load_level_platforms(int level_id) {
    int tmx_id = level_to_dat_id(level_id);
    const char* cursor;
    int idx = 0;

    if (tmx_id < 0) {
        die("invalid level id %d for TMX (platforms)", level_id);
    }

    if (dat_file[tmx_id].dat == NULL) {
        die("cannot load TMX data for level %d (platforms)", level_id);
    }

    cursor = (const char*)dat_file[tmx_id].dat;

    while ((cursor = strstr(cursor, "<object ")) != NULL) {
        int id;
        char name[64], object_type[8];
        int x, y, width, height;

        int matched = sscanf(cursor,
            "<object id=\"%d\" name=\"%63[^\"]\" type=\"%7[^\"]\" x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\"",
            &id, name, object_type, &x, &y, &width, &height);

        if (matched >= 6 && strcmp(name, "PLATFORM") == 0) {
            int yy = (object_type[0] == 'L') ? y : (y + height); // for vertical platforms, adjust y to bottom edge
            platforms[idx].sp = level_id == 5? dat_file[PLATAFORMA2_BMP].dat : dat_file[PLATAFORMA_BMP].dat;
            platforms[idx].x = x;
            platforms[idx].y = y;
            platforms[idx].pos.x = x;
            platforms[idx].pos.y = yy;
            platforms[idx].prev_pos.x = x;
            platforms[idx].prev_pos.y = yy;
            // type: first char of object_type (L for LR, D for DT)
            platforms[idx].type = object_type[0];
            // distance: width for LR, height for DT
            platforms[idx].distance = (object_type[0] == 'L') ? width : height;
            // initial direction: 1 for LR (move right), -1 for DT (move up)
            platforms[idx].dir = (object_type[0] == 'L') ? 1 : -1;
            platforms[idx].active = TRUE;
            idx++;
            if (idx >= MAX_PLATFORMS) {
                break;
            }
        }

        cursor++; // advance past current '<' to find next tag
    }
    platform_count = idx;
}

void platform_update(int scroll_x) {
    int i;
    for (i = 0; i < platform_count; i++) {
        if (!platforms[i].active) {
            continue;
        }

        platforms[i].prev_pos = platforms[i].pos;

        if (platforms[i].move_tick) {
            platforms[i].move_tick = FALSE;
        } else {
            platforms[i].move_tick = TRUE;
            if (platforms[i].type == 'L') {
                // LR (horizontal) movement
                platforms[i].pos.x += platforms[i].dir;
                if (platforms[i].pos.x >= platforms[i].x + platforms[i].distance) {
                    platforms[i].dir = -1;
                } else if (platforms[i].pos.x <= platforms[i].x) {
                    platforms[i].dir = 1;
                }
            } else if (platforms[i].type == 'D') {
                // DT (vertical) movement
                platforms[i].pos.y += platforms[i].dir;
                if (platforms[i].pos.y <= platforms[i].y) {
                    platforms[i].dir = 1;
                } else if (platforms[i].pos.y >= platforms[i].y + platforms[i].distance) {
                    platforms[i].dir = -1;
                }
            }
        }
    }
}

void draw_platforms(int scroll_x) {
    for (int i = 0; i < platform_count; i++) {
        if (platforms[i].active) {
            int screen_x = platforms[i].pos.x - scroll_x;
            draw_sprite(current_screen, platforms[i].sp, screen_x, platforms[i].pos.y);
        }
    }
}

void platform_get_all_aabb(collisionType* boxes) {
    for (int i = 0; i < MAX_PLATFORMS; i++) {
        if (i < platform_count && platforms[i].active) {
            boxes[i].x = platforms[i].pos.x;
            boxes[i].y = platforms[i].pos.y;
            boxes[i].w = MOVING_PLATFORM_W;
            boxes[i].h = MOVING_PLATFORM_H;
        } else {
            boxes[i].x = 0;
            boxes[i].y = 0;
            boxes[i].w = 0;
            boxes[i].h = 0;
        }
    }
}

int platform_get_delta(int index, int* dx, int* dy) {
    if (!dx || !dy) {
        return FALSE;
    }

    *dx = 0;
    *dy = 0;

    if (index < 0 || index >= platform_count || !platforms[index].active) {
        return FALSE;
    }

    *dx = platforms[index].pos.x - platforms[index].prev_pos.x;
    *dy = platforms[index].pos.y - platforms[index].prev_pos.y;
    return TRUE;
}

int platform_find_support(collisionType area, int max_snap_pixels, int* platform_index, int* platform_top_y) {
    if (!platform_index || !platform_top_y || area.w <= 0 || area.h <= 0) {
        return FALSE;
    }

    if (max_snap_pixels < 0) {
        max_snap_pixels = 0;
    }

    int area_left = area.x;
    int area_right = area.x + area.w;
    int area_bottom = area.y + area.h;

    for (int i = 0; i < platform_count; i++) {
        if (!platforms[i].active) {
            continue;
        }

        int platform_left = platforms[i].pos.x;
        int platform_right = platforms[i].pos.x + MOVING_PLATFORM_W;
        int platform_top = platforms[i].pos.y;

        int overlap_x = area_left < platform_right && area_right > platform_left;
        if (!overlap_x) {
            continue;
        }

        if (area_bottom >= platform_top - max_snap_pixels && area_bottom <= platform_top + max_snap_pixels) {
            *platform_index = i;
            *platform_top_y = platform_top;
            return TRUE;
        }
    }

    return FALSE;
}
