
#ifndef HELPERS_H
#define HELPERS_H

#define FINISHED 1
#define NOT_FINISHED 0

// helpers define common types and functions used across multiple modules, such as player, enemy, and game management.
#include <allegro.h>

// this is the maximum number of frames an animation can have, not the total number of frames across all animations
#define MAX_FRAMES 13

/** @struct animeItem
 * @brief Represents an animation item with frame information
 * @var animeItem::move_count - Number of moves for the animation
 * @var animeItem::frames - Array of frame indices
 * @var animeItem::length - Length of the animation
 * @var animeItem::frame_interval - Interval between frames
 */
typedef struct {
    int move_count;
    int frames[MAX_FRAMES];
    uint8_t length;
    int frame_interval;
} animeItem;

typedef struct {
    int x;
    int y;
} coordsType;

typedef enum {
    TXT_STAGE1_01,
    TXT_STAGE1_02,
    TXT_STAGE1_03,
    TXT_STAGE1_04,
    TXT_STAGE1_05,
    TXT_STAGE1_06,
    TXT_STAGE1_07,
    TXT_STAGE2_01,
    TXT_STAGE2_02,
    TXT_ROOM_01,
    TXT_ROOM_02,
    TXT_ROOM_03,
    TXT_ROOM_04,
    TXT_ROOM_05,
    TXT_STAGE3_01,
    TXT_STAGE3_02,
    TXT_STAGE3_03,
    TXT_COUNT
} gameTextId;

/**
 * @struct collisionType
 * @brief Represents a collision bounding box with position and dimensions
 * @var collisionType::x - X coordinate of the collision box
 * @var collisionType::y - Y coordinate of the collision box
 * @var collisionType::w - Width of the collision box
 * @var collisionType::h - Height of the collision box
 */
typedef struct {
    int x;
    int y;
    int w;
    int h;
    int meta; // optional field for extra info, usage depends on context (e.g., enemy type, tile type, etc.)
} collisionType;

/**
 * @brief Emits a beep sound at the specified frequency and duration.
 *
 * @param frequency The frequency of the beep in Hertz (Hz).
 * @param duration The duration of the beep in milliseconds (ms).
 */
void beep(int frequency, int duration);

/** @brief Waits for the spacebar key to be pressed and released. */
void wait_for_space();

/** @brief Prints text at a specific position on screen. */
void print_at(int x, int y, const char* texto, int col, int bg);

/** @brief Prints text character by character using current game font. */
void print_at_slow(int x, int y, const char* texto, int col, int bg);

/**
 * @brief Prints text at a specific position on screen using current game font.
 * * This is a simplified version of print_at that does not handle line wrapping or
 * long texts. It is intended for short messages or debug output where the caller
 * ensures the text fits within the screen width.
 */
void printf_at_simple(int x, int y, int col, int bg, const char* format, ...);

/** @brief Gets game text by ID for the selected compile-time language. */
const char* game_text(gameTextId id);

/** @brief Converts a level number to its corresponding data ID. */
int level_to_dat_id(int level);

extern BITMAP* current_screen;


#endif