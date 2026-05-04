#include "coin.h"
#include "dat_manager.h"
#include "errors.h"
#include "game.h"
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
static Coin enemy_drop_coins[MAX_ENEMY_COINS];
static BITMAP* coin_sprite = NULL;
static int coin_count = 0; // number of coins loaded for current level

static void _clear_coin(Coin* coin) {
    coin->x = 0;
    coin->y = 0;
    coin->active = FALSE;
    coin->collected = FALSE;
}

void load_coin_spritesheet() {
    coin_sprite = dat_file[MONEY_BMP].dat;
    if (!coin_sprite) {
        die("cannot load MONEY_BMP from datafile");
    }
}

void reset_coins() {
    for (int i = 0; i < MAX_COINS; i++) {
        _clear_coin(&coins[i]);
    }
    for (int i = 0; i < MAX_ENEMY_COINS; i++) {
        _clear_coin(&enemy_drop_coins[i]);
    }
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

    for (int i = 0; i < MAX_ENEMY_COINS; i++) {
        if (enemy_drop_coins[i].active && !enemy_drop_coins[i].collected) {
            int screen_x = enemy_drop_coins[i].x - scroll_x;
            if (screen_x <= -coin_sprite->w || screen_x >= SCREEN_W) {
                enemy_drop_coins[i].active = FALSE;
                continue;
            }
            draw_sprite(screen, coin_sprite, screen_x, enemy_drop_coins[i].y);
        }
    }
}

void coin_spawn_enemy_drop(int x, int y) {
    if (y < 90) {
        y = 90;
    }
    for (int i = 0; i < MAX_ENEMY_COINS; i++) {
        if (!enemy_drop_coins[i].active || enemy_drop_coins[i].collected) {
            enemy_drop_coins[i].x = x;
            enemy_drop_coins[i].y = y;
            enemy_drop_coins[i].active = TRUE;
            enemy_drop_coins[i].collected = FALSE;
            return;
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

    for (int i = 0; i < MAX_ENEMY_COINS; i++) {
        int box_index = MAX_COINS + i;
        if (enemy_drop_coins[i].active && !enemy_drop_coins[i].collected) {
            boxes[box_index].x = enemy_drop_coins[i].x;
            boxes[box_index].y = enemy_drop_coins[i].y;
            boxes[box_index].w = coin_sprite ? coin_sprite->w : 16;
            boxes[box_index].h = coin_sprite ? coin_sprite->h : 16;
        } else {
            boxes[box_index].x = 0;
            boxes[box_index].y = 0;
            boxes[box_index].w = 0;
            boxes[box_index].h = 0;
        }
    }
}

void coin_on_collect(int index) {
    if (index >= 0 && index < coin_count && !coins[index].collected) {
        coins[index].collected = TRUE;
        game_on_coin_collected();
        return;
    }

    if (index >= MAX_COINS && index < MAX_TOTAL_COINS) {
        int enemy_coin_index = index - MAX_COINS;
        if (enemy_drop_coins[enemy_coin_index].active && !enemy_drop_coins[enemy_coin_index].collected) {
            enemy_drop_coins[enemy_coin_index].collected = TRUE;
            enemy_drop_coins[enemy_coin_index].active = FALSE;
            game_on_coin_collected();
        }
    }
}

int get_coins_collected() {
    return game_get_coins_collected();
}
