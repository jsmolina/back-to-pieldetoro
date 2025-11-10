#ifndef GAME_H
#define GAME_H
#include <allegro.h>
#define SCREEN_VIRTUAL 640
#define COCHE_FRAMES 2

extern BITMAP *sp_coche[COCHE_FRAMES];
extern BITMAP *scroller;
extern BITMAP *current_background;

void load_coche_spritesheet(BITMAP ** sp_coche);
void destroy_coche_spritesheet(BITMAP ** sp_coche);
void start_new_game();
inline void update_game();
inline void draw_game();

#endif
