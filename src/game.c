#include "game.h"
#include "book.h"
#include "coin.h"
#include "dat_manager.h"
#include "door.h"
#include "enemy.h"
#include "helpers.h"
#include "object.h"
#include "pause.h"
#include "player.h"
#include "room.h"
#include "intros.h"
#include "statics.h"
#include "tiles.h"
#include <allegro.h>
#include <stdio.h>

#define START_STAGE 0
#define GAME_RUN 1
#define PLAYER_FALL 2
#define STAGE_CLEAR 3
#define GAME_OVER 4
#define RESTART_STAGE 5
#define WBACK_IN_TIME 6
#define LEVEL1_GROUND_Y 67
#define LEVEL2_GROUND_Y 118
#define MAX_MARTIN_VX 1
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
int next_x = 0;
// BITMAP* scroller;
BITMAP* current_background;
PALETTE pal_flash;
// int levels_bg[] = {BG0_TMX, BG1_TMX};

static int hud_last_level = -1;
static int hud_last_energy = -1;
static int hud_last_lives = -1;
static int hud_last_coins = -1;
static int coins_collected = 0;
static int game_money = 0;
static volatile int stage_tick_count = 0;
int stage_elapsed_minutes = 0;
int stage_elapsed_seconds = 0;

static void _stage_tick() { stage_tick_count++; }
END_OF_FUNCTION(_stage_tick)

void game_on_coin_collected() {
    coins_collected++;
    game_money += COIN_MONEY_VALUE;
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

void print_year(int n1, int n2, int n3, int n4) {
    draw_sprite(screen, numbers_sprites[n1], 42, 182);
    draw_sprite(screen, numbers_sprites[n2], 62, 182);
    draw_sprite(screen, numbers_sprites[n3], 82, 182);
    draw_sprite(screen, numbers_sprites[n4], 102, 182);
}

void lifebar() {
    int force_full_redraw = (hud_last_level != current_level);
    if (force_full_redraw) {
        blit(dat_file[LIFEBAR_BMP].dat, screen, 0, 0, 0, 170, 320, 30); // draw full HUD background
        if (current_level == 1) {
            print_year(1, 9, 8, 4);
        } else if (current_level == 2) {
            print_year(2, 0, 2, 5);
        }
    }

    int e = player.energy;
    if (e < 0)
        e = 0;
    if (e > HUD_MAX_ENERGY)
        e = HUD_MAX_ENERGY;

    if (force_full_redraw || hud_last_energy != e) {
        // restore energy slot background before redrawing current energy state
        blit(dat_file[LIFEBAR_BMP].dat, screen, 145, 0, 145, 170, 29, 30);
        blit(dat_file[LIFEBAR_MARTIN_BMP].dat, screen, 0, 0, 145, 170, 29, 30);
        int bruno_h = ((HUD_MAX_ENERGY - e) << 2) + (HUD_MAX_ENERGY - e);
        if (bruno_h > 0) {
            blit(dat_file[LIFEBAR_BRUNO_BMP].dat, screen, 0, 0, 145, 170, 29, bruno_h);
        }
        hud_last_energy = e;
    }

    if (force_full_redraw || hud_last_lives != player.lives) {
        // clear lives area with HUD background, then draw current amount
        blit(dat_file[LIFEBAR_BMP].dat, screen, 190, 0, 190, 170, 130, 30);
        int x = 182;
        for (int i = 0; i < player.lives; i++) {
            draw_sprite(screen, dat_file[HEAD_BMP].dat, x, 185);
            x += 20;
        }
        hud_last_lives = player.lives;
    }

    int money = game_get_money();
    if (force_full_redraw || hud_last_coins != money) {
        blit(dat_file[LIFEBAR_BMP].dat, screen, 255, 5, 255, 175, 65, 15);
        printf_at_simple(250, 185, 41, 43, "%6d", money);
        hud_last_coins = money;
    }

    hud_last_level = current_level;
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
        char buf[32];
        snprintf(buf, sizeof(buf), "FINISHED!! %02dm %02ds", stage_elapsed_minutes, stage_elapsed_seconds);
        print_at_slow(90, 40, buf, 31, 16);
        wait_for_space();
    }

    current_level++;
    int dat_id = level_to_dat_id(current_level);
    if (current_level > 0 && current_level < 4) {
        current_background = load_background(dat_id);
        world_state = START_STAGE;
    } else {
        world_state = GAME_OVER;
    }
  
}

