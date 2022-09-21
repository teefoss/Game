//
//  a_list.c
//  Game
//
//  Created by Thomas Foster on 9/15/22.
//

#include "a_actor.h"
#include "w_world.h"
#include "sprites.h"
#include "mylib/input.h"

#define PLAYER_VELOCITY (1.5f * TILE_SIZE) // tiles / second
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

    // Reposition the world camera.
    vec2_t camera_target = ScaleVector(player->velocity, 3.0f);
    camera_target = AddVectors(player->position, camera_target);
    LerpVector(&player->world->camera, &camera_target, 0.1f);
}

actor_state_t player_stand = {
    .sprite = &sprites[SPRITE_PLAYER_STAND],
    .handle_input = PlayerHandleInput,
    .update = PlayerUpdate,
    .contact = NULL, // TODO: player stand contact
};

static actor_t actor_definitions[NUM_ACTOR_TYPES] = {
    [ACTOR_PLAYER] = {
        //.flags = ACTOR_FLAG_SOLID,
        .state = &player_stand,
        .hitbox_width = 6,
        .hitbox_height = 4,
    },
    [ACTOR_TREE] = {
        .flags = ACTOR_FLAG_SOLID,
        .sprite = &sprites[SPRITE_TREE],
        .hitbox_width = 4,
        .hitbox_height = 4,
    },
};

actor_t GetActorDefinition(actor_type_t type)
{
    return actor_definitions[type];
}
