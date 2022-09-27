//
//  a_list.c
//  Game
//
//  Created by Thomas Foster on 9/15/22.
//

#include "a_actor.h"
#include "game.h"
#include "w_world.h"
#include "sprites.h"
#include "mylib/input.h"

#define PLAYER_ACCEL (0.2f * SCALED_TILE_SIZE)
#define PLAYER_MAX_VELOCITY (2.5f * SCALED_TILE_SIZE)

void PlayerHandleInput(actor_t * player, float dt);
void PlayerStandUpdate(actor_t * player, float dt);
void PlayerWalkUpdate(actor_t * player, float dt);
void ButterflyUpdate(actor_t * actor, float dt);

static actor_state_t player_stand = {
    .sprite = &sprites[SPRITE_PLAYER_STAND],
    .handle_input = PlayerHandleInput,
    .update = PlayerStandUpdate,
    .contact = NULL, // TODO: player stand contact
};

static actor_state_t player_run = {
    .sprite = &sprites[SPRITE_PLAYER_WALK],
    .handle_input = PlayerHandleInput,
    .update = PlayerWalkUpdate
};

static actor_state_t state_butterfly = {
    .sprite = &sprites[SPRITE_BUTTERFLY],
    .update = ButterflyUpdate,
};


void PlayerHandleInput(actor_t * player, float dt)
{
    if ( keyboard[SDL_SCANCODE_A] ) {
        player->vel.x -= PLAYER_ACCEL;
    }

    if ( keyboard[SDL_SCANCODE_D] ) {
        player->vel.x += PLAYER_ACCEL;
    }

    if ( keyboard[SDL_SCANCODE_W] ) {
        player->vel.y -= PLAYER_ACCEL;
    }

    if ( keyboard[SDL_SCANCODE_S] ) {
        player->vel.y += PLAYER_ACCEL;
    }

    CLAMP(player->vel.x, -PLAYER_MAX_VELOCITY, PLAYER_MAX_VELOCITY);
    CLAMP(player->vel.y, -PLAYER_MAX_VELOCITY, PLAYER_MAX_VELOCITY);
}

void PlayerUpdateCamera(actor_t * player, float dt)
{
    world_t * world = player->world;

    if ( player->vel.x || player->vel.y ) {
        vec2_t position = player->pos;
        world->camera_target = Vec2Normalize(player->vel);

        // Place the camera 3 tiles to the side of the player
        world->camera_target = Vec2Scale(world->camera_target, SCALED_TILE_SIZE * 3.0f);
        world->camera_target = Vec2Add(position, world->camera_target);
    }

    VectorLerpEpsilon(&world->camera, &world->camera_target, dt, 1.0f);
}

void PlayerStandUpdate(actor_t * player, float dt)
{
    if ( player->vel.x || player->vel.y ) {
        player->state = &player_run;
    }

    PlayerUpdateCamera(player, dt);
}

void PlayerWalkUpdate(actor_t * player, float dt)
{
    const float damping = 0.4f;
    const float decel_ep = 0.2f;

    // apply horizontal friction
    if ( !keyboard[SDL_SCANCODE_A] && ! keyboard[SDL_SCANCODE_D] ) {
        player->vel.x = LerpEpsilon(player->vel.x, 0.0f, damping, decel_ep);
    }

    // apply vertical friction
    if ( !keyboard[SDL_SCANCODE_W] && ! keyboard[SDL_SCANCODE_S] ) {
        player->vel.y = LerpEpsilon(player->vel.y, 0.0f, damping, decel_ep);
    }

    if ( player->vel.x == 0.0f && player->vel.y == 0.0f ) {
        player->state = &player_stand;
    }

    PlayerUpdateCamera(player, dt);
}

void ButterflyUpdate(actor_t * actor, float dt)
{
    if ( --actor->info.timer <= 0 ) {
        actor->info.timer = MS2TICKS(Random(100, 1000), FPS);

        if ( actor->vel.x == 0 && actor->vel.y == 0 ) {
            // commence fluttering
            actor->vel = (vec2_t){ 0.25f * SCALED_TILE_SIZE, 0.0f };
        }

        actor->vel = Vec2Rotate(actor->vel, DEG2RAD(Random(0, 359)));
    }
}

static actor_t actor_definitions[NUM_ACTOR_TYPES] = {
    [ACTOR_PLAYER] = {
        .flags = ACTOR_FLAG_DIRECTIONAL | ACTOR_FLAG_ANIMATED,
        .state = &player_stand,
        .hitbox_width = 5,
        .hitbox_height = 4,
    },
    [ACTOR_TREE] = {
        .flags = ACTOR_FLAG_SOLID,
        .sprite = &sprites[SPRITE_TREE],
        .hitbox_width = 4,
        .hitbox_height = 4,
    },
    [ACTOR_BUTTERFLY] = {
        .flags = ACTOR_FLAG_ANIMATED | ACTOR_FLAG_FLY | ACTOR_FLAG_NONINTERACTIVE,
        .state = &state_butterfly,
    },
    [ACTOR_LOG] = { .flags = ACTOR_FLAG_COLLETIBLE },
};

actor_t GetActorDefinition(actor_type_t type)
{
    return actor_definitions[type];
}
