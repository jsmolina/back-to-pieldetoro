#include "enemy.h"
#include "dat_manager.h"
#include "errors.h"
#include "game.h"
#include "helpers.h"
#include "player.h"
#include "statics.h"
#include <allegro.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ESTOP 1
#define EMOVE_LEFT 2
#define EMOVE_RIGHT 3
#define EFALL 4
#define EFALL2 5
#define EDEAD 6
#define EFALL_END 7
#define EDEAD_END 8
#define ECROUCHING 9
#define ETHROWING 10

/*#define BSTOP 1
#define BMOVE_LEFT 2
#define BMOVE_RIGHT 3
#define BDEAD 4
#define BDEAD_END 5*/

#define TOTAL_ENEMY_DATA 3

// Attack threshold: if close enough, throw objects
#define ATTACK_DISTANCE 80
FILE* enemy_log_file;
#define GRAVITY 1
static EnemyData enemy_data[TOTAL_ENEMY_DATA] = {
    { 0, 0, 0, 0, 0, 0, NULL, NULL },
    { 0, 0, 0, 0, 0, 0, NULL, NULL },
    { 0, 0, 0, 0, 0, 0, NULL, NULL }
}; // static data for each enemy type
static Enemy spawnable_enemies[MAX_SPAWNABLE_ENEMIES];
static Enemy active_enemies[MAX_ACTIVE_ENEMIES];

static animeItem joven_animations[11] = {
    { 0, { 0 }, 0, -1 },                                         // NONE
    { 1, { 0 }, 1, 60 },                                         // ESTOP
    { 12, { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 }, 13, 5 }, // EMOVE_LEFT
    { 12, { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 }, 13, 5 }, // EMOVE_RIGHT
    { 1, { 13 }, 1, 1 },                                         // EFALL
    { 1, { 13 }, 1, 1 },                                         // EFALL2
    { 30, { 0 }, 1, 30 },                                        // EDEAD
    { 60, { 14 }, 1, 60 },                                       // EFALL_END
    { 70, { 0 }, 1, 70 },                                        // EDEAD_END
    { 1, { 14 }, 1, 30 },                                        // ECROUCHING
    { 5, { 15 }, 1, 0 },                                         // ETHROWING OBJECT
};

static animeItem bird_animations[11] = {
    { 0, { 0 }, 0, -1 },   // NONE
    { 1, { 0 }, 1, 60 },   // BSTOP
    { 2, { 0, 1 }, 2, 4 }, // BMOVE_LEFT
    { 2, { 0, 1 }, 2, 4 }, // BMOVE_RIGHT
    { 1, { 0 }, 1, 1 },  // EFALL
    { 1, { 0 }, 1, 1 },   // EFALL2
    { 12, { 0 }, 2, 4 },   // BDEAD
    { 60, { 0 }, 1, 60 },  // EFALL_END
    { 2, { 0 }, 2, 4 },    // EDEAD_END
        { 1, { 0 }, 1, 30 },  // ECROUCHING
    { 5, { 0 }, 1, 0 },      // ETHROWING OBJECT
};

static animeItem dog_animations[11] = {
    { 0, { 6 }, 0, -1 },               // NONE
    { 1, { 0 }, 1, 60 },               // BSTOP
    { 6, { 0, 1, 2, 3, 4, 5 }, 6, 4 }, // BMOVE_LEFT
    { 6, { 0, 1, 2, 3, 4, 5 }, 6, 4 }, // BMOVE_RIGHT
    { 1, { 0 }, 1, 1 },                // EFALL
    { 1, { 0 }, 1, 1 },                // EFALL2
    { 12, { 0 }, 2, 4 },               // BDEAD
    { 60, { 0 }, 1, 60 },               // EFALL_END
    { 2, { 0 }, 2, 4 },                // BDEAD_END
    { 1, { 0 }, 1, 30 },           // ECROUCHING
    { 5, { 0 }, 1, 0 },                // ETHROWING OBJECT
};

