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
        .x = camera.x - GAME_WIDTH / 2,
        .y = camera.y - GAME_HEIGHT / 2,
        .w = GAME_WIDTH,
        .h = GAME_HEIGHT
    };

    return r;
}

void ResolveHorizontalCollision
(   actor_t * actor,
    SDL_FRect actor_box,
    SDL_FRect block_box )
{
    if ( actor->velocity.x > 0 ) { // clip to left side
        actor_box.x = block_box.x - actor_box.w;
    } else if ( actor->velocity.x < 0 ) { // clip to right side
        actor_box.x = block_box.x + block_box.w;
    }

    actor->position = PositionFromHitbox(actor, actor_box);
    actor->velocity.x = 0;
}

void ResolveVerticalCollision
(   actor_t * actor,
    SDL_FRect actor_box,
    SDL_FRect block_box )
{
    if ( actor->velocity.y > 0 ) { // clip to top side
        actor_box.y = block_box.y - actor_box.h;
    } else if ( actor->velocity.y < 0 ) { // clip to bottom side
        actor_box.y = block_box.y + block_box.h;
    }

    actor->position = PositionFromHitbox(actor, actor_box);
    actor->velocity.y = 0;
}

void DoCollisions(bool vertical, actor_t * actor, actor_t ** blocks, int num_blocks)
{
    SDL_FRect ibox = ActorHitbox(actor);

    for ( int j = 0; j < num_blocks; j++ ) {
        SDL_FRect jbox = ActorHitbox(blocks[j]);

        if ( SDL_HasIntersectionF(&ibox, &jbox) ) {
            if ( vertical ) {
                ResolveVerticalCollision(actor, ibox, jbox);
            } else {
                ResolveHorizontalCollision(actor, ibox, jbox);
            }
        }
    }
}

void UpdateWorld(world_t * world, float dt)
{
    actor_t * active_actors[MAX_ACTORS] = { 0 };
    actor_t * blocks[MAX_ACTORS] = { 0 };
    int num_active = 0;
    int num_blocks = 0;

    // Calculate the active rect:
    // the visible rect + 'tile_margin' tiles on all sides
    SDL_Rect active_rect = GetVisibleRect(world->camera);
    int tile_margin = 8;
    active_rect.w += tile_margin * TILE_SIZE * 2;
    active_rect.h += tile_margin * TILE_SIZE * 2;
    active_rect.x -= tile_margin * TILE_SIZE;
    active_rect.y -= tile_margin * TILE_SIZE;

    // Add all actors within the active rect, they will be processed.
    // Add solid actors to a separate list of blocks.
    for ( int i = 0; i < world->num_actors; i++ ) {
        actor_t * actor = &world->actors[i];

        if ( RectsIntersect(GetActorVisibleRect(actor), active_rect) ) {
            if ( num_active < MAX_ACTORS ) {
                active_actors[num_active++] = actor;
                if ( actor->flags & ACTOR_FLAG_SOLID ) {
                    blocks[num_blocks++] = actor;
                }
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
    for ( int i = 0; i < num_active; i++ ) {
        actor_t * actor = active_actors[i];

        // Move actors.
        if ( !(actor->flags & ACTOR_FLAG_NONINTERACTIVE) ) {
            // Do horizontal and vertical movement separately, resolving
            // collisions with solid actors at each step.

            // horizontal movement:
            if ( actor->velocity.x ) {
                actor->position.x += actor->velocity.x * dt;
                DoCollisions(false, actor, blocks, num_blocks);
            }

            // vertical movement:
            if ( actor->velocity.y ) {
                actor->position.y += actor->velocity.y * dt;
                DoCollisions(true, actor, blocks, num_blocks);
            }
        } else {
            vec2_t velocity = ScaleVector(actor->velocity, dt);
            actor->position = AddVectors(actor->position, velocity);
        }

        UpdateActor(actor, dt);
    }

    // Handle any collisions with interactable objects (non-solid things).
    for ( int i = 0; i < num_active; i++ ) {
        actor_t * ai = active_actors[i];

        if ( ai->flags & ACTOR_FLAG_REMOVE ) {
            continue;
        }

        SDL_FRect hitbox_i = ActorHitbox(ai);

        for ( int j = i + 1; j < num_active; j++ ) {
            actor_t * aj = active_actors[j];

            if ( aj->flags & ACTOR_FLAG_REMOVE ) {
                continue;
            }

            if ( ai->flags & ACTOR_FLAG_SOLID || aj->flags & ACTOR_FLAG_SOLID ) {
                continue; // don't bother
            }

            SDL_FRect hitbox_j = ActorHitbox(aj);
            if ( SDL_HasIntersectionF(&hitbox_i, &hitbox_j) ) {

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
