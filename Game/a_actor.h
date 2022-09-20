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

typedef struct actor_state actor_state_t;

typedef struct {
    actor_type_t type;

    vec2_t position; // in world pixels, the bottom center of the visible sprite
    vec2_t velocity;
    vec2_t old_position;
    actor_flags_t flags;

    sprite_t * sprite;
    float current_frame;
    u8 hitbox_width;
    u8 hitbox_height;
    
    actor_state_t * state;
    int state_timer;

    SDL_Rect hitbox;
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

void SpawnActor
(   actor_type_t type,
    vec2_t position,
    actor_t * array,
    int * array_count );
sprite_t * GetActorSprite(const actor_t * actor);
void UpdateActor(actor_t * actor, float dt);

/// Actor's visible rect in world pixel space.
SDL_Rect GetActorVisibleRect(const actor_t * actor);

/// Actor's hitbox in world pixel space.
SDL_FRect ActorHitbox(const actor_t * actor);
SDL_Rect HitBox(sprite_t * sprite, vec2_t actor_position);
vec2_t PositionFromHitbox(const actor_t * actor, SDL_FRect hitbox);

bool ActorCollisionWithRect(actor_t * actor, SDL_Rect rect, vec2_t * point);
void ClipActor(actor_t * actor, SDL_Rect actor_hitbox, SDL_Rect rect);

// -----------------------------------------------------------------------------
// a_definitions.c

/// Get an actor type's definition.
///
/// An actor definition is a template used when creating new actors.
actor_t GetActorDefinition(actor_type_t type);

#endif /* actor_h */
