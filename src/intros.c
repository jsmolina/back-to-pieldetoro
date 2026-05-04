#include "dat_manager.h"
#include "helpers.h"
#include "statics.h"
#include <allegro.h>

#define START_Y 165
#define OFFSET 18

void level1_intro() {
    set_palette((RGB*)dat_file[PALETE_INTRO_BMP].dat);

    blit(dat_file[PIELDETORO_INTRO3_BMP].dat, screen, 0, 0, 0, 0, 320, 200);
    rectfill(screen, 0, 160, SCREEN_W, SCREEN_H, makecol(1, 1, 1));
    print_at(10, START_Y, game_text(TXT_STAGE1_01), makecol(255, 255, 255), 16);
    print_at(10, START_Y + OFFSET, game_text(TXT_STAGE1_02), makecol(255, 205, 205), 16);

    wait_for_space();
    print_at(10, START_Y, game_text(TXT_STAGE1_03), makecol(255, 205, 205), 16);
    print_at(10, START_Y + OFFSET, game_text(TXT_STAGE1_04), makecol(255, 255, 255), 16);
    wait_for_space();
    print_at(10, START_Y, game_text(TXT_STAGE1_05), makecol(255, 205, 205), 16);
    print_at(10, START_Y + OFFSET, game_text(TXT_STAGE1_06), makecol(255, 255, 255), 16);
    wait_for_space();
    print_at(10, START_Y, game_text(TXT_STAGE1_07), makecol(255, 255, 255), 16);
    wait_for_space();
    set_palette(default_palette);
}

void level2_intro() {
    blit(dat_file[LEVEL2_INTRO_BMP].dat, screen, 0, 0, 0, 0, 320, 200);
    rectfill(screen, 0, 180, SCREEN_W, SCREEN_H, makecol(1, 1, 1));
    print_at(10, 180, game_text(TXT_STAGE2_01), makecol(255, 205, 205), 16);
    print_at(10, 190, game_text(TXT_STAGE2_02), makecol(255, 255, 255), 16);

    wait_for_space();
}

void level3_intro() {
    /*blit(dat_file[LEVEL3_INTRO_BMP].dat, screen, 0, 0, 0, 0, 320, 200);
    rectfill(screen, 0, 180, SCREEN_W, SCREEN_H, makecol(1, 1, 1));
    print_at(10, 180, game_text(TXT_STAGE3_01), makecol(255, 205, 205), 16);
    print_at(10, 190, game_text(TXT_STAGE3_02), makecol(255, 255, 255), 16);*/

    wait_for_space();
}

void show_intro(int level) {
    switch (level) {
    case 1:
        level1_intro();
        break;
    case 2:
        level2_intro();
        break;
    case 3:
        level3_intro();
        break;
    default:
        break;
    }
}
