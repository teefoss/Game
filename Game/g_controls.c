//
//  g_controls.c
//  Game
//
//  Created by Thomas Foster on 10/16/22.
//

#include "g_game.h"
#include "mylib/video.h"

// TODO: load from file or something
// TODO: use mapping
const control_t controls[NUM_CONTROLS] = {
    [CONTROL_INVENTORY_TOGGLE] = {
        .button = SDL_CONTROLLER_BUTTON_Y,
        .key = SDL_SCANCODE_TAB,
        .state = IN_PRESSED,
    },
    [CONTROL_INVENTORY_SELECT] = {
        .button = SDL_CONTROLLER_BUTTON_A,
        .mouse_button = SDL_BUTTON_LEFT,
        .state = IN_PRESSED,
    },
    [CONTROL_MENU_TOGGLE] = {
        .button = SDL_CONTROLLER_BUTTON_START,
        .key = SDL_SCANCODE_ESCAPE,
        .state = IN_PRESSED,
    },
    [CONTROL_MENU_UP] = {
        .button = SDL_CONTROLLER_BUTTON_DPAD_UP,
        .key = SDL_SCANCODE_UP,
        .state = IN_PRESSED,
    },
    [CONTROL_MENU_DOWN] = {
        .button = SDL_CONTROLLER_BUTTON_DPAD_DOWN,
        .key = SDL_SCANCODE_DOWN,
        .state = IN_PRESSED,
    },
    [CONTROL_MENU_SELECT] = {
        .button = SDL_CONTROLLER_BUTTON_A,
        .key = SDL_SCANCODE_RETURN,
        .state = IN_PRESSED,
    },
    [CONTROL_PLAYER_MOVE_UP] = {
        .button = SDL_CONTROLLER_BUTTON_DPAD_UP,
        .key = SDL_SCANCODE_W,
        .state = IN_HELD,
    },
    [CONTROL_PLAYER_MOVE_DOWN] = {
        .button = SDL_CONTROLLER_BUTTON_DPAD_DOWN,
        .key = SDL_SCANCODE_S,
        .state = IN_HELD,
    },
    [CONTROL_PLAYER_MOVE_LEFT] = {
        .button = SDL_CONTROLLER_BUTTON_DPAD_LEFT,
        .key = SDL_SCANCODE_A,
        .state = IN_HELD,
    },
    [CONTROL_PLAYER_MOVE_RIGHT] = {
        .button = SDL_CONTROLLER_BUTTON_DPAD_RIGHT,
        .key = SDL_SCANCODE_D,
        .state = IN_HELD,
    },
    [CONTROL_PLAYER_STRIKE_UP] = {
        .button = SDL_CONTROLLER_BUTTON_INVALID,
        .key = SDL_SCANCODE_I,
        .state = IN_PRESSED
    },
    [CONTROL_PLAYER_STRIKE_DOWN] = {
        .button = SDL_CONTROLLER_BUTTON_INVALID,
        .key = SDL_SCANCODE_K,
        .state = IN_PRESSED
    },
    [CONTROL_PLAYER_STRIKE_LEFT] = {
        .button = SDL_CONTROLLER_BUTTON_INVALID,
        .key = SDL_SCANCODE_J,
        .state = IN_PRESSED
    },
    [CONTROL_PLAYER_STRIKE_RIGHT] = {
        .button = SDL_CONTROLLER_BUTTON_INVALID,
        .key = SDL_SCANCODE_L,
        .state = IN_PRESSED
    },
};

bool control_state[NUM_CONTROLS];

static bool ControlPressed(input_state_t * input,  control_id_t control_id)
{
    control_t ctrl = controls[control_id];
    button_state_t state = ctrl.state;

    return
    IN_GetControllerButtonState(input, ctrl.button) == state ||
    IN_GetKeyState(input, ctrl.key) == state ||
    IN_GetMouseButtonState(input, ctrl.mouse_button) == state;
}

void G_UpdateControlState(input_state_t * input, control_state_t * state)
{
    for ( int i = 0; i < NUM_CONTROLS; i++ ) {
        state->controls[i] = ControlPressed(input, i);
    }

    state->left_stick = IN_GetStickDirection(input, SIDE_LEFT);
    state->right_stick = IN_GetStickDirection(input, SIDE_RIGHT);
    state->left_trigger = IN_GetTriggerState(input, SIDE_LEFT);
    state->right_trigger = IN_GetTriggerState(input, SIDE_RIGHT);

    float a = 0.5f;

    if ( !state->menu_direction_pressed
        && fabsf(state->left_stick.y) > fabsf(state->left_stick.x) )
    {
        if ( state->left_stick.y < -a ) {
            state->controls[CONTROL_MENU_UP] = true;
            state->menu_direction_pressed = true;
        } else if ( state->left_stick.y > a ) {
            state->controls[CONTROL_MENU_DOWN] = true;
            state->menu_direction_pressed = true;
        }
    } else if ( fabsf(state->left_stick.y) < a ) {
        state->menu_direction_pressed = false;
    }

    // Update game cursor position from real mouse position.

    vec2_t mouse_position = IN_GetMousePosition(input);

    float scale_x;
    float scale_y;
    SDL_RenderGetScale(renderer, &scale_x, &scale_y);
    mouse_position.x /= scale_x;
    mouse_position.y /= scale_y;

    state->cursor_x = (int)mouse_position.x;
    state->cursor_y = (int)mouse_position.y;
}

bool G_ControlPressed(const control_state_t * control_state, control_id_t id)
{
    return control_state->controls[id];
}
