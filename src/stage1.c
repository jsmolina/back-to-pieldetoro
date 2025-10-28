#include "stage1.h"
#include "allegro/color.h"
#include "allegro/gfx.h"
#include "allegro/text.h"
#include "dat_manager.h"
#include "statics.h"

void level1_intro() {
    BITMAP * intro_bmp = dat_file[PIELDETORO_INTRO_BMP].dat;
    blit(intro_bmp, screen, 0, 0, 0, 0, 320, 240);
    textprintf_ex(screen, font, 10, 200, makecol(255, 255, 255), makecol(1, 1, 1), "has montado una máquina del tiempo");
    textprintf_ex(screen, font, 10, 210, makecol(255, 255, 255), makecol(1, 1, 1), "en una loca?");
    textprintf_ex(screen, font, 10, 220, makecol(255, 205, 205), makecol(1, 1, 1), "si vas a hacer una máquina del tiempo");
    textprintf_ex(screen, font, 10, 230, makecol(255, 205, 205), makecol(1, 1, 1), "por qué no hacerlo con clase?");
   // textout_ex(screen, font, "Martin: has montado una máquina del tiempo en una loca?", 0, 300, makecol(255, 255, 255), makecol(1, 1, 1));
   // textout_ex(screen, font, "Emeterio: claro, si vas a hacer una máquina del tiempo, por qué no hacerlo con clase?", 0, 310, makecol(255, 255, 255), makecol(1, 1, 1));
    do {
        vsync();
    } while (!key[KEY_SPACE]);

}
