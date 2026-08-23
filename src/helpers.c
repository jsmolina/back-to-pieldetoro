
#include "helpers.h"
#include "dat_manager.h"
#include "statics.h"

#include <allegro.h>

#include <dos.h>
#include <pc.h>
#include <stdio.h>


#if !defined(LANG_EN) && !defined(LANG_ES)
#define LANG_ES 1
#endif

#define PRINT_SLOW_DELAY_MS 10

#if defined(LANG_EN)
static const char* const game_texts[TXT_COUNT] = {
    "YOU BUILT A TIME MACHINE IN A BETSY?",
    "IF YOU BUILD A TIME MACHINE, MAKE IT COOL, DUDE!",
    "I NEED YOU TO COME TO THE FUTURE",
    "ARE WE GOING DORK OR WHAT?",
    "YOUR KIDS LISTEN TO REGGAETON AND SAY BRO",
    "DAMN, START THE BETSY, I PUSH IT TO 140 KM/H",
    "WATCH OUT, THE ROAD IS A SHAMBLES",
    "DUDE, NOW BRUNO TANEZ IS IN CHARGE",
    "NO WAY, ARE YOU FUCKING ME?",
    "BRO, BUY",
    "UR BROKE",
    "BOUGHT BRO",
    "NO STOCK BRO",
    "I NEED THAT ALMANAC",
    "ARE YOU IN OR WHAT, BRO?",
    "NAH, BASED",
    "HELL YEAH, BASED, FOR HIM!",
    "WE LIVED IN POVERTY AND HE CAME, IN 1954,"
    "HOW DO YOU THINK OF BUYING THIS?",
    "WIN SOME BETS, DUDE",
    "HE ROBBED IT AND CRASHED",
    "NOW WE HAVE TO RECOVER THE PIECES",
    "BRUNO'S DOWN, BUT THE MUSIC AIN'T DEAD"
};
#else
static const char* const game_texts[TXT_COUNT] = {
    "HAS MONTAO UNA MAQUINA DEL TIEMPO EN UNA LOCA?",
    "SI VAS A MONTAR UNA MAQUINA DEL TIEMPO,QUE MOLE,TRON!",
    "NECESITO QUE VENGAS AL FUTURO",
    "NOS VOLVEMOS TOLAIS O ASI?",
    "TUS HIJOS TRON,ESCUCHAN REGUETON Y DICEN BRO",
    "OSTIA, ARRANCA LA LOCA QUE LA METO A 140 KM/H",
    "CUIDAO QUE ESTA LA CARRETERA HECHA UN CISCO",
    "NEN, AHORA GOBIERNA BRUNO TANEZ",
    "OSTIAS, NO ME JODAS.",
    "COMPRA BRO",
    "ESTAS BROKE",
    "COMPRADO BRO",
    "NO QUEDA BRO",
    "NECESITO ESE ALMANAQUE",
    "TE APUNTAS, BRO?",
    "NAH, BASADO",
    "DE UNA, BASADO, POR EL!",
    "VIVIAMOS EN LA POBREZA Y VINO EL, EN 1954",
    "COMO SE TE OCURRE COMPRAR ESTO?",
    "GANAR ALGUNA APUESTICA, NEN",
    "NOS LO HA ROBADO Y SE LA HA PEGADO",
    "AHORA HAY QUE RECUPERAR LAS PIEZAS",
    "BRUNO HA SIDO DERROTADO, PERO QUEDA SU MUSICA"
};
#endif

BITMAP* current_screen;

const char* game_text(gameTextId id) {
    if (id < 0 || id >= TXT_COUNT) {
        return "";
    }
    return game_texts[id];
}

void wait_for_space() {
    do {
    } while (!key[KEY_SPACE]);
    rectfill(screen, 0, 160, SCREEN_W, SCREEN_H, makecol(1, 1, 1));
    do {
    } while (key[KEY_SPACE]);
}

void print_at(int x, int y, const char* texto, int col, int bg) {
    FONT* myfont = dat_file[FONT_FNT].dat;
    int i = 0;
    int longitud = strlen(texto);
    char trozo[36];

    while (i < longitud) {
        int end = i;
        int last_space = -1;

        while (i < longitud && texto[i] == ' ') {
            i++;
        }
        if (i >= longitud) {
            break;
        }

        end = i;
        while (end < longitud && end - i < 35) {
            if (texto[end] == ' ') {
                last_space = end;
            }
            end++;
        }

        if (end < longitud && texto[end] != ' ' && last_space >= i) {
            end = last_space;
        }

        if (end == i) {
            end = i + 35;
            if (end > longitud) {
                end = longitud;
            }
        }

        strncpy(trozo, texto + i, end - i);
        trozo[end - i] = '\0';
        textprintf_ex(screen, myfont, x, y, col, bg, "%s", trozo);
        i = end;

        while (i < longitud && texto[i] == ' ') {
            i++;
        }

        y += 8;
        if (y > 240) {
            y = 0;
        }
    }
}

void printf_at_simple(int x, int y, int col, int bg, const char* format, ...) {
    FONT* myfont = dat_file[FONT_FNT].dat;
    char buffer[256];

    if (!format || !myfont) {
        return;
    }

    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    buffer[sizeof(buffer) - 1] = '\0';
    textprintf_ex(screen, myfont, x, y, col, bg, "%s", buffer);
}

void printf_at_ingame(int x, int y, int col, int bg, const char* format, ...) {
    FONT* myfont = dat_file[FONT_FNT].dat;
    char buffer[256];

    if (!format || !myfont) {
        return;
    }

    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    buffer[sizeof(buffer) - 1] = '\0';
    textprintf_ex(current_screen, myfont, x, y, col, bg, "%s", buffer);
}

void print_at_slow(int x, int y, const char* texto, int col, int bg) {
    FONT* myfont = dat_file[FONT_FNT].dat;
    int cursor_x = x;

    if (!texto || !myfont) {
        return;
    }

    for (int i = 0; texto[i] != '\0'; i++) {
        char ch[2];
        ch[0] = texto[i];
        ch[1] = '\0';

        textprintf_ex(screen, myfont, cursor_x, y, col, bg, "%s", ch);
        cursor_x += text_length(myfont, ch);
        rest(PRINT_SLOW_DELAY_MS);
    }
}

void screen_shake() {
    int offsets[] = { 3, -3, 2, -2, 1, 0 };  // logical, map to pel values
    int i;
    for (i = 0; i < 6; i++) {
        // pel panning: 0x3C0 index 0x13, value 0-7
        int pel = offsets[i] < 0 ? 0 : offsets[i];
        outportb(0x3C0, 0x13);
        outportb(0x3C0, pel & 0x07);
        rest(16); // ~1 frame at 60fps
    }
    // reset
    outportb(0x3C0, 0x13);
    outportb(0x3C0, 0);
}

void beep(int frequency, int duration) {
    int div = 1193180 / frequency;

    outportb(0x43, 0xb6);
    outportb(0x42, div & 0xff);
    outportb(0x42, div >> 8);

    outportb(0x61, inportb(0x61) | 3);
    // KEEP: In this DOS/DJGPP build we intentionally use delay() here.
    // Do not auto-replace with rest() in this function.
    delay(duration);
    outportb(0x61, inportb(0x61) & 0xfc);
}

int level_to_dat_id(int level) {
    switch (level) {
    case 1:
        return BG0_TMX;
    case 2:
        return BG1_TMX;
    case 3:
        return BG4_TMX;
    case 4:
        return BG2_TMX;
    case 5:
        return BG3_TMX;
    case 6:
        return BG5_TMX;
    default:
        return -1;
    }
}
