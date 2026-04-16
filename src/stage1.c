#include "stage1.h"
#include "allegro/color.h"
#include "allegro/gfx.h"
#include "allegro/inline/draw.inl"
#include "allegro/palette.h"
#include "helpers.h"
#include "dat_manager.h"
#include "statics.h"
#define START_Y 165
#define OFFSET 18


void level1_intro() {
    set_palette((RGB*) dat_file[PALETE_INTRO_BMP].dat);
    
    blit(dat_file[PIELDETORO_INTRO3_BMP].dat, screen, 0, 0, 0, 0, 320, 200);    
    rectfill(screen, 0, 160, SCREEN_W, SCREEN_H, makecol(1, 1, 1));
    print_at(10, START_Y, "HAS MONTAO UNA MAQUINA DEL TIEMPO  EN UNA LOCA?", makecol(255, 255, 255));
    print_at(10, START_Y + OFFSET, "SI VAS A MONTAR UNA MAQUINA DEL    TIEMPO,QUE MOLE,TRON!", makecol(255, 205, 205));

   wait_for_space();   
   print_at(10, START_Y, "NECESITO QUE VENGAS AL FUTURO", makecol(255, 205, 205));
   print_at(10, START_Y + OFFSET, "NOS VOLVEMOS TOLAIS O ASI?", makecol(255, 255, 255));   
   wait_for_space();
   print_at(10, START_Y, "TUS HIJOS TRON,ESCUCHAN REGUETON Y DICEN BRO", makecol(255, 205, 205));
   print_at(10, START_Y + OFFSET, "OSTIA, ARRANCA LA LOCA QUE LA METO A 140 KM/H", makecol(255, 255, 255));
    wait_for_space();
    print_at(10, START_Y, "CUIDAO QUE ESTA LA CARRETERA HECHA UN CISCO", makecol(255, 255, 255));
    wait_for_space();
    set_palette(default_palette);
}
