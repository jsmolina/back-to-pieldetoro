#include "game.h"
#include "allegro/digi.h"
#include "allegro/midi.h"
#include "book.h"
#include "boss.h"
#include "coin.h"
#include "dat_manager.h"
#include "door.h"
#include "enemy.h"
#include "enemy_throw.h"
#include "helpers.h"
#include "intros.h"
#include "object.h"
#include "passcode.h"
#include "pause.h"
#include "piece.h"
#include "platform.h"
#include "player.h"
#include "room.h"
#include "sinking.h"
#include "statics.h"
#include "tiles.h"
#include "tnt.h"
#include <allegro.h>
#include <stdio.h>

#define START_STAGE 0
#define GAME_RUN 1
#define PLAYER_FALL 2
#define STAGE_CLEAR 3
#define GAME_OVER 4
#define RESTART_STAGE 5
#define WBACK_IN_TIME 6
#define CONTINUE 7
#define GAME_PASSED 8
#define LEVEL1_GROUND_Y 67
#define LEVEL2_GROUND_Y 118
#define MAX_MARTIN_VX 2
#define MAX_CAR_VX 5
#define HUD_MAX_ENERGY 6

// gravedad
float GRAVITY = 0.8;
int JUMP_STRENGTH = -15;
int PLAYER_SPEED = 5;
int GROUND_Y = LEVEL1_GROUND_Y;
int game_pause = FALSE;
int scroll_x;

int current_level = 0;
short world_state = 0;
int megahit_mode = 0;
int next_x = 0;
int score = 0;
// BITMAP* scroller;
BITMAP* current_background;
BITMAP* continue_bg;
PALETTE pal_flash;
// int levels_bg[] = {BG0_TMX, BG1_TMX};

static int hud_last_level = -1;
static int hud_last_energy = -1;
static int hud_last_lives = -1;
static int hud_last_coins = -1;
static int hud_last_books = -1;
static int hud_last_pieces_collected = -1;
static int hud_last_pieces_total = -1;
static int coins_collected = 0;
static int game_money = 0;
static volatile int stage_tick_count = 0;
int stage_elapsed_minutes = 0;
int stage_elapsed_seconds = 0;
static int flash_count = 0;
static int flash_state = 0;
static int pal_slowdown_cycle = 0;
static int pal_cycle_step = 0;

static void _stage_tick() { stage_tick_count++; }
END_OF_FUNCTION(_stage_tick)

/**
 * @brief cascade effect rotating palette indexes 31, 11, 9 and 3
 */
static void cascade_palette() {
    PALETTE current_pal;
    static const int target_idx[4] = { 30, 11, 9, 3 };
    static const int cycle_map[3][4] = {
        { 3, 30, 11, 9 },
        { 9, 3, 30, 11 },
        { 11, 9, 3, 30 }
    };
    pal_slowdown_cycle++;
    if (pal_slowdown_cycle >= 10) {
        int i;
        pal_slowdown_cycle = 0;
        get_palette(current_pal);

        for (i = 0; i < 4; i++) {
            current_pal[target_idx[i]] = palette[cycle_map[pal_cycle_step][i]];
        }

        pal_cycle_step++;
        if (pal_cycle_step >= 3) {
            pal_cycle_step = 0;
        }

        set_palette(current_pal);
    }
}

static void sea_sparkle() {
    PALETTE current_pal;
    pal_slowdown_cycle++;
    if (pal_slowdown_cycle >= 20) {
        pal_slowdown_cycle = 0;
        RGB tmp;
        get_palette(current_pal);
        tmp = current_pal[54];
        current_pal[54] = current_pal[33];
        current_pal[33] = tmp;
        set_palette(current_pal);
    }
}

static void reset_palette_to_vga_original() {
    set_palette(default_palette);
}

void game_on_coin_collected() {
    coins_collected++;
    game_money += COIN_MONEY_VALUE;
    score += 5;
}

int game_get_coins_collected() {
    return coins_collected;
}

int game_get_money() {
    return game_money;
}

int game_try_spend_money(int amount) {
    if (amount <= 0) {
        return TRUE;
    }

    if (game_money < amount) {
        return FALSE;
    }

    game_money -= amount;
    return TRUE;
}

