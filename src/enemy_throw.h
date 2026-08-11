#ifndef ENEMY_THROW_H
#define ENEMY_THROW_H

#include "helpers.h"
#define MAX_ENEMY_THROWABLE_OBJECTS 2

typedef enum {
    MIC_TYPE,
    USHANKA_TYPE,
    // Add more types here if needed
} EnThrowableEnum;

typedef struct {
    int x;
    int y;
    int active;
    int flip;
    EnThrowableEnum type;
} ThrowableObject;


/**
 * @file enemy_throw.h
 * @brief Header file for managing throwable objects (books) in the game
 */

 
/** @brief Initializes a new throwable object.
 *
 * @param x The x-coordinate of the object.
 * @param y The y-coordinate of the object.
 * @param flip The flip flag for the object. 
 * @return TRUE if the object was initialized successfully, FALSE otherwise.
 */
int init_enemy_throwable(int x, int y, int flip);

/** @brief Draws all active throwable objects on the screen.
 *
 * @param scroll_x The horizontal scroll offset.
 */
void draw_enemy_throwable(int scroll_x);

/** @brief Updates the position of all active throwable objects.
 *
 * @param scroll_x The horizontal scroll offset.
 */
void enemy_throwable_update(int scroll_x);

/**
 * @brief Checks collision of active throwable objects and fills the provided boxes array with their positions and sizes.
 *
 * @param boxes An array of collisionType to be filled with the bounding boxes of three active throwable objects.
 */
void enemy_throwable_get_all_aabb(collisionType* boxes);

/**
 * @brief Marks the throwable object at the given index as hit (inactive) and resets its position.
 *
 * @param index the index of the throwable object to be marked as hit. Must be between 0 and MAX_THROWABLE_OBJECTS - 1.
 */
void enemy_throwable_on_hit(int index);

//collisionType enemy_throwable_get_aabb(int index);
// TODO: implement enemy_throwable_on_hit
#endif
