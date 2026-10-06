/* linux/windows: Allegro 4 keeps timers, sound, bitmaps and datafiles; SDL2 shows the frame
   and reads the keyboard. Allegro's console/fbcon drivers fight KMSDRM handhelds (R36S) and
   GDI can't scale well. DOS keeps Allegro's VGA driver */
#ifndef __DJGPP__
#include <allegro.h>
#ifdef _WIN32
#include <winalleg.h>
#endif
#define SDL_MAIN_HANDLED   /* main() belongs to Allegro (END_OF_MAIN), not SDL2main */
#include <SDL.h>
#include <stdio.h>
#include "helpers.h"

#define GFX_SDL2 AL_ID('S','D','L','2')
#define KEYBUF_SIZE 32 /* power of two: wraps with a mask */

static SDL_Window* window;
static SDL_Renderer* renderer;
static SDL_Texture* texture;
static Uint32 pal32[256];
static volatile int dirty;
static int last_retrace;
static unsigned char key_map[SDL_NUM_SCANCODES];
static int keybuf[KEYBUF_SIZE];
static int keybuf_head, keybuf_tail;
/* a tap whose press and release arrive in the same pump keeps key[] set until the next
   pump, so loops polling key[] still see it (a DOS key interrupt can't batch them) */
static int pump_gen, down_gen[KEY_MAX];
static int held[16], held_count;

static void pump(void) {
    SDL_Event e;
    pump_gen++;
    while (held_count)
        key[held[--held_count]] = 0;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT)
            exit(0);
        if ((e.type == SDL_KEYDOWN || e.type == SDL_KEYUP) && key_map[e.key.keysym.scancode]) {
            int k = key_map[e.key.keysym.scancode];
            if (e.type == SDL_KEYDOWN) {
                key[k] = 1;
                down_gen[k] = pump_gen;
            } else if (down_gen[k] == pump_gen && held_count < 16) {
                held[held_count++] = k;
            } else {
                key[k] = 0;
            }
            if (e.type == SDL_KEYDOWN && ((keybuf_head + 1) & (KEYBUF_SIZE - 1)) != keybuf_tail) {
                int sym = e.key.keysym.sym;
                keybuf[keybuf_head] = (k << 8) | (sym < 128 ? sym : 0);
                keybuf_head = (keybuf_head + 1) & (KEYBUF_SIZE - 1);
            }
        }
    }
}

static void sdl_present(void) {
    void* dst;
    int pitch, x, y;
    unsigned char* src = screen->line[0];   /* memory bitmap: rows are contiguous */
    dirty = 0;
    if (SDL_LockTexture(texture, NULL, &dst, &pitch) == 0) {
        for (y = 0; y < 200; y++) {
            Uint32* row = (Uint32*)((Uint8*)dst + y * pitch);
            for (x = 0; x < 320; x++)
                row[x] = pal32[*src++];
        }
        SDL_UnlockTexture(texture);
    }
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);   /* logical 320x240: 4:3, black bars */
    SDL_RenderPresent(renderer);
    pump();
}

/* present_screen() only marks the frame: the next vsync/readkey presents it once */
void sdl_mark_dirty(void) {
    dirty = 1;
}

static int sdl_keypressed(void) {
    if (dirty)
        sdl_present();
    pump();
    return keybuf_head != keybuf_tail;
}

static int sdl_readkey(void) {
    int k;
    while (!sdl_keypressed())
        SDL_Delay(5);
    k = keybuf[keybuf_tail];
    keybuf_tail = (keybuf_tail + 1) & (KEYBUF_SIZE - 1);
    return k;
}

/* like a VGA retrace wait: present, then wait for the next 70Hz tick of Allegro's timer */
static void sdl_vsync(void) {
    sdl_present();
    while (retrace_count == last_retrace)
        SDL_Delay(1);
    last_retrace = retrace_count;
}

/* may run on Allegro's timer thread (palette rotation): no SDL calls here */
static void sdl_set_palette(AL_CONST RGB* p, int from, int to, int retracesync) {
    int i;
    (void)retracesync;
    for (i = from; i <= to; i++)   /* 6-bit VGA to 8-bit: x * 255 / 63 without the division */
        pal32[i] = 0xFF000000 | (((p[i].r << 2) | (p[i].r >> 4)) << 16)
            | (((p[i].g << 2) | (p[i].g >> 4)) << 8) | ((p[i].b << 2) | (p[i].b >> 4));
    dirty = 1;
}

static void sdl_exit(BITMAP* b);

/* installed by hand (gfx_driver/screen), not via set_gfx_mode: works with the prebuilt
   windows Allegro 4.2 DLL too, which can't register drivers. designated fields: the
   struct differs between 4.2 and 4.4 */
