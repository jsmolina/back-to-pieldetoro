#include "tnt.h"
#include "dat_manager.h"
#include "errors.h"
#include "helpers.h"
#include "object.h"
#include "player.h"
#include "statics.h"
#include <allegro.h>
#include <stdio.h>
#include <string.h>

// inventory display (screen space, not scrolled)
#define TNT_HUD_X 10
#define TNT_HUD_Y 10
#define TNT_HUD_STEP 15

// placed TNT offsets relative to a BOX
#define TNT_BOX_OFFSET_Y 16
#define TNT_BOX_STEP_X 15

// collectible TNT sitting in the level
typedef struct {
    int x;
    int y;
    int active;
    int collected;
} Tnt;

// BOX interaction target that can hold placed TNT
typedef struct {
    int x;
    int y;
    int width;
    int height;
    int active;
    int tnt_on_box; // number of TNT placed on this box (0..MAX_TNT_PER_BOX)
} TntBox;

static Tnt tnts[MAX_TNT];
static TntBox boxes[MAX_TNT];
static int tnt_count = 0;
static int box_count = 0;
static BITMAP* tnt_sprite = NULL;

void load_tnt_spritesheet() {
    tnt_sprite = dat_file[TNT_BMP].dat;
    if (!tnt_sprite) {
        die("cannot load TNT_BMP from datafile");
    }
}

void reset_tnt() {
    for (int i = 0; i < MAX_TNT; i++) {
        tnts[i].x = 0;
        tnts[i].y = 0;
        tnts[i].active = FALSE;
        tnts[i].collected = FALSE;
        boxes[i].x = 0;
        boxes[i].y = 0;
        boxes[i].width = 0;
        boxes[i].height = 0;
        boxes[i].active = FALSE;
        boxes[i].tnt_on_box = 0;
    }
    tnt_count = 0;
    box_count = 0;
    player.tnt_count = 0;
}

void load_level_tnt(int level_id) {
    int tmx_id = level_to_dat_id(level_id);
    const char* cursor;
    int tidx = 0;
    int bidx = 0;

    if (tmx_id < 0) {
        die("invalid level id %d for TMX (tnt)", level_id);
    }

    if (dat_file[tmx_id].dat == NULL) {
        die("cannot load TMX data for level %d (tnt)", level_id);
    }

    cursor = (const char*)dat_file[tmx_id].dat;

    while ((cursor = strstr(cursor, "<object ")) != NULL) {
        int id;
        char name[64], object_type[8];
        int x, y, width, height;

        int matched = sscanf(cursor,
            "<object id=\"%d\" name=\"%63[^\"]\" type=\"%7[^\"]\" x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\"",
            &id, name, object_type, &x, &y, &width, &height);

        if (matched >= 5 && strcmp(name, "TNT") == 0 && tidx < MAX_TNT) {
            tnts[tidx].x = x;
            tnts[tidx].y = y-14;
            tnts[tidx].active = TRUE;
            tnts[tidx].collected = FALSE;
            tidx++;
        } else if (matched == 7 && strcmp(name, "BOX") == 0 && bidx < MAX_TNT) {
            boxes[bidx].x = x;
            boxes[bidx].y = y;
            boxes[bidx].width = width;
            boxes[bidx].height = height;
            boxes[bidx].active = TRUE;
            boxes[bidx].tnt_on_box = 0;
            bidx++;
        }

        cursor++; // advance past current '<' to find next tag
    }
    tnt_count = tidx;
    box_count = bidx;
}

// draws `count` TNT sprites side by side starting at (x, y), stepping step_x
static void _draw_tnt_row(int x, int y, int count, int step_x) {
    for (int i = 0; i < count; i++) {
        draw_sprite(current_screen, tnt_sprite, x, y);
        x += step_x;
    }
}

void tnt_draw(int scroll_x) {
    if (!tnt_sprite) {
        return;
    }

    // world TNT still waiting to be collected
    for (int i = 0; i < tnt_count; i++) {
        if (tnts[i].active && !tnts[i].collected) {
            int screen_x = tnts[i].x - scroll_x;
            if (screen_x > -tnt_sprite->w && screen_x < SCREEN_W) {
                draw_sprite(current_screen, tnt_sprite, screen_x, tnts[i].y);
            }
        }
    }

    // TNT placed on boxes (world space)
    for (int i = 0; i < box_count; i++) {
        if (boxes[i].active && boxes[i].tnt_on_box > 0) {
            int screen_x = boxes[i].x - scroll_x;
            _draw_tnt_row(screen_x, boxes[i].y, boxes[i].tnt_on_box, TNT_BOX_STEP_X);
        }
    }

    // carried-TNT inventory (screen space, not scrolled)
    _draw_tnt_row(TNT_HUD_X, TNT_HUD_Y, player.tnt_count, TNT_HUD_STEP);
}

void tnt_get_all_aabb(collisionType* boxes_out) {
    for (int i = 0; i < MAX_TNT; i++) {
        if (i < tnt_count && tnts[i].active && !tnts[i].collected) {
            boxes_out[i].x = tnts[i].x;
            boxes_out[i].y = tnts[i].y;
            boxes_out[i].w = tnt_sprite ? tnt_sprite->w : 16;
            boxes_out[i].h = tnt_sprite ? tnt_sprite->h : 16;
            boxes_out[i].meta = 0;
        } else {
            boxes_out[i].x = 0;
            boxes_out[i].y = 0;
            boxes_out[i].w = 0;
            boxes_out[i].h = 0;
            boxes_out[i].meta = 0;
        }
    }
}

void tnt_on_collect(int index) {
    if (index < 0 || index >= tnt_count) {
        return;
    }
    if (!tnts[index].active || tnts[index].collected) {
        return;
    }
    // do nothing if the player is already carrying the maximum
    if (player.tnt_count >= MAX_TNT_CARRY) {
        return;
    }
    tnts[index].collected = TRUE;
    tnts[index].active = FALSE; // permanently removed from the level
    player.tnt_count++;
}

void tnt_box_get_all_aabb(collisionType* boxes_out) {
    for (int i = 0; i < MAX_TNT; i++) {
        if (i < box_count && boxes[i].active) {
            boxes_out[i].x = boxes[i].x;
            boxes_out[i].y = boxes[i].y;
            boxes_out[i].w = boxes[i].width;
            boxes_out[i].h = boxes[i].height;
            boxes_out[i].meta = 0;
        } else {
            boxes_out[i].x = 0;
            boxes_out[i].y = 0;
            boxes_out[i].w = 0;
            boxes_out[i].h = 0;
            boxes_out[i].meta = 0;
        }
    }
}

void tnt_place_on_box(int index) {
    if (index < 0 || index >= box_count || !boxes[index].active) {
        return;
    }
    // transfer TNT from inventory to the box until the box is full or empty-handed
    while (player.tnt_count > 0 && boxes[index].tnt_on_box < MAX_TNT_PER_BOX) {
        boxes[index].tnt_on_box++;
        player.tnt_count--;
    }
}
