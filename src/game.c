#include "game.h"
#include "allegro/gfx.h"
#include "dat_manager.h"
#include "statics.h"
#include "tiles.h"
#include "stage1.h"
#include "player.h"

#define START_STAGE 0
#define GAME_RUN 1
#define PLAYER_FALL 2
#define STAGE_CLEAR 3
#define GAME_OVER 4


// debería cambiar por nivel
float GRAVITY = 0.8;
int JUMP_STRENGTH = -15;
int PLAYER_SPEED = 5;
int GROUND_Y = 70;
int game_pause = 0;


int current_level = 0;
short world_state = 0;
int next_x = 0;
BITMAP* scroller;
BITMAP* current_background;


int checkHitObj() {
    // will check if player laterally collides PLAYER => [OBJ]
 
    return FALSE;
}

// loads first level and passes it to scroller bitmap
void start_new_game() {
    load_coche_spritesheet();

    player.checkHitObj = checkHitObj;
    player_init(10, 60);
    current_background = load_background(BG0_TMX, SCREEN_VIRTUAL);
    current_level = 1;
    world_state = START_STAGE;
}




void update_game_run() {
    // https://github.com/yenshan/goggle_jumper_chronicles/blob/main/World.js#L171
    // https://gist.github.com/pofi-gist/6e193e06fe9d53b996aa01013b4b9524#file-2d-mario-style-platformer-L612
    switch(current_level) {
        case 1:
            player_affect_force(0, GRAVITY);
            player_update();
             if (player_is_deading()) {
                world_state = PLAYER_FALL;
                return;
            }
            // attack_enemy() check if player collides enemies
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
                //next_x++;
            }
        break;
    }
    
}

/**
 */
inline void draw_game() {
    switch(current_level) {
        case 1:

            blit(current_background, screen, player.pos.x + next_x -2, player.pos.y, player.pos.x + next_x-2, player.pos.y, 137, 50);
            // TODO: player should be responsible of drawing himself!!
            draw_sprite(screen, sp_coche[player.sprite_index], player.pos.x + next_x, player.pos.y);

            rectfill(scroller, next_x, 201, next_x + 200, 240, makecol(16, 16, 16));
            textprintf_ex(scroller, font, 10 + next_x, 210, makecol(255, 255, 255), makecol(1, 1, 1), "vx:%d,x:%d,%d", player.vx, player.pos.x, player.state);

            // draw objects, player, enemies

            // scroll the screen
            //scroll_screen(next_x, 0);
            // todo move to video memory
        break;
    }
}

void start_stage() {
    switch(current_level) {
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
            // this.player.affectForce(0, GRAVITY);
            // this.player.update();
            // if (this.player.y > this.h * MAP_ELEM_SIZE) {
            //     this.state = State.GAME_OVER;
            // }
            break;
        case GAME_OVER:
            break;
    }
}

void unload_game_memory() {
    destroy_coche_spritesheet();
}