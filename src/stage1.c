#include "stage1.h"
#include "allegro/color.h"
#include "allegro/gfx.h"
#include "allegro/inline/draw.inl"
#include "allegro/palette.h"
#include "dat_manager.h"
#include "helpers.h"
#include "statics.h"
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
