//
//  a_list.c
//  Game
//
//  Created by Thomas Foster on 9/15/22.
//

#include "a_actor.h"
#include "game.h"
#include "m_misc.h"
#include "w_world.h"
#include "sprites.h"
#include "mylib/input.h"

#define PLAYER_VELOCITY (2.5f * SCALED_TILE_SIZE)

void PlayerHandleInput(actor_t * player, input_state_t * input_state, float dt);

void PlayerStandUpdate(actor_t * player, float dt);
void PlayerWalkUpdate(actor_t * player, float dt);
void ButterflyUpdate(actor_t * actor, float dt);

void PlayerStrikeContact(actor_t *, actor_t *);

void DrawPlayer(actor_t * player, int x, int y);

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

static actor_t actor_definitions[NUM_ACTOR_TYPES] = {
    [ACTOR_PLAYER] = {
        .flags = ACTOR_FLAG_DIRECTIONAL | ACTOR_FLAG_ANIMATED,
        .state = &player_stand,
        .hitbox_width = 5,
        .hitbox_height = 4,
        .draw = DrawPlayer,
        .health = { .amount = 100, .minimum_damage_level = 0 },
    },
    [ACTOR_HAND_STRIKE] = {
//        .flags = 0,
        .flags = ACTOR_FLAG_REMOVE,
        .sprite = &sprites[SPRITE_ICON_NO_ITEM],
        .hitbox_width = TILE_SIZE,
        .hitbox_height = TILE_SIZE,
        .damage = { .level = 0, .amount = 10 },
        .contact = PlayerStrikeContact,
    },
    [ACTOR_TREE] = {
        .flags = ACTOR_FLAG_SOLID,
        .sprite = &sprites[SPRITE_TREE],
        .hitbox_width = 4,
        .hitbox_height = 4,
        .health = { .amount = 30, .minimum_damage_level = 3 },
    },
    [ACTOR_BUTTERFLY] = {
        .flags = ACTOR_FLAG_ANIMATED | ACTOR_FLAG_FLY | ACTOR_FLAG_NONINTERACTIVE,
        .state = &state_butterfly,
    },
    [ACTOR_LOG] = { .flags = ACTOR_FLAG_COLLETIBLE },
};

#pragma mark -

actor_t GetActorDefinition(actor_type_t type)
{
    return actor_definitions[type];
}

#pragma mark - INPUT FUNCTIONS

/*
 I personally moved from a force/mass/acceleration model to a simpler, less realistic but more controllable/reliable dampening method for most of my entities.
 I did this because my force/acceleration model wasn't working for humanoids (and nor should it, it's an inappropriate model, legged creatures aren't billiard balls).
 Basically:
 New Velocity = old_velocity * (1 - delta_time * transition_speed) + desired_velocity * (delta_time * transition_speed)
 (we're just lerping here)
 transition_speed is one of a few constants (based on terrain type) largely simulated friction, water/ground has a transition speed of 4.0, and to have a smidge of air control, the air transition speed is 0.7.
 desired_velocity is a unit length "running force" (whatever the entity is trying to do) multiplied by the characters max speed for what they're standing on, e.g. higher for land than water.
 Implementing this immediately made my FPS experience immediately feel much tighter, and all issues I had applying f = ma to humanoids disappeared.
 Projectiles are still doing f = ma (with drag etc.), but I only apply it to new entities if the simple model fails me.
 Edit:
 If at some point you feel as if physics really is important in your game, check out:
 http://gafferongames.com/game-physics/integration-basics/
 With respect to integration and timesteps, just something to look at if you decide "my physics isn't working well enough". If you're going to go with an approach that's not "correct" or based purely in the physical world, you may as well know why and how it's incorrect.
 */

