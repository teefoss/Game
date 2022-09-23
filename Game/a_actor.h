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

typedef enum {
    NORTH,
    SOUTH,
    EAST,
    WEST,
    // WEAST,
    NUM_DIRECTIONS,
} cardinal_t;

typedef enum {
    ACTOR_FLAG_REMOVE           = 0x0001,
    ACTOR_FLAG_SOLID            = 0x0002,

    // don't check if tile is walkable
    ACTOR_FLAG_FLY              = 0x0004,

    // don't do collision checking
    ACTOR_FLAG_NONINTERACTIVE   = 0x0008,
} actor_flags_t;

typedef struct actor_state actor_state_t;
typedef struct world world_t;

typedef struct actor {
    actor_type_t type;

    vec2_t position; // in world pixels, the bottom center of the visible sprite

    // 0 if actor is on the ground. This doesn't affect anything except
    // where an actor's sprite is rendered.
    int z;

    vec2_t velocity;
    actor_flags_t flags;
    cardinal_t direction;

    sprite_t * sprite;
    float current_frame;

    // Actors get their lighting from the tile they're standing on.
    vec3_t lighting;

    // An actor's hitbox is centered on its x position and the hitbox's
    // bottom aligns with the actor's y position.
    u8 hitbox_width;
    u8 hitbox_height;
    
    actor_state_t * state;
    int state_timer; // 0 = advance to next state

    union {
        s16 timer;
    } info;

    // Actors may change the world, so keep an internal reference.
    world_t * world;
} actor_t;

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

actor_t * SpawnActor(actor_type_t type, vec2_t position, world_t * world);
sprite_t * GetActorSprite(const actor_t * actor);
void UpdateActor(actor_t * actor, float dt);

/// Actor's visible rect in world pixel space.
SDL_Rect GetActorVisibleRect(const actor_t * actor);

/// Actor's hitbox in world pixel space.
SDL_FRect ActorHitbox(const actor_t * actor);

void DoCollisions(bool vertical, actor_t * actor, actor_t ** blocks, int num_blocks);

// -----------------------------------------------------------------------------
// a_definitions.c

/// Get an actor type's definition.
///
/// An actor definition is a template used when creating new actors.
actor_t GetActorDefinition(actor_type_t type);

#endif /* actor_h */