void lifebar() {
    int force_full_redraw = (hud_last_level != current_level);
    BITMAP* hud_screen = screen;
    #ifdef _WIN32
        hud_screen = current_screen;
        force_full_redraw = TRUE;
    #endif

    if (force_full_redraw) {
        blit(dat_file[LIFEBAR_BMP].dat, hud_screen, 0, 0, 0, 170, 320, 30); // draw full HUD background

        int year = 0001;
        if (current_level == 1) {
            year = 2026;
        } else if (current_level == 2) {
            year = 2039;
        } else if (current_level == 3) {
            year = 2039;
        } else if (current_level == LEVEL_MOUNTAIN) {
            year = 1954;
        } else if (current_level == LEVEL_CITY ||
                   current_level == LEVEL_AUTOVOICE ||
                   current_level == 7) {
            year = 1997;
        }

        printf_at_simple(hud_screen, 26, 185, 46, -1, "%d", year);
    }

    int e = player.energy;
    if (e < 0)
        e = 0;
    if (e > HUD_MAX_ENERGY)
        e = HUD_MAX_ENERGY;

    if (force_full_redraw || hud_last_energy != e) {
        // restore energy slot background before redrawing current energy state
        blit(dat_file[LIFEBAR_BMP].dat, hud_screen,
             145, 0, 145, 170, 29, 30);

        blit(dat_file[LIFEBAR_MARTIN_BMP].dat, hud_screen,
             0, 0, 145, 170, 29, 30);

        int bruno_h = ((HUD_MAX_ENERGY - e) << 2) +
                      (HUD_MAX_ENERGY - e);

        if (bruno_h > 0) {
            blit(dat_file[LIFEBAR_BRUNO_BMP].dat, hud_screen,
                 0, 0, 145, 170, 29, bruno_h);
        }

        hud_last_energy = e;
    }

    if (force_full_redraw || hud_last_lives != player.lives) {
        // clear lives area with HUD background, then draw current amount
        blit(dat_file[LIFEBAR_BMP].dat, hud_screen,
             190, 0, 190, 170, 130, 30);

        int x = 182;
        for (int i = 0; i < player.lives; i++) {
            draw_sprite(hud_screen, dat_file[HEAD_BMP].dat, x, 185);
            x += 15;
        }

        hud_last_lives = player.lives;
    }

    int money = game_get_money();

    if (force_full_redraw || hud_last_coins != money) {
        blit(dat_file[LIFEBAR_BMP].dat, hud_screen,
             255, 5, 255, 175, 65, 15);

        printf_at_simple(hud_screen, 255, 185, 41, -1, "%6d", money);

        hud_last_coins = money;
    }

    int books = get_book_count();

    if (force_full_redraw || hud_last_books != books) {
        /*
        rect(hud_screen, 83, 185, 132, 190, 19);
        blit(dat_file[LIFEBAR_THROWABLE_BMP].dat,
             hud_screen, 0, 0, 83, 185, 65, 5);
        */

        int width = 65;

        for (int i = books; i < DEFAULT_STOCK; i++)
            width -= 5;

        if (width >= 0) {
            rectfill(hud_screen, 83, 185, 132, 190, 19);

            blit(dat_file[LIFEBAR_THROWABLE_BMP].dat,
                 hud_screen, 0, 0, 83, 185, width, 5);
        }

        hud_last_books = books;
    }

    hud_last_level = current_level;

    #ifdef _WIN32
        int hud_height = screen->h * 30 / 200;
        stretch_blit(current_screen, screen, 0, 170, 320, 30,
                    0, screen->h - hud_height, screen->w, hud_height);
    #endif
}

/**
* Loads current background
*/
static void load_by_stage() {
    int dat_id = level_to_dat_id(current_level);
    if (current_level > 0) {
        current_background = load_background(dat_id);
        world_state = START_STAGE;
    } else {
        world_state = CONTINUE;
    }
}

