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
        .x = (int)position.x,
        .y = (int)position.y
    };

    return tile;
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
