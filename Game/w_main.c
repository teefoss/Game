//
//  w_main.c
//  Game
//
//  Created by Thomas Foster on 9/11/22.
//
//  General world code and utilities functions.

#include "w_world.h"
#include "game.h"
#include "sprites.h"

#include "mylib/genlib.h"
#include "mylib/input.h"
#include "mylib/video.h"

#include <stdlib.h>

tile_t * GetTile(tile_t * tiles, int x, int y)
{
    if ( x < 0 || x >= WORLD_WIDTH ) {
        return NULL;
    }

    if ( y < 0 || y >= WORLD_HEIGHT ) {
        return NULL;
    }

    return &tiles[y * WORLD_HEIGHT + x];
}

void GetAdjacentTiles
(   int x,
    int y,
    tile_t * world_tiles,
    tile_t * out[NUM_DIRECTIONS] )
{
    out[NORTH] = GetTile(world_tiles, x, y - 1);
    out[SOUTH] = GetTile(world_tiles, x, y + 1);
    out[EAST]  = GetTile(world_tiles, x + 1, y);
    out[WEST]  = GetTile(world_tiles, x - 1, y);
}

void DestroyWorld(world_t * world)
{
    SDL_DestroyTexture(world->debug_map);
    
    for ( int i = 0; i < WORLD_WIDTH * WORLD_HEIGHT; i++ ) {
        if ( world->tiles[i].effect ) {
            SDL_DestroyTexture(world->tiles[i].effect);
        }
    }
    
    free(world);
}

SDL_Rect GetVisibleRect(vec2_t camera)
{
    SDL_Rect r = {
        .x = camera.x - GAME_WIDTH / 2.0f,
        .y = camera.y - GAME_HEIGHT / 2.0f,
        .w = GAME_WIDTH,
        .h = GAME_HEIGHT
    };

    return r;
}

void GetVisibleTileRange(world_t * world, SDL_Point * min, SDL_Point * max)
{
    SDL_Rect visible_rect = GetVisibleRect(world->camera);

    // Tile coordinate of tile visible in upper left corner.
    int min_x = visible_rect.x / SCALED_TILE_SIZE;
    int min_y = visible_rect.y / SCALED_TILE_SIZE;

    if ( min ) {
        min->x = min_x;
        min->y = min_y;
    }

    if ( max ) {
        // Number of tiles across and down the screen.
        // + 1 accounts for any screen sizes that result in a fractional
        // number of tiles.
        max->x = min_x + GAME_WIDTH / SCALED_TILE_SIZE + 1;
        max->y = min_y + GAME_HEIGHT / SCALED_TILE_SIZE + 1;
    }
}

