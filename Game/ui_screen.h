//
//  ui_screen.h
//  Game
//
//  Created by Thomas Foster on 10/17/22.
//

#ifndef ui_screen_h
#define ui_screen_h

#include <SDL_events.h>
#include <stdbool.h>

typedef enum
{
    UI_MENU,
    UI_HUD,
    UI_INVENTORY,
    UI_NUM_SCREENS,
} screen_id_t;

typedef struct game game_t;

typedef struct {
    bool (* process_event)(game_t * game, const SDL_Event * event);
    bool (* process_input)(game_t * game);
    void (* update)(game_t * game); // TODO: dt?
    void (* render)(game_t * game);
} screen_t;

void UI_PushScreen(game_t * game, screen_id_t id);
void UI_PopScreen(game_t * game);
bool UI_ProcessEvent(game_t * game, const SDL_Event * event);
bool UI_ProcessInput(game_t * game);
bool UI_ProcessControls(game_t * game);
void UI_Update(game_t * game);
void UI_Render(game_t * game);

#endif /* ui_screen_h */
