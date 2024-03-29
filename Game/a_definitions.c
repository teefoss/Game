//
//  a_list.c
//  Game
//
//  Created by Thomas Foster on 9/15/22.
//

#include "a_actor.h"
#include "g_game.h"
#include "inventory.h"
#include "m_misc.h"
#include "w_world.h"
#include "sprites.h"
#include "mylib/input.h"

#define PLAYER_VELOCITY (2.5f * SCALED_TILE_SIZE)

void PlayerHandleInput(actor_t * player, const control_state_t * control_state, float dt);

void PlayerStandUpdate(actor_t * player, float dt);
void PlayerWalkUpdate(actor_t * player, float dt);
void ButterflyUpdate(actor_t * actor, float dt);
static void PlayerStrike(actor_t * player);

void PlayerContact(actor_t * player, actor_t * hit);
void PlayerStrikeContact(actor_t *, actor_t *);

void DrawPlayer(actor_t * player, int x, int y);

static actor_state_t player_stand = {
    .sprite = &sprites[SPRITE_PLAYER_STAND],
    .handle_input = PlayerHandleInput,
    .update = PlayerStandUpdate,
    .contact = PlayerContact, // TODO: does the player only need 1 contact func?
};

static actor_state_t player_run = {
    .sprite = &sprites[SPRITE_PLAYER_WALK],
    .handle_input = PlayerHandleInput,
    .update = PlayerWalkUpdate,
    .contact = PlayerContact,
};

static actor_state_t state_butterfly = {
    .sprite = &sprites[SPRITE_BUTTERFLY],
    .update = ButterflyUpdate,
};

static actor_state_t player_strike = {
    .length = MS2TICKS(100, FPS),
    .next_state = &player_stand,
    .sprite = &sprites[SPRITE_PLAYER_STRIKE],
    .on_exit = PlayerStrike,
};

static actor_state_t player_wind_up = {
    .length = MS2TICKS(200, FPS),
    .next_state = &player_strike,
    .sprite = &sprites[SPRITE_PLAYER_SWING],
};

static actor_t actor_definitions[NUM_ACTOR_TYPES] = {
    [ACTOR_NONE] = {
        0
    },
    [ACTOR_PLAYER] = {
        .flags =    ACTOR_FLAG_DIRECTIONAL |
        ACTOR_FLAG_ANIMATED |
        ACTOR_FLAG_CAN_BE_DAMAGED |
        ACTOR_FLAG_CASTS_SHADOW,
            .state = &player_stand,
            .hitbox_width = 5,
            .hitbox_height = 4,
            .draw = DrawPlayer,
            .health = { .amount = 100, .minimum_damage_level = 0 },
    },
    [ACTOR_HAND_STRIKE] = {
        .flags = ACTOR_FLAG_REMOVE,
        //.sprite = &sprites[SPRITE_ICON_NO_ITEM],
        .hitbox_width = TILE_SIZE,
        .hitbox_height = TILE_SIZE,
        .damage = { .level = 0, .amount = 10 },
        .contact = PlayerStrikeContact,
    },
    [ACTOR_TREE] = {
        .flags =    ACTOR_FLAG_SOLID |
        ACTOR_FLAG_CAN_BE_DAMAGED |
        ACTOR_DROPS_ITEMS |
        ACTOR_FLAG_CASTS_SHADOW,
            .sprite = &sprites[SPRITE_TREE],
            .hitbox_width = 4,
            .hitbox_height = 4,
            .health = { .amount = 30, .minimum_damage_level = 0 },
            .info.drops = {
                { 1, ACTOR_LOG },
                { 2, ACTOR_STICKS, },
                { 3, ACTOR_LEAVES },
            },
    },
    [ACTOR_BUSH] = {
        .flags =    ACTOR_FLAG_SOLID |
        ACTOR_FLAG_CAN_BE_DAMAGED |
        ACTOR_DROPS_ITEMS |
        ACTOR_FLAG_CASTS_SHADOW,
            .sprite = &sprites[SPRITE_BUSH],
            .hitbox_width = 4,
            .hitbox_height = 4,
            .health = { .amount = 30, .minimum_damage_level = 0 },
    },
    [ACTOR_BUTTERFLY] = {
        .flags =    ACTOR_FLAG_ANIMATED |
        ACTOR_FLAG_FLY |
        ACTOR_FLAG_NONINTERACTIVE |
        ACTOR_FLAG_CASTS_SHADOW,
            .state = &state_butterfly,
    },
    [ACTOR_LOG] = {
        .flags = ACTOR_FLAG_COLLETIBLE,
        .sprite = &sprites[SPRITE_LOG_WORLD],
        .info.item = {
            .width = 2,
            .height = 2,
            .sprite = &sprites[SPRITE_LOG_INVENTORY]
        },
    },
    [ACTOR_LEAVES] = {
        .flags = ACTOR_FLAG_COLLETIBLE,
        .sprite = &sprites[SPRITE_LEAVES],
        .info.item = {
            .width = 1,
            .height = 1,
            .sprite = &sprites[SPRITE_LEAVES],
        },
    },
    [ACTOR_STICKS] = {
        .flags = ACTOR_FLAG_COLLETIBLE,
        .sprite = &sprites[SPRITE_STICKS_WORLD],
        .info.item = {
            .width = 1,
            .height = 2,
            .sprite = &sprites[SPRITE_STICKS_INVENTORY],
        },
    },
};

#pragma mark -

actor_t GetActorDefinition(actor_type_t type)
{
    return actor_definitions[type];
}

#pragma mark - INPUT FUNCTIONS

