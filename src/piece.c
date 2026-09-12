#include "piece.h"
#include "dat_manager.h"
#include "errors.h"
#include "helpers.h"
#include "statics.h"
#include <allegro.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    int x;
    int y;
    PieceType type;
    int active;
    int collected;
} Piece;

static Piece pieces[MAX_PIECES];
static BITMAP* piece_sprites[PIECE_TYPE_COUNT];
static int piece_count = 0;
static int remaining_pieces = 0;

// forces all collected for debugging purposes
void all_collected() {
    for (int i = 0; i < piece_count; i++) {
        pieces[i].collected = TRUE;
    }
    remaining_pieces = 0;
}

static inline BITMAP* _get_piece_sprite(PieceType type) {
    if (type < 0 || type >= PIECE_TYPE_COUNT) {
        return NULL;
    }
    return piece_sprites[type];
}

void load_piece_spritesheet() {
    piece_sprites[PIECE_TYPE_CONDENSER] = dat_file[PCONDENSER_BMP].dat;
    piece_sprites[PIECE_TYPE_FUEL] = dat_file[PFUEL_BMP].dat;
    piece_sprites[PIECE_TYPE_FLOW] = dat_file[PFLOW_BMP].dat;

    if (!piece_sprites[PIECE_TYPE_CONDENSER]) {
        die("cannot load PCONDENSER_BMP from datafile");
    }
    if (!piece_sprites[PIECE_TYPE_FUEL]) {
        die("cannot load PFUEL_BMP from datafile");
    }
    if (!piece_sprites[PIECE_TYPE_FLOW]) {
        die("cannot load PFLOW_BMP from datafile");
    }
}

void reset_pieces() {
    for (int i = 0; i < MAX_PIECES; i++) {
        pieces[i].x = 0;
        pieces[i].y = 0;
        pieces[i].type = PIECE_TYPE_CONDENSER;
        pieces[i].active = FALSE;
        pieces[i].collected = FALSE;
    }
    piece_count = 0;
    remaining_pieces = 0;
}

void load_level_pieces(int level_id) {
    int tmx_id = level_to_dat_id(level_id);
    const char* cursor;
    int idx = 0;
    int type_cycle = 0; // cycles 0,1,2 across pieces (no modulo)

    if (tmx_id < 0) {
        die("invalid level id %d for TMX (pieces)", level_id);
    }

    if (dat_file[tmx_id].dat == NULL) {
        die("cannot load TMX data for level %d (pieces)", level_id);
    }

    cursor = (const char*)dat_file[tmx_id].dat;

    while ((cursor = strstr(cursor, "<object ")) != NULL) {
        int id;
        char name[64], object_type[8];
        int x, y;

        int matched = sscanf(cursor,
            "<object id=\"%d\" name=\"%63[^\"]\" type=\"%7[^\"]\" x=\"%d\" y=\"%d\"",
            &id, name, object_type, &x, &y);

        if (matched == 5 && strcmp(name, "PIECE") == 0 && strcmp(object_type, "O") == 0) {
            pieces[idx].x = x;
            pieces[idx].y = y - 18;
            pieces[idx].type = (PieceType)type_cycle;
            pieces[idx].active = TRUE;
            pieces[idx].collected = FALSE;
            idx++;
            type_cycle++;
            if (type_cycle >= PIECE_TYPE_COUNT) {
                type_cycle = 0;
            }
            if (idx >= MAX_PIECES) {
                break;
            }
        }

        cursor++;
    }
    piece_count = idx;
    remaining_pieces = piece_count;
}

inline void draw_pieces(int scroll_x) {
    printf_at_ingame(10, 10, 71, 1, "PIECES: %d", remaining_pieces);
    
    for (int i = 0; i < piece_count; i++) {
        if (pieces[i].active && !pieces[i].collected) {
            BITMAP* sprite = _get_piece_sprite(pieces[i].type);
            if (!sprite) {
                continue;
            }
            int screen_x = pieces[i].x - scroll_x;
            if (screen_x > -sprite->w && screen_x < 320) {
                draw_sprite(current_screen, sprite, screen_x, pieces[i].y);
            }
        }
    }
}

void piece_get_all_aabb(collisionType* boxes) {
    for (int i = 0; i < MAX_PIECES; i++) {
        if (i < piece_count && pieces[i].active && !pieces[i].collected) {
            BITMAP* sprite = _get_piece_sprite(pieces[i].type);
            boxes[i].x = pieces[i].x;
            boxes[i].y = pieces[i].y;
            boxes[i].w = sprite ? sprite->w : 16;
            boxes[i].h = sprite ? sprite->h : 16;
            boxes[i].meta = 0;
        } else {
            boxes[i].x = 0;
            boxes[i].y = 0;
            boxes[i].w = 0;
            boxes[i].h = 0;
            boxes[i].meta = 0;
        }
    }
}

void piece_on_collect(int index) {
    if (index >= 0 && index < piece_count && !pieces[index].collected) {
        pieces[index].collected = TRUE;
        remaining_pieces--;
    }
}

int piece_get_remaining() {
    return remaining_pieces;
}

int piece_get_total_count() {
    return piece_count;
}