static inline void start_music() {
    stop_midi();

    switch(current_level) {
        case 1:
            play_midi(dat_file[LEVEL1_FUNKY_MID].dat, TRUE);
        break;
        case 2:
            play_midi(dat_file[LEVEL2_POLICIACO_MID].dat, TRUE);
        break;
        case 3:
            play_midi(dat_file[LEVEL3_MID].dat, TRUE);
        break;
        case 4:
            play_midi(dat_file[LEVEL4_DETECTIVE_MID].dat, TRUE);
        break;
        case 5:
            play_midi(dat_file[LEVEL5_MID].dat, TRUE);
        break;
        case 6:
            play_midi(dat_file[LEVEL6_MID].dat, TRUE);
        break;
        case 7:
            play_midi(dat_file[FINAL_MID].dat, TRUE);
        break;
        case 8:
            
        break;
    }
}

void advance_stage() {
    remove_int(_stage_tick);
    int secs = stage_tick_count;
    stage_tick_count = 0;
    stage_elapsed_minutes = 0;
    while (secs >= 60) {
        secs -= 60;
        stage_elapsed_minutes++;
    }
    stage_elapsed_seconds = secs;
    if (current_level != 0) {
        char buf[40];
        play_sample(dat_file[FINISHED_WAV].dat, 255, 127, 1000, 0);
        snprintf(buf, sizeof(buf), "  FINISHED!! %02dm %02ds  ", stage_elapsed_minutes, stage_elapsed_seconds);
        print_at_slow(90, 40, buf, 31, 16);
        wait_for_space();
    }

    current_level++;
    load_by_stage();
    reset_palette_to_vga_original();
}



void continue_game(int cl, int liv, int mon, int sco, int books) {
    current_level = cl;
    player.lives = liv;
    game_money = mon;
    player_energy_up();
    set_book_count(books);
    load_by_stage();
}

// loads first level and passes it to scroller bitmap
void start_new_game() {
    stop_midi();
    // current_background = load_background(BG0_TMX);
    current_level = 0;
    pal_cycle_step = 0;
    pal_slowdown_cycle = 0;
    coins_collected = 0;
    game_money = 0;
    score = 0;
    room_reset_purchased_items();
    advance_stage();
    world_state = START_STAGE;

    for (int i = 0; i < 255; i++) {
        pal_flash[i].r = palette[i].r;
        pal_flash[i].g = palette[i].g;
        pal_flash[i].b = palette[i].b;
    }
    pal_flash[78].r = 0;
    pal_flash[78].g = 229;
    pal_flash[78].b = 255;

    pal_flash[44].r = 243;
    pal_flash[44].g = 156;
    pal_flash[44].b = 0;
}

void update_game_run() {
    // https://github.com/yenshan/goggle_jumper_chronicles/blob/main/World.js#L171
    // https://gist.github.com/pofi-gist/6e193e06fe9d53b996aa01013b4b9524#file-2d-mario-style-platformer-L612
    if (current_level != 1) {
        scroll_x = player.pos.x - 160;
    } else {
        scroll_x = player.pos.x - 120;
    }
    if (scroll_x < 0)
        scroll_x = 0;

    if (player.boss_mode == TRUE || scroll_x > map_pixel_width - SCREEN_W) {
        scroll_x = map_pixel_width - SCREEN_W;
    }

    switch (current_level) {
    case 1:
        player_update(current_level);
        FlowEventType flow_event = player_consume_flow_event();
        if (flow_event.type == PLAYER_FLOW_RESTART_STAGE) {
            world_state = RESTART_STAGE;
            return;
        }
        if (flow_event.type == PLAYER_FLOW_GAME_OVER) {
            world_state = CONTINUE;
            return;
        }
        // this.attackEnemy(this.player);
        // this.warp_if_outside(this.player);
        if (player_is_deading()) {
            world_state = PLAYER_FALL;
            return;
        }
        int tile_below_type = wheels_on_tiles();
        if (tile_below_type == HARMFUL) {
            player_on_hit();
        } else if (tile_below_type == BACK_IN_TIME) {
            // back in time tile
            world_state = WBACK_IN_TIME;
            return;
        }
        // this.world.checkHitObj(this.foot_area(0, dy));

        if (next_x < 320) {
            // next_x++;
        }
        break;
    default:
        platform_update(scroll_x);
        if (current_level == LEVEL_CITY) {
            sinking_update(scroll_x);
        }
        if (current_level == LEVEL_BOSS) {
            boss_update(scroll_x);
        }
        player_update(current_level);

        flow_event = player_consume_flow_event();
        if (flow_event.type == PLAYER_FLOW_RESTART_STAGE) {
            world_state = RESTART_STAGE;
            return;
        } else if (flow_event.type == PLAYER_FLOW_GAME_OVER) {
            world_state = CONTINUE;
            return;
        } else if (flow_event.type == PLAYER_ENTER_ROOM) {
            do {
            } while (key[KEY_SPACE]);
            int room_choice = enter_room(flow_event.data, current_level);
            (void)room_choice;
            hud_last_level = -1; // force HUD redraw on room exit
            return;
        } else if (flow_event.type == PLAYER_ADVANCE_STAGE) {
            world_state = STAGE_CLEAR;
            return;
        } else if (flow_event.type == PLAYER_BEAT_BOSS) {
            world_state = GAME_PASSED;
            return;
        }
        
        if (current_level == LEVEL_MOUNTAIN && player.boss_mode == TRUE && boss_hits >= BOSS_HITS_TO_KILL) {
            world_state = STAGE_CLEAR;
        }

        if (player_is_deading()) {
            world_state = PLAYER_FALL;
            return;
        }
        enemy_pool_update(scroll_x);
        enemy_update(scroll_x);
        throwable_update(scroll_x);
        enemy_throwable_update(scroll_x);        

        break;
    }
}

