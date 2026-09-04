#include "boss.h"
#include "dat_manager.h"
#include "errors.h"
#include "helpers.h"
#include "statics.h"
#include <allegro.h>

// boss state machine
#define BOSS_INACTIVE 0
#define BOSS_WALK_IN 1     // walking right to the stop point, frames 0..12
#define BOSS_ATTACK 2      // stopped, facing left, firing the wave attack
#define BOSS_DEAD 3
#define BOSS_ATTACK_LINE 4 // straight horizontal line attack at y=122
#define BOSS_TIRED 5       // vulnerable rest between attacks (no firing)

// projectile kinds (share wave_sprite)
#define WAVE_KIND_WAVE 0 // follows the oscillating wave curve
#define WAVE_KIND_LINE 1 // straight horizontal line at LINE_Y

#define BOSS_TIRED_FRAMES 180 // 120 ~1.7s or vulnerable window between attacks (~3.4s)

// placement and stats
#define BOSS_START_X 215
#define BOSS_START_Y 104
#define BOSS_STOP_X 240
#define BOSS_FRAME_COUNT 28 // frames 0..25
#define BOSS_MAX_HP 20
#define BOSS_HURT_COOLDOWN 10 // ticks of invulnerability after a hit
#define BOSS_WALK_SPEED 1
#define BOSS_WALK_INTERVAL 4 // ticks per walk-in animation frame
#define BOSS_ATTACK_INTERVAL 6

// phase boundaries and tuning
#define BOSS_PHASE2_HP 10
#define BOSS_ATTACK_DELAY_P1 200
#define BOSS_ATTACK_DELAY_P2 220

// wave projectiles: moved much slower so the player has time to dodge
#define WAVE_SPEED_P1 3
#define WAVE_SPEED_P2 4
#define WAVE_Y_TOP 106
#define WAVE_Y_BOTTOM 160
#define WAVE_TRAIL 5      // instances fired per burst (the visible trail)
#define WAVE_TRAIL_GAP 12 // x spacing between trail instances

// straight-line attack: fixed y, faster than the wave
#define LINE_Y 122
#define LINE_SPEED_P1 10
#define LINE_SPEED_P2 12

// phase-1 attack loop: frames 12,11,10,9,8. phase-2: frames 13..25.
//static const int boss_attack_p1_frames[5] = { 12, 11, 10, 9, 8 };
#define BOSS_P1_FIRST_FRAME 8
#define BOSS_P1_LAST_FRAME 12
#define BOSS_P2_FIRST_FRAME 13
#define BOSS_P2_LAST_FRAME 25

typedef struct {
    int active;
    int x;
    int kind; // WAVE_KIND_WAVE or WAVE_KIND_LINE
} Wave;

typedef struct {
    int x;
    int y;
    int hp;
    short state;
    int flip; // TRUE when rendered facing left
    int hurt_cooldown;
    int attack_timer;
    int anime_count;
    int anime_index;
    int frame;       // current sprite frame index
    int fired;       // TRUE once the current attack has fired its burst
    int next_attack; // which attack to run after the current BOSS_TIRED
} Boss;

static Boss boss;
static Wave waves[BOSS_MAX_WAVES];
static BITMAP* boss_sprites[BOSS_FRAME_COUNT];
static BITMAP* wave_sprite = NULL;
static int boss_w = 0;
static int boss_h = 0;

void load_boss_spritesheet() {
    BITMAP* sheet = dat_file[PELOBROCOLI_SPRITESHEET_BMP].dat;
    if (!sheet) {
        die("cannot load PELOBROCOLI_SPRITESHEET_BMP from datafile");
    }
    boss_w = 30;
    boss_h = sheet->h;
    int offset = 0;
    for (int i = 0; i < BOSS_FRAME_COUNT; i++) {
        boss_sprites[i] = create_sub_bitmap(sheet, offset, 0, boss_w, boss_h);
        offset += boss_w;
    }
    wave_sprite = dat_file[WAVE_BULLET_BMP].dat;
    if (!wave_sprite) {
        die("cannot load WAVE_BULLET_BMP from datafile");
    }
}

void reset_boss() {
    boss.x = BOSS_START_X;
    boss.y = BOSS_START_Y;
    boss.hp = BOSS_MAX_HP;
    boss.state = BOSS_INACTIVE;
    boss.flip = FALSE;
    boss.hurt_cooldown = 0;
    boss.attack_timer = 0;
    boss.anime_count = 0;
    boss.anime_index = 0;
    boss.frame = 0;
    boss.fired = FALSE;
    boss.next_attack = BOSS_ATTACK; // first attack is the wave
    for (int i = 0; i < BOSS_MAX_WAVES; i++) {
        waves[i].active = FALSE;
        waves[i].x = 0;
        waves[i].kind = WAVE_KIND_WAVE;
    }
}

