#include "main_menu.h"
#include "allegro/keyboard.h"
#include "allegro/text.h"
#include "dat_manager.h"
#include "helpers.h"
#include "passcode.h"
#include "statics.h"
#include <allegro.h>

#define MAIN_MENU_W 90
#define MAIN_MENU_X ((SCREEN_W - MAIN_MENU_W) >> 1)   /* centered, no division */
#define MAIN_MENU_Y 30
#define MAIN_MENU_HEIGHT 87
#define MAIN_MENU_OPTION_COUNT 3
#define MENU_MENU_OPTION_HEIGHT 10
#define MENU_MENU_BG_COLOR 17            /* dark blue */
#define MENU_MENU_TEXT_COLOR 67          /* yellow */
#define MENU_MENU_TEXT_SELECTED_COLOR 31 /* white */
#define MENU_MENU_SELECTED_COLOR 67      /* yellow */
#define MENU_MENU_BORDER_COLOR 63        /* bright white */
#define PASSCODE_Y 80

static const char* main_menu_options[MAIN_MENU_OPTION_COUNT] = {
    "START",
    "PASSSWORD",
    "EXIT TO DOS"
};

static int show_menu = 1;
static int read_passcode(char* buf) {
    int len = 0;
    buf[0] = '\0';
    rectfill(screen, MAIN_MENU_X, PASSCODE_Y - 8, MAIN_MENU_X + MAIN_MENU_W, PASSCODE_Y + 16, 21);
    textprintf_ex(screen, font, 
        MAIN_MENU_X + 20, 
        PASSCODE_Y, 
        MENU_MENU_TEXT_COLOR, 
        -1, 
        "PASS: %s", buf);
    while (1) {
        if (!keypressed()) continue;
        int k = readkey();
        int scan = k >> 8;
        char ascii = k & 0xFF;
        if (scan == KEY_ENTER) return 0;
        if (scan == KEY_ESC) return 1;
        if (scan == KEY_BACKSPACE && len > 0) buf[--len] = '\0';
        else if (len < PASSCODE_LENGTH && ascii >= 32 && ascii < 127)
            buf[len++] = (ascii >= 'a' && ascii <= 'z') ? ascii - 32 : ascii;
        buf[len] = '\0';
        rectfill(screen, MAIN_MENU_X, PASSCODE_Y - 8, MAIN_MENU_X + MAIN_MENU_W, PASSCODE_Y + 16, 21);
        textprintf_ex(screen, font, 
            MAIN_MENU_X + 20, 
            PASSCODE_Y, 
            MENU_MENU_TEXT_COLOR, 
            -1, 
            "PASS: %s", buf
        );
        vsync();
    }
}

static void draw_main_menu(int selected) {
    BITMAP* minicar = dat_file[MINICAR_BMP].dat;
    BITMAP* bg = dat_file[MENU2_BMP].dat;
    int total_h = MAIN_MENU_OPTION_COUNT * MENU_MENU_OPTION_HEIGHT + 6;
    int car_x = MAIN_MENU_X - minicar->w - 2;
    /* restore background over the whole menu strip so the car cursor does not ghost */
    blit(bg, screen, car_x, MAIN_MENU_Y + 40, car_x, MAIN_MENU_Y,
         MAIN_MENU_X + MAIN_MENU_W - car_x, total_h + 1);
    if (show_menu == 1) {
        rectfill(screen, MAIN_MENU_X, MAIN_MENU_Y, MAIN_MENU_X + MAIN_MENU_W, MAIN_MENU_Y + total_h, MENU_MENU_BG_COLOR);
        int offset_y = MAIN_MENU_Y + 4;
        for (int i = 0; i < MAIN_MENU_OPTION_COUNT; i++) {
            int fg = (i == selected) ? MENU_MENU_TEXT_SELECTED_COLOR : MENU_MENU_TEXT_COLOR;
            if (i == selected && minicar)
                draw_sprite(screen, minicar, car_x, offset_y - 4);
            printf_at_simple(MAIN_MENU_X + 4, offset_y, fg, -1, "%s", main_menu_options[i]);
            offset_y += MENU_MENU_OPTION_HEIGHT;
        }
    }
}

MainMenuResult show_main_menu() {
    MainMenuResult res;
    res.selected = NEW_GAME;
    show_menu = 1;

    while (key[KEY_ESC]) {
        vsync();
    }
    clear_keybuf();

    while (1) {
        draw_main_menu(res.selected);
        int key_code = readkey() >> 8;

        if (key_code == KEY_UP) {
            if (res.selected == 0) {
                res.selected = MAIN_MENU_OPTION_COUNT - 1;
            } else {
                res.selected--;
            }
        } else if (key_code == KEY_DOWN) {
            res.selected++;
            if (res.selected >= MAIN_MENU_OPTION_COUNT) {
                res.selected = 0;
            }
        } else if (show_menu == 1 && key_code == KEY_ENTER ) {
            clear_keybuf();
            if (res.selected == 1) {
                /* Draw passcode write */
                clear_keybuf();
                while(key[KEY_ENTER]) {
                    
                };
                char buf[PASSCODE_LENGTH];
                read_passcode( buf);
                if (strlen(buf) && 
                    load_pass(buf, 
                        &res.current_level, 
                        &res.lives, 
                        &res.money, 
                        &res.score) == TRUE) {
                      return res;
                }                
            }
            else {
                return res;
            } 

        }  else if (key_code == KEY_ESC || key_code == KEY_SPACE) {
            show_menu = !show_menu;
        }
    }
}
