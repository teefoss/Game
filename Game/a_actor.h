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
    ACTOR_FLAG_SOLID = 0x0001,
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

typedef struct {
    actor_t array[MAX_ACTORS];
    bool removed[MAX_ACTORS];
    u16 num_actors;
} actor_storage_t;

//
//  Actor Quadtree Storage
//
#define ACTOR_TREE_NODE_SIZE 4
#define ACTOR_TREE_NUM_LEVELS 6 // 512 x 512 world, max level = 16 x 16
typedef struct actor_tree_node actor_tree_node_t;
struct actor_tree_node {
    int num_actors;
    actor_t actors[ACTOR_TREE_NODE_SIZE];
    SDL_Rect bounds;
    int level;
    actor_tree_node_t * quadrants[4];
};

// -----------------------------------------------------------------------------
// a_main.c

void SpawnActor(actor_type_t type, vec2_t position, actor_storage_t * storage);
sprite_t * GetActorSprite(const actor_t * actor);
void UpdateActor(actor_t * actor, float dt);
SDL_Rect ActorRect(const actor_t * actor);

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

actor_tree_node_t * InitActorTreeNode(SDL_Rect bounds, int level);
bool ActorTreeInsert_r(actor_tree_node_t * node, actor_t actor);
void GetActorsInRect_r
(   actor_storage_t * fill,
    actor_tree_node_t * node,
    SDL_Rect rect );
void FreeActorTree_r(actor_tree_node_t * node);

#endif /* actor_h */
