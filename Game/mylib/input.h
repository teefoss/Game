//
//  input.h
//  Game
//
//  Created by Thomas Foster on 9/27/22.
//

#ifndef input2_h
#define input2_h

#include "genlib.h"
#include "vector.h"

typedef enum {
    BUTTON_STATE_UNDEFINED = -1,
    BUTTON_STATE_NONE,
    BUTTON_STATE_PRESSED,
    BUTTON_STATE_HELD,
    BUTTON_STATE_RELEASED
} button_state_t; // also for keys

typedef enum {
    SIDE_LEFT,
    SIDE_RIGHT,
} controller_side_t;

typedef struct input_state input_state_t;

input_state_t * I_Initialize(void);
void I_StartFrame(input_state_t *);
void I_ProcessEvent(input_state_t *, SDL_Event event);
void I_Update(input_state_t *);

// Keyboard

bool I_IsKeyDown(input_state_t *, SDL_Scancode code);
button_state_t I_GetKeyState(input_state_t *, SDL_Scancode code);

// Controller

void I_ConnectController(input_state_t *);
void I_DisconnectController(input_state_t *);
bool I_IsControllerConnected(input_state_t *);
bool I_IsControllerButtonDown(input_state_t *, SDL_GameControllerButton button);
button_state_t I_GetControllerButtonState(input_state_t *, SDL_GameControllerButton button);
vec2_t I_GetStickDirection(input_state_t *, controller_side_t side);

// Mouse

bool I_IsMouseButtonDown(input_state_t *, int button);
button_state_t I_GetMouseButtonState(input_state_t * state, int button);
vec2_t I_GetMousePosition(input_state_t *);

#endif /* input2_h */