void spawn_boss() {
    reset_boss();
    boss.state = BOSS_WALK_IN;
}

int boss_is_active() {
    return boss.state != BOSS_INACTIVE && boss.state != BOSS_DEAD;
}

static inline int boss_is_enraged() {
    return boss.hp <= BOSS_PHASE2_HP;
}

// vertical position of the wave curve at world x: triangle between the top and
// bottom bounds (no float math). period is twice the span so it ramps up/down.
static int wave_y(int x) {
    int span = WAVE_Y_BOTTOM - WAVE_Y_TOP; // 54
    int period = span << 1;                // 108
    int p = x % period;
    if (p < 0) {
        p += period;
    }
    int off = (p < span) ? p : (period - p);
    return WAVE_Y_TOP + off;
}

// fires one burst of the given kind: WAVE_TRAIL instances staggered in x so
// they form a continuous trail (a curve for WAVE, a straight line for LINE)
static void boss_fire(int kind) {
    int spawned = 0;
    int gap = 0;
    for (int i = 0; i < BOSS_MAX_WAVES && spawned < WAVE_TRAIL; i++) {
        if (waves[i].active) {
            continue;
        }
        waves[i].active = TRUE;
        waves[i].kind = kind;
        waves[i].x = boss.x + gap; // trail extends to the right (behind the head)
        gap += WAVE_TRAIL_GAP;
        spawned++;
    }
}

// y of a projectile: the oscillating curve for WAVE, a fixed line for LINE
static int wave_pos_y(const Wave* w) {
    return (w->kind == WAVE_KIND_LINE) ? LINE_Y : wave_y(w->x);
}

static void boss_update_waves() {
    int enraged = boss_is_enraged();
    for (int i = 0; i < BOSS_MAX_WAVES; i++) {
        if (!waves[i].active) {
            continue;
        }
        int speed;
        if (waves[i].kind == WAVE_KIND_LINE) {
            speed = enraged ? LINE_SPEED_P2 : LINE_SPEED_P1;
        } else {
            speed = enraged ? WAVE_SPEED_P2 : WAVE_SPEED_P1;
        }
        waves[i].x -= speed;
        if (waves[i].x < -wave_sprite->w) {
            waves[i].active = FALSE;
        }
    }
}

// TRUE when no projectiles are on screen (the current attack has cleared)
static int boss_waves_cleared() {
    for (int i = 0; i < BOSS_MAX_WAVES; i++) {
        if (waves[i].active) {
            return FALSE;
        }
    }
    return TRUE;
}

// walk-in animation: step frames 0..12, clamp at 12
static void boss_anim_walk_in() {
    boss.anime_count++;
    if (boss.anime_count >= BOSS_WALK_INTERVAL) {
        boss.anime_count = 0;
        if (boss.frame < 12) {
            boss.frame++;
        }
    }
}

// attack animation: phase 1 loops {12..8}, phase 2 loops {13..25}
static void boss_anim_attack() {
    boss.anime_count++;
    if (boss.anime_count < BOSS_ATTACK_INTERVAL) {
        return;
    }
    boss.anime_count = 0;

    if (boss_is_enraged()) {
        int span = BOSS_P2_LAST_FRAME - BOSS_P2_FIRST_FRAME + 1; // 13 frames
        boss.anime_index++;
        if (boss.anime_index >= span) {
            boss.anime_index = 0;
        }
        boss.frame = BOSS_P2_FIRST_FRAME + boss.anime_index;
    } else {
        int span = BOSS_P1_LAST_FRAME - BOSS_P1_FIRST_FRAME + 1;
        boss.anime_index++;
        if (boss.anime_index >= span) {
            boss.anime_index = 0;
        }
        boss.frame = BOSS_P1_FIRST_FRAME + boss.anime_index;
    }
}

// enter the vulnerable rest state; `upcoming` is the attack to run afterwards
static void boss_enter_tired(int upcoming) {
    boss.state = BOSS_TIRED;
    boss.next_attack = upcoming;
    boss.attack_timer = 0;
    boss.fired = FALSE;
    if (boss_is_enraged()) {
        boss.frame = 27;
    } else {
        boss.frame = 26; 
    }
}

