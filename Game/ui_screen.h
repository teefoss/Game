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

#define MAX_PANELS 20
#define MAX_UI_BUTTONS 20
#define NO_BUTTON -1

typedef enum
{
    UI_MENU,
    UI_HUD,
    UI_INVENTORY,
    UI_NUM_SCREENS,
} screen_id_t;

typedef struct {
    int num_buttons;
    SDL_Rect (* button_rect)(int index);
    SDL_Rect (* panel_rect)(void);
} panel_t;

typedef struct game game_t;
typedef struct screen screen_t;

struct screen {
    bool (* process_controls)(game_t * game, screen_t * screen);
    void (* update)(game_t * game); // TODO: dt?
    void (* render)(game_t * game, screen_t * screen);

    int num_panels;
    panel_t panels[MAX_PANELS];
};

void UI_PushScreen(game_t * game, screen_id_t id);
void UI_PopScreen(game_t * game);
bool UI_ProcessControls(game_t * game);
void UI_Update(game_t * game);
void UI_Render(game_t * game);

#endif /* ui_screen_h */
