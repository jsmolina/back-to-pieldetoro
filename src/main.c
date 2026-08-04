#include <stdio.h>

#include "allegro/fli.h"
#include "allegro/gfx.h"
#include "allegro/keyboard.h"
#include "allegro/midi.h"
#include "allegro/timer.h"
#include "coin.h"
#include "dat_manager.h"
#include "enemy.h"
#include "game.h"
#include "piece.h"
#include "platform.h"
#include "player.h"
#include "tiles.h"
#include "main_menu.h"

#include <allegro.h>
// https://hysblog.com/en/lets-make-a-2d-pixel-art-jump-action-game-with-javascript-final-part-with-love-to-mario/

// game states
#define TITLE 0
#define GAME 1
#define GAME_OVER 2
// screen scroll size

short game_state = TITLE;

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

static int skip_fli_on_space(void) {
    return key[KEY_SPACE] ? 1 : 0;
}

static void wait_for_space_release(void) {
    while (key[KEY_SPACE]) {
        rest(0);
    }
    clear_keybuf();
}

static void wait_a_bit(int amount) {
    clear_keybuf();
    int start = retrace_count;
    while (!key[KEY_SPACE] && (retrace_count - start) < amount) {
        vsync();
    }
}

inline void show_intro_menu() {
    BITMAP* menu = dat_file[MENU2_BMP].dat;

    for (int i = 0; i < 40; i++) {
        blit(menu, screen, 0, i, 0, 0, 320, 200);
        vsync();
    }
}

int main(int argc, char *argv[]) {
    if (argc > 1 && strcmp(argv[1], "megahit") == 0) {
        megahit_mode = 1;
    }

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
    current_screen = create_bitmap(320, 200);
    /* the scrolling area is twice the width of the screen (640x240) */
    // scroller = create_sub_bitmap(screen, 0, 0, SCREEN_W, SCREEN_H);
    // scroller = create_video_bitmap(SCREEN_W, SCREEN_H);

    extract_data();

    load_tiles();
    /*palette[0].r = 10;
    palette[0].g = 10;
    palette[0].b = 10;
    set_pallete(palette);*/
    get_pallete(palette);
    set_color(0, &black);
    gfx_init_timer();

    if (install_sound(DIGI_AUTODETECT, MIDI_AUTODETECT, "./allegro.cfg") != 0) {
        allegro_message("Error: cannot enable sound\n%s\n", allegro_error);
        return 1;
    }
    play_midi(dat_file[INTRO_MID].dat, 0);

    set_palette((RGB*)dat_file[PALETE_JORDI_LOGO_BMP].dat);
    blit(dat_file[JORDI_LOGO_BMP].dat, screen, 0, 0, 0, 0, 320, 200);
    wait_a_bit(700);
    stop_midi();
    play_midi(dat_file[MSDOS_MID].dat, 0);
    set_palette(palette);
    blit(dat_file[MSDOSCLUB_BMP].dat, screen, 0, 0, 0, 0, 320, 200);
    DATAFILE *video_data1 = obtain_videodata("INTRO2_FLI");
    DATAFILE *video_data2 = obtain_videodata("INTRO_FLI");
    wait_a_bit(200);
    // BITMAP* bm1 = load_background(BG0_TMX, SCREEN_VIRTUAL);
    // rectfill(scroller, 0, 0, SCREEN_W, 100, 6);
    // rectfill(scroller, 0, 100, SCREEN_W, SCREEN_H, 2);
    //DATAFILE *video_obj = load_datafile_object("intro.dat", "INTRO_VIDEO");
    play_memory_fli(video_data1->dat, screen, 0, skip_fli_on_space);
    wait_for_space_release();
    play_memory_fli(video_data2->dat, screen, 0, skip_fli_on_space);    
    wait_for_space_release();
    unload_datafile_object(video_data1);
    unload_datafile_object(video_data2);
    wait_a_bit(200);
    set_palette(palette);

    show_intro_menu();

    load_enemy_spritesheets();
    load_coche_spritesheet();
    load_martin_spritesheet();
    load_coin_spritesheet();
    load_piece_spritesheet();
    load_platform_spritesheet();

    short exit_game = 0;
    do {
        switch (game_state) {
        case TITLE:
            if (key[KEY_SPACE]) {
                while (key[KEY_SPACE]) {

                }
                MainMenuResult res = show_main_menu();
                if (res.selected == EXIT_TO_DOS) {
                    exit_game = 1;
                } else if(res.selected == PASSWORD) {
                    game_state = GAME;
                    stop_midi();
                    start_new_game();
                    // now continue
                    continue_game(
                        res.current_level, 
                        res.lives, 
                        res.lives, 
                        res.score
                    );
                } else {
                    game_state = GAME;
                    stop_midi();
                    start_new_game();
                }
            }
            break;
        case GAME:
            if (update_game() == 1) {
                game_state = GAME_OVER;
            }
            break;
        case GAME_OVER:
            print_at_slow(90, 40, "  GAME OVER  ", 31, 16);
            wait_for_space();
            set_palette(palette);
            show_intro_menu();
            game_state = TITLE;
            break;
        }

        // blit(scroller, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
        vsync();

        if (key[KEY_ESC]) {
            if (game_state == GAME) {
                enum PauseMenuResult pause_result = game_handle_pause();
                switch (pause_result) {
                case PAUSE_RESULT_CONTINUE:
                    break; // resume game
                case PAUSE_RESULT_RESTART:
                    stop_midi();
                    game_state = TITLE; // go back to title/menu
                    show_main_menu();
                    break;
                case PAUSE_RESULT_EXIT:
                    stop_midi();
                    exit_game = 1; // exit game
                    break;
                }
            } else {
                stop_midi();
                exit_game = 1; // exit from title/other states
            }
            do {
            } while (key[KEY_ESC]);
        }

    } while (exit_game == 0);
    // die("Exiting game...");

    // destroy_bitmap(scroller);
    unload_game_memory();
    // unload_datafile(dat_file);
    printf("Enjoyed playing? See you soon!\n");
    clear_keybuf();
    return 0;
}

END_OF_MAIN()
