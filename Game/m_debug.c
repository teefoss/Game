//
//  m_debug.c
//  Game
//
//  Created by Thomas Foster on 9/22/22.
//

#include "m_debug.h"
#include "g_game.h"
#include "w_world.h"
#include "mylib/video.h"
#include "mylib/input.h"

// Debug info, toggled by function keys.
bool show_geometry;
bool show_world;
bool show_debug_info;
bool show_inventory;
bool show_chunk_map;

int debug_hours;
int debug_minutes;

vec2_t mouse_tile;

int frame;
int frame_ms;
int render_ms;
int update_ms;
float debug_dt;

void DisplayScreenGeometry(void)
{
    //SDL_RenderSetScale(renderer, DRAW_SCALE, DRAW_SCALE);
    int hw = (GAME_WIDTH / 2);
    int hh = (GAME_HEIGHT / 2);
    V_SetRGBA(255, 0, 0, 128);
    SDL_RenderDrawLine(renderer, hw, 0, hw, GAME_HEIGHT);
    SDL_RenderDrawLine(renderer, 0, hh, GAME_WIDTH, hh);
}

void DisplayGeneralInfo(world_t * world)
{
    V_SetGray(255);
    int h = V_CharHeight();
    int row = 0;

    V_PrintString(0, row++ * h, "Frame time: %2d ms", frame_ms);
    V_PrintString(0, row++ * h, "- Render time: %2d ms", render_ms);
    V_PrintString(0, row++ * h, "- Update time: %2d ms", update_ms);
    V_PrintString(0, row++ * h, "- dt: %.3f ms", debug_dt);
    V_PrintString(0, row++ * h, "Camera Tile: %.2f, %.2f",
          world->camera.x / SCALED_TILE_SIZE,
          world->camera.y / SCALED_TILE_SIZE);
    V_PrintString(0, row++ * h, "%2d:%02d %s",
          debug_hours > 12 ? debug_hours - 12 : debug_hours,
          debug_minutes,
          debug_hours < 12 ? "AM" : "PM" );
}

void DisplayTileInfo(world_t * world, vec2_t mouse_position)
{
    SDL_Rect visible_rect = GetVisibleRect(world->camera);
    vec2_t upper_left = { visible_rect.x, visible_rect.y };

    vec2_t mouse_coord = Vec2Add(mouse_position, upper_left); // world space
    mouse_tile = Vec2Scale(mouse_coord, 1.0f / SCALED_TILE_SIZE);
    tile_t * tile = GetTile(world->tiles, mouse_tile.x, mouse_tile.y);

    V_PrintString(GAME_WIDTH * 0.5, 0,
          "Mouse Tile: %d, %d\n"
          "- Tile light: %.1f, %.1f, %.1f",
          (int)mouse_tile.x, (int)mouse_tile.y,
          tile->lighting.x,
          tile->lighting.y,
          tile->lighting.z);
}

void DisplayPlayerInventory(array_t * actors)
{
    actor_t * player = GetActorType(actors, ACTOR_PLAYER);
    inventory_t * inventory = player->info.player.inventory;

    int row = 0;
    int h = V_CharHeight();
    V_SetGray(255);
    V_PrintString(0, h*row++, "Player Inventory (%d items)", inventory->num_items);

    for ( int i = 0; i < inventory->num_items; i++ ) {
        V_PrintString(0, h*row++, "%d: %s", i, ActorName(inventory->items[i].type));
    }

    for ( int y = 0; y < inventory->grid_height; y++ ) {
        for ( int x = 0; x < inventory->grid_width; x++ ) {
            V_PrintString(x * V_CharWidth() * 2, h * row, "%02X", inventory->grid[y][x]);
        }
        row++;
    }
}

void DisplayChunkMap(world_t * world)
{
    for ( int y = 0; y < WORLD_HEIGHT / CHUNK_SIZE; y++ ) {
        for ( int x = 0; x < WORLD_WIDTH / CHUNK_SIZE; x++ ) {
            int darken = (x + y) % 2 == 0 ? -32 : 0;
            int size = TILE_SIZE;
            SDL_Rect r = { x * size, y * size, size, size };
            if ( world->loaded_chunks[y][x] ) {
                V_SetRGB(255 + darken, 64 + darken, 64 + darken);
                V_FillRect(&r);
            } else {
                V_SetRGB(64 + darken, 255 + darken, 64 + darken);
                V_FillRect(&r);
            }
        }
    }

    actor_t * player = GetActorType(world->actors, ACTOR_PLAYER);
    vec2_t pt = { player->pos.x / SCALED_TILE_SIZE, player->pos.y / SCALED_TILE_SIZE };
    V_SetGray(255);
    SDL_RenderDrawLine(renderer, pt.x, 0, pt.x, WORLD_HEIGHT);
    SDL_RenderDrawLine(renderer, 0, pt.y, WORLD_WIDTH, pt.y);

    SDL_Rect load_region = {
        .x = pt.x - CHUNK_LOAD_RADIUS_TILES,
        .y = pt.y - CHUNK_LOAD_RADIUS_TILES,
        .w = CHUNK_LOAD_RADIUS_TILES * 2,
        .h = CHUNK_LOAD_RADIUS_TILES * 2
    };
    V_DrawRect(&load_region);

    SDL_Rect screen = GetVisibleRect(world->camera);
    screen.x /= SCALED_TILE_SIZE;
    screen.y /= SCALED_TILE_SIZE;
    screen.w /= SCALED_TILE_SIZE;
    screen.h /= SCALED_TILE_SIZE;
    V_DrawRect(&screen);
}

void DisplayDebugInfo(world_t * world, vec2_t mouse_position)
{
    if ( show_geometry ) {
        DisplayScreenGeometry();
    }

    // draw debug world texture
    if ( show_world ) {
        SDL_Rect dst = { 0, 0, GAME_HEIGHT, GAME_HEIGHT };
        V_DrawTexture(world->debug_map, NULL, &dst);
    }

    if ( show_debug_info ) {
        DisplayGeneralInfo(world);
    }

    if ( show_debug_info ) {
        DisplayTileInfo(world, mouse_position);
    }

    if ( show_inventory ) {
        DisplayPlayerInventory(world->actors);
    }

    if ( show_chunk_map ) {
        DisplayChunkMap(world);
    }
}

bool ProcessDebugEvent(game_t * game, const SDL_Event * event)
{
    if ( event->type != SDL_KEYDOWN ) {
        return false;
    }

    switch ( event->key.keysym.sym ) {
        case SDLK_F1:
            show_debug_info = !show_debug_info;
            return true;
        case SDLK_F2:
            show_world = !show_world;
            if ( show_world ) {
                UpdateDebugMap
                (   game->world->tiles,
                    &game->world->debug_map,
                    game->world->camera );
            }
            return true;
        case SDLK_F3:
            show_geometry = !show_geometry;
            return true;
        case SDLK_F4:
            show_inventory = !show_inventory;
            return true;
        case SDLK_F5:
            show_chunk_map = !show_chunk_map;
            return true;
        case SDLK_RIGHT:
            game->world->clock += HOUR_TICKS / 2;
            return true;
        case SDLK_LEFT:
            game->world->clock -= HOUR_TICKS / 2;
            if ( game->world->clock < 0 ) {
                game->world->clock += DAY_LENGTH_TICKS;
            }
            return true;
        default:
            break;
    }

    return false;
}
