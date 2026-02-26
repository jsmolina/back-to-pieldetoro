#include "stage1.h"
#include "allegro/color.h"
#include "allegro/gfx.h"
#include "allegro/inline/draw.inl"
#include "allegro/palette.h"
#include "helpers.h"
#include "dat_manager.h"
#include "statics.h"


void level1_intro() {
    BITMAP * intro_bmp = dat_file[PIELDETORO_INTRO3_BMP].dat;    
    set_palette((RGB*) dat_file[PALETE_INTRO_BMP].dat);
    
    blit(intro_bmp, screen, 0, 0, 0, 0, 320, 200);
    destroy_bitmap(intro_bmp);
    rectfill(screen, 0, 160, SCREEN_W, SCREEN_H, makecol(1, 1, 1));
    print_at(10, 160, "has montado una máquina del tiempoen una loca?", makecol(255, 255, 255));
    print_at(10, 180, "Si vas a montar una cacharra del   tiempo,que mole,tron!", makecol(255, 205, 205));

   wait_for_space();   
   print_at(10, 160, "Necesito que vengas al futuro", makecol(255, 205, 205));
   print_at(10, 180, "Nos volvemos tolais o así?", makecol(255, 255, 255));   
   wait_for_space();
   print_at(10, 160, "Tus hijos tron,escuchan regueton y dicen bro", makecol(255, 205, 205));
   print_at(10, 180, "Ostia, arranca la loca que la meto a 140 km/h", makecol(255, 255, 255));
    wait_for_space();
    print_at(10, 160, "Cuidao que esta la carretera hecha un cisco", makecol(255, 255, 255));
    wait_for_space();
    set_palette(default_palette);
}