/*
 I personally moved from a force/mass/acceleration model to a simpler,
 less realistic but more controllable/reliable dampening method for most
 of my entities. I did this because my force/acceleration model wasn't working
 for humanoids (and nor should it, it's an inappropriate model, legged creatures
 aren't billiard balls).

 Basically:
 New Velocity = old_velocity *
 (1 - dt * transition_speed) + desired_velocity * (dt * transition_speed)
 (we're just lerping here)

 transition_speed is one of a few constants (based on terrain type) largely
 simulated friction, water/ground has a transition speed of 4.0, and to have a
 smidge of air control, the air transition speed is 0.7.

 desired_velocity is a unit length "running force" (whatever the entity is
 trying to do) multiplied by the characters max speed for what they're standing
 on, e.g. higher for land than water.

 Implementing this immediately made my FPS experience immediately feel much
 tighter, and all issues I had applying f = ma to humanoids disappeared.
 Projectiles are still doing f = ma (with drag etc.), but I only apply it to new
 entities if the simple model fails me.

 Edit:
 If at some point you feel as if physics really is important in your game,
 check out:
 http://gafferongames.com/game-physics/integration-basics/

 With respect to integration and timesteps, just something to look at if you
 decide "my physics isn't working well enough". If you're going to go with an
 approach that's not "correct" or based purely in the physical world, you may as
 well know why and how it's incorrect.
 */

static void PlayerStrike(actor_t * player)
{
    player_info_t * info = &player->info.player;
    info->strike_button_down = true;

    cardinal_t facing;
    if ( player->facing == NO_DIRECTION ) {
        facing = player->direction;
    } else {
        facing = player->facing;
    }

    tile_coord_t tile_coord = GetAdjacentTile(player->pos, facing);

    // upper left corner
    position_t coord = GetTileCenter(tile_coord);
    coord.y += SCALED_TILE_SIZE / 2; // actor position is at the bottom

    SpawnActor(ACTOR_HAND_STRIKE, coord, player->world);
}

void PlayerHandleInput
 (  actor_t * player,
    const control_state_t * control_state,
    float dt )
{
    player_info_t * info = &player->info.player;
    info->stopping_x = true;
    info->stopping_y = true;

    // Start off with the value from the stick.
    vec2_t move_dir = control_state->left_stick;

    // Check the keyboard too.
    if ( G_ControlPressed(control_state, CONTROL_PLAYER_MOVE_UP) ) {
        move_dir.y = -1.0f;
        player->facing = NORTH;
    }

    if ( G_ControlPressed(control_state, CONTROL_PLAYER_MOVE_DOWN) ) {
        move_dir.y = 1.0f;
        player->facing = SOUTH;
    }

    if ( G_ControlPressed(control_state, CONTROL_PLAYER_MOVE_LEFT) ) {
        move_dir.x = -1.0f;
        player->facing = WEST;
    }

    if ( G_ControlPressed(control_state, CONTROL_PLAYER_MOVE_RIGHT) ) {
        move_dir.x = 1.0f;
        player->facing = EAST;
    }

    // Stick: make it easier to go exactly east/west and north/south.
    {
        const float minimum = 0.2f;

        if ( fabsf(move_dir.x) < minimum ) {
            move_dir.x = 0.0f;
        }

        if ( fabsf(move_dir.y) < minimum ) {
            move_dir.y = 0.0f;
        }
    }

    if ( move_dir.x ) {
        info->stopping_x = false;
    }

    if ( move_dir.y ) {
        info->stopping_y = false;
    }

    vec2_t vel = Vec2Scale(move_dir, PLAYER_VELOCITY);
    Vec2Lerp(&player->vel, &vel, dt * 10);

    // Set player facing according to right controller stick.
    {
        // TODO: figure out keyboard control for this

        vec2_t facing = control_state->right_stick;
        player->facing = VectorToCardinal(facing);
    }

    // Hit stuff?
    {
        float right_trigger = control_state->right_trigger;

        if ( !info->strike_button_down && right_trigger > 0.0f ) {
            player->vel = (vec2_t){ 0 };
            ChangeActorState(player, &player_wind_up);
        }

        if ( right_trigger == 0.0f ) {
            info->strike_button_down = false;
        }

        if ( control_state->controls[CONTROL_PLAYER_STRIKE_UP] ) {
            player->facing = NORTH;
            ChangeActorState(player, &player_wind_up);
        }

        if ( control_state->controls[CONTROL_PLAYER_STRIKE_DOWN] ) {
            player->facing = SOUTH;
            ChangeActorState(player, &player_wind_up);
        }

        if ( control_state->controls[CONTROL_PLAYER_STRIKE_LEFT] ) {
            player->facing = WEST;
            ChangeActorState(player, &player_wind_up);
        }

        if ( control_state->controls[CONTROL_PLAYER_STRIKE_RIGHT] ) {
            player->facing = EAST;
            ChangeActorState(player, &player_wind_up);
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

    //PlayerUpdateCamera(player, dt);
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

    //PlayerUpdateCamera(player, dt);
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

void PlayerContact(actor_t * player, actor_t * hit)
{
    if ( hit->flags & ACTOR_FLAG_COLLETIBLE ) {
        if ( INV_InsertItem(hit, player->info.player.inventory) ) {
            hit->flags |= ACTOR_FLAG_REMOVE;
        }
    }
}

void PlayerStrikeContact(actor_t * strike, actor_t * hit)
{
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
        tile_coord_t tile = GetAdjacentTile(player->pos, player->facing);
        position_t ret_pos = GetTileCenter(tile);
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
