#include "enemy.h"
#include "allegro/base.h"
#include "dat_manager.h"
#include "errors.h"
#include "statics.h"

#define GRAVITY 1
EnemyData enemy_data[2]; // static data for each enemy type
Enemy spawnable_enemies[MAX_SPAWNABLE_ENEMIES];
Enemy active_enemies[MAX_ACTIVE_ENEMIES];

// called on stage init to load bitmaps and initialize static data for enemy types
void init_enemy(int index, enum EnemyType type, int x, int y) {
    if (index < 0 || index >= MAX_SPAWNABLE_ENEMIES)
        return;
    spawnable_enemies[index].type = type;
    spawnable_enemies[index].pos.x = x;
    spawnable_enemies[index].pos.y = y;
    spawnable_enemies[index].active = FALSE;
    spawnable_enemies[index].flip = 0;
    spawnable_enemies[index].move_count = 0;
    spawnable_enemies[index].anime_count = 0;
    spawnable_enemies[index].anime_index = 0;
    spawnable_enemies[index].sprite_index = 0;
    spawnable_enemies[index].state = 0;
    spawnable_enemies[index].prev_state = 0;
    spawnable_enemies[index].killed = FALSE;
    spawnable_enemies[index].origin = index;
    spawnable_enemies[index].data = &enemy_data[type];
}

// load bitmaps and initialize static data for enemy types
void load_enemy_generic(enum EnemyType type, int frame_count, int bitmap_id) {
    EnemyData* enem = &enemy_data[type];
    BITMAP* enemy_spritesheet = dat_file[bitmap_id].dat;
    int frame_width = (int)enemy_spritesheet->w / frame_count;
    enem->width = frame_width;
    enem->height = enemy_spritesheet->h;
    for (int i = 0; i < frame_count; i++) {
        enem->sprites[i] = create_sub_bitmap(enemy_spritesheet, i * frame_width, 0, frame_width, enemy_spritesheet->h);
    }
    enem->total_frames = frame_count;
}

// load bitmaps and initialize static data
void load_enemy_spritesheets() {
    load_enemy_generic(ENEMY_JOVEN, JOVEN_FRAMES, JOVEN_SPRITESHEET_BMP);
    load_enemy_generic(ENEMY_BIRD, BIRD_FRAMES, BIRD_SPRITESHEET_BMP);
}

void reset_spawnable_enemies() {
    for (int i = 0; i < MAX_SPAWNABLE_ENEMIES; ++i) {
        spawnable_enemies[i].active = FALSE;
        spawnable_enemies[i].killed = FALSE;
        spawnable_enemies[i].origin = -1;
        spawnable_enemies[i].pos.x = 0;
        spawnable_enemies[i].pos.y = 0;
        spawnable_enemies[i].data = NULL;
    }
}

// clean up bitmaps for enemy type
void destroy_enemy_spritesheets() {
    for (int i = 0; i < 2; i++) {
        EnemyData* enem = &enemy_data[i];
        for (int j = 0; j < enem->total_frames; j++) {
            destroy_bitmap(enem->sprites[j]);
        }
    }
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

inline int get_enemy_bitmap_id(enum EnemyType type) {
    switch (type) {
    case ENEMY_BIRD:
        return BIRD_SPRITESHEET_BMP;
    case ENEMY_JOVEN:
        return JOVEN_SPRITESHEET_BMP;
    default:
        return BIRD_SPRITESHEET_BMP;
    }
}

void update_enemy(Enemy* enemy) {
    // Update enemy logic here
}

inline void enemy_affect_force(Enemy* enemy, int vx, int vy) {
    // Simplified: store velocities in x/y as movement deltas would be needed
    // For now, use x/y as positions and apply vertical force only
    enemy->pos.y += vy;
    enemy->pos.x += vx;
}

static int find_free_active_slot() {
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

static void spawn_from_static(int spawn_index) {
    if (spawn_index < 0 || spawn_index >= MAX_SPAWNABLE_ENEMIES) {
        return;
    }

    int slot = find_free_active_slot();
    if (slot < 0)
        return; // no room
    active_enemies[slot] = spawnable_enemies[spawn_index];
    active_enemies[slot].active = TRUE;
    active_enemies[slot].origin = spawn_index;
    /* Mark the static spawn as active so it won't be spawned repeatedly */
    spawnable_enemies[spawn_index].active = TRUE;
}

void enemy_pool_update(int camera_x) {
    int spawn_margin = 32;
    int despawn_left = 128;

    // Spawn: check static list
    for (int i = 0; i < MAX_SPAWNABLE_ENEMIES; ++i) {
        if (spawnable_enemies[i].active == TRUE || spawnable_enemies[i].killed == TRUE)
            continue;
        if (spawnable_enemies[i].data == NULL)
            continue;

        int ex = spawnable_enemies[i].pos.x;
        if (ex < camera_x + SCREEN_W + spawn_margin && ex > camera_x - spawn_margin) {
            spawn_from_static(i);
        }
    }

    // Update active enemies
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
            draw_sprite(
                screen,
                data->sprites[e->sprite_index],
                e->pos.x - scroll_x,
                e->pos.y
            );
        }

       
    }
}