// loads first level and passes it to scroller bitmap
void start_new_game() {
    stop_midi();
    // current_background = load_background(BG0_TMX);
    current_level = 0;
    coins_collected = 0;
    game_money = 0;
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
    scroll_x = player.pos.x - SCREEN_W / 3;
    if (scroll_x < 0)
        scroll_x = 0;

    if (scroll_x > map_pixel_width - SCREEN_W) {
        scroll_x = map_pixel_width - SCREEN_W;
    }

    switch (current_level) {
    case 1:
        player_update();
        FlowEventType flow_event = player_consume_flow_event();
        if (flow_event.type == PLAYER_FLOW_RESTART_STAGE) {
            world_state = RESTART_STAGE;
            return;
        }
        if (flow_event.type == PLAYER_FLOW_GAME_OVER) {
            world_state = GAME_OVER;
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
        player_update();

        flow_event = player_consume_flow_event();
        if (flow_event.type == PLAYER_FLOW_RESTART_STAGE) {
            world_state = RESTART_STAGE;
            return;
        } else if (flow_event.type == PLAYER_FLOW_GAME_OVER) {
            world_state = GAME_OVER;
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
        }
        if (player_is_deading()) {
            world_state = PLAYER_FALL;
            return;
        }
        enemy_update(scroll_x);
        enemy_pool_update(scroll_x);
        throwable_update(scroll_x);

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

inline void draw_game() {
    // int t1 = get_tile_at_position(player.pos.x + player.width, player.pos.y + player.height);
    if (key[KEY_F1]) {
        world_state = STAGE_CLEAR;
        while (key[KEY_F1])
            ; // wait key release
    }
    collisionType f2;

    switch (current_level) {

    case 1:
        // textprintf_ex(scroller, font, 10 + scroll_x, 220, makecol(255, 255, 255), makecol(1, 1, 1), "t1:%d,t2:%d,t3:%d,t4:%d", tiles_at_positions[0],tiles_at_positions[1], tiles_at_positions[2], tiles_at_positions[3]);
        blit(current_background, screen, scroll_x, 0, 0, 0, SCREEN_W, 170);
        /*if (player.data && player.sprite_index >= 0 && player.sprite_index < player.data->total_frames && player.data->sprites[player.sprite_index] != NULL) {
            draw_sprite(screen, player.data->sprites[player.sprite_index], player.pos.x - scroll_x, player.pos.y);
        } else {
            textprintf_ex(screen, font, 10, 10, makecol(255, 0, 0), -1, "DEBUG: invalid sprite idx %d", player.sprite_index);
        }*/
        player_draw(scroll_x);
        lifebar();
        // draw objects, player, enemies
        break;
    default:
        /* Draw background and player sprite first. Only call player_foot_area
           if player.data is valid to avoid dereferencing NULL and SIGSEGV. */
        blit(current_background, screen, scroll_x, 0, 0, 0, SCREEN_W, 170);
        player_draw(scroll_x);
        draw_enemies(scroll_x);
        draw_throwable(scroll_x);
        draw_coins(scroll_x);
        collision_check_throwable_vs_enemy();
        collision_check_enemy_vs_player(scroll_x);
        collision_check_player_vs_coins();
        lifebar();
        // f2 = player_foot_area();
        // rect(screen, f2.x - scroll_x, f2.y, f2.x + f2.w - scroll_x, f2.y + f2.h, makecol(255, 0, 0));
        // rectfill(screen, 10, 190, 290, 200, 16);
        textprintf_ex(screen, font, 10, 10, makecol(255, 0, 0), -1, "x:%d, y:%d, vy:%d, s:%d", player.pos.x, player.pos.y, player.vy, scroll_x);

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
        GROUND_Y = LEVEL1_GROUND_Y;
        player_new_game();
        player_init(10, GROUND_Y, current_level, MAX_CAR_VX, -8);
        break;
    default:
        show_intro(current_level);
        GROUND_Y = LEVEL2_GROUND_Y;
        player_init(20, GROUND_Y, current_level, MAX_MARTIN_VX, -4);
        // initializes level enemies
        // init_enemy(0, ENEMY_BIRD, 310, GROUND_Y - 5, -1, 93);
        // init_enemy(1, ENEMY_JOVEN, 20, GROUND_Y, 0, 160);
        // enemy_pool_init();
        // load_level_enemies(2);
        load_level_enemies_v2(current_level);
        reset_coins();
        load_level_coins(current_level);
        reset_doors();
        load_level_doors(current_level);
        break;
    }
}

int flash_count = 0;
int flash_state = 0;

void palete_flash() {
    flash_count++;
    if ((flash_count % 4) == 0) {
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

inline void update_game() {
    switch (world_state) {
    case START_STAGE:
        // start title
        start_stage();
        // blit(current_background, scroller, 0, 0, 0, 0, SCREEN_VIRTUAL, 201);
        world_state = GAME_RUN;
        break;
    case RESTART_STAGE:
        player_init(10, GROUND_Y, current_level, player.max_vx, player.jump_vy);
        enemy_pool_init();
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
        advance_stage();
        break;
    case PLAYER_FALL:
        // player_update();
        //  check lives first: DEAD_END makes player_is_deading() return FALSE
        if (player.lives <= 0) {
            world_state = GAME_OVER;
        } else {
            world_state = RESTART_STAGE;
        }
        // draw_game();
        break;
    case GAME_OVER:
        break;
    }
}

enum PauseMenuResult game_handle_pause(void) {
    game_pause = TRUE;
    enum PauseMenuOption pause_choice = show_pause_menu();
    game_pause = FALSE;

    switch (pause_choice) {
    case PAUSE_CONTINUE:
        return PAUSE_RESULT_CONTINUE;
    case PAUSE_MENU:
        return PAUSE_RESULT_RESTART;
    case PAUSE_EXIT_TO_DOS:
        return PAUSE_RESULT_EXIT;
    default:
        return PAUSE_RESULT_CONTINUE;
    }
}

void unload_game_memory() {
    /*destroy_coche_spritesheet();
    destroy_enemy_spritesheets();*/
}