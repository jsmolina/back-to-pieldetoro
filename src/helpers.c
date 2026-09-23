
#include "helpers.h"
#include "allegro/datafile.h"
#include "dat_manager.h"
#include "langs.h"
#include "statics.h"

#include <allegro.h>
#include <dos.h>
#include <pc.h>
#include <stdio.h>

#define PRINT_SLOW_DELAY_MS 10

static char* game_texts[TXT_COUNT] = {
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
    "BRUNO HA SIDO DERROTADO, PERO QUEDA SU MUSICA",
    "EL AUTOVOICE, HAY QUE DESTRUIRLO",
    "VE Y DESTRUYE LA SALA DE SERVIDORES CON TNT",
    "MATASTE A MI AUTOVOICE, PREPARATE A MORIR",
    "TOMAS TU LOCA, JENNY TE ESPERA",
    "GUAPO VEN AQUI",
    "NECESITO LAS 3 PIEZAS ANTES"
};

BITMAP* current_screen;
int page_generation = 1;
static BITMAP* video_page[2];
static BITMAP* displayed_page;

void present_frame(void) {
    // scroll_screen waits for vertical blank before switching the display to the finished page
    scroll_screen(current_screen->x_ofs, current_screen->y_ofs);
    displayed_page = current_screen;
    current_screen = (current_screen == video_page[0]) ? video_page[1] : video_page[0];
}

void init_video_pages(void) {
    // full virtual width (336) so screen_shake can pan a few px into a cleared margin;
    // page 0 lands at VRAM origin (same memory as screen), page 1 below it at y=200
    video_page[0] = create_video_bitmap(VIRTUAL_W, SCREEN_H);
    video_page[1] = create_video_bitmap(VIRTUAL_W, SCREEN_H);
    clear_bitmap(video_page[0]);
    clear_bitmap(video_page[1]);
    displayed_page = video_page[0];
    current_screen = video_page[1];
}

int current_page_index(void) {
    return current_screen == video_page[1];
}

void show_screen_page(void) {
    page_generation++;
    if (displayed_page == video_page[0])
        return;
    // keep the last game frame visible under overlays like pause or dialogs
    blit(video_page[1], video_page[0], 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    scroll_screen(0, 0);
    displayed_page = video_page[0];
    current_screen = video_page[1];
}

void lang_load(int lang_id, DATAFILE* lang_dat_file) {
    char* text = malloc(lang_dat_file[lang_id].size + 1);
    int count = 0;
    memcpy(text, lang_dat_file[lang_id].dat, lang_dat_file[lang_id].size);
    text[lang_dat_file[lang_id].size] = '\0';

    for (char* p = strtok(text, "\n"); p && count < TXT_COUNT; p = strtok(NULL, "\n")) {
        game_texts[count++] = p;
    }
}

inline void menu_lang_print(unsigned int selected, unsigned int val, int y, const char* str, FONT* font_data) {
    textprintf_ex(current_screen, font_data, 130, y, 0, selected == val ? 15 : 7, "%s%s", selected==val?"> ":"  ", str);
}

void lang_select(int lang) {
    DATAFILE* lang_dat_file = load_datafile("lang.dat");

    if (lang > 0) {
        lang_load(lang, lang_dat_file);
        unload_datafile(lang_dat_file);
        return;
    }

    FONT* fnt = lang_dat_file[MENU_FONT_FNT].dat;
    int done = FALSE;

    clear_keybuf();
    unsigned int selected = KEY_1;
    clear_to_color(current_screen, 0);
    while (done != TRUE) {
        menu_lang_print(selected, KEY_1, 58, "1-English", fnt);
        menu_lang_print(selected, KEY_2, 70, "2-Espanol", fnt);
        menu_lang_print(selected, KEY_3, 82, "3-Catala", fnt);
        menu_lang_print(selected, KEY_4, 94, "4-Galego", fnt);
        menu_lang_print(selected, KEY_0, 112, "0-EXIT", fnt);
        textprintf_ex(current_screen, fnt, 130, 122, 2, -1, "v0.8");

        blit(current_screen, screen, 125, 55, 125, 55, 195, 145);

        clear_keybuf();
        int key_code = readkey() >> 8;

        switch (key_code) {
        case KEY_UP:
            if (selected == KEY_0) {
                selected = KEY_4;
                break;
            }
            selected--;
            break;
        case KEY_DOWN:
            if (selected == KEY_4) {
                selected = KEY_0;
                break;
            }
            selected++;
            break;
        case KEY_SPACE:
        case KEY_ENTER:
            key_code = selected;
            break;
        }

        switch (key_code) {
        case KEY_0_PAD:
        case KEY_0:
            exit(0);
            break;
        case KEY_1_PAD:
        case KEY_1:
            lang_load(LANG_EN_TXT, lang_dat_file);
            done = TRUE;
            break;
        case KEY_2_PAD:
        case KEY_2:
            lang_load(LANG_ES_TXT, lang_dat_file);
            done = TRUE;
            break;
        case KEY_3_PAD:
        case KEY_3:
            lang_load(LANG_CAT_TXT, lang_dat_file);
            done = TRUE;
            break;
        case KEY_4_PAD:
        case KEY_4:
            lang_load(LANG_GAL_TXT, lang_dat_file);
            done = TRUE;
            break;
        }
    }
    unload_datafile(lang_dat_file);
}

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
    // shift the displayed page via scroll_screen: it keeps video enabled, uses valid pan values,
    // and waits for retrace, so each step lasts exactly one frame
    static const int offsets[] = { 3, 0, 2, 0, 1, 0 };
    for (int i = 0; i < 6; i++)
        scroll_screen(displayed_page->x_ofs + offsets[i], displayed_page->y_ofs);
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
    case 7:
        return BG6_TMX;
    default:
        return -1;
    }
}
