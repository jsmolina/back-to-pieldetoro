#include "sinking.h"
#include "dat_manager.h"
#include "errors.h"
#include "helpers.h"
#include "statics.h"
#include <allegro.h>
#include <stdio.h>
#include <string.h>

// state machine phases
#define SINKING_SOLID 0     // SINKING1_BMP shown, player can stand
#define SINKING_CRUMBLING 1 // SINKING2_BMP shown, player falls
#define SINKING_GONE 2      // nothing drawn, player falls

// durations in game ticks (70fps): solid ~2s, crumbling ~0.5s, gone ~1s
#define SINKING_SOLID_FRAMES 140
#define SINKING_CRUMBLING_FRAMES 40
#define SINKING_GONE_FRAMES 70

typedef struct {
    int x;
    int y;
    int active;
    short state;
    int tick;
    char type;
} Sinking;

static Sinking sinking[MAX_SINKING];
static BITMAP* sinking_solid_sprite = NULL;
static BITMAP* sinking_crumble_sprite = NULL;
static int sinking_w = 0;
static int sinking_count = 0;

void load_sinking_spritesheet() {
    sinking_solid_sprite = dat_file[SINKING1_BMP].dat;
    sinking_crumble_sprite = dat_file[SINKING2_BMP].dat;
    if (!sinking_solid_sprite || !sinking_crumble_sprite) {
        die("cannot load SINKING sprites from datafile");
    }
    sinking_w = sinking_solid_sprite->w;
}

void reset_sinking() {
    for (int i = 0; i < MAX_SINKING; i++) {
        sinking[i].x = 0;
        sinking[i].y = 0;
        sinking[i].active = FALSE;
        sinking[i].state = SINKING_SOLID;
        sinking[i].tick = 0;
    }
    sinking_count = 0;
}

void load_level_sinking(int level_id) {
    int tmx_id = level_to_dat_id(level_id);
    const char* cursor;
    int idx = 0;

    if (tmx_id < 0) {
        die("invalid level id %d for TMX (sinking)", level_id);
    }

    if (dat_file[tmx_id].dat == NULL) {
        die("cannot load TMX data for level %d (sinking)", level_id);
    }

    cursor = (const char*)dat_file[tmx_id].dat;

    while ((cursor = strstr(cursor, "<object ")) != NULL) {
        int id;
        char name[64], object_type[8];
        int x, y, width, height;

        int matched = sscanf(cursor,
            "<object id=\"%d\" name=\"%63[^\"]\" type=\"%7[^\"]\" x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\"",
            &id, name, object_type, &x, &y, &width, &height);

        if (matched >= 6 && strcmp(name, "SINKING") == 0) {
            sinking[idx].x = x;
            sinking[idx].y = y;
            sinking[idx].active = TRUE;
            sinking[idx].state = object_type[0]== 'L' ? SINKING_SOLID : SINKING_GONE;
            sinking[idx].tick = 0;
            sinking[idx].type = object_type[0];
            idx++;
            if (idx >= MAX_SINKING) {
                break;
            }
        }

        cursor++; // advance past current '<' to find next tag
    }
    sinking_count = idx;
}

void sinking_update(int scroll_x) {
    for (int i = 0; i < sinking_count; i++) {
        if (!sinking[i].active) {
            continue;
        }

        sinking[i].tick++;
        switch (sinking[i].state) {
        case SINKING_SOLID:
            if (sinking[i].tick >= SINKING_SOLID_FRAMES) {
                sinking[i].state = SINKING_CRUMBLING;
                sinking[i].tick = 0;
            }
            break;
        case SINKING_CRUMBLING:
            if (sinking[i].tick >= SINKING_CRUMBLING_FRAMES) {
                sinking[i].state = SINKING_GONE;
                sinking[i].tick = 0;
            }
            break;
        case SINKING_GONE:
            if (sinking[i].tick >= SINKING_GONE_FRAMES) {
                sinking[i].state = SINKING_SOLID;
                sinking[i].tick = 0;
            }
            break;
        }
    }
}

void sinking_draw(int scroll_x) {
    for (int i = 0; i < sinking_count; i++) {
        if (!sinking[i].active) {
            continue;
        }
        int screen_x = sinking[i].x - scroll_x;
        if (sinking[i].state == SINKING_SOLID) {
            draw_sprite(current_screen, sinking_solid_sprite, screen_x, sinking[i].y);
        } else {
            draw_sprite(current_screen, sinking_crumble_sprite, screen_x, sinking[i].y);
        }
    }
}

void sinking_get_all_aabb(collisionType* boxes) {
    for (int i = 0; i < MAX_SINKING; i++) {
        if (i < sinking_count && sinking[i].active && sinking[i].state == SINKING_SOLID) {
            boxes[i].x = sinking[i].x;
            boxes[i].y = sinking[i].y;
            boxes[i].w = sinking_w;
            boxes[i].h = sinking_solid_sprite->h;
        } else {
            boxes[i].x = 0;
            boxes[i].y = 0;
            boxes[i].w = 0;
            boxes[i].h = 0;
        }
    }
}

int sinking_find_support(collisionType area, int max_snap_pixels, int* out_index, int* out_top_y) {
    if (!out_index || !out_top_y || area.w <= 0 || area.h <= 0) {
        return FALSE;
    }

    if (max_snap_pixels < 0) {
        max_snap_pixels = 0;
    }

    int area_left = area.x;
    int area_right = area.x + area.w;
    int area_bottom = area.y + area.h;

    for (int i = 0; i < sinking_count; i++) {
        // only solid platforms give footing; crumbling/gone let the player fall
        if (!sinking[i].active || sinking[i].state != SINKING_SOLID) {
            continue;
        }

        int left = sinking[i].x;
        int right = sinking[i].x + sinking_w;
        int top = sinking[i].y;

        int overlap_x = area_left < right && area_right > left;
        if (!overlap_x) {
            continue;
        }

        if (area_bottom >= top - max_snap_pixels && area_bottom <= top + max_snap_pixels) {
            *out_index = i;
            *out_top_y = top;
            return TRUE;
        }
    }

    return FALSE;
}
