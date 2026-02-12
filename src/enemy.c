#include "enemy.h"
#include "dat_manager.h"
#include "statics.h"

Enemy spawnable_enemies[4] = {ENEMY_BIRD, 0, 0, FALSE,
                              ENEMY_BIRD, 0, 0, FALSE,
                              ENEMY_BIRD, 0, 0, FALSE,
                              ENEMY_BIRD, 0, 0, FALSE
                            };

void init_enemy(int index, enum EnemyType type) {
    spawnable_enemies[index].type = type;
    spawnable_enemies[index].x = 0;
    spawnable_enemies[index].y = 0;
    spawnable_enemies[index].active = TRUE;
}

inline int get_enemy_bitmap_id(enum EnemyType type) {
    switch (type) {
        case ENEMY_BIRD:
            return BIRD_SPRITESHEET_BMP;
        default:
            return BIRD_SPRITESHEET_BMP;
    }
}

void update_enemy(Enemy* enemy) {
    // Update enemy logic here
}

void draw_enemies() {
    for (int i = 0; i < 4; i++) {
        if (spawnable_enemies[i].active) {
            draw_sprite(
                screen, 
                dat_file[get_enemy_bitmap_id(spawnable_enemies[i].type)].dat, 
                spawnable_enemies[i].x, 
                spawnable_enemies[i].y
            );
        }
    }
}
