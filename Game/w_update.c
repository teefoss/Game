//
//  w_update.c
//  Game
//
//  Created by Thomas Foster on 9/21/22.
//

#include "w_world.h"
#include "m_debug.h"
#include "m_misc.h"
#include "mylib/vector.h"

static void UpdateTiles(world_t * world)
{
    SDL_Point min, max;
    GetVisibleTileRange(world, &min, &max);

    // Extend the range a bit to make sure light is updated and lerped
    // before it comes into view.
    const int margin = 4;
    min.x -= margin;
    max.x += margin;
    min.y -= margin;
    max.y += margin;

    for ( int y = min.y; y <= max.y; y++ ) {
        for ( int x = min.x; x <= max.x; x++ ) {
            tile_t * tile = GetTile(world->tiles, x, y);

            VectorLerp(&tile->lighting, &world->lighting, 0.1f);
        }
    }
}

static void UpdateActors(world_t * world, input_state_t * input_state, float dt)
{
    const int max_active = 1000;
    actor_t * active_actors[max_active] = { 0 };
    actor_t * blocks[max_active] = { 0 };
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
    for ( int i = 0; i < world->actors->count; i++ ) {
        actor_t * actor = GetElement(world->actors, i);

        if ( RectsIntersect(GetActorVisibleRect(actor), active_rect) ) {
            if ( num_active < max_active ) {
                active_actors[num_active++] = actor;
                if ( actor->flags & ACTOR_FLAG_SOLID ) {
                    blocks[num_blocks++] = actor;
                }
            }
        }
    }

    // Any actors spawned during updated will be added to the
    // pending_actors array, to be process on the next frame.
    world->updating_actors = true;

    // Let any actors that respond to input do so.
    if ( input_state ) {
        for ( actor_t ** actor = active_actors; *actor; actor++ ) {
            if ( (*actor)->state && (*actor)->state->handle_input ) {
                (*actor)->state->handle_input(*actor, input_state, dt);
            }
        }
    }

    // Update actors.
    for ( int i = 0; i < num_active; i++ ) {
        actor_t * actor = active_actors[i];

        // Move actors.
        // Do horizontal and vertical movement separately, resolving
        // collisions with solid actors at each step.

        // horizontal movement:
        if ( actor->vel.x ) {
            actor->pos.x += actor->vel.x * dt;
            if ( !(actor->flags & ACTOR_FLAG_NONINTERACTIVE) ) {
                DoCollisions(false, actor, blocks, num_blocks);
            }
        }

        // vertical movement:
        if ( actor->vel.y ) {
            actor->pos.y += actor->vel.y * dt;
            if ( !(actor->flags & ACTOR_FLAG_NONINTERACTIVE) ) {
                DoCollisions(true, actor, blocks, num_blocks);
            }
        }

        UpdateActor(actor, dt);
    }

    // Handle any collisions with interactable objects (non-solid things).
    for ( int i = 0; i < num_active; i++ ) {
        actor_t * ai = active_actors[i];

        SDL_FRect hitbox_i = ActorHitbox(ai);

        for ( int j = i + 1; j < num_active; j++ ) {
            actor_t * aj = active_actors[j];

            SDL_FRect hitbox_j = ActorHitbox(aj);
            if ( SDL_HasIntersectionF(&hitbox_i, &hitbox_j) ) {

                //printf("%s hit an %s\n", ActorName(ai->type), ActorName(aj->type));

                // contact each other
                if ( ai->contact ) {
                    ai->contact(ai, aj);
                } else if ( ai->state && ai->state->contact ) {
                    ai->state->contact(ai, aj);
                }

                if ( aj->contact ) {
                    aj->contact(aj, ai);
                } else if ( aj->state && aj->state->contact ) {
                    aj->state->contact(aj, ai);
                }
            }
        }
    }

    world->updating_actors = false;

    // Remove any actors that were flagged for removal.
    for ( int i = world->actors->count - 1; i >= 0; i-- ) {
        actor_t * actor = GetElement(world->actors, i);
        if ( actor->flags & ACTOR_FLAG_REMOVE ) {
            Remove(world->actors, i);
        }
    }

    // Move all pending actors to main array.
    for ( int i = world->pending_actors->count - 1; i >= 0; i-- ) {
        Append(world->actors, GetElement(world->pending_actors, i));
        Remove(world->pending_actors, i);
    }
}

void UpdateWorld(world_t * world, input_state_t * input_state, float dt)
{
    int update_start = SDL_GetTicks(); // debug

    if ( ++world->clock > DAY_LENGTH_TICKS ) {
        world->clock = 0;
    }

    // update debug info
    debug_hours = world->clock / HOUR_TICKS;
    debug_minutes = (world->clock - debug_hours * HOUR_TICKS) / (HOUR_TICKS / 60);

    // TODO: omg
    if ( world->clock < MORNING_START_TICKS || world->clock >= DUSK_END_TICKS ) {
        world->lighting.x = 32; // TODO: define lighting contants somewhere
        world->lighting.y = 32;
        world->lighting.z = 96;
    } else if ( world->clock >= MORNING_START_TICKS && world->clock < MORNING_END_TICKS ) {
        world->lighting.x
            = MAP(world->clock, MORNING_START_TICKS, MORNING_END_TICKS - 1, 32, 255);
        world->lighting.y
            = MAP(world->clock, MORNING_START_TICKS, MORNING_END_TICKS - 1, 32, 255);
        world->lighting.z
            = MAP(world->clock, MORNING_START_TICKS, MORNING_END_TICKS - 1, 96, 255);
    } else if ( world->clock >= DUSK_START_TICKS && world->clock < DUSK_END_TICKS ) {
        world->lighting.x = MAP(world->clock, DUSK_START_TICKS, DUSK_END_TICKS - 1, 255, 32);
        world->lighting.y = MAP(world->clock, DUSK_START_TICKS, DUSK_END_TICKS - 1, 255, 32);
        world->lighting.z = MAP(world->clock, DUSK_START_TICKS, DUSK_END_TICKS - 1, 255, 96);
    } else {
        world->lighting.x = 255;
        world->lighting.y = 255;
        world->lighting.z = 255;
    }

    // load chunks around the player
    actor_t * player = GetActorType(world->actors, ACTOR_PLAYER);
    LoadChunkInRegion(world, player->pos, CHUNK_LOAD_RADIUS_TILES);

    UpdateTiles(world); // lighting
    UpdateActors(world, input_state, dt);

    update_ms = SDL_GetTicks() - update_start; // debug
}
