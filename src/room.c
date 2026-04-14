#include "room.h"
#include <allegro.h>

void enter_room() {
    // draw room placeholder: black background
    rectfill(screen, 0, 0, SCREEN_W, SCREEN_H, makecol(0, 0, 0));
    textprintf_ex(screen, font, 120, 90, makecol(255, 255, 255), -1, "TIENDA");
    textprintf_ex(screen, font, 60, 120, makecol(200, 200, 200), -1, "Pulsa ESPACIO para salir");

    // room loop
    while (1) {
        vsync();
        if (key[KEY_SPACE] || key[KEY_ESC]) {
            break;
        }
    }

    // wait for key release before returning to game
    do {} while (key[KEY_SPACE] || key[KEY_ESC]);
}
