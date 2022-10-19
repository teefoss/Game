//
//  coord.h
//  Game
//
//  Created by Thomas Foster on 10/15/22.
//

#ifndef coord_h
#define coord_h

#include "mylib/genlib.h"
#include "mylib/vector.h"

typedef vec2_t position_t; // location in world pixels

#define COORD struct { s16 x, y; }

typedef COORD window_coord_t;
typedef COORD tile_coord_t;
typedef COORD chunk_coord_t;

tile_coord_t ChunkToTile(chunk_coord_t chunk);
tile_coord_t PositionToTile(position_t position);
chunk_coord_t PositionToChunk(position_t position);
chunk_coord_t TileToChunk(tile_coord_t tile_coord);

#endif /* coord_h */
