#include "door.h"
#include "dat_manager.h"
#include "errors.h"
#include "statics.h"
#include <allegro.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    int x;
    int y;
    int width;
    int height;
    int active;
    int static_id; // TMX ID of the destination room/level, stored in the name field of the TMX object
} Door;

static Door doors[MAX_DOORS];
static int door_count = 0;

void reset_doors() {
    for (int i = 0; i < MAX_DOORS; i++) {
        doors[i].x = 0;
        doors[i].y = 0;
        doors[i].width = 0;
        doors[i].height = 0;
        doors[i].active = FALSE;
    }
    door_count = 0;
}

void load_level_doors(int level_id) {
    int tmx_id = level_to_dat_id(level_id);
    const char* cursor;
    int idx = 0;

    if (tmx_id < 0) {
        die("invalid level id %d for TMX (doors)", level_id);
    }

    if (dat_file[tmx_id].dat == NULL) {
        die("cannot load TMX data for level %d (doors)", level_id);
    }

    cursor = (const char*)dat_file[tmx_id].dat;

    while ((cursor = strstr(cursor, "<object ")) != NULL) {
        int id;
        char name[64], object_type[8];
        int x, y, width, height;

        int matched = sscanf(cursor,
            "<object id=\"%d\" name=\"%63[^\"]\" type=\"%7[^\"]\" x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\"",
            &id, name, object_type, &x, &y, &width, &height);

        if (matched == 7 && strcmp(name, "DOOR") == 0) {
            doors[idx].static_id = atoi(object_type); // store destination tmx in static_id for later use
            doors[idx].x = x;
            doors[idx].y = y;
            doors[idx].width = width;
            doors[idx].height = height;
            doors[idx].active = TRUE;
            idx++;
            if (idx >= MAX_DOORS) {
                break;
            }
        }

        cursor++; // advance past current '<' to find next tag
    }
    door_count = idx;
}

void door_get_all_aabb(collisionType* boxes) {
    for (int i = 0; i < MAX_DOORS; i++) {
        if (doors[i].active) {
            boxes[i] = (collisionType){
                .x = doors[i].x,
                .y = doors[i].y,
                .w = doors[i].width,
                .h = doors[i].height,
                .meta = doors[i].static_id
            };
        } else {
            boxes[i] = (collisionType){ .x = 0, .y = 0, .w = 0, .h = 0 };
        }
    }
}

void draw_door_getin(int scroll_x) {

    for (int i = 0; i < MAX_DOORS; i++) {
        if (!doors[i].active) {
            continue;
        }
        int screen_x = doors[i].x - scroll_x;
        if (screen_x < 320) {
            draw_sprite(current_screen, dat_file[ARROW_GETIN_BMP].dat, screen_x, 100);
        }
    }
}