// called on stage init to load bitmaps and initialize static data for enemy types
void init_enemy(int index, enum EnemyType type, int x, int y, int vx, int screen_spawn_x) {
    if (index < 0 || index >= MAX_SPAWNABLE_ENEMIES)
        return;
    spawnable_enemies[index].type = type;
    spawnable_enemies[index].pos.x = x;
    spawnable_enemies[index].pos.y = y;
    spawnable_enemies[index].screen_spawn_x = screen_spawn_x;
    spawnable_enemies[index].vx = vx;
    spawnable_enemies[index].vy = 0;
    spawnable_enemies[index].active = FALSE;
    spawnable_enemies[index].move_count = 0;
    spawnable_enemies[index].anime_count = 0;
    spawnable_enemies[index].anime_index = 0;
    spawnable_enemies[index].sprite_index = 0;
    spawnable_enemies[index].state = ESTOP;
    spawnable_enemies[index].flip = FALSE;
    spawnable_enemies[index].prev_state = 0;
    spawnable_enemies[index].killed = FALSE;
    spawnable_enemies[index].origin = index;
    spawnable_enemies[index].data = &enemy_data[type];
}

// resets up the pool
void enemy_pool_init() {
    for (int i = 0; i < MAX_ACTIVE_ENEMIES; ++i) {
        active_enemies[i].active = FALSE;
        active_enemies[i].killed = FALSE;
        active_enemies[i].origin = -1;
        active_enemies[i].type = ENEMY_BIRD;
        active_enemies[i].pos.x = 0;
        active_enemies[i].pos.y = 0;
        active_enemies[i].flip = 0;
        active_enemies[i].move_count = 0;
        active_enemies[i].anime_count = 0;
        active_enemies[i].anime_index = 0;
        active_enemies[i].sprite_index = 0;
        active_enemies[i].state = 0;
        active_enemies[i].prev_state = 0;
        active_enemies[i].data = &enemy_data[ENEMY_BIRD];
    }
}

void load_level_enemies(int level_id) {
    enemy_log_file = fopen("enemy_log.txt", "w");

    char* data = dat_file[STAGE_ENEMIES_DEF].dat;
    if (data == NULL) {
        die("cannot load stage_enemies.def");
    }

    // Skip header line
    char* cursor = strchr(data, '\n');
    if (cursor == NULL) {
        die("stage_enemies.def has no header line");
    }
    cursor++; // move past the '\n'

    int enemy_index = 0;
    int current_level, enemy_type, enemy_x, enemy_y, enemy_vx, enemy_spawn_x;

    while (*cursor != '\0' && enemy_index < MAX_SPAWNABLE_ENEMIES) {
        int parsed = sscanf(cursor, "%d,%d,%d,%d,%d,%d",
            &current_level, &enemy_type,
            &enemy_x, &enemy_y, &enemy_vx, &enemy_spawn_x);
        if (parsed == 0) {
            cursor = strchr(cursor, '\n');
            if (cursor == NULL)
                break;
            cursor++;
            continue;
        }
        if (parsed != 6) {
            die("invalid line in stage_enemies.def %d", parsed);
        }

        if (current_level == level_id) {
            // enemy_x + enemy_spawn_x because enemies are defined relative to their spawn point, which allows us to reuse the same enemy definition
            // for multiple spawn points just by changing the spawn_x value
            init_enemy(enemy_index, (enum EnemyType)enemy_type, enemy_x + enemy_spawn_x, enemy_y, enemy_vx, enemy_spawn_x);
            enemy_index++;
        }

        // Advance cursor to next line
        cursor = strchr(cursor, '\n');
        if (cursor == NULL)
            break;
        cursor++;
    }

    enemy_pool_init();
}

static enum EnemyType parse_enemy_type(const char* name) {
    if (strcmp(name, "ENEMY_DOG") == 0)
        return ENEMY_DOG;
    if (strcmp(name, "ENEMY_JOVEN") == 0)
        return ENEMY_JOVEN;
    if (strcmp(name, "ENEMY_BIRD") == 0)
        return ENEMY_BIRD;
    return -1;
}

