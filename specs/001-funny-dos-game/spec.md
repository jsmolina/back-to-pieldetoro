# Feature Specification: Funny DOS Game

**Feature Branch**: `001-funny-dos-game`  
**Created**: 2026-05-12  
**Status**: Draft  
**Input**: User description: "Build a videogame for ms-dos that is funny. Enemies and maps are defined in *.tmx files, each file has own responsability. We prefer readability but keeping performance."

## User Scenarios & Testing *(mandatory)*

### User Story 1 - Play a level (Priority: P1)

As a player, I want to load a level and play through it, encountering enemies and obstacles defined
in the level's .tmx file so I can enjoy a short, humorous DOS game experience.

**Why this priority**: Core playable loop that delivers the primary user experience.

**Independent Test**: Manually run the built executable in DOSBox, load the target .tmx level file,
observe that map tiles render correctly, player can move, enemies spawn and interact, and the level
can be completed or failed.

**Acceptance Scenarios**:

1. **Given** the game is started and level file `level1.tmx` is present, **When** the player selects
   "Start", **Then** the level loads and player can control the character using configured keys.
2. **Given** an enemy defined in the level's .tmx file, **When** the player approaches, **Then** the
   enemy behaves according to its type (patrol, chase, stationary) and causes expected damage.

---

### User Story 2 - Funny enemy interactions (Priority: P2)

As a player, I want enemies to have amusing behaviors and reactions so the game feels comical.

**Why this priority**: Adds personality and entertainment value after basic play is stable.

**Independent Test**: Manually observe defined humorous behaviors (e.g., enemies slip on banana
tiles, emit a humorous sound, or perform a silly animation) when interacting with the player.

**Acceptance Scenarios**:

1. **Given** an enemy with type `slippery`, **When** it walks over a `banana` tile, **Then** it plays
   a slip animation and briefly stops.

---

## Edge Cases

- What happens if a .tmx file references a missing sprite or tileset? The game should fail to load
  the level and show a clear error message for manual debugging.
- If an enemy definition contains invalid parameters, the engine should skip the invalid enemy and
  continue loading the level while logging the problem to a file for diagnosis.

## Requirements *(mandatory)*

### Functional Requirements

- **FR-001**: The game MUST load levels from `.tmx` files placed in the `static/` directory.
- **FR-002**: Each `.tmx` file is responsible for its own map layout and enemy spawn definitions;
  the engine MUST interpret enemy definitions from the `.tmx` objects layer.
- **FR-003**: The engine MUST render map tiles, sprites, and animations at 320x200 VGA resolution.
- **FR-004**: Player controls MUST be responsive.
- **FR-005**: The engine MUST avoid runtime multiplications/divisions in hot loops; prefer integer
  arithmetic and lookup tables where performance-critical.

### Key Entities *(if feature involves data)*

- **Level (.tmx)**: Contains tile layers, object layers for spawn points.
- **enemy.c**: File responsible of enemy behaviour.
- **tiles.bmp**: 8x8 tiles reused by maps in tmx files.

## Success Criteria *(mandatory)*

### Measurable Outcomes

- **SC-001**: A level described by a valid `.tmx` file loads and is playable within 5 seconds of
  selecting Start on a typical development machine running the canonical Docker build.
- **SC-002**: Player input latency remains imperceptible for a human player (no noticeable lag
  during movement or jump actions in manual testing).
- **SC-003**: No crashes occur when loading valid `.tmx` files; invalid references produce a
  diagnostic log entry and a clear error message.

## Assumptions

- The project will continue to use Allegro 4.2.2 and DJGPP cross-compile toolchain.  
- Level files are authored in Tiled (`.tmx`) and placed in `static/maps/`.  
- Manual testing will be used to validate behaviour (see project constitution: manual testing policy).
