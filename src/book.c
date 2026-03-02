#include "book.h"
#include "allegro/gfx.h"
#include "allegro/inline/draw.inl"
#include "object.h"
#define MAX_THROWABLE_OBJECTS 3

typedef struct {
    int x;
    int y;
    int active;
    int flip;
} ThrowableObject;

ThrowableObject throwable_objects[] = {
    { 0, 0, FALSE, FALSE }, 
    { 0, 0, FALSE, FALSE },
    { 0, 0, FALSE, FALSE },
};


void init_book(int x, int y, int flip) {
    for (int i = 0; i < MAX_THROWABLE_OBJECTS; i++) {
        if (throwable_objects[i].active == FALSE) {
            throwable_objects[i].x = x;
            throwable_objects[i].y = y;
            throwable_objects[i].flip = flip;
            throwable_objects[i].active = TRUE;
            return;
        }
    }
}


void draw_book() {
    BITMAP* book_bmp = dat_file[BOOK_BMP].dat;
    for (int i = 0; i < MAX_THROWABLE_OBJECTS; i++) {
        if (throwable_objects[i].active == TRUE) {
            draw_sprite(screen, book_bmp, throwable_objects[i].x, throwable_objects[i].y);
        }
    }    
}

void update_book() {
    for (int i = 0; i < MAX_THROWABLE_OBJECTS; i++) {
        if (throwable_objects[i].active == TRUE) {
            // simple movement logic: move right if flip is false, left if true
            if (throwable_objects[i].flip == FALSE) {
                throwable_objects[i].x += 2; // move right
            } else {
                throwable_objects[i].x -= 2; // move left
            }

            // Deactivate if out of screen bounds (assuming 320 width)
            if (throwable_objects[i].x < 0 || throwable_objects[i].x > 320) {
                throwable_objects[i].active = FALSE;
            }
        }
    }
}


/**
* @brief Checks collision of active throwable objects and fills the provided boxes array with their positions and sizes.
 *
 * @param boxes An array of collisionType to be filled with the bounding boxes of three active throwable objects.
 */
void book_collision_box(collisionType * boxes) {
    for (int i = 0; i < MAX_THROWABLE_OBJECTS; i++) {
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
