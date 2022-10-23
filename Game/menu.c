//
//  menu.c
//  Game
//
//  Created by Thomas Foster on 10/12/22.
//

#include "menu.h"
#include "g_game.h"
#include "mylib/video.h"
#include "w_world.h"

#pragma mark - MENUS

void M_Action_GoBack(game_t * game, int action_type);
void M_Action_Navigate(game_t * game, int destination);
void M_Action_Close(game_t * game, int unused);

// TODO: move to array
menu_t main_menu = {
    .title = "Game Alpha",
    .num_items = 4,
    .items = {
        {
            .name = "New Game",
            .action_type = ACTION_SELECT,
            .action = M_Action_NewGame
        },
        {
            .name = "Load Game",
            .action_type = ACTION_SELECT,
            .action = NULL
        },
        {
            .name = "Options",
            .destination = MENU_OPTIONS,
            .action_type = ACTION_SELECT,
            .action = M_Action_Navigate,
        },
        {
            .name = "Quit to Desktop",
            .action_type = ACTION_SELECT,
            .action = M_Action_QuitGame
        },
    },
};

menu_t play_menu = {
    .title = "Game Alpha",
    .num_items = 3,
    .items = {
        {
            .name = "Continue",
            .action_type = ACTION_SELECT,
            .action = M_Action_Close,
        },
        {
            .name = "Options",
            .destination = MENU_OPTIONS,
            .action_type = ACTION_SELECT,
            .action = M_Action_Navigate,
        },
        {
            .name = "Save and Quit to Title",
            .action_type = ACTION_SELECT,
            .action = M_Action_ReturnToMainMenu
        },
    },
};

menu_t options_menu = {
    .title = "Options",
    .num_items = 3,
    .items = {
        { .name = "Test Item 1", .action_type = ACTION_SELECT, .action = NULL },
        { .name = "Test Item 2", .action_type = ACTION_SELECT, .action = NULL },
        { .name = "Back", .action_type = ACTION_SELECT, .action = M_Action_GoBack },
    },
};

menu_t * menus[MENU_COUNT] = {
    [MENU_MAIN] = &main_menu,
    [MENU_PLAY] = &play_menu,
    [MENU_OPTIONS] = &options_menu,
};

menu_t * M_GetCurrentMenu(game_t * game)
{
    if ( game->num_menus == 0 ) {
        return NULL;
    }

    return &game->menus[game->num_menus - 1];
}

void M_PushMenu(game_t * game, menu_id_t id)
{
    game->menus[game->num_menus++] = *menus[id];
}

void M_PopMenu(game_t * game)
{
    if ( game->num_menus > 0 ) {
        --game->num_menus;
    }
}



#pragma mark - MENU ACTIONS

void M_Action_GoBack(game_t * game, int unused)
{
    (void)unused;
    M_PopMenu(game);
}

void M_Action_Navigate(game_t * game, int destination)
{
    M_PushMenu(game, destination);
}

void M_Action_Close(game_t * game, int unused)
{
    (void)unused;
    game->num_menus = 0;
    UI_PopScreen(game);
    game->paused = false;
}

void M_Action_NewGame(game_t * game, int action_type)
{
    game->world = CreateWorld();
    G_PushState(game, GAME_STATE_PLAY);
    M_Action_Close(game, 0);
}

void M_Action_QuitGame(game_t * game, int action_type)
{
    game->is_running = false;
}

void M_Action_ReturnToMainMenu(game_t * game, int action_type)
{
    // TODO: save etc.
    G_PopState(game);
    M_PopMenu(game);
    M_PushMenu(game, MENU_MAIN);
}

#pragma mark -

void M_Render(game_t * game)
{
    menu_t * menu = M_GetCurrentMenu(game);
    if ( menu == NULL ) {
        return;
    }

    int h = V_CharHeight();
    int x = 32;
    int y = 32;

    V_SetRGBA(0, 255, 255, 255);
    V_PrintString(x, y, "%s", menu->title);
    y += h * 2;

    for ( int i = 0; i < menu->num_items; i++ ) {
        if ( menu->selected_item == i ) {
            V_SetRGB(255, 170, 170);
        } else {
            V_SetGray(255);
        }

        V_PrintString(x, y, "%s", menu->items[i].name);
        y += h + 8;
    }
}

static void M_MoveUp(menu_t * menu)
{
    --menu->selected_item;
    if ( menu->selected_item < 0 ) {
        menu->selected_item = menu->num_items - 1;
    }
}

static void M_MoveDown(menu_t * menu)
{
    ++menu->selected_item;
    if ( menu->selected_item >= menu->num_items ) {
        menu->selected_item = 0;
    }
}

bool M_ProcessControls(game_t * game)
{
    menu_t * menu = M_GetCurrentMenu(game);
    if ( menu == NULL ) {
        return false;
    }

    menu_item_t * item = &menu->items[menu->selected_item];

    if ( game->control_state.controls[CONTROL_MENU_TOGGLE] ) {
        M_Action_Close(game, 0);
        return true;
    } else if ( game->control_state.controls[CONTROL_MENU_UP] ) {
        M_MoveUp(menu);
        return true;
    } else if ( game->control_state.controls[CONTROL_MENU_DOWN] ) {
        M_MoveDown(menu);
        return true;
    } else if ( game->control_state.controls[CONTROL_MENU_SELECT] ) {
        if ( item->action && item->action_type == ACTION_SELECT ) {
            item->action(game, item->destination);
        }
        return true;
    }

    return false;
}
