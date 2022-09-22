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

static void UpdateActors(world_t * world, float dt)
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
            vec2_t velocity = Vec2Scale(actor->velocity, dt);
            actor->position = Vec2Add(actor->position, velocity);
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


void UpdateWorld(world_t * world, float dt)
{
    int update_start = SDL_GetTicks(); // debug

    if ( ++world->clock > DAY_LENGTH_TICKS ) {
        world->clock = 0;
    }

    extern int debug_hours;
    extern int debug_minutes;
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

    UpdateTiles(world);
    UpdateActors(world, dt);

    update_ms = SDL_GetTicks() - update_start; // debug
}
