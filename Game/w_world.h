//
//  w_world.h
//  Game
//
//  Created by Thomas Foster on 9/11/22.
//

#ifndef world_h
#define world_h

#include "w_tile.h"
#include "a_actor.h"
#include "coord.h"
#include "g_game.h"
#include "mylib/array.h"
#include "mylib/mathlib.h"

#define WORLD_WIDTH  512
#define WORLD_HEIGHT 512
#define SCALED_TILE_SIZE (TILE_SIZE * DRAW_SCALE)
#define CHUNK_SIZE 16
#define CHUNK_LOAD_RADIUS_TILES 24

#define DAY_LENGTH_TICKS    (int)(1200000.0f / (1000.0f / FPS))
#define HOUR_TICKS          (DAY_LENGTH_TICKS / 24)
#define MORNING_START_TICKS (HOUR_TICKS * 6)
#define MORNING_END_TICKS   (HOUR_TICKS * 7)
#define NOON_TICKS          (HOUR_TICKS * 12)
#define DUSK_START_TICKS    (HOUR_TICKS * 20) // 8 PM
#define DUSK_END_TICKS      (HOUR_TICKS * 21) // 9 PM

#define PENDING_ACTORS_MAX 200

typedef struct world {
    bool loaded_chunks[WORLD_HEIGHT / CHUNK_SIZE][WORLD_WIDTH / CHUNK_SIZE];
    tile_t tiles[WORLD_WIDTH * WORLD_HEIGHT];

//    actor_t * actors;
//    int actor_array_capacity; // Total number of actors
//    int num_actors; // Current array count
    array_t * actors;
    array_t * pending_actors;

//    actor_t pending_actors[PENDING_ACTORS_MAX];
//    int num_pending_actors;
    bool updating_actors;

    // The world pixel coordinate that's centered on screen.
    vec2_t camera;
    vec2_t camera_target; // camera lerps to target each frame

    int clock;

    // Lighting is a color mod value that's applied to textures.
    // Tiles get their lighting from the world, then from any actors
    // that cast light. Actors in turn get their lighting from the tile
    // they stand on.
    vec3_t lighting;

    // debug:
    SDL_Texture * debug_map; // rendering of entire world, for debuggery
    actor_t * player;

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

void RenderWorld(world_t * world);
void RenderGrassEffectTexture
(   tile_t * tile,
    tile_t ** adjacent_tiles,
    int tile_x,
    int tile_y );

void DestroyWorld(world_t * world); // maybe FreeWorld would be more positive?

/// Update world clock, lighting, tiles, and actors.
void UpdateWorld
(   world_t * world,
    const control_state_t * control_state,
    float dt );


// TODO: move to debug.c
void UpdateDebugMap(tile_t * tiles,  SDL_Texture ** debug_map, vec2_t camera);

// w_generation.c

void LoadChunkIfNeeded(world_t * world, chunk_coord_t chunk_coord);
void LoadChunkInRegion(world_t * world, position_t center, int tile_radius);

void PlayerUpdateCamera(actor_t * player, float dt);

#endif /* world_h */