void load_level_enemies_v2(int level_id) {
    int tmx_id = level_to_dat_id(level_id);
    const char* cursor;
    int enemy_index = 0;

    enemy_log_file = fopen("enemy_log.txt", "w");

    if (tmx_id < 0) {
        die("invalid level id %d for TMX", level_id);
    }

    if (dat_file[tmx_id].dat == NULL) {
        die("cannot load TMX data for level %d", level_id);
    }

    cursor = (const char*)dat_file[tmx_id].dat;

    while ((cursor = strstr(cursor, "<object ")) != NULL) {
        int id;
        char name[64], object_type[8];
        int x, y;

        int matched = sscanf(cursor,
            "<object id=\"%d\" name=\"%63[^\"]\" type=\"%7[^\"]\" x=\"%d\" y=\"%d\"",
            &id, name, object_type, &x, &y);

        if (matched == 5) {
            enum EnemyType enemy_type = parse_enemy_type(name);

            if ((strcmp(object_type, "EL") == 0 || strcmp(object_type, "ER") == 0) && enemy_type != -1) {
                int enemy_spawn_x = (strcmp(object_type, "ER") == 0)
                    ? x - SCREEN_W
                    : x;
                // if x is smaller than screen size, we can end in a negative spawn point, so we clamp it to 0
                if (enemy_spawn_x < 0) {
                    enemy_spawn_x = 0;
                }
                int vx = 0;
                init_enemy(enemy_index, enemy_type, x, y, vx, enemy_spawn_x);
                enemy_index++;
                if (enemy_index >= MAX_SPAWNABLE_ENEMIES) {
                    break;
                }
            }
        }

        cursor++; /* advance past current '<' to find next tag */
    }
    enemy_pool_init();
}

// load bitmaps and initialize static data for enemy types
static void _load_enemy_generic(enum EnemyType type, int frame_count, int bitmap_id) {
    EnemyData* enem = &enemy_data[type];
    BITMAP* enemy_spritesheet = dat_file[bitmap_id].dat;
    int frame_width = (int)enemy_spritesheet->w / frame_count;
    enem->width = frame_width;
    enem->height = enemy_spritesheet->h;
    int offset = 0;
    for (int i = 0; i < frame_count; i++) {
        enem->sprites[i] = create_sub_bitmap(enemy_spritesheet, offset, 0, frame_width, enemy_spritesheet->h);
        offset += frame_width;
    }
    enem->total_frames = frame_count;
    if (type == ENEMY_JOVEN) {
        enem->animations = joven_animations;
    } else if (type == ENEMY_BIRD) {
        enem->animations = bird_animations;
    } else if (type == ENEMY_DOG) {
        enem->animations = dog_animations;
    }
}

// load bitmaps and initialize static data
void load_enemy_spritesheets() {
    _load_enemy_generic(ENEMY_JOVEN, JOVEN_FRAMES, JOVEN_SPRITESHEET_BMP);
    _load_enemy_generic(ENEMY_BIRD, BIRD_FRAMES, BIRD_SPRITESHEET_BMP);
    _load_enemy_generic(ENEMY_DOG, DOG_FRAMES, DOG_SPRITESHEET_BMP);
}

void reset_spawnable_enemies() {
    for (int i = 0; i < MAX_SPAWNABLE_ENEMIES; ++i) {
        spawnable_enemies[i].active = FALSE;
        spawnable_enemies[i].killed = FALSE;
        spawnable_enemies[i].origin = -1;
        spawnable_enemies[i].pos.x = 0;
        spawnable_enemies[i].pos.y = 0;
        spawnable_enemies[i].data = NULL;
        spawnable_enemies[i].type = -1;
        spawnable_enemies[i].vx = 0;
        spawnable_enemies[i].vy = 0;
        spawnable_enemies[i].move_count = 0;
        spawnable_enemies[i].anime_count = 0;
        spawnable_enemies[i].anime_index = 0;
        spawnable_enemies[i].sprite_index = 0;
        spawnable_enemies[i].state = 0;
        spawnable_enemies[i].prev_state = -1;
    }
}

// clean up bitmaps for enemy type
void destroy_enemy_spritesheets() {
    for (int i = 0; i < TOTAL_ENEMY_DATA; i++) {
        EnemyData* enem = &enemy_data[i];
        if (enem->sprites[0] == NULL) {
            continue; // skip if no sprites loaded for this enemy type
        }
        for (int j = 0; j < enem->total_frames; j++) {
            if (enem->sprites[j] != NULL) {
                destroy_bitmap(enem->sprites[j]);
            }
        }
    }
    if (enemy_log_file) {
        fclose(enemy_log_file);
    }
}

static void enemy_change_state(int index, unsigned int state) {
    active_enemies[index].prev_state = active_enemies[index].state;
    active_enemies[index].state = state;
    active_enemies[index].move_count = active_enemies[index].data->animations[state].move_count;
}

static inline unsigned int _enemy_dead_state(enum EnemyType type) {
    if (type == ENEMY_JOVEN) {
        return EDEAD;
    }

    return EDEAD;
}

static inline int _enemy_uses_forces(int index) {
    if (active_enemies[index].type == ENEMY_JOVEN) {
        return TRUE;
    }

    return active_enemies[index].state == EDEAD || active_enemies[index].state == EDEAD_END;
}

