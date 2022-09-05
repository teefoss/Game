#include "input.h"

// For controller sticks and triggeres.
// On some controllers they do not perfectly register when a controller stick
// is centered.
#define DEAD_ZONE 2400

const u8 * keyboard;
SDL_GameController * controller; // TODO: support 4 controllers

static void
Shutdown(void)
{
    if ( controller ) {
        SDL_GameControllerClose(controller);
    }
}

vec2_t
ControllerStickDirection(controllerSide_t stick)
{
    Sint16 x, y;

    if ( stick == SIDE_LEFT ) {
        x = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTX);
        y = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTY);
    } else {
        x = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_RIGHTX);
        y = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_RIGHTY);
    }

    if ( abs(x) < DEAD_ZONE ) {
        x = 0;
    }

    if ( abs(y) < DEAD_ZONE ) {
        y = 0;
    }

    vec2_t direction = { x, y };
    direction.x /= 0x7FFF;
    direction.y /= 0x7FFF;
    
    return direction;
}

float
ControllerTriggerState(controllerSide_t trigger)
{
    float n;
    if ( trigger == SIDE_LEFT ) {
        n = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_TRIGGERLEFT);
    } else {
        n = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_TRIGGERRIGHT);
    }

    return n / 0x7FFF;
}

void
ConnectController(void)
{
    if ( controller ) {
        return;
    }

    for ( int i=0 ; i<SDL_NumJoysticks() ; i++ ) {
        if ( SDL_IsGameController(i) ) {
            controller = SDL_GameControllerOpen(i);

            if ( controller == NULL ) {
                Error("failed to open controller");
            }

            const char * name = SDL_GameControllerNameForIndex(i);
            printf("Controller %s connected.\n", name);
            //SDL_GameControllerAddMappingsFromFile("assets/gamecontroller.txt");
            return;
        }
    }
}

void
DisconnectController(void)
{
    if ( !controller )
        return;

    printf("Controller %s disconnected.\n", SDL_GameControllerName(controller));
    SDL_GameControllerClose(controller);
    controller = NULL;
}

void
InitInput(void)
{
    keyboard = SDL_GetKeyboardState(NULL);

    if ( SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER) < 0 ) {
        Error("could not init SDL game controller subsystem");
    }

    ConnectController();
    atexit(Shutdown);
}

extern bool IsKeyDown(SDL_Scancode scancode);
extern bool IsButtonDown(SDL_GameControllerButton button);
