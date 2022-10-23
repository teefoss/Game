//
//  game.h
//  Game
//
//  Created by Thomas Tillistrand on 9/5/22.
//

#ifndef g_game_h
#define g_game_h

#include "inventory.h"
#include "menu.h"
#include "ui_screen.h"

#include "mylib/sprite.h"
#include "mylib/vector.h"
#include "mylib/stack.h"

#define FPS 30.0f
#define FRAME_TIME_SEC (1.0f / FPS)

// The internal resolution of the game.
// Aspect ratio is 16:9
// TODO: support non-16:9 ratios?
// This makes a visible area of 30 x 16.875 tiles
// 4x this resolution = 1920x1080
#define DRAW_SCALE  3
#define GAME_WIDTH  (480 * DRAW_SCALE)
#define GAME_HEIGHT (270 * DRAW_SCALE)

typedef struct world world_t;
typedef struct input_state input_state_t;
typedef struct game game_t;
typedef struct control_state control_state_t;

typedef struct {
    SDL_GameControllerButton button;
    SDL_Scancode key;
    button_state_t state;
} control_t;

typedef enum {
    CONTROL_INVENTORY_TOGGLE,

    CONTROL_MENU_TOGGLE,
    CONTROL_MENU_UP,
    CONTROL_MENU_DOWN,
    CONTROL_MENU_SELECT,

    CONTROL_PLAYER_MOVE_UP,
    CONTROL_PLAYER_MOVE_DOWN,
    CONTROL_PLAYER_MOVE_LEFT,
    CONTROL_PLAYER_MOVE_RIGHT,

    NUM_CONTROLS,
} control_id_t;

struct control_state {
    bool controls[NUM_CONTROLS];

    vec2_t left_stick;
    vec2_t right_stick;
    float left_trigger;
    float right_trigger;

    bool menu_direction_pressed;
};

typedef enum {
    GAME_STATE_TITLE,
    GAME_STATE_PLAY,
    NUM_GAME_STATES
} game_state_id_t;

typedef struct {
    bool (* process_control)(game_t * game);
    void (* update)(game_t * game, float dt);
    void (* render)(game_t * game);
} game_state_t;

#define MAX_GAME_STATES 10
#define MAX_MENUS 10
#define MAX_SCREENS 20

struct game {
    bool is_running;
    bool paused;

    game_state_id_t states[MAX_GAME_STATES];
    int state_top;

    menu_t menus[MAX_MENUS];
    int num_menus;

    screen_id_t screens[MAX_SCREENS];
    int num_screens;

    int ticks;
    bool controls_processed;
    control_state_t control_state;

    world_t * world;
};

// game.c

void G_Main(void);
void M_Action_NewGame(game_t * game, int action_type); // TODO: move to menu
void M_Action_QuitGame(game_t * game, int action_type);
void M_Action_ReturnToMainMenu(game_t * game, int action_type);

// g_state.c

void G_PushState(game_t * game, game_state_id_t state);
void G_PopState(game_t * game);

bool G_ProcessControl(game_t * game);
void G_Update(game_t * game, float dt);
void G_Render(game_t * game);

// g_controls.c

void G_UpdateControlState(input_state_t * input, control_state_t * state);
bool G_ControlPressed(const control_state_t * control_state, control_id_t id);

#endif /* g_game_h */
