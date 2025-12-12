#include "game.h"
#include "allegro/gfx.h"
#include "object.h"
#include "player.h"
#include "stage1.h"
#include "statics.h"
#include "tiles.h"

#define START_STAGE 0
#define GAME_RUN 1
#define PLAYER_FALL 2
#define STAGE_CLEAR 3
#define GAME_OVER 4

// gravedad
float GRAVITY = 0.8;
int JUMP_STRENGTH = -15;
int PLAYER_SPEED = 5;
int GROUND_Y = 67;
int game_pause = 0;

int current_level = 0;
short world_state = 0;
int next_x = 0;
BITMAP* scroller;
BITMAP* current_background;



// loads first level and passes it to scroller bitmap
void start_new_game() {
    load_coche_spritesheet();
    player_init(10, GROUND_Y);
    current_background = load_background(BG0_TMX, SCREEN_VIRTUAL);
    current_level = 1;
    world_state = START_STAGE;
}



void update_game_run() {
    // https://github.com/yenshan/goggle_jumper_chronicles/blob/main/World.js#L171
    // https://gist.github.com/pofi-gist/6e193e06fe9d53b996aa01013b4b9524#file-2d-mario-style-platformer-L612
    switch (current_level) {
    case 1:
        player_affect_force(0, (player.anime_index & 1) == 0);
        player_update();
        // this.attackEnemy(this.player);
        // this.warp_if_outside(this.player);
        if (player_is_deading()) {
            world_state = PLAYER_FALL;
            return;
        }

        if (wheels_on_harmful_tiles()) {
            player_killed();
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

    if (scroll_x > map_width) {
        scroll_x = map_width;
    }

    switch (current_level) {

    case 1:

        blit(current_background, screen, player.pos.x + next_x - 6, player.pos.y - 15, player.pos.x + next_x - 6, player.pos.y - 15, 141, 63);
        // TODO: player should be responsible of drawing himself!!
        draw_sprite(screen, sp_coche[player.sprite_index], player.pos.x + next_x, player.pos.y);

        rectfill(scroller, next_x, 201, next_x + 320, 240, makecol(25, 25, 25));
        textprintf_ex(scroller, font, 10 + scroll_x, 210, makecol(255, 255, 255), makecol(1, 1, 1), "vy:%d,vx:%d,s:%d,w:%d,m:%d", player.vy, player.vx, player.state, world_state, player.move_count);
        textprintf_ex(scroller, font, 10 + scroll_x, 220, makecol(255, 255, 255), makecol(1, 1, 1), "t1:%d,t2:%d,t3:%d,t4:%d", tiles_at_positions[0],tiles_at_positions[1], tiles_at_positions[2], tiles_at_positions[3]);

        struct collisionType f = foot_area();
        rect(screen, f.x, f.y, f.x + f.w, f.y + f.h, makecol(255, 0, 0));   
        struct collisionType f2 = foot_area2();
        rect(screen, f2.x, f2.y, f2.x + f2.w, f2.y + f2.h, makecol(255, 0, 0));   
        // draw objects, player, enemies

        // scroll the screen
        scroll_screen(scroll_x, 0);
        // todo move to video memory
        break;
    }
}

void start_stage() {
    switch (current_level) {
    case 1:
        level1_intro();
        break;
    }
}

inline void update_game() {
    switch (world_state) {
    case START_STAGE:
        // start title
        start_stage();
        blit(current_background, scroller, 0, 0, 0, 0, SCREEN_VIRTUAL, 201);
        world_state = GAME_RUN;
        break;
    case GAME_RUN:
        update_game_run();
        draw_game();
        break;
    case STAGE_CLEAR:
        // world_state=START_STAGE
        break;
    case PLAYER_FALL:
        // dead fall
        player_affect_force(0, (player.anime_index & 1) == 0);
        player_update();
        if (player.pos.y > 170) {
            world_state = GAME_OVER;            
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