void PlayerHandleInput(actor_t * player, input_state_t * input_state, float dt)
{
    player_info_t * info = &player->info.player;
    info->stopping_x = true;
    info->stopping_y = true;

#if 1
    if ( I_IsControllerConnected(input_state) ) {
        vec2_t move_dir = I_GetStickDirection(input_state, SIDE_LEFT);

        // Make it easier to go exactly east/west and north/south
        const float minimum = 0.2f;
        if ( fabsf(move_dir.x) < minimum ) {
            move_dir.x = 0.0f;
        }

        if ( fabsf(move_dir.y) < minimum ) {
            move_dir.y = 0.0f;
        }

        if ( move_dir.x ) {
            info->stopping_x = false;
        }

        if ( move_dir.y ) {
            info->stopping_y = false;
        }

        vec2_t vel = Vec2Scale(move_dir, PLAYER_VELOCITY);
        Vec2Lerp(&player->vel, &vel, dt * 10);

        vec2_t facing = I_GetStickDirection(input_state, SIDE_RIGHT);
        player->facing = VectorToCardinal(facing);

        float left_trigger = I_GetTriggerState(input_state, SIDE_RIGHT);

        if ( !info->strike_button_down && left_trigger > 0.0f ) {
            info->strike_button_down = true;

            vec2_t tile = GetAdjacentTile(player->pos, player->facing);

            // upper left corner
            vec2_t coord = GetTileCenter(tile.x, tile.y);
            coord.y += SCALED_TILE_SIZE / 2; // actor position is at the bottom

            SpawnActor(ACTOR_HAND_STRIKE, coord, player->world);
            puts("left trigger");
        }

        if ( left_trigger == 0.0f ) {
            info->strike_button_down = false;
        }
    }
    else
#endif
    {
        float factor = 4.0f;
        if ( I_IsKeyDown(input_state, SDL_SCANCODE_A) ) {
            player->vel.x = Lerp(player->vel.x, -PLAYER_VELOCITY, dt * factor);
            info->stopping_x = false;
        }

        if ( I_IsKeyDown(input_state, SDL_SCANCODE_D) ) {
            player->vel.x = Lerp(player->vel.x, PLAYER_VELOCITY, dt * factor);
            info->stopping_x = false;
        }

        if ( I_IsKeyDown(input_state, SDL_SCANCODE_W) ) {
            player->vel.y = Lerp(player->vel.y, -PLAYER_VELOCITY, dt * factor);
            info->stopping_y = false;
        }

        if ( I_IsKeyDown(input_state, SDL_SCANCODE_S) ) {
            player->vel.y = Lerp(player->vel.y, PLAYER_VELOCITY, dt * factor);
            info->stopping_y = false;
        }
    }
}

#pragma mark - UPDATE FUNCTIONS

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
    const float damping = 0.5f;
    const float decel_ep = 0.2f;

    // apply horizontal friction
    if ( player->info.player.stopping_x ) {
        player->vel.x = LerpEpsilon(player->vel.x, 0.0f, damping, decel_ep);
    }

    // apply vertical friction
    if ( player->info.player.stopping_y ) {
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

#pragma mark - CONTACT FUNCTIONS

void PlayerStrikeContact(actor_t * strike, actor_t * hit)
{
    printf("hit a %d\n", hit->type);
    DamageActor(strike, hit);
}

#pragma mark - DRAW FUNCTIONS

void DrawPlayer(actor_t * player, int x, int y)
{
    // Draw the reticle if a direction is being held.
    if ( player->facing != NO_DIRECTION ) {
        sprite_t * spr = &sprites[SPRITE_ICON_NO_ITEM];
        SDL_Rect visible_rect = GetVisibleRect(player->world->camera);

        // Center the reticle on the center of tile adjacent to the player
        vec2_t tile = GetAdjacentTile(player->pos, player->facing);
        vec2_t ret_pos = GetTileCenter(tile.x, tile.y);
        ret_pos.x -= (spr->location.w * DRAW_SCALE) / 2.0f;
        ret_pos.y -= (spr->location.h * DRAW_SCALE) / 2.0f;

        DrawSprite
        (   spr,
            0,
            0,
            ret_pos.x - visible_rect.x,
            ret_pos.y - visible_rect.y,
            DRAW_SCALE,
            0 );
    }

    DrawActorSprite(player, GetActorSprite(player), x, y);
}
