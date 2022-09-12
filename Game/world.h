//
//  world.h
//  Game
//
//  Created by Thomas Foster on 9/11/22.
//

#ifndef world_h
#define world_h

#include "mylib/mathlib.h"

#define WORLD_WIDTH  512
#define WORLD_HEIGHT 512
#define TILE_SIZE 16

typedef enum {
    NORTH,
    SOUTH,
    EAST,
    WEST,
    // WEAST,
    NUM_DIRECTIONS,
} cardinal_t;

typedef enum {
    TERRAIN_DEEP_WATER,
    TERRAIN_SHALLOW_WATER,
    TERRAIN_BEACH,
    TERRAIN_GRASS,
    TERRAIN_FOREST,
    TERRAIN_DARK_FOREST,
    TERRAIN_END,
    NUM_TERRAIN_TYPES,
} terrain_t;

typedef struct tile tile_t;
struct tile {
    terrain_t terrain;

    // Some tiles have extra, per-tile
    // visual effects. This can be generated
    // dynamically as needed.
    SDL_Texture * effect;

    // A value that can be used to
    // randomize various tile properties.
    u8 variety;
};

typedef struct {
    tile_t tiles[WORLD_WIDTH * WORLD_HEIGHT];
    SDL_Texture * debug_texture; // rendering of entire world

    // debug
    bool effect_textures_on;

    // The camera unit is world tiles, e.g., camera = { 12.5, 15.5 } means
    // the center of tile (12, 15) is centered on screen.
    vec2_t camera;
} world_t;

/// Allocates and creates the world.
///
/// - Returns: A pointer to the allocated world. Caller should free the pointer.
world_t * CreateWorld(void);

void RenderWorld(world_t * world);
void DestroyWorld(world_t * world); // maybe FreeWorld would be more positive?

#endif /* world_h */