/*
// Repaint only dirty tiles
void repaint_dirty_tiles() {
    for (int y = 0; y < MAX_VERT_TILES; ++y) {
        for (int x = 0; x < curr_tiles_width; ++x) {
            if (dirty_tiles[y][x]) {
                blit(
                    current_background,
                    screen,
                    x * TILES_SIZE,
                    y * TILES_SIZE,
                    x * TILES_SIZE,
                    y * TILES_SIZE,
                    TILES_SIZE,
                    TILES_SIZE);
                dirty_tiles[y][x] = 0; // clear after repaint
            }
        }
    }
}*/
void init_per_stages() {
    if (current_level == 1) {
        player_init(10, GROUND_Y, current_level, MAX_CAR_VX, -8);
    } else if (current_level == 3) {
        player_init(20, GROUND_Y, current_level, MAX_MARTIN_VX, -14);
    } else {
        player_init(20, GROUND_Y, current_level, MAX_MARTIN_VX, -8);
    }
}

inline void draw_game() {
    // int t1 = get_tile_at_position(player.pos.x + player.width, player.pos.y + player.height);
    if (megahit_mode == 1 && key[KEY_F1]) {
        world_state = STAGE_CLEAR;
        while (key[KEY_F1])
            ; // wait key release
    } else if (megahit_mode == 1 && key[KEY_F2]) {
        all_collected();
        player.pos.x = almanac_tile_x -1;
    } else if (megahit_mode == 1 && key[KEY_F3]) {
        player.energy = 6;
    } 
    collisionType f2;
    int current_door_id;

    switch (current_level) {

    case 1:
        sea_sparkle();
        blit(current_background, current_screen, scroll_x, 0, 0, 0, SCREEN_W, 170);

        player_draw(scroll_x);
        lifebar();
        // draw objects, player, enemies
        #ifdef _WIN32
            stretch_blit(current_screen, screen,
                0, 0, 320, 170,
                0, 0, WIN32_WIDTH, WIN32_HEIGHT - WIN32_HUD_START);
        #else 
            blit(current_screen, screen, 0, 0, 0, 0, SCREEN_W, 170);
        #endif
        break;

    default:
        if (current_level == 3 || current_level == 2) {
            sea_sparkle();
        } else if (current_level == LEVEL_MOUNTAIN) {
            cascade_palette();
        }
        /* Draw background and player sprite first. Only call player_foot_area
           if player.data is valid to avoid dereferencing NULL and SIGSEGV. */
        blit(current_background, current_screen, scroll_x, 0, 0, 0, SCREEN_W, 170);
        draw_door_getin(scroll_x);
        if (current_level == LEVEL_CITY) {
            sinking_draw(scroll_x);
        }
        if (current_level == LEVEL_AUTOVOICE) {
            tnt_draw(scroll_x);
        }
        if (current_level == LEVEL_BOSS) {
            boss_draw(scroll_x);
        }
        player_draw(scroll_x);
        draw_enemies(scroll_x);
        draw_throwable(scroll_x);
        draw_enemy_throwable(scroll_x);
        draw_coins(scroll_x);
        draw_platforms(scroll_x);

        if (current_level == LEVEL_MOUNTAIN) {
            draw_pieces(scroll_x);
        }
   
        collision_check_throwable_vs_enemy();
        collision_check_enemy_throwable_vs_player();
        collision_check_enemy_vs_player(scroll_x);
        collision_check_player_vs_coins();
        if (current_level == LEVEL_MOUNTAIN) {
            collision_check_player_vs_pieces();
        }
        if (current_level == LEVEL_AUTOVOICE) {
            collision_check_player_vs_tnt();
        }
        if (current_level == LEVEL_BOSS) {
            collision_check_player_vs_boss();
        }
        lifebar();
        if (player.boss_mode == TRUE) {
            // TODO reduce 50, based on bruno enemy life
            int width = 50;  
            for (int i = 0; i < boss_hits; i++) {
                width -= 5;
            }
            blit(dat_file[LIFEBAR_ENEMY_BMP].dat, current_screen, 0, 0, 120, 2, width, 5);
        }
        // f2 = player_foot_area();
        // rect(screen, f2.x - scroll_x, f2.y, f2.x + f2.w - scroll_x, f2.y + f2.h, makecol(255, 0, 0));
        // rectfill(screen, 10, 190, 290, 200, 16);
        // textprintf_ex(current_screen, font, 10, 10, makecol(255, 0, 0), -1, "l:%d, y:%d, vy:%d, s:%d", current_level, player.pos.y, player.vy, player.state);
        //textprintf_ex(current_screen, font, 0, 10, 31, 16, "%d %d", current_level, player.pos.y);
        #ifdef _WIN32
            stretch_blit(current_screen, screen,
                0, 0, 320, 170,
                0, 0, WIN32_WIDTH, WIN32_HEIGHT - WIN32_HUD_START);
        #else 
             blit(current_screen, screen, 0, 0, 0, 0, SCREEN_W, 170);
        #endif

        break;
    }
}

