#ifndef GAME_H
#define GAME_H
#include <allegro.h>
#define SCREEN_VIRTUAL 640


extern BITMAP *scroller;
extern BITMAP *current_background;
extern int game_pause;

void start_new_game();
inline void update_game();
inline void draw_game();
void unload_game_memory();

#endif
