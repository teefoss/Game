//
//  a_storage.c
//  Game
//
//  Created by Thomas Foster on 9/15/22.
//

#include "a_actor.h"
#include "w_world.h"

actor_t * AddActor(actor_storage_t * storage, actor_t actor)
{
    if ( storage->count == MAX_ACTORS ) {
        printf("%s: out of space, please increase MAX_ACTORS\n", __func__);
    }

    storage->array[storage->count] = actor;
    storage->count++;

    return &storage->array[storage->count - 1];
}

void AppendActorStorage
(   actor_storage_t * storage,
    const actor_storage_t * append )
{
    for ( int i = 0; i < append->count; i++ ) {
        AddActor(storage, append->array[i]);
    }
}

void RemoveActor(actor_storage_t * storage, int index)
{
    storage->array[index] = storage->array[--storage->count];
}

actor_tree_node_t * InitActorTreeNode(SDL_Rect bounds, int level)
{
    actor_tree_node_t * node = calloc(1, sizeof(*node));
    if ( node == NULL ) {
        Error("could not allocate actor tree node");
    }

    node->num_actors = 0;
    node->bounds = bounds;
    node->level = level;
    for ( int i = 0; i < 4; i++ ) {
        node->quadrants[i] = NULL;
    }

    return node;
}

static SDL_Rect GetRectForQuadrant(int quadrant, SDL_Rect parent_bounds)
{
    SDL_Rect result = {
        .w = parent_bounds.w / 2,
        .h = parent_bounds.h / 2
    };

    switch ( quadrant ) {
        case 0:
            result.x = parent_bounds.x;
            result.y = parent_bounds.y;
            break;
        case 1:
            result.x = parent_bounds.x + result.w;
            result.y = parent_bounds.y;
            break;
        case 2:
            result.x = parent_bounds.x;
            result.y = parent_bounds.y + result.h;
            break;
        case 3:
            result.x = parent_bounds.x + result.w;
            result.y = parent_bounds.y + result.h;
            break;
        default:
            Error("bad quadrant index!");
    }

    return result;
}

bool ActorTreeInsert_r(actor_tree_node_t * node, actor_t actor)
{
    sprite_t * sprite = GetActorSprite(&actor);
    SDL_Rect actor_rect = {
        actor.position.x,
        actor.position.y,
        sprite ? sprite->location.w / TILE_SIZE : 0,
        sprite ? sprite->location.h / TILE_SIZE : 0,
    };

    if ( !RectInRect(actor_rect, node->bounds) ) {
        return false;
    }

    if ( node->num_actors < ACTOR_TREE_NODE_SIZE - 1 ) {
        node->actors[node->num_actors++] = actor;
        return true;
    }

    //
    // subdivide this node:
    //

    if ( node->level == ACTOR_TREE_NUM_LEVELS - 1 ) {
        return false; // reach max depth
    }

    // init children
    for ( int i = 0; i < 4; i++ ) {
        SDL_Rect quadrant_bounds = GetRectForQuadrant(i, node->bounds);
        node->quadrants[i] = InitActorTreeNode(quadrant_bounds, node->level + 1);
    }

    // insert the actor into the first qualifying child node
    for ( int i = 0; i < 4; i++ ) {
        if ( ActorTreeInsert_r(node->quadrants[i], actor) ) {
            return true;
        }
    }

    return false;
}

void GetActorsInRect_r
(   actor_storage_t * fill,
    actor_tree_node_t * node,
    SDL_Rect rect )
{
    if ( !RectsIntersect(rect, node->bounds) ) {
        return;
    }

    // add actors in this quadrant
    for ( int i = 0; i < node->num_actors; i++ ) {
        AddActor(fill, node->actors[i]);
    }

    if ( node->quadrants[0] != NULL ) {
        for ( int i = 0; i < 4; i++ ) {
            GetActorsInRect_r(fill, node->quadrants[i], rect);
        }
    }
}

void FreeActorTree_r(actor_tree_node_t * node)
{
    if ( node == NULL ) {
        return;
    }

    for ( int i = 0; i < 4; i++ ) {
        FreeActorTree_r(node->quadrants[i]);
        node->quadrants[i] = NULL;
    }

    free(node);
}
