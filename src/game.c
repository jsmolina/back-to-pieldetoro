#include "game.h"
#include "allegro/gfx.h"
#include "dat_manager.h"
#include "statics.h"
#include "tiles.h"
#include "stage1.h"

#define START_STAGE 0
#define GAME_RUN 1
#define PLAYER_FALL 2
#define STAGE_CLEAR 3
#define GAME_OVER 4

int current_level = 0;
short world_state = 0;
int next_x = 0;
BITMAP* sp_coche[COCHE_FRAMES];
BITMAP* scroller;
BITMAP* current_background;

// loads first level and passes it to scroller bitmap
void start_new_game() {
    current_background = load_background(BG0_TMX, SCREEN_VIRTUAL);
    current_level = 1;
    world_state = START_STAGE;
}

void load_coche_spritesheet(BITMAP** sp_coche) {
    BITMAP* coche_spritesheet = dat_file[COCHE_SPRITESHEET_BMP].dat;
    int frame_width = (int)coche_spritesheet->w / COCHE_FRAMES;
    for (int i = 0; i < COCHE_FRAMES; i++) {
        sp_coche[i] = create_sub_bitmap(coche_spritesheet, i * frame_width, 0, frame_width, coche_spritesheet->h);
    }
}

void destroy_coche_spritesheet(BITMAP** sp_coche) {
    BITMAP* coche_spritesheet = dat_file[COCHE_SPRITESHEET_BMP].dat;
    int frame_width = (int)coche_spritesheet->w / COCHE_FRAMES;
    for (int i = 0; i < COCHE_FRAMES; i++) {
        destroy_bitmap(sp_coche[i]);
    }
}

void update_game_run() {
    // https://github.com/yenshan/goggle_jumper_chronicles/blob/main/World.js#L171
    // https://gist.github.com/pofi-gist/6e193e06fe9d53b996aa01013b4b9524#file-2d-mario-style-platformer-L612
    if (next_x < 320) {
        next_x++;
    }

    // scroll the screen
    scroll_screen(next_x, 0);
    // todo move to video memory
}

/**
 */
inline void draw_game() {
    blit(current_background, screen, 10 + next_x, 60, 10 + next_x, 60, 135, 50);
    draw_sprite(screen, sp_coche[(next_x & 1)], 10 + next_x, 60);

    rectfill(scroller, next_x, 201, next_x + 100, 240, makecol(16, 16, 16));
    textprintf_ex(scroller, font, 10 + next_x, 210, makecol(255, 255, 255), makecol(1, 1, 1), "Score: %05d", next_x);
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
