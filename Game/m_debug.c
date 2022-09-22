//
//  m_debug.c
//  Game
//
//  Created by Thomas Foster on 9/22/22.
//

#include "m_debug.h"
#include "game.h"
#include "mylib/video.h"
#include "mylib/text.h"
#include "mylib/input.h"

// Debug info, toggled by function keys.
bool show_geometry;
bool show_world;
bool show_debug_info;

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
    int hw = GAME_WIDTH / 2;
    int hh = GAME_HEIGHT / 2;
    SetRGBA(255, 0, 0, 128);
    SDL_RenderDrawLine(renderer, hw, 0, hw, hh);
    SDL_RenderDrawLine(renderer, 0, hh, GAME_WIDTH, hh);

    for ( int y = 0; y <= GAME_HEIGHT / TILE_SIZE; y++ ) {
        for ( int x = 0; x <= GAME_WIDTH / TILE_SIZE; x++ ) {
            DrawPoint(x * TILE_SIZE, y * TILE_SIZE);
        }
    }
}

void DisplayGeneralInfo(world_t * world)
{
    SetGray(255);
    int h = CharHeight();
    int row = 0;

    Print(0, row++ * h, "Frame time: %2d ms", frame_ms);
    Print(0, row++ * h, "- Render time: %2d ms", render_ms);
    Print(0, row++ * h, "- Update time: %2d ms", update_ms);
    Print(0, row++ * h, "- dt: %.3f ms", debug_dt);
    Print(0, row++ * h, "Camera Tile: %.2f, %.2f",
          world->camera.x / TILE_SIZE,
          world->camera.y / TILE_SIZE);
    Print(0, row++ * h, "%2d:%02d %s",
          debug_hours > 12 ? debug_hours - 12 : debug_hours,
          debug_minutes,
          debug_hours < 12 ? "AM" : "PM" );
}

void DisplayTileInfo(world_t * world)
{
    SDL_Rect visible_rect = GetVisibleRect(world->camera);
    vec2_t upper_left = { visible_rect.x, visible_rect.y };

    vec2_t mouse_pixel = GetMousePosition(DRAW_SCALE); // window space
    vec2_t mouse_coord = Vec2Add(mouse_pixel, upper_left); // world space
    mouse_tile = Vec2Scale(mouse_coord, 1.0f / TILE_SIZE);
    tile_t * tile = GetTile(world->tiles, mouse_tile.x, mouse_tile.y);

    Print(GAME_WIDTH * 0.5 * DRAW_SCALE, 0,
          "Mouse Tile: %d, %d\n"
          "- Tile light: %.1f, %.1f, %.1f",
          (int)mouse_tile.x, (int)mouse_tile.y,
          tile->lighting.x,
          tile->lighting.y,
          tile->lighting.z);
}

void DisplayDebugInfo(world_t * world)
{
    if ( show_geometry ) {
        DisplayScreenGeometry();
    }

    // draw debug world texture
    if ( show_world ) {
        SDL_Rect dst = { 0, 0, GAME_HEIGHT, GAME_HEIGHT };
        DrawTexture(world->debug_map, NULL, &dst);
    }

    if ( show_debug_info ) {
        DisplayGeneralInfo(world);
    }

    if ( show_debug_info ) {
        DisplayTileInfo(world);
    }
}
