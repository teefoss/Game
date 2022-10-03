//
//  game.h
//  Game
//
//  Created by Thomas Tillistrand on 9/5/22.
//

#ifndef game_h
#define game_h

#include "mylib/sprite.h"
#include "mylib/vector.h"
#include "inventory.h"

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

typedef struct {
    void (* update)(game_t * game, float dt);
    void (* render)(game_t * game);
} game_state_t;

#define MAX_GAME_STATES 10

struct game {
    bool is_running;

    int state_stack_top;
    game_state_t state[MAX_GAME_STATES];

    int ticks;
    input_state_t * input_state;
    world_t * world;
};

void GameMain(void);

#endif /* game_h */
