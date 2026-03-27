#include "stage2.h"
#include "helpers.h"
#include "dat_manager.h"
#include "statics.h"
#include "tiles.h"


void level2_intro() {
    blit(dat_file[LEVEL2_INTRO_BMP].dat, screen, 0, 0, 0, 0, 320, 200);
    rectfill(screen, 0, 180, SCREEN_W, SCREEN_H, makecol(1, 1, 1));
    print_at(10, 180, "Nen, ahora gobierna Bruno Tanez", makecol(255, 205, 205));
    print_at(10, 190, "Ostias, no me jodas.", makecol(255, 255, 255));

   wait_for_space();
}

/*
    if (current_background) {
        destroy_bitmap(current_background);
    }*/