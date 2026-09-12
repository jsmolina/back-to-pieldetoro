#include "main_menu.h"
#include "allegro/gfx.h"
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
#define PASSCODE_Y 40


static int show_menu = 1;

static inline void present_menu() {
#ifdef _WIN32
    stretch_blit(current_screen, screen,
        0, 0, 320, 200,
        0, 0, WIN32_WIDTH, WIN32_HEIGHT);
#else
    blit(current_screen, screen, 0, 0, 0, 0, SCREEN_W, 200);
#endif
}

static int read_passcode(char* buf) {
    int len = 0;
    buf[0] = '\0';
    rectfill(current_screen, MAIN_MENU_X, PASSCODE_Y - 8, MAIN_MENU_X + 155, PASSCODE_Y + 16, 21);
    textprintf_ex(current_screen, font,
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
        rectfill(current_screen, MAIN_MENU_X, PASSCODE_Y - 8, MAIN_MENU_X + MAIN_MENU_W, PASSCODE_Y + 16, 21);
        textprintf_ex(current_screen, font,
            MAIN_MENU_X + 20, 
            PASSCODE_Y, 
            MENU_MENU_TEXT_COLOR, 
            -1, 
            "PASS: %s", buf
        );
        present_menu();
        vsync();
    }
}

static void draw_main_menu(int selected) {
    BITMAP* minicar = dat_file[MINICAR_BMP].dat;
    BITMAP* bg = dat_file[MENU2_BMP].dat;
    BITMAP* menu_sprite = dat_file[TEXT_MENU_BMP].dat;
    int car_x = MAIN_MENU_X - minicar->w - 2;
    /* restore background over the whole menu strip so the car cursor does not ghost */
    
    blit(bg, current_screen, 0, 40, 0, 0,
         SCREEN_W, SCREEN_H);
    if (show_menu == 1) {
        // maybe 70
        draw_sprite(current_screen, menu_sprite, MAIN_MENU_X, MAIN_MENU_Y);
        int offset_y = MAIN_MENU_Y + 4;
        // TODO offset_y should increase +19 based on selected item        
        for (int i = 0; i < selected; i++) {
            offset_y += 19;
        }
        draw_sprite(current_screen, minicar, car_x, offset_y - 4);
    }
    present_menu();
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
            play_sample(dat_file[CLONK_WAV].dat, 255, 127, 1000, 0);
            if (res.selected == 0) {
                res.selected = MAIN_MENU_OPTION_COUNT - 1;
            } else {
                res.selected--;
            }
        } else if (key_code == KEY_DOWN) {
            play_sample(dat_file[CLONK_WAV].dat, 255, 127, 1000, 0);
            res.selected++;
            if (res.selected >= MAIN_MENU_OPTION_COUNT) {
                res.selected = 0;
            }
        } else if (show_menu == 1 && key_code == KEY_ENTER ) {
            clear_keybuf();
            if (res.selected == PASSWORD) {
                /* Draw passcode write */
                clear_keybuf();
                while(key[KEY_ENTER]) {
                    
                };
                char buf[PASSCODE_LENGTH + 1];
                read_passcode( buf);
                if (strlen(buf) && 
                    load_pass(buf,
                        &res.current_level,
                        &res.lives,
                        &res.money,
                        &res.score,
                        &res.books) == TRUE) {
                      return res;
                }                
            }
            else {
                return res;
            } 

        }  else if (key_code == KEY_ESC) {
            show_menu = !show_menu;
        } else if (key_code == KEY_SPACE) {
            show_menu = 1;
        }
        vsync();
    }
}
