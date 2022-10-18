//
//  g_controls.c
//  Game
//
//  Created by Thomas Foster on 10/16/22.
//

#include "g_game.h"

// TODO: load from file or something
// TODO: use mapping
const control_t controls[NUM_CONTROLS] = {
    [CONTROL_INVENTORY_TOGGLE] = { SDL_CONTROLLER_BUTTON_Y, SDL_SCANCODE_TAB },
    [CONTROL_MENU_TOGGLE] = { SDL_CONTROLLER_BUTTON_START, SDL_SCANCODE_ESCAPE },
    [CONTROL_MENU_UP] = { SDL_CONTROLLER_BUTTON_DPAD_UP, SDL_SCANCODE_UP },
    [CONTROL_MENU_DOWN] = { SDL_CONTROLLER_BUTTON_DPAD_DOWN, SDL_SCANCODE_DOWN },
    [CONTROL_MENU_SELECT] = { SDL_CONTROLLER_BUTTON_A, SDL_SCANCODE_RETURN },
};

bool control_state[NUM_CONTROLS];

bool G_ControlPressed(input_state_t * input,  control_id_t control)
{
    return
    IN_GetControllerButtonState(input, controls[control].button) == IN_PRESSED
    || IN_GetKeyState(input, controls[control].key) == IN_PRESSED;
}

void G_UpdateControlState(input_state_t * input)
{
    for ( int i = 0; i < NUM_CONTROLS; i++ ) {
        control_state[i] = G_ControlPressed(input, i);
    }
}
