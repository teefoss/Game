//
//  w_world.h
//  Game
//
//  Created by Thomas Foster on 9/11/22.
//

#ifndef world_h
#define world_h

#include "a_actor.h"
#include "game.h"
#include "mylib/mathlib.h"

#define WORLD_WIDTH  512
#define WORLD_HEIGHT 512
#define TILE_SIZE 16
#define CHUNCK_SIZE 32
#define MAX_ACTORS 5000

#define DAY_LENGTH_TICKS    (int)(1200000.0f / (1000.0f / FPS))
#define HOUR_TICKS          (DAY_LENGTH_TICKS / 24)
#define MORNING_START_TICKS (HOUR_TICKS * 6)
#define MORNING_END_TICKS   (HOUR_TICKS * 7)
#define NOON_TICKS          (HOUR_TICKS * 12)
#define DUSK_START_TICKS    (HOUR_TICKS * 20) // 8 PM
#define DUSK_END_TICKS      (HOUR_TICKS * 21) // 9 PM

typedef enum {
    TERRAIN_DEEP_WATER,
    TERRAIN_SHALLOW_WATER,
    TERRAIN_GRASS,
    TERRAIN_FOREST,
    TERRAIN_DARK_FOREST,
    TERRAIN_END,
    NUM_TERRAIN_TYPES,
} terrain_t;

typedef struct {
    terrain_t terrain;

    // Some tiles have extra, per-tile decoration/
    // visual effects that are laid over the regular texture
    // This can be generated dynamically as needed.
    SDL_Texture * effect;

    // A value that can be used to
    // randomize various tile properties.
    u8 variety;

    // Determined by world lighting and any nearby light-casting actors.
    vec3_t lighting;
} tile_t;

typedef struct world {
    tile_t tiles[WORLD_WIDTH * WORLD_HEIGHT];

    actor_t actors[MAX_ACTORS];
    int num_actors;

    // The world pixel coordinate that's centered on screen.
    vec2_t camera;

    int clock;

    // Lighting is a color mod value that's applied to textures.
    // Tiles get their lighting from the world, then from any actors
    // that cast light. Actors in turn get their lighting from the tile
    // they stand on.
    vec3_t lighting;

    SDL_Texture * debug_map; // rendering of entire world, for debuggery

    void (* draw)(tile_t * tile);
} world_t;

/// Allocates and creates the world.
///
/// - Returns: A pointer to the allocated world. Caller should free the pointer.
world_t * CreateWorld(void);

tile_t * GetTile(tile_t * tiles, int x, int y);
void GetVisibleTileRange(world_t * world, SDL_Point * min, SDL_Point * max);
SDL_Rect GetVisibleRect(vec2_t camera);

void GetAdjacentTiles
(   int x,
    int y,
    tile_t * world_tiles,
    tile_t * out[NUM_DIRECTIONS] );

void RenderWorld(world_t * world, bool show_hitboxes);
void DestroyWorld(world_t * world); // maybe FreeWorld would be more positive?
void UpdateWorld(world_t * world, float dt);

void UpdateDebugMap(tile_t * tiles,  SDL_Texture ** debug_map, vec2_t camera);

#endif /* world_h */