// enter an attack state fresh (ready to fire its burst)
static void boss_enter_attack(int attack_state) {
    boss.state = attack_state;
    boss.fired = FALSE;
    boss.anime_index = 0;
    boss.anime_count = 0;
}

void boss_update(int scroll_x) {
    if (boss.hurt_cooldown > 0) {
        boss.hurt_cooldown--;
    }

    switch (boss.state) {
    case BOSS_WALK_IN:
        boss.x += BOSS_WALK_SPEED;
        boss_anim_walk_in();
        if (boss.x >= BOSS_STOP_X) {
            boss.x = BOSS_STOP_X;
            boss.flip = TRUE; // face the player (left)
            boss_enter_attack(BOSS_ATTACK);
        }
        break;
    case BOSS_ATTACK:
        boss_anim_attack();
        if (!boss.fired) {
            boss_fire(WAVE_KIND_WAVE);
            boss.fired = TRUE;
        } else if (boss_waves_cleared()) {
            boss_enter_tired(BOSS_ATTACK_LINE); // next attack is the line
        }
        break;
    case BOSS_ATTACK_LINE:
        boss_anim_attack();
        if (!boss.fired) {
            boss_fire(WAVE_KIND_LINE);
            boss.fired = TRUE;
        } else if (boss_waves_cleared()) {
            boss_enter_tired(BOSS_ATTACK); // next attack is the wave
        }
        break;
    case BOSS_TIRED:
        boss.attack_timer++;
        if (boss.attack_timer >= BOSS_TIRED_FRAMES) {
            boss_enter_attack(boss.next_attack);
        }
        break;
    default:
        break;
    }

    // projectiles keep travelling even while the boss dies
    boss_update_waves();
}

void boss_draw(int scroll_x) {
    if (boss.state == BOSS_INACTIVE) {
        return;
    }

    // wave trail
    if (wave_sprite) {
        for (int i = 0; i < BOSS_MAX_WAVES; i++) {
            if (!waves[i].active) {
                continue;
            }
            int screen_x = waves[i].x - scroll_x;
            draw_sprite(current_screen, wave_sprite, screen_x, wave_pos_y(&waves[i]));
        }
    }

    // boss body (blink while invulnerable)
    if (boss.state != BOSS_DEAD && !(boss.hurt_cooldown > 0 && ((boss.hurt_cooldown >> 1) & 1) == 0)) {
        BITMAP* sprite = boss_sprites[boss.frame];
        if (sprite) {
            int screen_x = boss.x - scroll_x;
            if (boss.flip == TRUE) {
                draw_sprite_h_flip(current_screen, sprite, screen_x, boss.y);
            } else {
                draw_sprite(current_screen, sprite, screen_x, boss.y);
            }
        }
    }

    // HP bar (top-left), like the existing enemy boss bar
    int width = boss.hp << 1; // 0..40 px
    if (width > 0) {
        blit(dat_file[LIFEBAR_ENEMY_BMP].dat, current_screen, 0, 0, 120, 2, width, 5);
    }
}

void boss_get_aabb(collisionType* box) {
    if (!box) {
        return;
    }
    if (!boss_is_active()) {
        *box = (collisionType){ 0, 0, 0, 0 };
        return;
    }
    *box = (collisionType){ .x = boss.x, .y = boss.y, .w = boss_w, .h = boss_h };
}

void boss_wave_get_all_aabb(collisionType* boxes) {
    for (int i = 0; i < BOSS_MAX_WAVES; i++) {
        if (waves[i].active && wave_sprite) {
            boxes[i].x = waves[i].x;
            boxes[i].y = wave_pos_y(&waves[i]);
            boxes[i].w = wave_sprite->w;
            boxes[i].h = wave_sprite->h;
        } else {
            boxes[i].x = 0;
            boxes[i].y = 0;
            boxes[i].w = 0;
            boxes[i].h = 0;
        }
    }
}

void boss_on_hit() {
    // only vulnerable while tired (the rest window between attacks)
    if (boss.hurt_cooldown > 0) {
        return;
    }
    boss.hurt_cooldown = BOSS_HURT_COOLDOWN;
    boss.hp--;
    screen_shake();
    if (boss.hp <= 0) {
        boss.hp = 0;
        boss.state = BOSS_DEAD;
        for (int i = 0; i < BOSS_MAX_WAVES; i++) {
            waves[i].active = FALSE;
        }
    }
}
