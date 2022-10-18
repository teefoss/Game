//
//  ui_screen.c
//  Game
//
//  Created by Thomas Foster on 10/17/22.
//

#include "ui_screen.h"
#include "g_game.h"
#include "menu.h"

screen_t screens[UI_NUM_SCREENS] = {
    [UI_MENU]       = { M_ProcessEvent, M_ProcessInput, NULL, M_Render },
    [UI_HUD]        = { NULL, NULL, NULL, NULL },
    [UI_INVENTORY]  = { NULL, NULL, NULL, NULL },
};

static inline screen_t * CurrentScreen(game_t * game)
{
    if ( game->num_screens == 0 ) {
        return NULL;
    }

    return &screens[game->screens[game->num_screens - 1]];
}

void UI_PushScreen(game_t * game, screen_id_t id)
{
    game->screens[game->num_screens++] = id;
}

void UI_PopScreen(game_t * game)
{
    --game->num_screens;
}

void UI_Update(game_t * game)
{
    screen_t * screen = CurrentScreen(game);

    if ( screen && screen->update ) {
        screen->update(game);
    }
}

void UI_Render(game_t * game)
{
    screen_t * screen = CurrentScreen(game);

    if ( screen && screen->render ) {
        screen->render(game);
    }
}

bool UI_ProcessInput(game_t * game)
{
    screen_t * screen = CurrentScreen(game);

    if ( screen && screen->process_input ) {
        return screen->process_input(game);
    }

    return false;
}

bool UI_ProcessEvent(game_t * game, const SDL_Event * event)
{
    screen_t * screen = CurrentScreen(game);

    if ( screen && screen->process_event ) {
        return screen->process_event(game, event);
    }

    return false;
}
