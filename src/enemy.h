#ifndef ENEMY_H
#define ENEMY_H

#include "helpers.h"
#include <allegro.h>

#define MAX_SPAWNABLE_ENEMIES 20
#define MAX_ACTIVE_ENEMIES 10
#define ENEMY_FRAMES 16
#define JOVEN_FRAMES 16
#define BRUNO_FRAMES 16
#define BIRD_FRAMES 2
#define DOG_FRAMES 7
#define BOMB_FRAMES 2
#define LAMP_FRAMES 1
#define SYRINGE_FRAMES 1

#define ENEMY_HITS_TO_KILL 2
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
    int max_vx;
    int width;
    int height;
    int total_frames;
    BITMAP* sprites[ENEMY_FRAMES];
    animeItem* animations; // pointer to sprite animations
} EnemyData;

enum EnemyType {
    ENEMY_JOVEN,
    ENEMY_BIRD,
    ENEMY_DOG,
    ENEMY_BRUNO,
    // NON-ANIMATED ENEMIES
    ENEMY_LAMP,
    ENEMY_SYRINGE,
    ENEMY_BOMB,
    TOTAL_ENEMY_DATA
};

typedef struct {
    enum EnemyType type;
    EnemyData* data; // pointer to static data for this enemy type
    coordsType pos;
    int screen_spawn_x; // the scroll amount when this enemy should be spawned, used for spawn logic
    int vx;
    int vy;
    int flip;
    int move_count;
    int anime_count;
    int anime_index;
    int sprite_index;
    unsigned int state;
    unsigned int prev_state;
    int hits; // number of hits taken, used for enemies that require multiple hits to die
    int active;
    int killed; // to mark if the enemy has been killed for static data
    int origin; // index in spawnable_enemies this instance was spawned from, -1 if none
} Enemy;

/** @brief Resets the spawnable enemies array to default values, marking all as inactive and not killed. -
 *
 */
void reset_spawnable_enemies();

/** @brief Loads and initializes all enemies for the specified level from stage_enemies.def.
 *  Reads the stage_enemies.def file from the dat file and calls init_enemy for each enemy
 *  entry where the level matches the provided level_id.
 *
 * @param level_id The level number to load enemies for.
 */
void load_level_enemies(int level_id);

/** @brief Loads enemies from a TMX XML map object layer.
 *  Creates spawnable enemies from <object> tags where type is EL or ER.
 *  Enemy type is read from the object name (ENEMY_JOVEN, ENEMY_BIRD, ENEMY_DOG).
 *
 * @param tmx_id TMX file ID packed in datos.dat (e.g. BG1_TMX).
 */
void load_level_enemies_v2(int level_id);

/** @brief Initializes the enemy pool, sets all the variables to default values and assigns static data to each enemy instance.
    also Called when restarting a stage to clear existing enemies.
 *
 */
void enemy_pool_init();

/** @brief initializes the list of spawnable enemies, marking all as inactive.
 Called at the start of each stage before loading new enemies.
 */
void enemy_spawn_init();

/** @brief Updates the enemy pool based on the camera position.
 *
 * @param camera_x The horizontal position of the camera.
 */
void enemy_pool_update(int camera_x);

/** @brief Draws all active enemies using the specified scroll x.
 *
 * @param scroll_x The horizontal scroll position.
 */
void draw_enemies(int scroll_x);

/** @brief Updates the state of the specified enemy.
 *
 * @param enemy The enemy to update.
 * @param scroll_x The horizontal scroll position.
 */
void enemy_update(int scroll_x);

/** @brief Loads the spritesheets for all enemy types.
 */
void load_enemy_spritesheets();

/** @brief Fills the provided boxes array with the bounding boxes of all active enemies.
 *
 * @param enemies An array of collisionType to be filled with the bounding boxes of active enemies.
 */
void enemy_get_all_aabb(collisionType* enemies);

/** @brief Marks the enemy at the given index as hit (inactive) and resets its position.
 *
 * @param enemy_id The index of the enemy to be marked as hit.
 */
void enemy_on_hit(int enemy_id);

/** @brief Cleans up the spritesheets for all enemy types, freeing associated memory.
 */
void destroy_enemy_spritesheets();
// collisionType enemy_get_aabb(const Enemy* e);
#endif // ENEMY_H
