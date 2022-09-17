//
//  a_list.c
//  Game
//
//  Created by Thomas Foster on 9/15/22.
//

#include "a_actor.h"
#include "sprites.h"
#include "mylib/input.h"

#define PLAYER_VELOCITY 1.5f
#define PLAYER_ACCEL 0.1f

void PlayerHandleInput(actor_t * player)
{
    if ( keyboard[SDL_SCANCODE_A] ) {
        player->velocity.x = Lerp(player->velocity.x, -PLAYER_VELOCITY, PLAYER_ACCEL);
    }

    if ( keyboard[SDL_SCANCODE_D] ) {
        player->velocity.x = Lerp(player->velocity.x, PLAYER_VELOCITY, PLAYER_ACCEL);
    }

    if ( keyboard[SDL_SCANCODE_W] ) {
        player->velocity.y = Lerp(player->velocity.y, -PLAYER_VELOCITY, PLAYER_ACCEL);
    }

    if ( keyboard[SDL_SCANCODE_S] ) {
        player->velocity.y = Lerp(player->velocity.y, PLAYER_VELOCITY, PLAYER_ACCEL);
    }
}

void PlayerUpdate(actor_t * player, float dt)
{
    // apply horizontal friction
    if ( !keyboard[SDL_SCANCODE_A] && ! keyboard[SDL_SCANCODE_D] ) {
        player->velocity.x = Lerp(player->velocity.x, 0, PLAYER_ACCEL);
    }

    // apply vertical friction
    if ( !keyboard[SDL_SCANCODE_W] && ! keyboard[SDL_SCANCODE_S] ) {
        player->velocity.y = Lerp(player->velocity.y, 0, PLAYER_ACCEL);
    }
}

actor_state_t player_stand = {
    .sprite = &sprites[SPRITE_PLAYER_STAND],
    .handle_input = PlayerHandleInput,
    .update = PlayerUpdate,
    .contact = NULL, // TODO: player stand contact
};

static actor_t actor_definitions[NUM_ACTOR_TYPES] = {
    [ACTOR_PLAYER] = {
        .flags = ACTOR_FLAG_SOLID,
        .state = &player_stand,
    },
    [ACTOR_TREE] = {
        .flags = ACTOR_FLAG_SOLID,
        .sprite = &sprites[SPRITE_TREE],
    },
};

actor_t GetActorDefinition(actor_type_t type)
{
    return actor_definitions[type];
}
