//
//  menu.h
//  Game
//
//  Created by Thomas Foster on 10/12/22.
//

#ifndef menu_h
#define menu_h

#include "ui_screen.h"
#include "mylib/array.h"
#include <stdbool.h>
#include <SDL.h>

/*
 MAIN MENU
 New Game
 Load Game
 Options
 Quit

 PLAY MENU
 Options
 Save & Quit
 */

typedef struct game game_t;
typedef struct input_state input_state_t;
typedef struct menu menu_t;

typedef enum {
    MENU_MAIN,
    MENU_OPTIONS,
    MENU_PLAY,
    MENU_COUNT
} menu_id_t;

typedef enum {
    ACTION_SELECT,
} action_type_t;

typedef struct {
    const char * name;
    int destination;
    action_type_t action_type;
    void (* action)(game_t * game, int type);
} menu_item_t;

#define MAX_MENU_ITEMS 20

struct menu {
    const char * title;

    int selected_item;
    int num_items;
    menu_item_t items[MAX_MENU_ITEMS];
};

extern menu_t main_menu;
extern menu_t play_menu;

void M_PushMenu(game_t * game, menu_id_t id);
void M_PopMenu(game_t * game);

bool M_ProcessControls(game_t * game, screen_t * screen);
void M_Render(game_t * game, screen_t * screen);
void M_Action_Close(game_t * game, int unused);

#endif /* menu_h */
