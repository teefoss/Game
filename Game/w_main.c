//
//  w_main.c
//  Game
//
//  Created by Thomas Foster on 9/11/22.
//
//  General world code and utilities functions.

#include "w_world.h"
#include "game.h"
#include "sprites.h"

#include "mylib/genlib.h"
#include "mylib/input.h"
#include "mylib/video.h"

#include <stdlib.h>

tile_t * GetTile(tile_t * tiles, int x, int y)
{
    if ( x < 0 || x >= WORLD_WIDTH ) {
        return NULL;
    }

    if ( y < 0 || y >= WORLD_HEIGHT ) {
        return NULL;
    }

    return &tiles[y * WORLD_HEIGHT + x];
}

void GetAdjacentTiles
(   int x,
    int y,
    tile_t * world_tiles,
    tile_t * out[NUM_DIRECTIONS] )
{
    out[NORTH] = GetTile(world_tiles, x, y - 1);
    out[SOUTH] = GetTile(world_tiles, x, y + 1);
    out[EAST]  = GetTile(world_tiles, x + 1, y);
    out[WEST]  = GetTile(world_tiles, x - 1, y);
}

void DestroyWorld(world_t * world)
{
    for ( int i = 0; i < WORLD_WIDTH * WORLD_HEIGHT; i++ ) {
        if ( world->tiles[i].effect ) {
            SDL_DestroyTexture(world->tiles[i].effect);
        }
    }
    
    free(world);
}

void UpdateWorld(world_t * world, float dt)
{
    // Let any actors that respond to input do so.
    for ( int i = 0; i < world->actors.count; i++ ) {
        actor_t * actor = &world->actors.array[i];

        if ( actor->state && actor->state->handle_input ) {
            actor->state->handle_input(actor);
        }
    }

    // Update actors.
    for ( int i = 0; i < world->actors.count; i++ ) {
        actor_t * actor = &world->actors.array[i];

        UpdateActor(actor, dt);
        if ( actor->type == ACTOR_PLAYER ) {
            world->camera = actor->position;
        }
    }

    // TODO: handle collisions

    // remove any removeable actors
    for ( int i = world->actors.count - 1; i >= 0; i-- ) {
        if ( world->actors.array[i].flags & ACTOR_FLAG_REMOVE ) {
            RemoveActor(&world->actors, i);
        }
    }
}
