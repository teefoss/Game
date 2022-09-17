//
//  gobject.c
//  Game
//
//  Created by Thomas Foster on 9/12/22.
//

#include "a_actor.h"
#include "mylib/genlib.h"

sprite_t * GetActorSprite(actor_t * actor)
{
    if ( actor->state != NULL ) {
        return actor->state->sprite;
    } else {
        return actor->sprite;
    }
}

void SpawnActor(actor_type_t type, vec2_t position, actor_storage_t * storage)
{
    actor_t actor = GetActorDefinition(type);
    actor.type = type;
    actor.position = position;

    AddActor(storage, actor);
}

void UpdateActor(actor_t * actor, float dt)
{
    // update position
    vec2_t scaled_velocity = ScaleVector(actor->velocity, dt);
    actor->position = AddVectors(actor->position, scaled_velocity);

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
            actor->state->update(actor);
        }
    }
}