void start_stage() {
    static int _timer_locked = FALSE;
    if (!_timer_locked) {
        LOCK_VARIABLE(stage_tick_count);
        LOCK_FUNCTION(_stage_tick);
        _timer_locked = TRUE;
    }
    remove_int(_stage_tick);
    stage_tick_count = 0;
    install_int_ex(_stage_tick, BPS_TO_TIMER(1));
    switch (current_level) {
    case 1:
        level1_intro();
        start_music();
        GROUND_Y = LEVEL1_GROUND_Y;
        player_new_game();
        break;
    default:
        show_intro(current_level);
        start_music();
        GROUND_Y = LEVEL2_GROUND_Y;
        enemy_spawn_init();
        load_level_enemies_v2(current_level);
        load_level_platforms(current_level);
        reset_sinking();
        if (current_level == LEVEL_CITY) {
            load_level_sinking(current_level);
        }
        reset_coins();
        load_level_coins(current_level);
        if (current_level == LEVEL_MOUNTAIN) {
            reset_pieces();
            load_level_pieces(current_level);
        } else {
            reset_pieces();
        }
        reset_doors();
        load_level_doors(current_level);
        reset_tnt();
        if (current_level == LEVEL_AUTOVOICE) {
            load_level_tnt(current_level);
        }
        reset_boss();
        if (current_level == LEVEL_BOSS) {
            spawn_boss();
        }
        break;
    }
    init_per_stages();
}

void palete_flash() {
    flash_count++;
    pal_slowdown_cycle++;
    if (pal_slowdown_cycle >= 4) {
        pal_slowdown_cycle = 0;
        flash_state = !flash_state;
        if (flash_state) {
            set_palette(pal_flash);
        } else {
            set_palette(palette);
        }
    }
    if (flash_count > 200) {
        flash_count = 0;
        world_state = STAGE_CLEAR;
        set_palette(palette);
    }
}

