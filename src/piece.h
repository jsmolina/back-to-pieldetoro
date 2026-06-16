#ifndef PIECE_H
#define PIECE_H

#include "helpers.h"

#define MAX_PIECES 20

typedef enum {
    PIECE_TYPE_CONDENSER = 0,
    PIECE_TYPE_FUEL = 1,
    PIECE_TYPE_FLOW = 2,
    PIECE_TYPE_COUNT = 3
} PieceType;

/** @brief Loads the piece sprite from datafile. Call once at startup. */
void load_piece_spritesheet();

/** @brief Resets piece slots and counters for the current stage. */
void reset_pieces();

/** @brief Loads level pieces from TMX objects NAME="PIECE" TYPE="O". */
void load_level_pieces(int level_id);

/** @brief Draws all active and not-collected pieces. */
void draw_pieces(int scroll_x);

/** @brief Gets AABB boxes for all active and not-collected pieces. */
void piece_get_all_aabb(collisionType* boxes);

/** @brief Marks one piece as collected by index. */
void piece_on_collect(int index);

/** @brief Returns number of collected pieces for current stage. */
int piece_get_remaining();

/** @brief Returns number of pieces loaded for current stage. */
int piece_get_total_count();

#endif