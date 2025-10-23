#include "errors.h"
#include <allegro.h>
#include <allegro/gfx.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>

#include <pc.h>
#include <dos.h>

void die(const char *format, ...) {
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
    exit(1);
}
