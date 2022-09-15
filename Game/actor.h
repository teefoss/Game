//
//  actor.h
//  Game
//
//  Created by Thomas Foster on 9/12/22.
//

#ifndef actor_h
#define actor_h

#include "mylib/mathlib.h"
#include "mylib/sprite.h"

#define MAX_ACTORS 1000

typedef enum {
    ACTOR_PLAYER,
} actor_type_t;

typedef struct actor actor_t;
typedef struct actor_state actor_state_t;

struct actor_state {
    int ticks; // when zero, change to next state
    int length; // state length in ticks
    actor_state_t * next_state;

    sprite_t * sprite; // &

    void (* update)(actor_t * self);
    void (* contact)(actor_t * self, actor_t * other);
};

struct actor {
    actor_type_t type;

    vec2_t position;
    vec2_t velocity;

    actor_state_t * state;
};

typedef struct {
    actor_t array[MAX_ACTORS];
    bool removed[MAX_ACTORS];
    u16 num_actors;
} actor_storage_t;

actor_t * AddActor(actor_storage_t * storage, actor_type_t type);
void FlagActorForRemoval(actor_storage_t * storage, int index);

/// Remove any actors that were flagged for removal
void CleanActorStorage(actor_storage_t * storage);

#endif /* actor_h */
