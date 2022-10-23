//
//  g_state.c
//  Game
//
//  Created by Thomas Foster on 10/16/22.
//

#include "g_game.h"
#include "w_world.h"
#include "mylib/video.h"

static bool PlayProcessControl(game_t * game);
static void PlayUpdate(game_t * game, float dt);
static void PlayRender(game_t * game);

//static void TitleUpdate(game_t * game, float dt);
static void TitleRender(game_t * game);

static game_state_t states[] = {
    [GAME_STATE_PLAY] = {
        .process_control = PlayProcessControl,
        .update = PlayUpdate,
        .render = PlayRender,
    },
    [GAME_STATE_TITLE] = {
//        .process_input = TitleProcessInput,
//        .update = TitleUpdate,
        .render = TitleRender,
    }
};

void G_PushState(game_t * game, game_state_id_t state)
{
    if ( game->state_top + 1 >= MAX_GAME_STATES ) {
        Error("ran out of room in game state stack, please increase MAX_GAME_STATES");
    }

    game->states[++game->state_top] = state;
}

void G_PopState(game_t * game)
{
    if ( --game->state_top < -1 ) {
        Error("tried to pop from empty game state stack!");
    }
}

static game_state_t * G_GetCurrentState(game_t * game)
{
    if ( game->state_top == -1 ) {
        return NULL;
    }

    return &states[game->states[game->state_top]];
}

bool G_ProcessControl(game_t * game)
{
    game_state_t * state = G_GetCurrentState(game);

    if ( state && state->process_control ) {
        return state->process_control(game);
    }

    return false;
}

static void TitleRender(game_t * game)
{
    //DrawMenu(CurrentMenu(&game->menu_stack));
}

static bool PlayProcessControl(game_t * game)
{
    if ( game->controls_processed ) {
        return false;
    }

    if ( game->control_state.controls[CONTROL_MENU_TOGGLE] ) {
        M_PushMenu(game, MENU_PLAY);
        UI_PushScreen(game, UI_MENU);
        game->paused = true;
        game->controls_processed = true;
    } else if ( game->control_state.controls[CONTROL_INVENTORY_TOGGLE] ) {
        UI_PushScreen(game, UI_INVENTORY);
        game->controls_processed = true;
    }

    return game->controls_processed;
}

static void PlayUpdate(game_t * game, float dt)
{
    if ( game->controls_processed ) {
        UpdateWorld(game->world, NULL, dt);
    } else {
        UpdateWorld(game->world, &game->control_state, dt);
    }
}

static void PlayRender(game_t * game)
{
    RenderWorld(game->world);
    // TODO: HUD render goes here
}

#pragma mark -

void G_Update(game_t * game, float dt)
{
    if ( game->state_top > -1 ) {
        game_state_id_t state = game->states[game->state_top];

        if ( states[state].update ) {
            states[state].update(game, dt);
        }
    }
}

void G_Render(game_t * game)
{
    if ( game->state_top > -1 ) {
        game_state_id_t state = game->states[game->state_top];

        if ( states[state].render ) {
            states[state].render(game);
        }
    }
}
