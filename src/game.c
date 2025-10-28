#include "game.h"
#include "allegro/gfx.h"
#include "statics.h"
#include "dat_manager.h"

#define START_STAGE 0 
#define GAME_RUN 1
#define PLAYER_FALL 2
#define STAGE_CLEAR 3
#define GAME_OVER 4

short world_state = 0;

void load_coche_spritesheet(BITMAP ** sp_coche) {
    BITMAP *coche_spritesheet = dat_file[COCHE_SPRITESHEET_BMP].dat;
    int frame_width = (int) coche_spritesheet->w / COCHE_FRAMES;
    for (int i = 0; i < COCHE_FRAMES; i++) {
        sp_coche[i] = create_sub_bitmap(coche_spritesheet, i * frame_width, 0, frame_width, coche_spritesheet->h);        
    }
}

void destroy_coche_spritesheet(BITMAP ** sp_coche) {
    BITMAP *coche_spritesheet = dat_file[COCHE_SPRITESHEET_BMP].dat;
    int frame_width = (int) coche_spritesheet->w / COCHE_FRAMES;
    for (int i = 0; i < COCHE_FRAMES; i++) {
        destroy_bitmap(sp_coche[i]);
    }
}

void update_game_run() {
    //https://github.com/yenshan/goggle_jumper_chronicles/blob/main/World.js#L171
    //https://gist.github.com/pofi-gist/6e193e06fe9d53b996aa01013b4b9524#file-2d-mario-style-platformer-L612
}

void update() {
    switch(world_state) {
        case START_STAGE:
            // start title
            break;
        case GAME_RUN:
            update_game_run();
            break;
        case STAGE_CLEAR:
            //world_state=START_STAGE
            break;
        case PLAYER_FALL:
            //this.player.affectForce(0, GRAVITY);
            //this.player.update();
            //if (this.player.y > this.h * MAP_ELEM_SIZE) {
            //    this.state = State.GAME_OVER;
            //}
            break;
        case GAME_OVER:
            break;
        }
}

/**
 */
void draw() {

}
