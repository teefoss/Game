//
//  w_tile.c
//  Game
//
//  Created by Thomas Foster on 9/26/22.
//

#include "w_tile.h"
#include "w_world.h"

void GetTileNoise(int tile_x, int tile_y, float out[TILE_SIZE][TILE_SIZE])
{
    for ( int y = 0; y < TILE_SIZE; y++ ) {
        for ( int x = 0; x < TILE_SIZE; x++ ) {
            int wx = tile_x * TILE_SIZE + x; // world coordinate of tile pixel
            int wy = tile_y * TILE_SIZE + y;
            out[y][x] = Noise(wx, wy, 1.0f);
        }
    }
}

vec2_t GetAdjacentTile(vec2_t position, cardinal_t direction)
{
    int tile_x = (int)(position.x / SCALED_TILE_SIZE) + x_dirs[direction];
    int tile_y = (int)(position.y / SCALED_TILE_SIZE) + y_dirs[direction];

    return (vec2_t){ tile_x, tile_y };
}
