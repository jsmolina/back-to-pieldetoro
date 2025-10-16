
#include "helpers.h"

#include <allegro.h>
#include <allegro/gfx.h>

#include <pc.h>
#include <dos.h>


void beep(int frequency, int duration) {
    int div = 1193180 / frequency;
    
    outportb(0x43, 0xb6);
    outportb(0x42, div & 0xff);
    outportb(0x42, div >> 8);
    
    outportb(0x61, inportb(0x61) | 3);
    
    delay(duration);    
    outportb(0x61, inportb(0x61) & 0xfc);
}
