//
//  gobject.c
//  Game
//
//  Created by Thomas Foster on 9/12/22.
//

#include "a_actor.h"
#include "mylib/genlib.h"

static sprite_t * GetActorSprite(actor_t * actor)
{
    if ( actor->state == NULL ) {
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
