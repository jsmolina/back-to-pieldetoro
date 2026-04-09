#include "coin.h"
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
    int active;
    int collected;
} Coin;

static Coin coins[MAX_COINS];
static BITMAP* coin_sprite = NULL;
static int coins_collected = 0;
static int coin_count = 0; // number of coins loaded for current level

void load_coin_spritesheet() {
    coin_sprite = dat_file[MONEY_BMP].dat;
    if (!coin_sprite) {
        die("cannot load MONEY_BMP from datafile");
    }
}

void reset_coins() {
    for (int i = 0; i < MAX_COINS; i++) {
        coins[i].x = 0;
        coins[i].y = 0;
        coins[i].active = FALSE;
        coins[i].collected = FALSE;
    }
    coins_collected = 0;
    coin_count = 0;
}

void load_level_coins(int level_id) {
    int tmx_id = level_to_dat_id(level_id);
    const char* cursor;
    int idx = 0;

    if (tmx_id < 0) {
        die("invalid level id %d for TMX (coins)", level_id);
    }

    if (dat_file[tmx_id].dat == NULL) {
        die("cannot load TMX data for level %d (coins)", level_id);
    }

    cursor = (const char*)dat_file[tmx_id].dat;

    while ((cursor = strstr(cursor, "<object ")) != NULL) {
        int id;
        char name[64], object_type[8];
        int x, y;

        int matched = sscanf(cursor,
            "<object id=\"%d\" name=\"%63[^\"]\" type=\"%7[^\"]\" x=\"%d\" y=\"%d\"",
            &id, name, object_type, &x, &y);

        if (matched == 5 && strcmp(name, "COIN") == 0 && strcmp(object_type, "O") == 0) {
            coins[idx].x = x;
            coins[idx].y = y - 18; // adjust y to align with ground level
            coins[idx].active = TRUE;
            coins[idx].collected = FALSE;
            idx++;
            if (idx >= MAX_COINS) {
                break;
            }
        }

        cursor++; // advance past current '<' to find next tag
    }
    coin_count = idx;
}

inline void draw_coins(int scroll_x) {
    if (!coin_sprite) {
        return;
    }

    for (int i = 0; i < coin_count; i++) {
        if (coins[i].active && !coins[i].collected) {
            int screen_x = coins[i].x - scroll_x;
            // only draw if visible on screen
            if (screen_x > -coin_sprite->w && screen_x < SCREEN_W) {
                draw_sprite(screen, coin_sprite, screen_x, coins[i].y);
            }
        }
    }
}

void coin_get_all_aabb(collisionType* boxes) {
    for (int i = 0; i < MAX_COINS; i++) {
        if (i < coin_count && coins[i].active && !coins[i].collected) {
            boxes[i].x = coins[i].x;
            boxes[i].y = coins[i].y;
            boxes[i].w = coin_sprite ? coin_sprite->w : 16;
            boxes[i].h = coin_sprite ? coin_sprite->h : 16;
        } else {
            boxes[i].x = 0;
            boxes[i].y = 0;
            boxes[i].w = 0;
            boxes[i].h = 0;
        }
    }
}

void coin_on_collect(int index) {
    if (index >= 0 && index < coin_count) {
        coins[index].collected = TRUE;
        coins_collected++;
    }
}

int get_coins_collected() {
    return coins_collected;
}
