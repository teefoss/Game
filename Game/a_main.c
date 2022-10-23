//
//  a_main.c
//  Game
//
//  Created by Thomas Foster on 9/12/22.
//

#include "a_actor.h"
#include "m_debug.h"
#include "m_misc.h"
#include "w_world.h"
#include "mylib/genlib.h"
#include "mylib/video.h"

sprite_t * GetActorSprite(const actor_t * actor)
{
    if ( actor->state != NULL ) {
        return actor->state->sprite;
    } else {
        return actor->sprite;
    }
}

actor_t * SpawnActor(actor_type_t type, vec2_t position, world_t * world)
{
    actor_t actor = GetActorDefinition(type);
    actor.type = type;
    actor.pos = position;
    actor.world = world;

    // A hitbox size of 0 signals to use the whatever the sprite size is.
    if ( actor.sprite ) {
        if ( actor.hitbox_width == 0 ) {
            actor.hitbox_width = actor.sprite->location.w;
        }

        if ( actor.hitbox_height == 0 ) {
            actor.hitbox_height = actor.sprite->location.h;
        }
    }

    switch ( type ) {
        case ACTOR_PLAYER: {
            actor.info.player.inventory = calloc(1, sizeof(inventory_t));
            inventory_t * inv = actor.info.player.inventory;

            inv->selected = GetActorDefinition(ACTOR_NONE);
            inv->right_hand = GetActorDefinition(ACTOR_NONE);
            inv->left_hand = GetActorDefinition(ACTOR_NONE);
            inv->grid_width = INITIAL_GRID_WIDTH;
            inv->grid_height = INITIAL_GRID_HEIGHT;
            memset(inv->grid, EMPTY_SLOT, sizeof(inv->grid));
            break;
        }
        default:
            break;
    }

    if ( !world->updating_actors ) {
        return Append(world->actors, &actor);
    } else {
        return Append(world->pending_actors, &actor);
    }
}

void KillActor(actor_t * actor)
{
    actor->flags |= ACTOR_FLAG_REMOVE;

    if ( actor->flags & ACTOR_DROPS_ITEMS ) {
        drop_t * drops = actor->info.drops;

        for ( int i = 0; i < MAX_DROPS; i++ ) {
            if ( drops[i].actor_type == 0 ) {
                break; // end of list
            }

            // TODO: randomize drop position
            for ( int j = 0; j < drops[i].quantity; j++ ) {
                SpawnActor(drops[i].actor_type, actor->pos, actor->world);
            }
        }
    }

    switch ( actor->type ) {
        case ACTOR_PLAYER:
            free(actor->info.player.inventory);
            break;
        default:
            break;
    }
}

void DamageActor(actor_t * attacker, actor_t * target)
{
    if ( attacker->damage.level >= target->health.minimum_damage_level ) {
        // attacker damage is strong enough
        target->health.amount -= attacker->damage.amount; // TODO: randomize
        if ( target->health.amount <= 0 ) {
            KillActor(target);
        }
    }
}

void ChangeActorState(actor_t * actor, actor_state_t * new_state)
{
    if ( actor->state && actor->state->on_exit ) {
        actor->state->on_exit(actor);
    }

    if ( new_state == NULL ) {
        KillActor(actor);
        return;
    }

    actor->state = new_state;

    if ( actor->state->on_enter ) {
        actor->state->on_enter(actor);
    }

    actor->state_timer = actor->state->length;
}

void UpdateActor(actor_t * actor, float dt)
{
    if ( actor->facing == NO_DIRECTION ) {
        // Update actor's facing direction according to its movement.
        if ( actor->vel.x || actor->vel.y ) {
            actor->direction = VectorToCardinal(actor->vel);
        }
    } else {
        actor->direction = actor->facing;
    }

    sprite_t * sprite = GetActorSprite(actor);
    if ( sprite ) {
        // Update sprite animation.
        if ( actor->flags & ACTOR_FLAG_ANIMATED ) {
            actor->current_frame += (float)sprite->fps * dt;
            while ( actor->current_frame >= sprite->num_frames ) {
                actor->current_frame -= sprite->num_frames; // wrap frame if needed
            }
        }

        // Update light.
        // Get which tile the actor is on and apply lighting color mod.
        tile_t * tile = GetTile
        (   actor->world->tiles,
            actor->pos.x / SCALED_TILE_SIZE,
            actor->pos.y / SCALED_TILE_SIZE );

        // TODO: lerp this
        actor->lighting = tile->lighting;
    }

    if ( actor->state ) {
        // Do state machine.
        if ( actor->state->length ) {
            if ( --actor->state_timer <= 0 ) {
                ChangeActorState(actor, actor->state->next_state);
            }
        }

        if ( actor->state->update ) {
            actor->state->update(actor, dt);
        }
    }
}

