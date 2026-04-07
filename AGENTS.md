# Back to Pieldetoro

Retro 2D side-scrolling game written in C targeting MS-DOS, using Allegro 4.2.2
and DJGPP cross-compiler. The game runs at 320x200 VGA (8-bit palette).
Licensed under MIT (Jordi Sesmero).

## Project Structure

```
src/            C source and headers (.c / .h pairs)
static/         Game assets: bitmaps, spritesheets, MIDI, FLI, Tiled maps (.tmx)
static/build.sh Script that packs assets into datos.dat using Allegro's `dat` tool
setup/          Allegro setup utility (setup.c / setup.h / keyconf.c)
vendor/         allegro-4.2.2-xc (cross-compile Allegro for DOS)
dist/           Build output (main.exe + datos.dat + cwsdpmi.exe)
Makefile        DJGPP cross-compilation rules
Dockerfile      Ubuntu-based image with DJGPP + Allegro toolchain
docker-compose.yaml  Two-stage build: assets (dat) then executable (exe)
```

## Build Commands

The entire build (assets + executable) runs inside Docker:

```bash
# Full build (assets + compile)
docker-compose up

# Build Docker image only
docker build -t build_pieldetoro .

# Inside the container the Makefile targets are:
make all        # compile all .c -> .o, link into dist/main.exe
make clean      # remove dist/* and all .o files
```

There is no test framework or linter configured. Validation is done by running
the built executable in DOSBox or similar DOS emulator.

### Asset Pipeline

Assets are packed into `datos.dat` via `static/build.sh` which calls the
Allegro `dat` CLI tool repeatedly. The generated index header is
`src/statics.h` -- DO NOT edit it by hand. It is auto-generated and copied
from `static/statics.h` to `src/statics.h` at the end of the asset build.

## Code Style Guidelines

### Language and Compiler

- C89/C99 compiled with DJGPP GCC (`-fgnu89-inline`).
- DOS-specific headers are used: `<pc.h>`, `<dos.h>`.
- Allegro 4 API is used throughout; never use Allegro 5 functions.

### File Organization

- Each module is a `.c` / `.h` pair: `player.c`/`player.h`, `enemy.c`/`enemy.h`, etc.
- Headers contain `#ifndef GUARD_H` / `#define GUARD_H` / `#endif` include guards.
  Use the pattern `MODULE_H` (e.g. `PLAYER_H`, `ENEMY_H`, `GAME_H`).
- Public function declarations and `extern` variables go in the `.h` file.
- Static (file-private) functions and variables stay in the `.c` file only.
- Shared types live in `helpers.h` (`coordsType`, `collisionType`, `animeItem`).
- Asset index constants (e.g. `TILES_BMP`, `INTRO_MID`) live in `statics.h`.

### Includes

- Own module header comes first: `#include "player.h"`.
- Then other project headers in alphabetical order: `#include "book.h"`, etc.
- Then Allegro sub-headers: `#include "allegro/gfx.h"`.
- Then `<allegro.h>` and standard library headers last: `<stdio.h>`, `<math.h>`.
- Example ordering in a `.c` file:

```c
#include "enemy.h"        // own header
#include "dat_manager.h"  // project headers
#include "errors.h"
#include "game.h"
#include "helpers.h"
#include "player.h"
#include "statics.h"
#include <allegro.h>      // allegro
#include <stdio.h>        // stdlib
```

### Formatting

- 4-space indentation (no tabs).
- Opening brace on the same line as the function/control statement.
- Single blank line between function definitions.
- Max line length: keep under ~120 characters where practical.

```c
void player_init(int x, int y, int current_level, int max_vx) {
    player.pos.x = x;
    player.pos.y = y;
    // ...
}
```

### Naming Conventions

