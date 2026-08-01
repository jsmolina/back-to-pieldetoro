#include "room.h"
#include "allegro/gfx.h"
#include "dat_manager.h"
#include "game.h"
#include "player.h"
#include "helpers.h"
#include "tiles.h"
#include "book.h"
#include <allegro.h>
#include <stdio.h>

#define ROOM_COUNT 3
#define ROOM_FIRST_ID 1
#define ROOM_OPTION_COUNT 3
#define ROOM_ARROW_Y 104
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

typedef struct {
    int x;
    int cost;
    int default_purchased;
} RoomOption;

static RoomOption room_options[ROOM_COUNT][ROOM_OPTION_COUNT] = {
    { { 110, 1000, FALSE }, { 160, 30, FALSE }, { 203, 1, TRUE } },
    { { 104, 5, FALSE }, { 160, 70, FALSE }, { 210, 6, FALSE } },
    { { 110, 5, FALSE }, { 160, 30, FALSE }, { 203, 10, FALSE } },
};

int purchased_items[ROOM_COUNT][ROOM_OPTION_COUNT] = { { FALSE, FALSE, TRUE }, { FALSE, FALSE, FALSE }, { FALSE, FALSE, FALSE } };

static int room_id_to_index(int room_id) {
    int room_index = room_id - ROOM_FIRST_ID;
    if (room_index < 0 || room_index >= ROOM_COUNT) {
        return 0;
    }
    return room_index;
}

void room_reset_purchased_items() {
    for (int i = 0; i < ROOM_COUNT; i++) {
        for (int j = 0; j < ROOM_OPTION_COUNT; j++) {
            purchased_items[i][j] = room_options[i][j].default_purchased;
        }
    }
}

static void draw_room_static_text(int room_id, int txt_id) {

    if (room_id == 1 || room_id == 3) {
        rectfill(screen, 83, 130, 180, 140, 29);
        print_at_slow(85, 130, game_text(txt_id), makecol(16, 16, 16), 29);
    } else if (room_id == 2) {
        rectfill(screen, 83, 148, 180, 156, 29);
        print_at_slow(85, 150, game_text(txt_id), makecol(16, 16, 16), 29);
    }
}

static void draw_money_panel() {
    rectfill(screen, ROOM_MONEY_PANEL_X1, ROOM_MONEY_PANEL_Y1, ROOM_MONEY_PANEL_X2, ROOM_MONEY_PANEL_Y2, 16);
    printf_at_simple(ROOM_MONEY_PANEL_X1 + 2, 190, 15, -1, "EUR %6d", game_get_money());
    rectfill(screen, ROOM_MONEY_PANEL_X2 + 10, ROOM_MONEY_PANEL_Y1, ROOM_MONEY_PANEL_X2 + 110, ROOM_MONEY_PANEL_Y2, 16);
    printf_at_simple(ROOM_MONEY_PANEL_X2 + 12, 190, 15, -1, "ESC to exit");
}

static void draw_selector_at(const RoomOption* options, int index, int room_index) {
    if (purchased_items[room_index][index] == TRUE) {
        textprintf_ex(screen, font, options[index].x, ROOM_ARROW_Y, makecol(255, 0, 0), -1, "X");
    } else {
        textprintf_ex(screen, font, options[index].x, ROOM_ARROW_Y, makecol(255, 255, 0), -1, "^");
    }
}

static void clear_arrow_at(BITMAP* bg, const RoomOption* options, int index) {
    blit(bg, screen, options[index].x, ROOM_ARROW_Y, options[index].x, ROOM_ARROW_Y, ROOM_ARROW_W, ROOM_ARROW_H);
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
    } else if (id == 3) {
        room_bg = load_shop_bg(BG2_SHOP_TMX);
        *owns_bitmap = TRUE;
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
    int room_index = room_id_to_index(room_id);
    RoomOption* options = room_options[room_index];

    BITMAP* bg = get_room_bg(room_id, level, &owns_bitmap);

    clear_keybuf();

    blit(bg, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    draw_room_static_text(room_id, TXT_ROOM_01);
    if (room_id ==1) {
        printf_at_simple(55, 16, 15, 16, game_text(TXT_ROOM_05));
    } 
    // Room has a reserved UI area with bricks for future HUD-like info:
    // X=[40..280], Y=[180..200].
    draw_money_panel();
    draw_selector_at(options, selected, room_index);

    while (1) {
        int key_code = readkey() >> 8;

        if (key_code == KEY_LEFT) {
            int prev_selected = selected;
            selected--;
            if (selected < 0) {
                selected = ROOM_OPTION_COUNT - 1;
            }
            clear_arrow_at(bg, options, prev_selected);
            draw_selector_at(options, selected, room_index);
        } else if (key_code == KEY_RIGHT) {
            int prev_selected = selected;
            selected++;
            if (selected >= ROOM_OPTION_COUNT) {
                selected = 0;
            }
            clear_arrow_at(bg, options, prev_selected);
            draw_selector_at(options, selected, room_index);
        } else if (key_code == KEY_SPACE || key_code == KEY_ENTER) {
            if (purchased_items[room_index][selected] == TRUE) {
                draw_room_static_text(room_id, TXT_ROOM_04);
            } else if (game_try_spend_money(options[selected].cost)) {
                purchased_items[room_index][selected] = TRUE;
                clear_arrow_at(bg, options, selected);
                draw_selector_at(options, selected, room_index);
                draw_room_static_text(room_id, TXT_ROOM_03);
                draw_money_panel();
                if (room_id == 2) {
                   // special case for 1st option in room 2, which is a health upgrade
                   if (selected == 0) {
                        player_energy_up();
                   } else if (selected == 2) {
                        reinit_book_stock();
                   }
                } else if (room_id == 1 && selected == 1) {
                    player_took_almanac();
                } else if (room_id == 3) {
                    if (selected == 0) {
                        player_energy_up();
                    } else if (selected == 1) {
                        player_life_up();
                    } else if(selected == 2) {
                        player_throwable_up();
                    }
                }
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
