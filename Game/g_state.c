//
//  g_state.c
//  Game
//
//  Created by Thomas Foster on 10/16/22.
//

#include "g_game.h"
#include "w_world.h"
#include "mylib/video.h"

static bool PlayProcessEvent(game_t * game, const SDL_Event * event);
static void PlayUpdate(game_t * game, input_state_t * input, float dt);
static void PlayRender(game_t * game);

static void InventoryUpdate(game_t * game, input_state_t * input, float dt);
static void InventoryRender(game_t * game);

//static void TitleUpdate(game_t * game, float dt);
static void TitleRender(game_t * game);

static game_state_t states[] = {
    [GAME_STATE_PLAY] = {
        .process_event = PlayProcessEvent,
        .update = PlayUpdate,
        .render = PlayRender,
    },
    [GAME_STATE_INVENTORY] = {
        .update = InventoryUpdate,
        .render = InventoryRender,
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

bool G_ProcessEvent(game_t * game, const SDL_Event * event)
{
    game_state_t * state = G_GetCurrentState(game);

    if ( state && state->process_event ) {
        return state->process_event(game, event);
    }

    return false;
}

static void TitleRender(game_t * game)
{
    //DrawMenu(CurrentMenu(&game->menu_stack));
}

static bool PlayProcessEvent(game_t * game, const SDL_Event * event)
{
    switch ( event->type ) {
        case SDL_KEYDOWN:
            switch ( event->key.keysym.sym ) {
                case SDLK_ESCAPE:
                    M_PushMenu(game, MENU_PLAY);
                    UI_PushScreen(game, UI_MENU);
                    game->paused = true;
                    return true;
                case SDLK_p:
                    game->paused ^= 1;
                    return true;
                default:
                    break;
            }
            break;
        case SDL_CONTROLLERBUTTONDOWN:
            switch ( event->cbutton.button ) {
                case SDL_CONTROLLER_BUTTON_START:
                    break;
            }
            break;
        default:
            break;
    }

    return false;
}

static void PlayUpdate(game_t * game, input_state_t * input, float dt)
{
    UpdateWorld(game->world, input, dt);
}

static void PlayRender(game_t * game)
{
    RenderWorld(game->world);
    // TODO: HUD render goes here
}

static void InventoryUpdate(game_t * game, input_state_t * input, float dt)
{
    // close inventory
    if ( G_ControlPressed(game->input_state, CONTROL_INVENTORY_TOGGLE) ) {
        G_PopState(game);
    }

    UpdateWorld(game->world, NULL, dt);
}

static void InventoryRender(game_t * game)
{
    RenderWorld(game->world);

    // commence hack version:

    actor_t * player = GetActorType(game->world->actors, ACTOR_PLAYER);
    inventory_t * inv = player->info.player.inventory;

    // draw grid
    int size = 12 * DRAW_SCALE;
    for ( int y = 0; y < inv->grid_height; y++ ) {
        for ( int x = 0; x < inv->grid_width; x++ ) {
            V_SetGray(255);
            SDL_Rect r = { x * size, y * size, size, size };
            V_DrawRect(&r);
        }
    }

    for ( int i = 0; i < inv->num_items; i++ ) {
        int x, y;
        if ( InventoryGetGridCell(inv, i, &x, &y) ) {
            actor_t * item = &inv->items[i];
            int dst_x = x * size;
            int dst_y = y * size;
            DrawSprite(item->info.item.sprite, 0, 0, dst_x, dst_y, DRAW_SCALE, 0);
        }
    }
}



#pragma mark -

void G_Update(game_t * game, input_state_t * input, float dt)
{
    if ( game->state_top > -1 ) {
        game_state_id_t state = game->states[game->state_top];

        if ( states[state].update ) {
            states[state].update(game, input, dt);
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
