#include "game.h"
#include "allegro/gfx.h"
#include "statics.h"
#include "dat_manager.h"

void load_coche_spritesheet(BITMAP ** sp_coche) {
    BITMAP *coche_spritesheet = dat_file[COCHE_SPRITESHEET_BMP].dat;
    int frame_width = (int) coche_spritesheet->w / COCHE_FRAMES;
    for (int i = 0; i < COCHE_FRAMES; i++) {
        sp_coche[i] = create_sub_bitmap(coche_spritesheet, i * frame_width, 0, frame_width, coche_spritesheet->h);        
    }
}

void destroy_coche_spritesheet(BITMAP ** sp_coche) {
    BITMAP *coche_spritesheet = dat_file[COCHE_SPRITESHEET_BMP].dat;
    int frame_width = (int) coche_spritesheet->w / COCHE_FRAMES;
    for (int i = 0; i < COCHE_FRAMES; i++) {
        destroy_bitmap(sp_coche[i]);
    }
}
