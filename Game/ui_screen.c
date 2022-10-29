//
//  ui_screen.c
//  Game
//
//  Created by Thomas Foster on 10/17/22.
//

#include "ui_screen.h"
#include "inventory.h"
#include "menu.h"

#include "mylib/texture.h"

SDL_Rect GridPanelRect(void)
{
    SDL_Texture * grid_texture = GetTexture(SPRITE_DIR"/inventory-grid-3x3.png");
    SDL_Rect rect = G_TextureSize(grid_texture);
    rect.x = (GAME_WIDTH - rect.w) / 2;
    rect.y = (GAME_HEIGHT - rect.h) / 2;

    return rect;
}

SDL_Rect GridButtonRect(int index)
{
    SDL_Rect grid_rect = GridPanelRect();
    int margin = 4 * DRAW_SCALE;
    int origin_x = grid_rect.x + margin;
    int origin_y = grid_rect.y + margin;
    int size = SLOT_SIZE * DRAW_SCALE;

    SDL_Rect button_rect = {
        .x = origin_x + (index % 3) * size,
        .y = origin_y + (index / 3) * size,
        .w = size,
        .h = size
    };

    return button_rect;
}

screen_t screens[UI_NUM_SCREENS] = {
    [UI_MENU]       = { M_ProcessControls, NULL, M_Render },
    [UI_HUD]        = { NULL, NULL, NULL },
    [UI_INVENTORY]  = {
        .process_controls = INV_ProcessControls,
        .update = NULL,
        .render = INV_Render,
        .num_panels = 1,
        .panels = {
            { 9, GridButtonRect, GridPanelRect },
        }
    },
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
        screen->render(game, screen);
    }
}

bool UI_ProcessControls(game_t * game)
{
    screen_t * screen = CurrentScreen(game);

    if ( screen && screen->process_controls ) {
        return screen->process_controls(game, screen);
    }

    return false;
}
