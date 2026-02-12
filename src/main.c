#include <stdio.h>

#include "allegro/gfx.h"
#include "errors.h"
#include "allegro/keyboard.h"
#include "dat_manager.h"
#include "game.h"
#include "tiles.h"
#include <allegro.h>
// https://hysblog.com/en/lets-make-a-2d-pixel-art-jump-action-game-with-javascript-final-part-with-love-to-mario/

// game states
#define TITLE 0
#define GAME 1
#define GAME_OVER 2
// screen scroll size

short game_state = 0;

static volatile int update_count, frame_count, fps = 0;

void gfx_timer_proc(void) { update_count = 1; }
END_OF_FUNCTION(gfx_timer_proc)

static void gfx_fps_proc(void) {
    fps = frame_count;
    frame_count = 0;
}
END_OF_FUNCTION(gfx_fps_proc)

void gfx_init_timer() {
    LOCK_VARIABLE(update_count);
    LOCK_VARIABLE(frame_count);
    LOCK_VARIABLE(fps);
    LOCK_FUNCTION(gfx_timer_proc);
    LOCK_FUNCTION(gfx_fps_proc);
    install_int_ex(gfx_timer_proc, BPS_TO_TIMER(70));
    install_int_ex(gfx_fps_proc, BPS_TO_TIMER(1));
}
static volatile long speed_counter = 0;

void increment_speed_counter() {
    speed_counter++;
}
END_OF_FUNCTION(increment_speed_counter);

int main(void) {
    RGB black = { 16, 16, 16, 0 };

    if (allegro_init() != 0)
        return 1;
    install_keyboard();

    if (set_gfx_mode(GFX_VGA, 320, 200, 320, 200) != 0) {
        set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
        allegro_message("Unable to set a 320x240 mode \n");
        return 1;
    }
    set_color_depth(8);
    set_color_conversion(COLORCONV_NONE);

    /* the scrolling area is twice the width of the screen (640x240) */
    //scroller = create_sub_bitmap(screen, 0, 0, SCREEN_W, SCREEN_H);
    //scroller = create_video_bitmap(SCREEN_W, SCREEN_H);

    extract_data();

    load_tiles();
    /*palette[0].r = 10;
    palette[0].g = 10;
    palette[0].b = 10;
    set_pallete(palette);*/
    get_pallete(palette);

    set_color(0, &black);

    // BITMAP* bm1 = load_background(BG0_TMX, SCREEN_VIRTUAL);
    BITMAP* menu = dat_file[MENU2_BMP].dat;

    /*coche_spritesheet->w
    coche_spritesheet->h*/
    short exit_game = 0;

    // rectfill(scroller, 0, 0, SCREEN_W, 100, 6);
    // rectfill(scroller, 0, 100, SCREEN_W, SCREEN_H, 2);
    for (int i = 0; i < 40 ; i++) {
        blit(menu, screen, 0, i, 0, 0, 320, 200);
        vsync();
    }
    
    gfx_init_timer();
    do {
        switch (game_state) {
        case TITLE:
            if (key[KEY_SPACE]) {
                game_state = GAME;
                do {} while(key[KEY_SPACE]);
                start_new_game();
            }
            break;
        case GAME:
            update_game();
            break;
        case GAME_OVER:
            break;
        }

        //blit(scroller, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
        vsync();

        if (key[KEY_ESC]) {
            exit_game = 1;
        }

    } while (exit_game == 0);
    //die("Exiting game...");
    
    //destroy_bitmap(scroller);
    //unload_game_memory();
    //unload_datafile(dat_file);
    printf("Enjoyed playing? See you soon!\n");
    clear_keybuf();
    return 0;
}

END_OF_MAIN()
