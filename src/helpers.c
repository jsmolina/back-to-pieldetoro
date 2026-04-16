
#include "helpers.h"
#include "statics.h"
#include "dat_manager.h"

#include <allegro.h>

#include <pc.h>
#include <dos.h>




void wait_for_space() {
    do {} while (!key[KEY_SPACE]);
    rectfill(screen, 0, 160, SCREEN_W, SCREEN_H, makecol(1, 1, 1));
    do {} while (key[KEY_SPACE]);
}

void print_at(int x, int y, char * texto, int col) {
    FONT *myfont = dat_file[FONT_FNT].dat;

    int longitud = strlen(texto);
    char trozo[36];
    //rectfill(screen, 0, y, SCREEN_W, y+20, makecol(1,1,1));
    for (int i = 0; i < longitud; i += 35) {
        // Calcula cuántos caracteres copiar (último trozo puede ser menor)
        int len = (longitud - i < 35) ? longitud - i : 35;
        
        strncpy(trozo, texto + i, len);
        trozo[len] = '\0'; // importante: terminar el string
        textprintf_ex(screen, myfont, x, y, col, makecol(1, 1, 1), "%s", trozo);
        y += 8;
        if (y > 240) {
            y = 0;
        }
    }
}


void beep(int frequency, int duration) {
    int div = 1193180 / frequency;
    
    outportb(0x43, 0xb6);
    outportb(0x42, div & 0xff);
    outportb(0x42, div >> 8);
    
    outportb(0x61, inportb(0x61) | 3);
    
    delay(duration);    
    outportb(0x61, inportb(0x61) & 0xfc);
}

int level_to_dat_id(int level) {
    switch (level) {
    case 1:
        return BG0_TMX;
    case 2:
        return BG1_TMX;
    default:
        return -1;
    }
}
