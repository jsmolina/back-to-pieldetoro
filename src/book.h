#ifndef BOOK_H
#define BOOK_H
#include <allegro.h>
#include "dat_manager.h"
#include "statics.h"
#include "object.h"

int init_book(int x, int y, int flip);
void draw_throwable(int scroll_x);
void throwable_update(int scroll_x);

#endif
