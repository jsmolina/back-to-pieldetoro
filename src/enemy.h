#ifndef ENEMY_H
#define ENEMY_H

#include <allegro.h>
#include "object.h"

#define MAX_SPAWNABLE_ENEMIES 4
#define MAX_ACTIVE_ENEMIES 4
#define ENEMY_FRAMES 16
#define JOVEN_FRAMES 16
#define BIRD_FRAMES 2


/**
spawn_enemies (Estáticos): Una lista o array con la posición
inicial y el tipo de todos los enemigos del nivel. Esto ocupa muy poca memoria.

active_enemies (Dinámicas): Un array de tamaño fijo (por ejemplo,
máximo 10 o 16 enemigos) que contiene los objetos que realmente se mueven,
colisionan y se dibujan.

Spawn: Cuando la cámara se acerca a la posición de un enemigo en la
"lista estática", el motor busca un hueco vacío en el array de activos
e instancia (o recicla) al enemigo ahí.

Update: Solo se ejecutan los update() y las colisiones de los enemigos
que están en el array de activos.

Despawn: Si el enemigo se aleja demasiado de la cámara (por ejemplo,
128 píxeles a la izquierda), se elimina del array de activos para
dejar espacio a otros.
----------

Inicialización: Al cargar el nivel, creas un array de, digamos,
20 enemigos "genéricos" y los mantienes desactivados.

Activación: Cuando la cámara llega a un punto de spawn,
tomas uno de esos 20, le asignas los datos del enemigo
correspondiente (ej. "ahora eres un Joven") y lo activas.

Estado de muerte: Si matas a un enemigo, debes marcarlo en tu lista de
"Datos Estáticos" para que no vuelva a aparecer si el jugador retrocede
y vuelve a avanzar.

Off-screen freeze: Algunos juegos no eliminan al enemigo si está cerca
pero fuera de pantalla, simplemente congelan su lógica para que no se
caiga por un precipicio mientras no lo ves.
 */

 // static enemy data for each type, to avoid reloading sprites and data for each instance
 typedef struct {
    int vx;
    int vy;
    int max_vx;
    int width;
    int height;
    int total_frames;
    BITMAP* sprites[ENEMY_FRAMES];
    struct animeItem * animations; // pointer to sprite animations
} EnemyData;

enum EnemyType {
    ENEMY_JOVEN,
    ENEMY_BIRD,
};

typedef struct {
    enum EnemyType type;
    EnemyData * data; // pointer to static data for this enemy type
    coordsType pos;
    int flip;
    int move_count;
    int anime_count;
    int anime_index;
    int sprite_index;
    unsigned int state;
    unsigned int prev_state;
    int active;
    int killed; // to mark if the enemy has been killed for static data
    int origin; // index in spawnable_enemies this instance was spawned from, -1 if none
} Enemy;


//static Enemy spawnable_enemies[MAX_SPAWNABLE_ENEMIES];
//static Enemy active_enemies[MAX_ACTIVE_ENEMIES];
//static EnemyData enemy_data[2]; // static data for each enemy type

/* Initialize static spawnable enemy (level data) */
void init_enemy(int index, enum EnemyType type, int x, int y);

/* reset the enemy pool */
void reset_spawnable_enemies();

/* Initialize the active enemy pool */
void enemy_pool_init();

/* Update pool: spawn/despawn/update active enemies using camera x */
void enemy_pool_update(int camera_x);

/* Draw active enemies using scroll_x */
void draw_enemies(int scroll_x);

/* Per-enemy operations */
void update_enemy(Enemy* enemy);
void enemy_affect_force(Enemy* enemy, int vx, int vy);
// loadspritesheets and initializes static data for enemy types
void load_enemy_spritesheets();

#endif // ENEMY_H
