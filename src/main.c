#include <stdio.h>

#include "dat_manager.h"
#include "tiles.h"
#include <allegro.h>

#define SCREEN_VIRTUAL 960

int main(void) {
    BITMAP* scroller;
    RGB black = { 0, 0, 0, 0 };
    int x = 0;
    int next_x = 0;
    int h = 100;

    if (allegro_init() != 0)
        return 1;
    install_keyboard();

    if (set_gfx_mode(GFX_MODEX, 320, 240, SCREEN_VIRTUAL, 240) != 0) {
        set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
        allegro_message("Unable to set a 320x240 mode with 640x240 "
                        "virtual dimensions\n");
        return 1;
    }
    set_color_depth(8);
    set_color_conversion(COLORCONV_NONE);

    /* the scrolling area is twice the width of the screen (640x240) */
    scroller = create_sub_bitmap(screen, 0, 0, SCREEN_VIRTUAL, 240);

    extract_data();

    load_tiles(dat_file);
    palette[0].r = 10;
    palette[0].g = 10;
    palette[0].b = 10;
    set_pallete(palette);

    set_color(0, &black);

    BITMAP* bm1 = load_background(PIELDETORO_INTRO_BMP, SCREEN_VIRTUAL);

    // rectfill(scroller, 0, 0, SCREEN_W, 100, 6);
    // rectfill(scroller, 0, 100, SCREEN_W, SCREEN_H, 2);
    blit(bm1, scroller, 0, 0, 0, 0, SCREEN_VIRTUAL, 200);
    

    do {

        if (next_x < 640) {
            next_x = x + 1;
        }

        /* scroll the screen */
        scroll_screen(next_x, 0);
        
        rectfill(scroller, x, 201, 320 +x, 240, makecol(1, 1, 1));
        textprintf_ex(scroller, font, 10 + x, 210, makecol(255, 255, 255), makecol(1,1,1), "Score: %05d", 222);


        x = next_x;

    } while (!keypressed());

    destroy_bitmap(scroller);

    clear_keybuf();
    destroy_tiles();
    cleanup_data();

    return 0;
}

END_OF_MAIN()