SDL_Rect GetActorVisibleRect(const actor_t * actor)
{
    sprite_t * sprite = GetActorSprite(actor);
    SDL_Rect rect = {
        .x = actor->pos.x,
        .y = actor->pos.y,
        .w = sprite ? sprite->location.w * DRAW_SCALE : 0,
        .h = sprite ? (sprite->location.h + actor->z) * DRAW_SCALE : 0
    };

    rect.x -= rect.w / 2;
    rect.y -= rect.h;

    return rect;
}

static vec2_t PositionFromHitbox(const actor_t * actor, SDL_FRect hitbox)
{
    float w = actor->hitbox_width * (float)DRAW_SCALE;
    float h = actor->hitbox_height * (float)DRAW_SCALE;

    return (vec2_t){
        .x = hitbox.x + w / 2.0f,
        .y = hitbox.y + h
    };
}

SDL_FRect ActorHitbox(const actor_t * actor)
{
    float w = actor->hitbox_width * (float)DRAW_SCALE;
    float h = actor->hitbox_height * (float)DRAW_SCALE;

    SDL_FRect hitbox = {
        .x = actor->pos.x - w / 2.0f,
        .y = actor->pos.y - h,
        .w = w,
        .h = h
    };

    return hitbox;
}

void ResolveHorizontalCollision
(   actor_t * actor,
    SDL_FRect actor_box,
    SDL_FRect block_box )
{
    if ( actor->vel.x > 0 ) { // clip to left side
        actor_box.x = block_box.x - actor_box.w;
    } else if ( actor->vel.x < 0 ) { // clip to right side
        actor_box.x = block_box.x + block_box.w;
    }

    actor->pos = PositionFromHitbox(actor, actor_box);
    actor->vel.x = 0;
}

void ResolveVerticalCollision
(   actor_t * actor,
    SDL_FRect actor_box,
    SDL_FRect block_box )
{
    if ( actor->vel.y > 0 ) { // clip to top side
        actor_box.y = block_box.y - actor_box.h;
    } else if ( actor->vel.y < 0 ) { // clip to bottom side
        actor_box.y = block_box.y + block_box.h;
    }

    actor->pos = PositionFromHitbox(actor, actor_box);
    actor->vel.y = 0;
}

void DoCollisions(bool vertical, actor_t * actor, actor_t ** blocks, int num_blocks)
{
    SDL_FRect ibox = ActorHitbox(actor);

    for ( int j = 0; j < num_blocks; j++ ) {
        SDL_FRect jbox = ActorHitbox(blocks[j]);

        if ( SDL_HasIntersectionF(&ibox, &jbox) ) {
            if ( vertical ) {
                ResolveVerticalCollision(actor, ibox, jbox);
            } else {
                ResolveHorizontalCollision(actor, ibox, jbox);
            }
        }
    }
}

void DrawActorSprite(actor_t * actor, sprite_t * sprite, int x, int y)
{
    DrawSprite
    (   sprite,
        actor->current_frame,
        actor->flags & ACTOR_FLAG_DIRECTIONAL
            ? SpriteDirection(actor->direction)
            : 0,
        x,
        y,
        DRAW_SCALE,
        0 ); // TODO: actor flippable?
}

void DrawActor(actor_t * actor, SDL_Rect visible_rect)
{
    sprite_t * sprite = GetActorSprite(actor);

    if ( sprite ) {
        SDL_Rect r = GetActorVisibleRect(actor);
        r.x -= visible_rect.x; // convert to window space
        r.y -= visible_rect.y;

        SetSpriteColorMod(sprite, actor->lighting);

        if ( actor->draw ) {
            actor->draw(actor, r.x, r.y);
        } else {
            DrawActorSprite(actor, sprite, r.x, r.y);
        }

        if ( show_geometry ) {
            SDL_FRect hitbox = ActorHitbox(actor);
            V_SetRGBA(90, 90, 255, 255);
            hitbox.x -= visible_rect.x;
            hitbox.y -= visible_rect.y;
            SDL_Rect hitbox_i = { hitbox.x, hitbox.y, hitbox.w, hitbox.h };
            V_DrawRect(&hitbox_i);
        }
    }
}

const char * ActorName(actor_type_t type)
{
    switch ( type ) {
            CASE_RETURN_STRING(ACTOR_NONE);
            CASE_RETURN_STRING(ACTOR_PLAYER);
            CASE_RETURN_STRING(ACTOR_HAND_STRIKE);
            CASE_RETURN_STRING(ACTOR_TREE);
            CASE_RETURN_STRING(ACTOR_BUSH);
            CASE_RETURN_STRING(ACTOR_BUTTERFLY);
            CASE_RETURN_STRING(ACTOR_LOG);
            CASE_RETURN_STRING(ACTOR_LEAVES);
            CASE_RETURN_STRING(ACTOR_STICKS);
        case NUM_ACTOR_TYPES:
            return NULL;
    }

    return NULL;
}

actor_t * GetActorType(array_t * actor_array, actor_type_t type)
{
    for ( int i = 0; i <  actor_array->count; i++ ) {
        actor_t * actor = GetElement(actor_array, i);
        if ( actor->type == type ) {
            return actor;
        }
    }

    return NULL;
}
