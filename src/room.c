#include "room.h"
#include "allegro/gfx.h"
#include "dat_manager.h"
#include "game.h"
#include "helpers.h"
#include "tiles.h"
#include <allegro.h>
#include <stdio.h>

#define ROOM_OPTION_COUNT 3
#define ROOM_ARROW_Y 104
#define ROOM_OPTION_COST 5
#define ROOM_INFO_AREA_X1 40
#define ROOM_INFO_AREA_X2 280
#define ROOM_INFO_AREA_Y1 180
#define ROOM_INFO_AREA_Y2 200
#define ROOM_MONEY_PANEL_X1 60
#define ROOM_MONEY_PANEL_X2 160
#define ROOM_MONEY_PANEL_Y1 190
#define ROOM_MONEY_PANEL_Y2 198
#define ROOM_ARROW_W 8
#define ROOM_ARROW_H 8

static const int room1_option_x[ROOM_OPTION_COUNT] = { 110, 160, 203 };
static const int room2_option_x[ROOM_OPTION_COUNT] = { 104, 160, 210 };
static const int room1_option_cost[ROOM_OPTION_COUNT] = { ROOM_OPTION_COST, ROOM_OPTION_COST, ROOM_OPTION_COST };
static const int room2_option_cost[ROOM_OPTION_COUNT] = { ROOM_OPTION_COST, ROOM_OPTION_COST, ROOM_OPTION_COST };

static const int* get_option_positions(int room_id) {
    if (room_id == 1) {
        return room1_option_x;
    }
    if (room_id == 2) {
        return room2_option_x;
    }
    return room1_option_x;
}

static const int* get_option_costs(int room_id) {
    if (room_id == 1) {
        return room1_option_cost;
    }
    if (room_id == 2) {
        return room2_option_cost;
    }
    return room1_option_cost;
}

static void draw_room_static_text(int room_id, int txt_id) {
    
    if (room_id == 1) {        
        rectfill(screen, 83, 130, 170, 140, 29);
        print_at_slow(85, 130, game_text(txt_id), makecol(16, 16, 16), 29);
    } else if (room_id == 2) {
        rectfill(screen, 83, 148, 170, 156, 29);
        print_at_slow(85, 150, game_text(txt_id), makecol(16, 16, 16), 29);
    }
}

static void draw_money_panel() {
    char money_text[24];

    rectfill(screen, ROOM_MONEY_PANEL_X1, ROOM_MONEY_PANEL_Y1, ROOM_MONEY_PANEL_X2, ROOM_MONEY_PANEL_Y2, 16);
    snprintf(money_text, sizeof(money_text), "EUR %6d", game_get_money());
    print_at(ROOM_MONEY_PANEL_X1 + 2, 190, money_text, makecol(255, 255, 255), 16);
}

static void draw_arrow_at(const int* option_x, int index) {
    textprintf_ex(screen, font, option_x[index], ROOM_ARROW_Y, makecol(255, 255, 0), -1, "^");
}

static void clear_arrow_at(BITMAP* bg, const int* option_x, int index) {
    blit(bg, screen, option_x[index], ROOM_ARROW_Y, option_x[index], ROOM_ARROW_Y, ROOM_ARROW_W, ROOM_ARROW_H);
}

/** @brief Get the background bitmap for a room based on its ID and level */
static inline BITMAP* get_room_bg(int id, int level, int* owns_bitmap) {
    BITMAP* room_bg = NULL;

    *owns_bitmap = FALSE;

    if (id == 1) {
        room_bg = load_shop_bg(BG1_SHOP_TMX);
        *owns_bitmap = TRUE;
    } else if (id == 2) {
        room_bg = dat_file[BG1_SHOP2_BMP].dat;
    } else {
        // default room background if room_id is not recognized
        room_bg = create_bitmap(SCREEN_W, SCREEN_H);
        rectfill(room_bg, 0, 0, SCREEN_W, SCREEN_H, makecol(255, 0, 0));
        *owns_bitmap = TRUE;
    }
    return room_bg;
}

int enter_room(int room_id, int level) {
    // draw room placeholder: black background
    rectfill(screen, 0, 0, SCREEN_W, SCREEN_H, makecol(0, 0, 0));

    int owns_bitmap;
    int selected = 0;
    const int* option_x = get_option_positions(room_id);
    const int* option_cost = get_option_costs(room_id);

    BITMAP* bg = get_room_bg(room_id, level, &owns_bitmap);

    clear_keybuf();

    blit(bg, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    draw_room_static_text(room_id, TXT_ROOM_01);
    // Room has a reserved UI area with bricks for future HUD-like info:
    // X=[40..280], Y=[180..200].
    draw_money_panel();
    draw_arrow_at(option_x, selected);

    while (1) {
        int key_code = readkey() >> 8;

        if (key_code == KEY_LEFT) {
            int prev_selected = selected;
            selected--;
            if (selected < 0) {
                selected = ROOM_OPTION_COUNT - 1;
            }
            clear_arrow_at(bg, option_x, prev_selected);
            draw_arrow_at(option_x, selected);
        } else if (key_code == KEY_RIGHT) {
            int prev_selected = selected;
            selected++;
            if (selected >= ROOM_OPTION_COUNT) {
                selected = 0;
            }
            clear_arrow_at(bg, option_x, prev_selected);
            draw_arrow_at(option_x, selected);
        } else if (key_code == KEY_SPACE) {
            if (game_try_spend_money(option_cost[selected])) {
                draw_room_static_text(room_id, TXT_ROOM_03);
                draw_money_panel();
                break;
            } else {
                draw_room_static_text(room_id, TXT_ROOM_02);
            }
        } else if (key_code == KEY_ESC) {
            selected = -1;
            break;
        }
    }

    // wait for key release before returning to game
    do {
    } while (key[KEY_SPACE] || key[KEY_ESC]);

    if (owns_bitmap) {
        destroy_bitmap(bg);
    }

    return selected;
}
