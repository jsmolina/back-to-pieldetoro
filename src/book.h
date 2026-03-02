#ifndef BOOK_H
#define BOOK_H
#include <allegro.h>
#include "dat_manager.h"
#include "statics.h"
#include "object.h"

void init_book(int x, int y, int flip);
void draw_book();
void update_book();

#endif
