#include "game.h"
#include "allegro/color.h"
#include "allegro/gfx.h"
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

// gravedad
float GRAVITY = 0.8;
int JUMP_STRENGTH = -15;
int PLAYER_SPEED = 5;
int GROUND_Y = 67;
int game_pause = 0;

int current_level = 0;
short world_state = 0;
int next_x = 0;
// BITMAP* scroller;
BITMAP* current_background;
PALETTE pal_flash;
//int levels_bg[] = {BG0_TMX, BG1_TMX};

// loads first level and passes it to scroller bitmap
void start_new_game() {
    load_coche_spritesheet();
    player_init(10, GROUND_Y);
    current_background = load_background(BG0_TMX, SCREEN_VIRTUAL);
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
    switch (current_level) {
    case 1:
        if (key[KEY_8_PAD]) {
            player.state = 1;
            player.pos.y--;
        } else if (key[KEY_2_PAD]) {
            player.pos.y++;
        }
        player_affect_force(0, (player.anime_index & 1) == 0);
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

        /**
        this.create_enemy();
        for (let e of this.enemy_list) {
            e.affectForce(0, GRAVITY);
            e.update();
            if (e.offensive()) {
                this.player.attack(e)
            }
            this.warp_if_outside2(e);
            if (e.y > this.h * MAP_ELEM_SIZE) {
                dead_enemies.push(e);
                this.num_of_dead_enemies++;
            }
        }
         */
        if (next_x < 320) {
            // next_x++;
        }
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
    int scroll_x = player.pos.x - SCREEN_W / 3;
    if (scroll_x < 0)
        scroll_x = 0;

    if (scroll_x > map_pixel_width - SCREEN_W) {
        scroll_x = map_pixel_width - SCREEN_W;
    }
    //int t1 = get_tile_at_position(player.pos.x + player.width, player.pos.y + player.height);
    if (key[KEY_F1]) {
        world_state = STAGE_CLEAR;
        while (key[KEY_F1]); // wait key release
    }

    switch (current_level) {

        case 1:

        // blit(current_background, screen, player.pos.x + next_x - 6, player.pos.y - 15, player.pos.x + next_x - 6, player.pos.y - 15, 141, 63);
        //  TODO: player should be responsible of drawing himself!!
        // draw_sprite(screen, sp_coche[player.sprite_index], player.pos.x + next_x, player.pos.y);

        // rectfill(scroller, next_x, 201, next_x + 320, 240, makecol(25, 25, 25));
        // textprintf_ex(scroller, font, 10 , 10, makecol(255, 255, 255), makecol(1, 1, 1), "vy:%d,vx:%d,s:%d,w:%d,m:%d", player.vy, player.vx, player.state, world_state, player.move_count);

        // textprintf_ex(scroller, font, 10 + scroll_x, 220, makecol(255, 255, 255), makecol(1, 1, 1), "t1:%d,t2:%d,t3:%d,t4:%d", tiles_at_positions[0],tiles_at_positions[1], tiles_at_positions[2], tiles_at_positions[3]);
        blit(current_background, screen, scroll_x, 0, 0, 0, SCREEN_W, 180);
        draw_sprite(screen, sp_coche[player.sprite_index], player.pos.x - scroll_x, player.pos.y);
        // rectfill(screen, 0, 201, SCREEN_W, SCREEN_H, 32);
        //struct collisionType f = foot_area();
        //rect(screen, f.x - scroll_x, f.y, f.x + f.w - scroll_x, f.y + f.h, makecol(255, 0, 0));
        //struct collisionType f2 = foot_area2();
        //rect(screen, f2.x - scroll_x, f2.y, f2.x + f2.w - scroll_x, f2.y + f2.h, makecol(255, 0, 0));
        // draw objects, player, enemies
        break;
        case 2:
            blit(current_background, screen, scroll_x, 0, 0, 0, SCREEN_W, 180);
        break;
    }
}

void start_stage() {
    switch (current_level) {
    case 1:
        level1_intro();
        break;
    case 2:
        level2_intro();
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
            //player_init(10, GROUND_Y);
            current_background = load_background(BG1_TMX, SCREEN_VIRTUAL);
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
            player_init(10, GROUND_Y);
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
            player_affect_force(0, (player.anime_index & 1) == 0);
            player_update();
            if (player.pos.y > GROUND_Y + player.height) {
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