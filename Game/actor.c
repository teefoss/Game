//
//  gobject.c
//  Game
//
//  Created by Thomas Foster on 9/12/22.
//

#include "actor.h"
#include "mylib/genlib.h"

actor_t * AddActor(actor_storage_t * storage, actor_type_t type)
{
    actor_t actor = {
        .type = type,
    };

    if ( storage->num_actors == MAX_ACTORS ) {
        printf("%s: out of space, please increase MAX_ACTORS\n", __func__);
    }

    storage->array[storage->num_actors] = actor;
    storage->removed[storage->num_actors] = false;
    storage->num_actors++;

    return &storage->array[storage->num_actors - 1];
}

void FlagActorForRemoval(actor_storage_t * storage, int index)
{
    if ( index >= storage->num_actors ) {
        Error("index out of range");
    }

    storage->removed[index] = true;
    //storage->array[index] = storage->array[--storage->num_actors];
}

void CleanActorStorage(actor_storage_t * storage)
{
    for ( int i = storage->num_actors - 1; i >= 0; i-- ) {
        if ( storage->removed[i] ) {
            storage->array[i] = storage->array[--storage->num_actors];
        }
    }
}
