#include "game.h"
#include <allegro.h>
#include <string.h>
#include "book.h"
#include "enemy.h"
#include "object.h"
#include "player.h"
#include "stage1.h"
#include "stage2.h"
#include "statics.h"
#include "tiles.h"


#define START_STAGE 0
#define GAME_RUN 1
#define PLAYER_FALL 2
#define STAGE_CLEAR 3
#define GAME_OVER 4
#define RESTART_STAGE 5
#define WBACK_IN_TIME 6
#define LEVEL1_GROUND_Y 67
#define LEVEL2_GROUND_Y 120
#define MAX_MARTIN_VX 1
#define MAX_CAR_VX 5

// gravedad
float GRAVITY = 0.8;
int JUMP_STRENGTH = -15;
int PLAYER_SPEED = 5;
int GROUND_Y = LEVEL1_GROUND_Y;
int game_pause = 0;
int scroll_x;

int current_level = 0;
short world_state = 0;
int next_x = 0;
// BITMAP* scroller;
BITMAP* current_background;
PALETTE pal_flash;
// int levels_bg[] = {BG0_TMX, BG1_TMX};

// loads first level and passes it to scroller bitmap
void start_new_game() {
    stop_midi();
    current_background = load_background(BG0_TMX);
    current_level = 1;
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
        // this.attackEnemy(this.player);
        // this.warp_if_outside(this.player);
        if (player_is_deading()) {
            world_state = PLAYER_FALL;
            return;
        }
        int tile_below_type = wheels_on_tiles();
        if (tile_below_type == HARMFUL) {
            player_killed();
            return;
        } else if (tile_below_type == BACK_IN_TIME) {
            // back in time tile
            world_state = WBACK_IN_TIME;
            return;
        }
        // attack_enemy() check if player collides enemies
        // TODO: check if player is on harming tiles
        // this.world.checkHitObj(this.foot_area(0, dy));
        // player_attack();

        if (next_x < 320) {
            // next_x++;
        }
        break;
    case 2:
        if (key[KEY_8_PAD]) {
            player.state = 1;
            player.pos.y--;
        } else if (key[KEY_2_PAD]) {
            player.pos.y++;
        } else if (key[KEY_4_PAD]) {
            player.pos.x--;
        } else if (key[KEY_6_PAD]) {
            player.pos.x++;
        }
        player_update();

        enemy_pool_update(scroll_x);
        throwable_update(scroll_x);

        break;
    }
}

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
}

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
        blit(current_background, screen, scroll_x, 0, 0, 0, SCREEN_W, 180);
        if (player.data && player.sprite_index >= 0 && player.sprite_index < player.data->total_frames && player.data->sprites[player.sprite_index] != NULL) {
            draw_sprite(screen, player.data->sprites[player.sprite_index], player.pos.x - scroll_x, player.pos.y);
        } else {
            textprintf_ex(screen, font, 10, 10, makecol(255, 0, 0), -1, "DEBUG: invalid sprite idx %d", player.sprite_index);
        }
        // draw objects, player, enemies
        break;
    case 2:
        /* Draw background and player sprite first. Only call player_foot_area
           if player.data is valid to avoid dereferencing NULL and SIGSEGV. */
        blit(current_background, screen, scroll_x, 0, 0, 0, SCREEN_W, 180);
        if (player.data && player.sprite_index >= 0 && player.sprite_index < player.data->total_frames && player.data->sprites[player.sprite_index] != NULL) {
            if (player.flip == TRUE) {
                draw_sprite_h_flip(screen, player.data->sprites[player.sprite_index], player.pos.x - scroll_x, player.pos.y);
            } else {
                draw_sprite(screen, player.data->sprites[player.sprite_index], player.pos.x - scroll_x, player.pos.y);
            }
        } else {
            textprintf_ex(screen, font, 10, 10, makecol(255, 0, 0), -1, "DEBUG: invalid sprite idx %d", player.sprite_index);
        }
        draw_enemies(scroll_x);
        draw_throwable(scroll_x);
        collision_check_throwable_vs_enemy();
        // f2 = player_foot_area();
        // rect(screen, f2.x - scroll_x, f2.y, f2.x + f2.w - scroll_x, f2.y + f2.h, makecol(255, 0, 0));
        rectfill(screen, 10, 190, 290, 200, 16);
        textprintf_ex(screen, font, 10, 190, makecol(255, 0, 0), -1, "s:%d, y:%d, vy:%d, vx:%d", player.state, player.pos.y, player.vy, player.vx);

        break;
    }
}

void start_stage() {
    switch (current_level) {
    case 1:
        level1_intro();
        GROUND_Y = LEVEL1_GROUND_Y;
        player_init(10, GROUND_Y, current_level, MAX_CAR_VX);
        break;
    case 2:
        level2_intro();
        GROUND_Y = LEVEL2_GROUND_Y;
        player_init(20, GROUND_Y, current_level, MAX_MARTIN_VX);
        // initializes level enemies
        init_enemy(0, ENEMY_BIRD, 400, GROUND_Y-5, TRUE);
        init_enemy(1, ENEMY_JOVEN, 500, GROUND_Y, TRUE);
        enemy_pool_init();
        break;
    }
}

void advance_stage() {
    current_level++;
    switch (current_level) {
    case 2:
        if (current_background) {
            destroy_bitmap(current_background);
        }
        // player_init(10, GROUND_Y);
        current_background = load_background(BG1_TMX);
        world_state = START_STAGE;
        break;
    default:
        world_state = GAME_OVER;
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
        player_init(10, GROUND_Y, current_level, player.max_vx);
        // blit(current_background, scroller, 0, 0, 0, 0, SCREEN_VIRTUAL, 201);
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
        // dead fall
        player_update();
        if (player.pos.y > GROUND_Y + player.data->height) {
            player.lives--;
            world_state = RESTART_STAGE;
        }
        draw_game();
        break;
    case GAME_OVER:
        break;
    }
}

void unload_game_memory() {
    destroy_coche_spritesheet();
}