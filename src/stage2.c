#include "stage2.h"
#include "allegro/color.h"
#include "allegro/gfx.h"
#include "allegro/inline/draw.inl"
#include "helpers.h"
#include "dat_manager.h"
#include "statics.h"
#include "tiles.h"


void level2_intro() {
    BITMAP * intro_bmp = dat_file[LEVEL2_INTRO_BMP].dat;
    blit(intro_bmp, screen, 0, 0, 0, 0, 320, 200);
    destroy_bitmap(intro_bmp);
    rectfill(screen, 0, 180, SCREEN_W, SCREEN_H, makecol(1, 1, 1));
    print_at(10, 180, "Nen, ahora gobierna Bruno Tanez", makecol(255, 205, 205));
    print_at(10, 190, "Ostias, no me jodas.", makecol(255, 255, 255));
   wait_for_space();
}

/*
    if (current_background) {
        destroy_bitmap(current_background);
    }*/