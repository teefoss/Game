//
//  a_list.c
//  Game
//
//  Created by Thomas Foster on 9/15/22.
//

#include "a_actor.h"
#include "sprites.h"

static actor_t actor_definitions[NUM_ACTOR_TYPES] = {
    [ACTOR_TREE] = {
        .flags = ACTOR_FLAG_SOLID,
        .sprite = &sprites[SPRITE_TREE],
    },
};

actor_t GetActorDefinition(actor_type_t type)
{
    return actor_definitions[type];
}
