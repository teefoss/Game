//
//  game.c
//  Game
//
//  Created by Thomas Tillistrand on 9/5/22.
//

#include "game.h"
#include "w_world.h"
#include "m_debug.h"

#include "mylib/genlib.h"
#include "mylib/video.h"
#include "mylib/text.h"
#include "mylib/texture.h"
#include "mylib/input.h"
#include "mylib/input.h"

#include <SDL.h>

static void PlayUpdate(game_t * game, float dt);
static void PlayRender(game_t * game);
static void InventoryUpdate(game_t * game, float dt);
static void InventoryRender(game_t * game);

game_state_t game_state_play = {
    .update = PlayUpdate,
    .render = PlayRender,
};

game_state_t game_state_inventory = {
    .update = InventoryUpdate,
    .render = InventoryRender,
};

// TODO: load from file or something
const control_t controls[NUM_CONTROLS] = {
    [CONTROL_INVENTORY] = {
        .button = SDL_CONTROLLER_BUTTON_Y,
        .key = SDL_SCANCODE_TAB,
    },
};

bool ControlPressed(input_state_t * input,  control_list_t control)
{
    return
    I_GetControllerButtonState(input, controls[control].button) == BUTTON_STATE_PRESSED
    || I_GetKeyState(input, controls[control].key) == BUTTON_STATE_PRESSED;
}



#pragma mark - GAME STATE STACK

static void PushState(game_t * game, game_state_t state)
{
    if ( game->state_stack_top + 1 >= MAX_GAME_STATES ) {
        Error("ran out of room in game state stack, please increase MAX_GAME_STATES");
    }

    game->state[++game->state_stack_top] = state;
}

static void PopState(game_t * game)
{
    if ( --game->state_stack_top < -1 ) {
        Error("tried to pop from empty game state stack!");
    }
}



#pragma mark - PLAY STATE

static void PlayUpdate(game_t * game, float dt)
{
    // open inventory
    if ( ControlPressed(game->input_state, CONTROL_INVENTORY) ) {
        PushState(game, game_state_inventory);
    }

    UpdateWorld(game->world, game->input_state, dt);
}

static void PlayRender(game_t * game)
{
    RenderWorld(game->world);
    // TODO: HUD render goes here
}



#pragma mark - INVENTORY STATE

static void InventoryUpdate(game_t * game, float dt)
{
    // close inventory
    if ( ControlPressed(game->input_state, CONTROL_INVENTORY) ) {
        PopState(game);
    }

    UpdateWorld(game->world, NULL, dt);
}

static void InventoryRender(game_t * game)
{
    RenderWorld(game->world);

    // commence hack version:

    actor_t * player = GetActorType(game->world->actors, ACTOR_PLAYER);
    inventory_t * inv = player->info.player.inventory;

    // draw grid
    int size = 12 * DRAW_SCALE;
    for ( int y = 0; y < inv->grid_height; y++ ) {
        for ( int x = 0; x < inv->grid_width; x++ ) {
            V_SetGray(255);
            SDL_Rect r = { x * size, y * size, size, size };
            V_DrawRect(&r);
        }
    }

    for ( int i = 0; i < inv->num_items; i++ ) {
        int x, y;
        if ( InventoryGetGridCell(inv, i, &x, &y) ) {
            actor_t * item = &inv->items[i];
            int dst_x = x * size;
            int dst_y = y * size;
            DrawSprite(item->info.item.sprite, 0, 0, dst_x, dst_y, DRAW_SCALE, 0);
        }
    }
}



#pragma mark -

static void DoFrame(game_t * game, float dt )
{
    I_StartFrame(game->input_state);

    SDL_Event event;
    while ( SDL_PollEvent(&event) ) {

        I_ProcessEvent(game->input_state, event);

        // Handle any "universal" events
        switch ( event.type ) {
            case SDL_QUIT:
                game->is_running = false;
                return;
            case SDL_KEYDOWN:
                if ( event.key.repeat != 0 ) {
                    break;
                }

                switch ( event.key.keysym.sym ) {
                    case SDLK_ESCAPE:
                        game->is_running = false;
                        return;
                    case SDLK_F1:
                        show_debug_info = !show_debug_info;
                        break;
                    case SDLK_F2:
                        show_world = !show_world;
                        if ( show_world ) {
                            UpdateDebugMap
                            (   game->world->tiles,
                                &game->world->debug_map,
                                game->world->camera );
                        }
                        break;
                    case SDLK_F3:
                        show_geometry = !show_geometry;
                        break;
                    case SDLK_F4:
                        show_inventory = !show_inventory;
                        break;
                    case SDLK_RIGHT:
                        game->world->clock += HOUR_TICKS / 2;
                        break;
                    case SDLK_LEFT:
                        game->world->clock -= HOUR_TICKS / 2;
                        if ( game->world->clock < 0 ) {
                            game->world->clock += DAY_LENGTH_TICKS;
                        }
                        break;
                    case SDLK_BACKSLASH:
                        ToggleFullscreen(DESKTOP);
                        break;
                    default:
                        break;
                }
                break;
            default:
                break;
        }
    }

    I_Update(game->input_state);

    game->state[game->state_stack_top].update(game, dt);

    V_SetGray(0);
    V_Clear();
    game->state[game->state_stack_top].render(game);
    DisplayDebugInfo(game->world, I_GetMousePosition(game->input_state));

    V_Present();

    // debug
    static int max_render = 0;
    if ( frame > FPS * 2 && render_ms > max_render ) {
        max_render = render_ms;
        printf("max render time: %d\n", max_render);
    }

    if ( render_ms > 1000 / FPS ) {
        printf("frame %d: rending took %d ms!\n", frame, render_ms);
    }

    frame++;
}

static void GameLoop(game_t * game)
{
    float old_time = ProgramTime();
    while ( game->is_running ) {
        float new_time = ProgramTime();
        float dt = new_time - old_time;

        if ( dt < 1.0f / FPS ) {
            SDL_Delay(1);
            continue;
        }
        //dt = 1.0f / 60.0f;
//        if ( dt > 0.05f ) {
//            dt = 0.05;
//        }
        dt = FRAME_TIME_SEC;
        debug_dt = dt;

        int frame_start = SDL_GetTicks();
        DoFrame(game, dt);
        frame_ms = SDL_GetTicks() - frame_start;

        if ( frame_ms > 1000 / FPS ) {
            printf("frame took %d ms!\n", frame_ms);
        }

        old_time = new_time;
    }
}

void GameMain(void)
{
    // system init

    window_info_t info = {
        .width = GAME_WIDTH,
        .height = GAME_HEIGHT,
        .render_flags = SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC
    };
    InitWindow(&info);
    SDL_RenderSetLogicalSize(renderer, GAME_WIDTH, GAME_HEIGHT);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SetTextRenderer(renderer);
    SetTextScale(2.0f, 2.0f);

    game_t * game = calloc(1, sizeof(*game));
    PushState(game, game_state_play);
    game->world = CreateWorld();
    game->input_state = I_Initialize();
    game->is_running = true;

    // debug: check things aren't getting too big
    printf("- tile data size: %zu bytes\n", sizeof(tile_t));
    printf("- world data size: %zu bytes\n", sizeof(world_t));
    printf("- actor size: %zu bytes\n", sizeof(actor_t));

    GameLoop(game);

    // clean up
    FreeAllTextures();
    DestroyWorld(game->world);
    free(game->input_state);
    free(game);
}
