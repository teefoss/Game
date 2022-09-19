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

#define MAX_ACTORS 5000

typedef enum {
    ACTOR_FLAG_REMOVE   = 0x0001,
    ACTOR_FLAG_SOLID    = 0x0002,
} actor_flags_t;

typedef struct actor actor_t;
typedef struct actor_state actor_state_t;

struct actor {
    actor_type_t type;

    vec2_t position; // in tiles, the bottom center of the visible sprite
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

// -----------------------------------------------------------------------------
// a_main.c

void SpawnActor
(   actor_type_t type,
    vec2_t position,
    actor_t * array,
    int * array_count );
sprite_t * GetActorSprite(const actor_t * actor);
void UpdateActor(actor_t * actor, float dt);
SDL_Rect ActorRect(const actor_t * actor);

// -----------------------------------------------------------------------------
// a_definitions.c

/// Get an actor type's definition.
///
/// An actor definition is a template used when creating new actors.
actor_t GetActorDefinition(actor_type_t type);

#endif /* actor_h */
