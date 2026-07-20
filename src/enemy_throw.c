#include "enemy_throw.h"
#include <allegro.h>
#include "dat_manager.h"
#include "statics.h"


#define THROW_COOLDOWN_FRAMES 30

static ThrowableObject throwable_objects[MAX_ENEMY_THROWABLE_OBJECTS];
static int throw_cooldown = 0;

int init_enemy_throwable(int x, int y, int flip) {
    if (throw_cooldown > 0) {
        throw_cooldown--;
        return FALSE;
    }
    for (int i = 0; i < MAX_ENEMY_THROWABLE_OBJECTS; i++) {
        if (throwable_objects[i].active == FALSE) {
            throwable_objects[i].x = x;
            throwable_objects[i].y = y;
            throwable_objects[i].flip = flip;
            throwable_objects[i].active = TRUE;
            throwable_objects[i].type = MIC_TYPE;
            throw_cooldown = THROW_COOLDOWN_FRAMES;
            return TRUE;
        }
    }
    return FALSE; // no free slot
}

inline void draw_enemy_throwable(int scroll_x) {
    BITMAP* book_bmp = dat_file[BOOK_BMP].dat;
    if (!book_bmp) {
        return; // bitmap not loaded
    }

    for (int i = 0; i < MAX_ENEMY_THROWABLE_OBJECTS; i++) {
        if (throwable_objects[i].active == TRUE) {
            draw_sprite(current_screen, book_bmp, throwable_objects[i].x - scroll_x, throwable_objects[i].y);
        }
    }
}

void enemy_throwable_update(int scroll_x) {
    for (int i = 0; i < MAX_ENEMY_THROWABLE_OBJECTS; i++) {
        if (throwable_objects[i].active == TRUE) {
            // simple movement logic: move right if flip is false, left if true
            if (throwable_objects[i].flip == FALSE) {
                throwable_objects[i].x += 2; // move right
            } else {
                throwable_objects[i].x -= 2; // move left
            }

            // Deactivate if out of screen bounds (assuming 320 width)
            if (throwable_objects[i].x < scroll_x || throwable_objects[i].x > (scroll_x + 320)) {
                throwable_objects[i].active = FALSE;
            }
        }
    }
}


void enemy_throwable_get_all_aabb(collisionType* boxes) {
    for (int i = 0; i < MAX_ENEMY_THROWABLE_OBJECTS; i++) {
        if (throwable_objects[i].active == TRUE) {
            boxes[i].x = throwable_objects[i].x;
            boxes[i].y = throwable_objects[i].y;
            boxes[i].w = 17;
            boxes[i].h = 20;
        } else {
            boxes[i].x = 0;
            boxes[i].y = 0;
            boxes[i].w = 0;
            boxes[i].h = 0;
        }
    }
}

void enemy_throwable_on_hit(int index) {
    if (index >= 0 && index < MAX_ENEMY_THROWABLE_OBJECTS) {
        throwable_objects[index].active = FALSE;
        throwable_objects[index].x = 0;
        throwable_objects[index].y = 0;
    }
}
