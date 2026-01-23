#ifndef ENEMY_H
#define ENEMY_H

#include <allegro.h>

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
correspondiente (ej. "ahora eres un Goomba") y lo activas.

Estado de muerte: Si matas a un enemigo, debes marcarlo en tu lista de 
"Datos Estáticos" para que no vuelva a aparecer si el jugador retrocede 
y vuelve a avanzar.

Off-screen freeze: Algunos juegos no eliminan al enemigo si está cerca 
pero fuera de pantalla, simplemente congelan su lógica para que no se 
caiga por un precipicio mientras no lo ves.
 */

typedef struct {
    BITMAP* image;
    int x;
    int y;
} Enemy;

extern Enemy spawnable_enemies[4];

void init_enemy(Enemy* enemy, int bitmap_id);
void draw_enemy(Enemy* enemy);
void update_enemy(Enemy* enemy);

#endif // ENEMY_H