static void _enemy_apply_death_impulse(int index) {
    Enemy* enemy = &active_enemies[index];
    int horizontal_impulse = player.flip == TRUE ? -2 : 2;

    enemy->vx = horizontal_impulse;
    enemy->vy = -7;
}

static void _enemy_apply_death_friction(int index) {
    Enemy* enemy = &active_enemies[index];

    if (enemy->vx > 0) {
        enemy->vx--;
    } else if (enemy->vx < 0) {
        enemy->vx++;
    }
}

static unsigned int enemy_count_move(int index, int dx, int dy) {
    if (active_enemies[index].move_count >= 0) {
        active_enemies[index].move_count--;
    }

    if (active_enemies[index].move_count < 0) {
        return FINISHED;
    }
    return NOT_FINISHED;
}

/** @brief Applies a force to the enemy, affecting its position.
 *
 * @param enemy The enemy to which the force will be applied.
 * @param vx The horizontal velocity to apply.
 * @param vy The vertical velocity to apply.
 */
static inline void _enemy_affect_force(int index, int vx, int vy) {
    if (!_enemy_uses_forces(index)) {
        return;
    }

    active_enemies[index].vx += vx;
    active_enemies[index].vy += vy;
}

static inline void _enemy_anime_update(int index) {
    // enemy animations
    Enemy* enemy = &active_enemies[index];
    if ((enemy->type == ENEMY_BIRD || enemy->type == ENEMY_DOG) && enemy->state > EDEAD_END) {
        die("error: bird in invalid state %d", enemy->state);
    }

    EnemyData* enemydata = enemy->data;
    animeItem* anim = &enemydata->animations[enemy->state];
    int* frames = anim->frames;
    int frame_interval = anim->frame_interval;

    if (enemy->anime_count >= frame_interval) {
        active_enemies[index].anime_index++;
        active_enemies[index].anime_count = 0;
    }

    if (anim->length == 0) {
        enemy->anime_index = 0;
    } else if (enemy->anime_index >= anim->length) {
        enemy->anime_index = 0;
    }

    int sprite_index = frames[enemy->anime_index];
    if (enemy->data != NULL && sprite_index >= 0 && sprite_index < enemy->data->total_frames) {
        enemy->sprite_index = sprite_index;
    } else {
        enemy->sprite_index = 0;
    }
    enemy->anime_count++;
}

/**
 * @brief Checks if enemy is over an object
 *
 * @return TRUE if enemy is in top of object
 */
static int enemy_is_on_obj(int index) {
    Enemy* enemy = &active_enemies[index];
    // returns true if sprite is over a walkable tile
    // todo foot_area collision
    if (enemy->pos.y > GROUND_Y) {
        return TRUE;
    }
    // collisionType f1 = enemy_foot_area();
    // return checkOverObj(f1);
    return FALSE;
}

/**
 * @brief Checks enemy speed
 *
 */
static void enemy_check_vy(int index) {
    if (active_enemies[index].state == EDEAD_END) {
        active_enemies[index].vy = 0;
        return;
    }

    if (active_enemies[index].state == EFALL_END) {
        active_enemies[index].vy = 0;
        return;
    }

    if (active_enemies[index].vy > 0) {
        if (enemy_is_on_obj(index)) {
            active_enemies[index].vy = 0;
        }
    }
}

static inline void enemy_check_vx(int index, int scroll_x) {
    if (active_enemies[index].pos.x < (scroll_x - 50)) {
        active_enemies[index].vx = 2;
        // active_enemies[index].state = BMOVE_RIGHT;
        active_enemies[index].flip = FALSE;
        enemy_change_state(index, EMOVE_RIGHT);
    } else if (active_enemies[index].pos.x > (scroll_x + SCREEN_W + 50)) {
        active_enemies[index].vx = -2;
        // active_enemies[index].state = BMOVE_LEFT;
        active_enemies[index].flip = TRUE;
        enemy_change_state(index, EMOVE_LEFT);
    }
}

static inline void _enemy_update_position(int index, int scroll_x) {
    // TODO: apply enemy-specific logic and forces here, for now just apply gravity and simple movement
    enemy_check_vx(index, scroll_x);
    enemy_check_vy(index);

    if (active_enemies[index].active == FALSE)
        return;

    active_enemies[index].pos.x = active_enemies[index].pos.x + active_enemies[index].vx;
    active_enemies[index].pos.y = active_enemies[index].pos.y + active_enemies[index].vy;
}

