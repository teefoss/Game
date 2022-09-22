//
//  a_main.c
//  Game
//
//  Created by Thomas Foster on 9/12/22.
//

#include "a_actor.h"
#include "m_misc.h"
#include "w_world.h"
#include "mylib/genlib.h"

sprite_t * GetActorSprite(const actor_t * actor)
{
    if ( actor->state != NULL ) {
        return actor->state->sprite;
    } else {
        return actor->sprite;
    }
}

void SpawnActor(actor_type_t type, vec2_t position, world_t * world)
{
    actor_t actor = GetActorDefinition(type);
    actor.type = type;
    actor.position = position;
    actor.world = world;

    world->actors[world->num_actors++] = actor;
}

void UpdateActor(actor_t * actor, float dt)
{
    // update actor's facing direction
    if ( actor->velocity.x || actor->velocity.y ) {
        actor->direction = VelocityToDirection(actor->velocity);
    }

    sprite_t * sprite = GetActorSprite(actor);
    if ( sprite ) {

        // Update sprite animation.
        if ( sprite->flags & SPRITE_FLAG_ANIMATED ) {
            actor->current_frame += (float)sprite->fps * dt;
            while ( actor->current_frame >= sprite->num_frames ) {
                actor->current_frame -= sprite->num_frames; // wrap frame if needed
            }
        }

        // Update light.
        // Get which tile the actor is on and apply lighting color mod.
        tile_t * tile = GetTile
        (   actor->world->tiles,
            actor->position.x / TILE_SIZE,
            actor->position.y / TILE_SIZE );

        // TODO: lerp this
        actor->lighting = tile->lighting;
    }

    if ( actor->state ) {
        if ( actor->state->update ) {
            actor->state->update(actor, dt);
        }
    }
}

SDL_Rect GetActorVisibleRect(const actor_t * actor)
{
    sprite_t * sprite = GetActorSprite(actor);
    SDL_Rect rect = {
        .x = actor->position.x,
        .y = actor->position.y,
        .w = sprite ? sprite->location.w : 0,
        .h = sprite ? sprite->location.h : 0
    };

    rect.x -= rect.w / 2;
    rect.y -= rect.h;

    return rect;
}

static vec2_t PositionFromHitbox(const actor_t * actor, SDL_FRect hitbox)
{
    return (vec2_t){
        .x = hitbox.x + actor->hitbox_width / 2.0f,
        .y = hitbox.y + actor->hitbox_height
    };
}

SDL_FRect ActorHitbox(const actor_t * actor)
{
    SDL_FRect hitbox = {
        .x = actor->position.x - actor->hitbox_width / 2.0f,
        .y = actor->position.y - actor->hitbox_height,
        .w = actor->hitbox_width,
        .h = actor->hitbox_height
    };

    return hitbox;
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
