#include "room.h"
#include "allegro/gfx.h"
#include "tiles.h"
#include <allegro.h>

void enter_room(int tmx_id) {
    // draw room placeholder: black background
    rectfill(screen, 0, 0, SCREEN_W, SCREEN_H, makecol(0, 0, 0));
    BITMAP* bg = load_shop_bg(tmx_id);

    blit(bg, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);

    textprintf_ex(screen, font, 60, 120, makecol(200, 200, 200), -1, "Pulsa ESPACIO para salir");

    // room loop
    while (1) {
        vsync();
        if (key[KEY_SPACE] || key[KEY_ESC]) {
            break;
        }
    }

    // wait for key release before returning to game
    do {
    } while (key[KEY_SPACE] || key[KEY_ESC]);
    destroy_bitmap(bg);
}
