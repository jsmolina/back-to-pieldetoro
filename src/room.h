#ifndef ROOM_H
#define ROOM_H

/** @brief Enters a room sub-loop (e.g. shop).
 *
 *  Draws the room background and runs its own input loop.
 *  The game loop is suspended until the player exits the room.
 */
void enter_room();

#endif
