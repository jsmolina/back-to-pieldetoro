#include "enemy.h"
#include "dat_manager.h"
#include "statics.h"

Enemy spawnable_enemies[4];

void init_enemy(Enemy* enemy, int bitmap_id) {
    enemy->image = dat_file[bitmap_id].dat;
    enemy->x = 0;
    enemy->y = 0;
}

void draw_enemy(Enemy* enemy) {
    draw_sprite(screen, enemy->image, enemy->x, enemy->y);
}

void update_enemy(Enemy* enemy) {
    // Update enemy logic here
}

void init_bird() {
    init_enemy(&spawnable_enemies[0], BIRD_SPRITESHEET_BMP);
}

void update_bird() {
    // Update bird logic here
}

void draw_bird() {
    draw_enemy(&spawnable_enemies[0]);
}