/** @brief Joven enemy AI for Shinobi-like movement. Moves toward player and attacks when close.
 *
 * @param index The index of the active enemy
 */
void joven_action_stop(int index) {
    // Only process if this is a valid active joven enemy
    if (index < 0 || index >= MAX_ACTIVE_ENEMIES)
        return;

    // Calculate distance to player
    int enemy_x = active_enemies[index].pos.x;
    int player_x = player.pos.x;

    // Move toward player
    if (player_x < enemy_x) {
        // Player is to the left, move left
        enemy_change_state(index, EMOVE_LEFT);
        // active_enemies[index].state = EMOVE_LEFT;
        active_enemies[index].vx = -2; // Move left
        active_enemies[index].flip = TRUE;
    } else if (player_x > enemy_x) {
        // Player is to the right, move right
        enemy_change_state(index, EMOVE_RIGHT);
        // active_enemies[index].state = EMOVE_RIGHT;
        active_enemies[index].vx = 2; // Move right
        active_enemies[index].flip = FALSE;
    }
    // Otherwise maintain current position (player directly above/below)
}

void bird_action_stop(int index) {
    // Only process if this is a valid active bird enemy
    if (index < 0 || index >= MAX_ACTIVE_ENEMIES)
        return;

    // Calculate distance to player
    int enemy_x = active_enemies[index].pos.x;
    int player_x = player.pos.x;

    // Move toward player
    if (player_x < enemy_x) {
        // Player is to the left, move left
        enemy_change_state(index, EMOVE_LEFT);
        active_enemies[index].vx = -1; // Move left
        active_enemies[index].flip = TRUE;
    } else if (player_x > enemy_x) {
        // Player is to the right, move right
        enemy_change_state(index, EMOVE_RIGHT);
        active_enemies[index].vx = 1; // Move right
        active_enemies[index].flip = FALSE;
    }
    // Otherwise maintain current position (player directly above/below)
}

void enemy_action_dead(int index) {
    if (enemy_count_move(index, 0, 0) == FINISHED) {
        active_enemies[index].active = FALSE;
        active_enemies[index].killed = TRUE;
        active_enemies[index].pos.x = 0;
        active_enemies[index].pos.y = 0;
        active_enemies[index].vx = 0;
        active_enemies[index].vy = 0;
        int origin_index = active_enemies[index].origin;
        if (origin_index >= 0 && origin_index < MAX_SPAWNABLE_ENEMIES) {
            spawnable_enemies[origin_index].killed = TRUE; // mark static as killed so it won't respawn
        }

        active_enemies[index].origin = -1;
        return;
    }

    //_enemy_apply_death_friction(index);
}

void enemy_action_dead_end(int index) {
}

static inline void _update_specific_enemy(int index, int scroll_x) {
    _enemy_affect_force(index, 0, (active_enemies[index].anime_index & 1) == 0);
    _enemy_update_position(index, scroll_x);

    switch (active_enemies[index].state) {
        case EMOVE_LEFT:
            // joven_action_move_left();
            break;
        case EMOVE_RIGHT:
            // joven_action_move_right();
            break;
        case EFALL:
        case EFALL2:
            // joven_action_fall();
            break;
        case ESTOP:
            joven_action_stop(index);
            break;
        case EDEAD:
            // joven_action_dead();
            enemy_action_dead(index);
            break;
        case EFALL_END:
            enemy_action_dead_end(index);
            // joven_action_fall_end();
            break;
        case ECROUCHING:
            // enemy_action_crouch(index);
            break;
        case ETHROWING:
            // joven_action_throw();
            break;
    }
    _enemy_anime_update(index);
}

void enemy_update(int scroll_x) {
    // Update enemy logic here
    for (int i = 0; i < MAX_ACTIVE_ENEMIES; ++i) {
        if (active_enemies[i].active == TRUE) {
            _update_specific_enemy(i, scroll_x);
        }
    }
}

static inline int _find_free_active_slot() {
    for (int i = 0; i < MAX_ACTIVE_ENEMIES; ++i) {
        if (!active_enemies[i].active)
            return i;
    }
    return -1;
}

static int is_spawned(int spawn_index) {
    for (int i = 0; i < MAX_ACTIVE_ENEMIES; ++i) {
        if (active_enemies[i].active && active_enemies[i].origin == spawn_index)
            return 1;
    }
    return 0;
}