static inline int skip_fli_on_space2(void) {
    return key[KEY_SPACE] ? 1 : 0;
}

inline int update_game() {
    switch (world_state) {
    case START_STAGE:
        // start title
        start_stage();
        // blit(current_background, scroller, 0, 0, 0, 0, SCREEN_VIRTUAL, 201);
        world_state = GAME_RUN;
        break;
    case RESTART_STAGE:
        init_per_stages();

        // enemy_pool_init();
        enemy_spawn_init();
        enemy_pool_init();
        if (current_level != 1) {
            reset_coins();
            load_level_coins(current_level);
        }
        if (current_level == LEVEL_MOUNTAIN) {
            reset_pieces();
            load_level_pieces(current_level);
        }
        if (current_level == LEVEL_AUTOVOICE) {
            reset_tnt();
            load_level_tnt(current_level);
        }
        if (current_level == LEVEL_BOSS) {
            spawn_boss();
        }
        // blit(current_background, scroller, 0, 0, 0, 0, SCREEN_VIRTUAL, 201);
        hud_last_level = -1; // force HUD redraw on stage restart
        world_state = GAME_RUN;
        break;
    case GAME_RUN:
        update_game_run();
        draw_game();
        break;
    case WBACK_IN_TIME:
        palete_flash();
        player.pos.x++;
        draw_game();
        break;
    case STAGE_CLEAR:
        score += 5;
        advance_stage();
        break;
    case PLAYER_FALL:
        // player_update();
        //  check lives first: DEAD_END makes player_is_deading() return FALSE
        if (player.lives <= 0) {
            world_state = CONTINUE;
        } else {
            world_state = RESTART_STAGE;
        }
        // draw_game();
        break;
    case CONTINUE:
        stop_midi();
        play_midi(dat_file[SAMBA_MID].dat, 1);
        continue_bg = load_shop_bg(CONTINUE_TMX);
        blit(continue_bg, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
        while (!key[KEY_Y] && !key[KEY_N]) {
            vsync();
        }
        if (key[KEY_Y]) {
            player_new_game();
            world_state = RESTART_STAGE;
            // a continue makes loose score
            score = 0;
        } else if (key[KEY_N]) {
            world_state = GAME_OVER;
        }
        destroy_bitmap(continue_bg);
        continue_bg = NULL;
        break;
    case GAME_OVER:
        return 1;
        stop_midi();
        break;
    case GAME_PASSED:
        stop_midi();
        score += 100;
        play_midi(dat_file[WON_MID].dat, TRUE);
        print_at_slow(90, 40, "  YOU WON!  ", 31, 16);
        wait_for_space();
        blit(dat_file[CARS_BMP].dat, screen, 0, 0, 48, 0, 223, 177);
        print_at_slow(2, 30, game_text(TXT_WON1), 31, 16);
        wait_for_space();
        blit(dat_file[DIARIO_BMP].dat, screen, 0, 0, 0, 0, 320, 200);
        wait_for_space();
        print_at_slow(2, 30, game_text(TXT_WON2), 31, 16);
        wait_for_space();
        play_upscaled_memory_fli(dat_file[GIRL_FLI].dat, screen, 1, skip_fli_on_space2);
        return 1;
        break;
    }
    return 0;
}

enum PauseMenuResult game_handle_pause(void) {
    game_pause = TRUE;
    char passcode[15];
    generate_pass(
        current_level,
        player.lives,
        game_money,
        score,
        get_book_count(),
        passcode);
    enum PauseMenuOption pause_choice = show_pause_menu(passcode);
    game_pause = FALSE;

    switch (pause_choice) {
    case PCONTINUE:
        return PAUSE_RESULT_CONTINUE;
    case PMENU:
        return PAUSE_RESULT_RESTART;
    case PEXIT_TO_DOS:
        return PAUSE_RESULT_EXIT;
    default:
        return PAUSE_RESULT_CONTINUE;
    }
}

void unload_game_memory() {
    /*destroy_coche_spritesheet();
    destroy_enemy_spritesheets();*/
}
