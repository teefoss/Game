//
//  coord.c
//  Game
//
//  Created by Thomas Foster on 10/15/22.
//

#include "coord.h"
#include "w_world.h"

tile_coord_t PositionToTile(position_t position)
{
    tile_coord_t tile = {
        .x = (int)position.x / SCALED_TILE_SIZE,
        .y = (int)position.y / SCALED_TILE_SIZE
    };

    return tile;
}

tile_coord_t GetAdjacentTile(vec2_t position, cardinal_t direction)
{
    tile_coord_t tile;
    tile.x = (int)(position.x / SCALED_TILE_SIZE) + x_dirs[direction];
    tile.y = (int)(position.y / SCALED_TILE_SIZE) + y_dirs[direction];

    return tile;
}

/// Tile coordinate to center of tile in world coords.
position_t GetTileCenter(tile_coord_t tile_coord)
{
    return (position_t){
        tile_coord.x * SCALED_TILE_SIZE + SCALED_TILE_SIZE / 2,
        tile_coord.y * SCALED_TILE_SIZE + SCALED_TILE_SIZE / 2
    };
}

position_t TileToPosition(tile_coord_t tile_coord)
{
    position_t position = {
        .x = (float)tile_coord.x * SCALED_TILE_SIZE,
        .y = (float)tile_coord.y * SCALED_TILE_SIZE
    };

    return position;
}

chunk_coord_t TileToChunk(tile_coord_t tile_coord)
{
    chunk_coord_t chunk = {
        .x = tile_coord.x / CHUNK_SIZE,
        .y = tile_coord.y / CHUNK_SIZE,
    };

    return chunk;
}

chunk_coord_t PositionToChunk(position_t position)
{
    return TileToChunk(PositionToTile(position));
}

tile_coord_t ChunkToTile(chunk_coord_t chunk)
{
    tile_coord_t tile = {
        .x = chunk.x * CHUNK_SIZE,
        .y = chunk.y * CHUNK_SIZE
    };

    return tile;
}
