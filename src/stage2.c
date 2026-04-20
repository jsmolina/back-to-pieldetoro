#include "stage2.h"
#include "dat_manager.h"
#include "helpers.h"
#include "statics.h"
#include "tiles.h"

void level2_intro() {
    blit(dat_file[LEVEL2_INTRO_BMP].dat, screen, 0, 0, 0, 0, 320, 200);
    rectfill(screen, 0, 180, SCREEN_W, SCREEN_H, makecol(1, 1, 1));
    print_at(10, 180, game_text(TXT_STAGE2_01), makecol(255, 205, 205), 16);
    print_at(10, 190, game_text(TXT_STAGE2_02), makecol(255, 255, 255), 16);

    wait_for_space();
}

/*
    if (current_background) {
        destroy_bitmap(current_background);
    }*/