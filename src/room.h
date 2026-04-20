#ifndef ROOM_H
#define ROOM_H

/** @brief Enters a room sub-loop (e.g. shop).
 *  @param room_id The ID of the room.
 *  @param level The current level of the game.
 *  @return Selected option index [0..2], or -1 if exited with ESC.
 *
 *  Draws the room background and runs its own input loop.
 *  The game loop is suspended until the player exits the room.
 */
int enter_room(int room_id, int level);

#endif
