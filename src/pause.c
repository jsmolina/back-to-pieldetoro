#include "pause.h"
#include "allegro/gfx.h"
#include "allegro/text.h"
#include "helpers.h"
#include "dat_manager.h"
#include "statics.h"
#include <allegro.h>

#define PAUSE_MENU_X 80
#define PAUSE_MENU_Y 60
#define PAUSE_MENU_W 160
#define PAUSE_MENU_H 87
#define PAUSE_MENU_OPTION_COUNT 3
#define PAUSE_OPTION_HEIGHT 30
#define PAUSE_BG_COLOR 17            /* dark blue */
#define PAUSE_TEXT_COLOR 67          /* yellow */
#define PAUSE_TEXT_SELECTED_COLOR 16 /* black */
#define PAUSE_SELECTED_COLOR 67      /* yellow */
#define PAUSE_BORDER_COLOR 63        /* bright white */

static const char* pause_menu_options[PAUSE_MENU_OPTION_COUNT] = {
    "CONTINUE",
    "MENU",
    "EXIT TO DOS"
};

static void draw_pause_menu(int selected, const char* passcode) {
    /* Draw dark blue background with border */
    rectfill(screen, PAUSE_MENU_X, PAUSE_MENU_Y, PAUSE_MENU_X + PAUSE_MENU_W, PAUSE_MENU_Y + PAUSE_MENU_H, PAUSE_BG_COLOR);
    rect(screen, PAUSE_MENU_X - 1, PAUSE_MENU_Y - 1, PAUSE_MENU_X + PAUSE_MENU_W + 1, PAUSE_MENU_Y + PAUSE_MENU_H + 1, PAUSE_BORDER_COLOR);
    int offset_y = PAUSE_MENU_Y + 10;
    /* Draw each option */
    for (int i = 0; i < PAUSE_MENU_OPTION_COUNT; i++) {
        int y = offset_y;
        int bg_color = (i == selected) ? PAUSE_SELECTED_COLOR : PAUSE_BG_COLOR;
        int fg_color = (i == selected) ? PAUSE_TEXT_SELECTED_COLOR : PAUSE_TEXT_COLOR;

        if (i == selected) {
            rectfill(screen, PAUSE_MENU_X, y - 8, PAUSE_MENU_X + PAUSE_MENU_W, y + 16, bg_color);
        }

        // textprintf_ex(screen, font, PAUSE_MENU_X + 20, y, fg_color, bg_color, "%s", pause_menu_options[i]);
        printf_at_simple(PAUSE_MENU_X + 20, y, fg_color, bg_color, "%s", pause_menu_options[i]);
        offset_y += PAUSE_OPTION_HEIGHT;
    }
    /* Draw passcode at the top of the menu */
    int passcode_y = 32;
    rectfill(screen, PAUSE_MENU_X, passcode_y - 8, PAUSE_MENU_X + PAUSE_MENU_W, passcode_y + 16, PAUSE_BG_COLOR);
    //printf_at_simple(PAUSE_MENU_X + 20, passcode_y, PAUSE_TEXT_COLOR, PAUSE_BG_COLOR, "PASS: %s", passcode);
    textprintf_ex(
        screen, font, PAUSE_MENU_X + 20, passcode_y, PAUSE_TEXT_COLOR, PAUSE_BG_COLOR, "PASS: %s", passcode);
}

enum PauseMenuOption show_pause_menu(const char* passcode) {
    int selected = PCONTINUE;

    while (key[KEY_ESC]) {
        vsync();
    }
    clear_keybuf();

    while (1) {
        draw_pause_menu(selected, passcode);
        int key_code = readkey() >> 8;

        if (key_code == KEY_UP) {
            play_sample(dat_file[CLONK_WAV].dat, 255, 127, 1000, 0);
            selected--;
            if (selected < 0) {
                selected = PAUSE_MENU_OPTION_COUNT - 1;
            }
        } else if (key_code == KEY_DOWN) {
            play_sample(dat_file[CLONK_WAV].dat, 255, 127, 1000, 0);
            selected++;
            if (selected >= PAUSE_MENU_OPTION_COUNT) {
                selected = 0;
            }
        } else if (key_code == KEY_ENTER || key_code == KEY_SPACE) {
            clear_keybuf();
            return (enum PauseMenuOption)selected;
        } else if (key_code == KEY_ESC) {
            clear_keybuf();
            return PCONTINUE;
        }
    }
}