static inline void _spawn_from_static(int spawn_index) {
    if (spawn_index < 0 || spawn_index >= MAX_SPAWNABLE_ENEMIES) {
        return;
    }

    int slot = _find_free_active_slot();
    if (slot < 0) {
        return; // no room
    }
    active_enemies[slot] = spawnable_enemies[spawn_index];
    active_enemies[slot].active = TRUE;
    active_enemies[slot].origin = spawn_index;
    /* Mark the static spawn as active so it won't be spawned repeatedly */
    spawnable_enemies[spawn_index].active = TRUE;
}

void enemy_get_all_aabb(collisionType* enemies) {
    for (int i = 0; i < MAX_ACTIVE_ENEMIES; i++) {
        if (active_enemies[i].active == TRUE) {
            enemies[i].x = active_enemies[i].pos.x;
            enemies[i].y = active_enemies[i].pos.y;
            EnemyData* data = active_enemies[i].data;
            if (data) {
                enemies[i].w = data->width;
                enemies[i].h = data->height;
            } else {
                enemies[i].w = 0;
                enemies[i].h = 0;
            }
        } else {
            enemies[i].x = 0;
            enemies[i].y = 0;
            enemies[i].w = 0;
            enemies[i].h = 0;
        }
    }
}

void enemy_on_hit(int enemy_id) {
    if (enemy_id < 0 || enemy_id >= MAX_ACTIVE_ENEMIES) {
        return;
    }
    // active_enemies[enemy_id].killed = TRUE;
    // active_enemies[enemy_id].active = FALSE;
    enemy_change_state(enemy_id, _enemy_dead_state(active_enemies[enemy_id].type));
    _enemy_apply_death_impulse(enemy_id);
}

void enemy_pool_update(int camera_x) {
    /*int spawn_margin = 32;
    int despawn_left = 128;*/

    // Spawn: check static list
    for (int i = 0; i < MAX_SPAWNABLE_ENEMIES; ++i) {
        if (spawnable_enemies[i].type == -1 || spawnable_enemies[i].data == NULL) {
            continue; // skip uninitialized spawn points
        }

        if (spawnable_enemies[i].active == TRUE || spawnable_enemies[i].killed == TRUE) {
            continue;
        }

        if (spawnable_enemies[i].screen_spawn_x == camera_x) {
            if (enemy_log_file) {
                fprintf(enemy_log_file, "* Spawn enemy index %d of type %d\n", i, spawnable_enemies[i].type);
            }
            _spawn_from_static(i);
        }
    }

    // Update active enemies
    // TODO: enemies update and attack player

    /*for (int i = 0; i < MAX_ACTIVE_ENEMIES; ++i) {
        Enemy* e = &active_enemies[i];
        if (!e->active)
            continue;

        // If far left off-screen, freeze logic
        if (e->pos.x < camera_x - despawn_left) {
            continue;
        }

        // apply gravity and simple update
        //enemy_affect_force(e, 0, (int)GRAVITY);
        update_enemy(e);

        // warp_if_outside-like: if below threshold mark dead and free slot
        if (e->pos.y > 200) {
            // mark static as killed if falled from a hole, so it won't respawn
            if (e->origin >= 0 && e->origin < MAX_SPAWNABLE_ENEMIES) {
                spawnable_enemies[e->origin].killed = TRUE;
            }
            e->active = FALSE;
            e->origin = -1;
        }
    }*/
}

void draw_enemies(int scroll_x) {
    for (int i = 0; i < MAX_ACTIVE_ENEMIES; ++i) {
        if (active_enemies[i].active == FALSE)
            continue;
        if (active_enemies[i].data == NULL) {
            die("Active enemy with no data at pos %d!", i);
        }
        // e is the enemy instance with position, animation state, etc.
        Enemy* e = &active_enemies[i];
        // data is static for the enemy type, so we can get dimensions and sprites from it
        EnemyData* data = e->data;

        // Bounds check: ensure sprite_index is valid
        if (e->sprite_index >= 0 && e->sprite_index < data->total_frames && data->sprites[e->sprite_index] != NULL) {
            if (e->flip == TRUE) {
                draw_sprite_h_flip(
                    screen,
                    data->sprites[e->sprite_index],
                    e->pos.x - scroll_x,
                    e->pos.y);
            } else {
                draw_sprite(
                    screen,
                    data->sprites[e->sprite_index],
                    e->pos.x - scroll_x,
                    e->pos.y);
            }
        }
    }
}
