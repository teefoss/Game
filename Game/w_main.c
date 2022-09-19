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

SDL_Rect GetVisibleRect(vec2_t camera)
{
    SDL_Rect r = {
        .x = camera.x * TILE_SIZE - GAME_WIDTH / 2,
        .y = camera.y * TILE_SIZE - GAME_HEIGHT / 2,
        .w = GAME_WIDTH,
        .h = GAME_HEIGHT
    };

    return r;
}

void UpdateWorld(world_t * world, float dt)
{
    actor_t * active_actors[MAX_ACTORS] = { 0 };
    int num_active = 0;

    // Calculate the active rect:
    // the visible rect + 'tile_margin' tiles on all sides
    SDL_Rect active_rect = GetVisibleRect(world->camera);
    int tile_margin = 8;
    active_rect.w += tile_margin * TILE_SIZE * 2;
    active_rect.h += tile_margin * TILE_SIZE * 2;
    active_rect.x -= tile_margin * TILE_SIZE;
    active_rect.y -= tile_margin * TILE_SIZE;

    // Add all actors within the active rect, they will be processed.
    for ( int i = 0; i < world->num_actors; i++ ) {
        actor_t * actor = &world->actors[i];

        if ( RectsIntersect(ActorRect(actor), active_rect) ) {
            if ( num_active < MAX_ACTORS ) {
                active_actors[num_active++] = actor;
            }
        }
    }

    // Let any actors that respond to input do so.
    for ( actor_t ** actor = active_actors; *actor; actor++ ) {
        if ( (*actor)->state && (*actor)->state->handle_input ) {
            (*actor)->state->handle_input(*actor);
        }
    }

    // Update actors.
    for ( actor_t ** actor = active_actors; *actor; actor++ ) {
        UpdateActor(*actor, dt);
        if ( (*actor)->type == ACTOR_PLAYER ) { // TODO: this goes elsewhere
            LerpVector(&world->camera, &(*actor)->position, 0.1f);
        }
    }

    // Handle any collisions.
    for ( int i = 0; i < num_active; i++ ) {
        for ( int j = i + 1; j < num_active; j++ ) {
            actor_t * ai = active_actors[i];
            actor_t * aj = active_actors[j];

            if ( RectsIntersect(ActorRect(ai), ActorRect(aj)) ) {
                if ( ai->flags & ACTOR_FLAG_SOLID ) {
                    // TODO: correct aj's position
                } else if ( aj->flags & ACTOR_FLAG_SOLID ) {
                    // TODO: correct ai's position
                }

                // contact each other
                if ( ai->state && ai->state->contact ) {
                    ai->state->contact(ai, aj);
                }
                if ( aj->state && aj->state->contact ) {
                    aj->state->contact(aj, ai);
                }
            }
        }
    }

    // Remove any actors that were flagged for removal. Loop through actual
    // world actor array when removing so that the array will stay packed.
    for ( int i = world->num_actors - 1; i >= 0; i-- ) {
        if ( world->actors[i].flags & ACTOR_FLAG_REMOVE ) {
            // Fast remove: move the last element to the element to be removed.
            world->actors[i] = world->actors[--world->num_actors];
        }
    }
}
