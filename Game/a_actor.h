//
//  a_actor.h
//  Game
//
//  Created by Thomas Foster on 9/12/22.
//

#ifndef actor_h
#define actor_h

#include "a_types.h"
#include "mylib/mathlib.h"
#include "mylib/sprite.h"

#define MAX_ACTORS 1000

typedef enum {
    ACTOR_FLAG_SOLID = 0x0001,
} actor_flags_t;

typedef struct actor actor_t;
typedef struct actor_state actor_state_t;

struct actor {
    actor_type_t type;

    vec2_t position; // the bottom center of the visible sprite
    vec2_t velocity;
    actor_flags_t flags;

    sprite_t * sprite;
    float current_frame;
    
    actor_state_t * state;
    int state_timer;
};

struct actor_state {
    int length; // state length in ticks
    actor_state_t * next_state;
    sprite_t * sprite; // &sprites[id]

    void (* handle_input)(actor_t * self);
    void (* update)(actor_t * self, float dt);
    void (* contact)(actor_t * self, actor_t * other);
};

typedef struct {
    actor_t array[MAX_ACTORS];
    bool removed[MAX_ACTORS];
    u16 num_actors;
} actor_storage_t;

// -----------------------------------------------------------------------------
// a_main.c

void SpawnActor(actor_type_t type, vec2_t position, actor_storage_t * storage);
sprite_t * GetActorSprite(actor_t * actor);
void UpdateActor(actor_t * actor, float dt);

// -----------------------------------------------------------------------------
// a_definitions.c

/// Get an actor type's definition.
///
/// An actor definition is a template used when creating new actors.
actor_t GetActorDefinition(actor_type_t type);

// -----------------------------------------------------------------------------
// a_storage.c

actor_t * AddActor(actor_storage_t * storage, actor_t actor);

/// Flag an actor for removal, to be removed later with `CleanActorStorage`.
void FlagActorForRemoval(actor_storage_t * storage, int index);

/// Remove any actors that were flagged for removal.
void CleanActorStorage(actor_storage_t * storage);

#endif /* actor_h */
