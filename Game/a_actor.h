//
//  a_actor.h
//  Game
//
//  Created by Thomas Foster on 9/12/22.
//

#ifndef actor_h
#define actor_h

#include "a_types.h"
#include "cardinal.h"
#include "mylib/mathlib.h"
#include "mylib/sprite.h"
#include "mylib/input.h"

typedef enum {
    ACTOR_FLAG_REMOVE           = 0x0001,
    ACTOR_FLAG_SOLID            = 0x0002,

    // sprite cell y according to actor's direction
    ACTOR_FLAG_DIRECTIONAL      = 0x0004,

    // sprite cell x according to actor's current_frame, otherwise its variety
    ACTOR_FLAG_ANIMATED         = 0x0008,

    // don't check if tile is walkable
    ACTOR_FLAG_FLY              = 0x0010,

    // don't do collision checking
    ACTOR_FLAG_NONINTERACTIVE   = 0x0020,

    // player can pick it up
    ACTOR_FLAG_COLLETIBLE       = 0x0040,
} actor_flags_t;

typedef struct {
    /// No movement buttons are pressed.
    bool stopping_x;
    bool stopping_y;
    bool strike_button_down;
} player_info_t;

typedef struct {
    u8 level; // damage "rating", e.g. hand = 0 (weak)
    u8 amount;
} damage_t;

typedef struct {
    u8 amount;

    // what damage level or greater is required to affect health
    u8 minimum_damage_level;
} health_t;

typedef struct actor actor_t;
typedef struct actor_state actor_state_t;
typedef struct world world_t;
typedef void (* update_func_t)(actor_t *, float);
typedef void (* contact_func_t)(actor_t *, actor_t *);

struct actor {
    actor_type_t type;

    vec2_t pos; // in world pixels, the bottom center of the visible sprite
    vec2_t vel;

    // 0 if actor is on the ground. This doesn't affect anything except
    // where an actor's sprite is rendered.
    int z;

    actor_flags_t flags;
    cardinal_t direction;
    cardinal_t facing;

    health_t health;
    damage_t damage;

    sprite_t * sprite; // used when this actor type has no state
    float current_frame;

    // Actors get their lighting from the tile they're standing on.
    vec3_t lighting;

    // An actor's hitbox is centered on its x position and the hitbox's
    // bottom aligns with the actor's y position.
    // size is in unscaled pixels
    u8 hitbox_width;
    u8 hitbox_height;

    update_func_t update; // used if type has no state
    contact_func_t contact; // "    "
    actor_state_t * state;
    int state_timer; // 0 = advance to next state

    union {
        s16 timer;
        player_info_t player;
    } info;

    // Actors may change the world, so keep an internal reference.
    world_t * world;

    void (* draw)(actor_t * self, int x, int y);
};

struct actor_state {
    int length; // state length in ticks
    actor_state_t * next_state;
    sprite_t * sprite; // &sprites[id]

    void (* handle_input)(actor_t * self, input_state_t *, float dt);
    update_func_t update;
    contact_func_t contact;
};

// -----------------------------------------------------------------------------
// a_main.c

actor_t * SpawnActor(actor_type_t type, vec2_t position, world_t * world);
sprite_t * GetActorSprite(const actor_t * actor);
void DamageActor(actor_t * attacker, actor_t * target);
void UpdateActor(actor_t * actor, float dt);

/// Actor's visible rect in world pixel space.
SDL_Rect GetActorVisibleRect(const actor_t * actor);

/// Actor's hitbox in world pixel space.
SDL_FRect ActorHitbox(const actor_t * actor);

void DoCollisions(bool vertical, actor_t * actor, actor_t ** blocks, int num_blocks);

void DrawActorSprite(actor_t * actor, sprite_t * sprite, int x, int y);
void DrawActor(actor_t * actor, SDL_Rect visible_rect, bool show_hitboxes);

// -----------------------------------------------------------------------------
// a_definitions.c

/// Get an actor type's definition.
///
/// An actor definition is a template used when creating new actors.
actor_t GetActorDefinition(actor_type_t type);

#endif /* actor_h */
