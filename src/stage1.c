#include "stage1.h"
#include "allegro/color.h"
#include "allegro/gfx.h"
#include "allegro/text.h"
#include "dat_manager.h"
#include "statics.h"


void wait_for_space() {
    do {} while (!key[KEY_SPACE]);
    rectfill(screen, 10, 200, 320, 240, makecol(1, 1, 1));
    do {} while (key[KEY_SPACE]);
}

void print_at(int x, int y, char * texto, int col) {
    int longitud = strlen(texto);
    char trozo[36];
    
    for (int i = 0; i < longitud; i += 35) {
        // Calcula cuántos caracteres copiar (último trozo puede ser menor)
        int len = (longitud - i < 35) ? longitud - i : 35;
        
        strncpy(trozo, texto + i, len);
        trozo[len] = '\0'; // importante: terminar el string
        textprintf_ex(screen, font, x, y, col, makecol(1, 1, 1), "%s", trozo);
        y += 10;
        if (y > 240) {
            y = 0;
        }
    }
}


void level1_intro() {
    BITMAP * intro_bmp = dat_file[PIELDETORO_INTRO_BMP].dat;
    blit(intro_bmp, screen, 0, 0, 0, 0, 320, 240);
    print_at(10, 200, "has montado una máquina del tiempoen una loca?", makecol(255, 255, 255));
    print_at(10, 220, "Si vas a montar una cacharra del   tiempo,hazlo con estilo,tron!", makecol(255, 205, 205));

   wait_for_space();   
   print_at(10, 200, "Necesito que vengas al futuro", makecol(255, 205, 205));
   print_at(10, 220, "Nos volvemos tolais o así?", makecol(255, 255, 255));   
   wait_for_space();
   print_at(10, 200, "Tus hijos tron,escuchan regueton", makecol(255, 205, 205));
   print_at(10, 220, "Ostia", makecol(255, 255, 255));
    wait_for_space();
}