| Element              | Convention              | Example                        |
|----------------------|-------------------------|--------------------------------|
| Functions            | `snake_case`            | `player_update`, `load_tiles`  |
| Local variables      | `snake_case`            | `scroll_x`, `frame_width`     |
| Struct types         | `PascalCase` typedef    | `PlayerData`, `EnemyData`      |
| Simple types         | `camelCase` typedef     | `coordsType`, `collisionType`  |
| Enum types           | `PascalCase`            | `EnemyType`, `playerEnum`      |
| Enum values          | `UPPER_SNAKE_CASE`      | `ENEMY_JOVEN`, `CAR_TYPE`      |
| #define constants    | `UPPER_SNAKE_CASE`      | `MAX_ACTIVE_ENEMIES`, `TILES_SIZE` |
| Game-state defines   | `UPPER_SNAKE_CASE`      | `GAME_RUN`, `PLAYER_FALL`      |
| Static/private funcs | `_prefixed_snake_case`  | `_load_enemy_generic`          |
| Global variables     | `snake_case`            | `game_pause`, `current_level`  |

### Types

- Use Allegro types: `BITMAP*`, `PALETTE`, `RGB`, `DATAFILE*`.
- Use `int` for most integers; `short` for small game-state flags.
- Use `uint8_t` for byte-sized counters or flags.
- Use `float` sparingly (only `GRAVITY`). Prefer integer math for performance.
- Use `TRUE`/`FALSE` macros from Allegro (not `true`/`false`).
- Return `collisionType` structs by value from collision functions.
- Use C99 designated initializers for structs:
  `(collisionType){ .x = 0, .y = 0, .w = 0, .h = 0 }`.

### Error Handling

- Use `die(format, ...)` from `errors.h` for fatal errors. It prints the
  message, resets to text mode, and calls `exit(1)`.
- Check return values of Allegro init functions (`allegro_init`, `set_gfx_mode`,
  `install_sound`) and exit or `die()` on failure.
- Validate pointers before dereferencing: check `player.data != NULL`,
  `sprite_index < total_frames`, etc. to avoid SIGSEGV.
- Bounds-check array indices (enemy pool slots, sprite indices, tile coords).

### Game Architecture Patterns

- **FSM (Finite State Machine)**: Player and enemies use state machines.
  States are `#define` constants (`STOP`, `MOVE_LEFT`, `JUMP_UP`, `DEAD`, etc.).
  Each state has an `action_` function. State transitions use `_change_state()`.
- **Object Pool**: Enemies use a static/active pool pattern.
  `spawnable_enemies[]` holds level data; `active_enemies[]` holds live instances.
  Pool size is capped by `MAX_ACTIVE_ENEMIES` / `MAX_SPAWNABLE_ENEMIES`.
- **Sprite Animation**: Defined by `animeItem` structs with frame arrays,
  intervals, and lengths. Updated each tick via `_anime_update()` functions.
- **Collision Detection**: AABB-based. `collisionType` boxes are obtained via
  `_get_all_aabb()` and checked with `collision()` in `object.c`.
- **Scrolling**: Camera follows the player via `scroll_x`. Sprites draw at
  `pos.x - scroll_x`. Background is blitted with scroll offset.
- **Data files**: All assets are packed in `datos.dat` and accessed via
  `dat_file[CONSTANT].dat`. Never load assets from loose files at runtime.
- **SOLID**: Try to keep game loop in game.c, enemies behaviour in enemy.c,
 player in player.c, collisions in object.c, tiles management in tiles.c, 
 datafile management in dat_manager.c

### Comments

- Use Doxygen-style `/** @brief ... */` comments for public functions in headers.
- Use `//` single-line comments for inline explanations in `.c` files.
- Comments may be in English or Spanish. Both are acceptable.
- Mark incomplete work with `// TODO:` comments.

### Allegro-Specific

- Timer callbacks must be wrapped with `END_OF_FUNCTION()` and variables
  with `LOCK_VARIABLE()` / `LOCK_FUNCTION()` for DJGPP compatibility.
- `END_OF_MAIN()` must appear after `main()`.
- Screen is 320x200 VGA. Use `SCREEN_W` and `SCREEN_H` constants.
- The palette is global (`palette` in `tiles.c`). Palette operations use
  `set_palette()`, `get_pallete()`, `set_color()`.
- pc.h is on {DJGPP_PATH}/i586-pc-msdosdjgpp/sys-include/pc.h

### Performance
- This game is intended for a 486 DX, so avoid multiplications and divisions when possible.
- Try to reuse existing code.
