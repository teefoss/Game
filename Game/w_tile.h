//
//  w_tile.h
//  Game
//
//  Created by Thomas Foster on 9/26/22.
//

#ifndef w_tile_h
#define w_tile_h

#include "cardinal.h"
#include "mylib/genlib.h"
#include "mylib/types.h"
#include "mylib/vector.h"
#include <SDL.h>

#define TILE_SIZE 16 // sprite size in pixels

typedef enum {
    TERRAIN_DEEP_WATER,
    TERRAIN_SHALLOW_WATER,
    TERRAIN_GRASS,
    TERRAIN_FOREST,
    TERRAIN_DARK_FOREST,
    TERRAIN_END,
    NUM_TERRAIN_TYPES,
} terrain_t;

typedef struct tile tile_t;
struct tile {
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

    void (* render)(tile_t *);
};

void GetTileNoise(int tile_x, int tile_y, float out[TILE_SIZE][TILE_SIZE]);

#endif /* w_tile_h */
