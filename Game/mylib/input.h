// -----------------------------------------------------------------------------
// SDL Input Library
//
// Handle controller connect/disconnect, and controller, keyboard, and mouse
// state.
// -----------------------------------------------------------------------------
#ifndef __INPUT_H__
#define __INPUT_H__

#include "genlib.h"
#include "mathlib.h"

// these are really just way shorter renames of SDL defines
#define BUTTON_UP      SDL_BUTTON_BUTTON_DPAD_UP
#define BUTTON_DOWN    SDL_BUTTON_BUTTON_DPAD_DOWN
#define BUTTON_LEFT    SDL_BUTTON_BUTTON_DPAD_LEFT
#define BUTTON_RIGHT   SDL_BUTTON_BUTTON_DPAD_RIGHT
#define BUTTON_A       SDL_BUTTON_BUTTON_A
#define BUTTON_B       SDL_BUTTON_BUTTON_B
#define BUTTON_X       SDL_BUTTON_BUTTON_X
#define BUTTON_Y       SDL_BUTTON_BUTTON_Y

/// Side for controller buttons that are found on either side
typedef enum {
    SIDE_LEFT,
    SIDE_RIGHT
} controllerSide_t;

extern const u8 * keyboard;
extern SDL_GameController * controller;

inline bool IsKeyDown(SDL_Scancode scancode)
{
    return keyboard[scancode];
}

inline bool IsButtonDown(SDL_GameControllerButton button)
{
    return SDL_GameControllerGetButton(controller, button);
}

/// Initialize the the SDL game controller subsystem, the `keyboard` state
/// array, and connect any controllers.
void InitInput(void);

 /// Handle connecting a controller. This should be called in response
 /// to the SDL event `SDL_CONTROLLERDEVICEADDED`.
void ConnectController(void);

 /// Handle disconnecting a controller. This should be called in response
 /// to the SDL event `SDL_CONTROLLERDEVICEREMOVED`.
void DisconnectController(void);

/// Get the direction of the right or left controller stick
/// - Returns: a vector, of length between 0.0 and 1.0, representing
/// the stick direction and distance from center.
vec2_t ControllerStickDirection(controllerSide_t stick);

/// Get the amount the right or left trigger is despressed.
/// - Returns: a value between 0.0 and 1.0 represent how far down
/// the trigger is depressed.
float ControllerTriggerState(controllerSide_t trigger);

#endif /* __INPUT_H__ */
