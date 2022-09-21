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
    if ( actor->velocity.x || actor->velocity.y ) {
        actor->direction = VelocityToDirection(actor->velocity);
    }

    // update sprite animation
    sprite_t * sprite = GetActorSprite(actor);
    if ( sprite && sprite->flags && SPRITE_FLAG_ANIMATED ) {
        actor->current_frame += (float)sprite->fps * dt;
        while ( actor->current_frame >= sprite->num_frames ) {
            actor->current_frame -= sprite->num_frames; // wrap frame if needed
        }
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

vec2_t PositionFromHitbox(const actor_t * actor, SDL_FRect hitbox)
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