static GFX_DRIVER gfx_sdl2 = {
    .id = GFX_SDL2, .name = "SDL2", .desc = "SDL2", .ascii_name = "SDL2",
    .exit = sdl_exit, .vsync = sdl_vsync, .set_palette = sdl_set_palette,
    .w = 320, .h = 200, .linear = TRUE, .windowed = TRUE
};

static void init_key_map(void) {
    int i;
    for (i = 0; i < 26; i++) key_map[SDL_SCANCODE_A + i] = KEY_A + i;
    for (i = 0; i < 9; i++) key_map[SDL_SCANCODE_1 + i] = KEY_1 + i;
    for (i = 0; i < 9; i++) key_map[SDL_SCANCODE_KP_1 + i] = KEY_1_PAD + i;
    for (i = 0; i < 12; i++) key_map[SDL_SCANCODE_F1 + i] = KEY_F1 + i;
    key_map[SDL_SCANCODE_0] = KEY_0;
    key_map[SDL_SCANCODE_KP_0] = KEY_0_PAD;
    key_map[SDL_SCANCODE_ESCAPE] = KEY_ESC;
    key_map[SDL_SCANCODE_RETURN] = KEY_ENTER;
    key_map[SDL_SCANCODE_KP_ENTER] = KEY_ENTER_PAD;
    key_map[SDL_SCANCODE_SPACE] = KEY_SPACE;
    key_map[SDL_SCANCODE_BACKSPACE] = KEY_BACKSPACE;
    key_map[SDL_SCANCODE_TAB] = KEY_TAB;
    key_map[SDL_SCANCODE_UP] = KEY_UP;
    key_map[SDL_SCANCODE_DOWN] = KEY_DOWN;
    key_map[SDL_SCANCODE_LEFT] = KEY_LEFT;
    key_map[SDL_SCANCODE_RIGHT] = KEY_RIGHT;
    key_map[SDL_SCANCODE_LCTRL] = KEY_LCONTROL;
    key_map[SDL_SCANCODE_RCTRL] = KEY_RCONTROL;
    key_map[SDL_SCANCODE_LALT] = KEY_ALT;
    key_map[SDL_SCANCODE_RALT] = KEY_ALTGR;
    key_map[SDL_SCANCODE_LSHIFT] = KEY_LSHIFT;
    key_map[SDL_SCANCODE_RSHIFT] = KEY_RSHIFT;
}

static int sdl_open(void) {
    /* desktop: 3x window; linux handheld (KMSDRM, no display server): whole screen */
#ifdef _WIN32
    int desktop = TRUE;
#else
    int desktop = getenv("DISPLAY") || getenv("WAYLAND_DISPLAY");
#endif
    SDL_RendererInfo info;
    SDL_SetMainReady();
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        ustrzcpy(allegro_error, ALLEGRO_ERROR_SIZE, SDL_GetError());
        return -1;
    }
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
    window = SDL_CreateWindow("Back to Piel de Toro", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        960, 720, desktop ? SDL_WINDOW_RESIZABLE : SDL_WINDOW_FULLSCREEN_DESKTOP);
    if (window)
        renderer = SDL_CreateRenderer(window, -1, 0);
    if (renderer)
        texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, 320, 200);
    if (!texture) {
        ustrzcpy(allegro_error, ALLEGRO_ERROR_SIZE, SDL_GetError());
        SDL_Quit();
        return -1;
    }
    SDL_RenderSetLogicalSize(renderer, 320, 240);
    SDL_ShowCursor(SDL_DISABLE);
    SDL_GetRendererInfo(renderer, &info);
    fprintf(stderr, "gfx: SDL2 video %s, renderer %s\n", SDL_GetCurrentVideoDriver(), info.name);
    return 0;
}

/* called by set_gfx_mode(GFX_TEXT), which then destroys `screen` itself */
static void sdl_exit(BITMAP* b) {
    (void)b;
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    texture = NULL; renderer = NULL; window = NULL;
    SDL_Quit();
}

int sdl_set_gfx_mode(void) {
    if (sdl_open() != 0)
        return -1;
    init_key_map();
    install_timer();   /* sdl_vsync waits on retrace_count */
    screen = create_bitmap_ex(8, 320, 200);
    clear_bitmap(screen);
    gfx_driver = &gfx_sdl2;
    set_palette(default_palette);   /* set_gfx_mode would have done it */
    install_keyboard_hooks(sdl_keypressed, sdl_readkey);
#ifdef _WIN32
    /* Allegro's own (0x0) window: off the taskbar; sound must not follow its focus */
    ShowWindow(win_get_window(), SW_HIDE);
    set_display_switch_mode(SWITCH_BACKGROUND);
#endif
    return 0;
}
#endif
