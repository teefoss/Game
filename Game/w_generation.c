//
//  w_generation.c
//  Game
//
//  Created by Thomas Foster on 9/13/22.
//
//  World generation code

#include "w_world.h"
#include "g_game.h"
#include "mylib/video.h"

static const float terrain_elevations[NUM_TERRAIN_TYPES] = {
    -1.00, // deep ocean
    -0.45, // shallow ocean
    //-0.20, // beach
    -0.20, // grass -0.15
     0.05, // forest
     0.30, // dark forest
     1.00,
};

// Used by GenerateTerrain() to initialize and set up a world tile at x, y, and
// assign its properties according a noise value for the tile.
static void SetUpTile(tile_t * tile, float tile_noise)
{
    // select terrain per noise (elevation)
    for ( int i = 0; i < NUM_TERRAIN_TYPES - 1; i++ ) {
        if ( tile_noise < terrain_elevations[i + 1] ) {
            tile->terrain = i;
            break;
        }
    }

    tile->variety = Random(0, 255);
    tile->effect = NULL;
}

// This is filled by GenerateTerrain during world generation
// and is used in later stages of generation.
struct {
    int x, y;
    tile_t * tile;
} grass_tiles[WORLD_WIDTH * WORLD_HEIGHT];
int num_grass_tiles = 0;

// Used by CreateWorld() to generate all terrain.
bool initial_generation;
static void GenerateTerrainInChunk(world_t * world, chunk_coord_t chunk_coord)
{
    RandomizeNoise(0);

    float half_width = WORLD_WIDTH / 2.0f;
    float half_height = WORLD_HEIGHT / 2.0f;

//    num_grass_tiles = 0;
    tile_coord_t corner = ChunkToTile(chunk_coord);
    tile_coord_t tile_coord;
    for (tile_coord.y = corner.y;
         tile_coord.y < corner.y + CHUNK_SIZE;
         tile_coord.y++) {
        for (tile_coord.x = corner.x;
             tile_coord.x < corner.x + CHUNK_SIZE;
             tile_coord.x++ ) {

            // get distance from this point to center of world
            float distance = DISTANCE
            (   half_width,
                half_height,
                tile_coord.x,
                tile_coord.y );

            // Use a gradient to apply a circular mask to the world.
            // Its radius is equal to WORLD_HEIGHT / 2.
            float gradient;

            float noise;

            // set noise and gradient
            if ( distance < half_height ) {
                // A gradient is applied around the world center: the farther
                // out a tile is, the more it's elevation is lowered.
                gradient = MAP(distance, 0.0f, half_height, 0.0f, 1.0f);
                noise = Noise2
                (   tile_coord.x,
                    tile_coord.y,
                    1.0f,
                    0.01f,
                    6,
                    1.0f,
                    0.5f,
                    2.0f);
                noise -= gradient;
            } else {
                // Outside the circular mask, land is removed entirely.
                noise = -1.0f;
            }

            tile_t * tile = GetTile(world->tiles, tile_coord.x, tile_coord.y);
            SetUpTile(tile, noise);

            // Track grass tiles for player spawn.
            if ( initial_generation && tile->terrain >= TERRAIN_GRASS ) {
                grass_tiles[num_grass_tiles].tile = tile;
                grass_tiles[num_grass_tiles].x = tile_coord.x;
                grass_tiles[num_grass_tiles].y = tile_coord.y;
                num_grass_tiles++;
            }
        }
    }
}

// track occupied tiles during generation
static bool occupied[WORLD_HEIGHT][WORLD_WIDTH];

// TODO: Ensure the player does not spawn on a tiny off-shore island.
// Get the highest elevation tile in the spawn region and
// flood fill from it to get a list of tiles that are (hopefully) on a main
// island.
void SpawnPlayer(world_t * world)
{
    // make a list of grass tiles ordered by how close they are to the
    // the center of the world and select one at random

    int center_x = WORLD_WIDTH / 2;
    int center_y = WORLD_HEIGHT / 2;

    int num_potentials = 0;
    struct potential_tile {
        tile_coord_t tile_coord;
        int distance;
    } potentials[WORLD_WIDTH * WORLD_HEIGHT];

    // fill the list
    // TODO: only check generated chunks
    for ( int y = 0; y < WORLD_HEIGHT; y++ ) {
        for ( int x = 0; x < WORLD_WIDTH; x++ ) {
            tile_t * tile = GetTile(world->tiles, x, y);
            if ( tile->terrain == TERRAIN_GRASS ) {
                potentials[num_potentials].tile_coord.x = x;
                potentials[num_potentials].tile_coord.y = y;
                potentials[num_potentials].distance
                    = DISTANCE(x, y, center_x, center_y);
                num_potentials++;
            }
        }
    }

    // sort the list by distance: earlier indices are closer to center of world
    for ( int i = 0; i < num_potentials; i++ ) {
        for ( int j = i + 1; j < num_potentials; j++ ) {
            if ( potentials[j].distance < potentials[i].distance) {
                SWAP(potentials[i], potentials[j]);
            }
        }
    }

    const int spawn_tile_count = 256;
    if ( num_potentials < spawn_tile_count ) {
        Error("Somehow there are < %d grass tiles in the world!", spawn_tile_count);
    }

    // Select one of the 50 grass tiles closest to the center of world.
    Randomize();
    int i = Random(0, spawn_tile_count - 1);
    occupied[potentials[i].tile_coord.y][potentials[i].tile_coord.x] = true;

    position_t position = GetTileCenter(potentials[i].tile_coord);
    SpawnActor(ACTOR_PLAYER, position, world);
    world->player = (actor_t *)GetElement(world->actors, 0);
    world->camera = position;
    world->camera_target = position;
}

void SpawnActorsInChunk(world_t * world, chunk_coord_t chunk_coord)
{
    tile_coord_t corner = ChunkToTile(chunk_coord);

    tile_coord_t tile_coord;
    for (tile_coord.y = corner.y;
         tile_coord.y < corner.y + CHUNK_SIZE;
         tile_coord.y++)
    {
        for (tile_coord.x = corner.x;
             tile_coord.x < corner.x + CHUNK_SIZE;
             tile_coord.x++ )
        {
            if ( occupied[tile_coord.y][tile_coord.x] ) {
                continue;
            }

            tile_t * tile = GetTile(world->tiles, tile_coord.x, tile_coord.y);
            position_t v = GetTileCenter(tile_coord);
            float r = SCALED_TILE_SIZE / 3;
            v.x += RandomFloat(-r, r);
            v.y += RandomFloat(-r, r);

            switch ( tile->terrain ) {
                case TERRAIN_GRASS:
                    // butterflies
                    if ( Chance(1.0f / 80.0f) ) {
                        vec2_t p = GetTileCenter(tile_coord);
                        actor_t * actor = SpawnActor(ACTOR_BUTTERFLY, p, world);
                        actor->z = 16;
                        continue;
                    }

                    // trees
                    if ( Chance(1.0f / 100.0f) ) {
                        SpawnActor(ACTOR_TREE, v, world);
                        occupied[tile_coord.y][tile_coord.x] = true;
                        continue;
                    }

                    // bushes
                    if ( Chance(1.0f / 50.0f) ) {
                        SpawnActor(ACTOR_BUSH, v, world);
                        occupied[tile_coord.y][tile_coord.x] = true;
                        continue;
                    }
                    break;
                case TERRAIN_FOREST:
                    if ( Chance(1.0f / 3.0f) ) {
                        SpawnActor(ACTOR_TREE, v, world);
                        occupied[tile_coord.y][tile_coord.x] = true;
                        continue;
                    }
                    break;
                default:
                    break;
            }
        }
    }
}

void SpawnActors(world_t * world)
{
    tile_coord_t tile_coord;
    for ( tile_coord.y = 0; tile_coord.y < WORLD_HEIGHT; tile_coord.y++ ) {
        for ( tile_coord.x = 0; tile_coord.x < WORLD_WIDTH; tile_coord.x++ ) {
            if ( occupied[tile_coord.y][tile_coord.x] ) {
                continue;
            }

            tile_t * tile = GetTile(world->tiles, tile_coord.x, tile_coord.y);
            position_t tile_center = GetTileCenter(tile_coord);

            float r = SCALED_TILE_SIZE / 3;
            position_t rand_pt = tile_center;
            rand_pt.x += RandomFloat(-r, r);
            rand_pt.y += RandomFloat(-r, r);

            switch ( tile->terrain ) {
                case TERRAIN_GRASS:
                    // butterflies
                    if ( Chance(1.0f / 80.0f) ) {
                        actor_t * actor = SpawnActor(ACTOR_BUTTERFLY, rand_pt, world);
                        actor->z = Random(12, 16);
                        continue;
                    }

                    // trees
                    if ( Chance(1.0f / 100.0f) ) {
                        SpawnActor(ACTOR_TREE, rand_pt, world);
                        occupied[tile_coord.y][tile_coord.x] = true;
                        continue;
                    }

                    // bushes
                    if ( Chance(1.0f / 50.0f) ) {
                        SpawnActor(ACTOR_BUSH, rand_pt, world);
                        occupied[tile_coord.y][tile_coord.x] = true;
                        continue;
                    }
                    break;
                case TERRAIN_FOREST:
                    if ( Chance(1.0f / 3.0f) ) {
                        SpawnActor(ACTOR_TREE, rand_pt, world);
                        occupied[tile_coord.y][tile_coord.x] = true;
                        continue;
                    }
                    break;
                default:
                    break;
            }
        }
    }
}

//static int GenerateAllTileNoise(void * data)
//{
//    world_t * world = (world_t *)data;
//
//    for ( int y = 0; y < WORLD_HEIGHT; y++ ) {
//        for ( int x = 0; x < WORLD_WIDTH; x++ ) {
//            tile_t * tile = GetTile(world->tiles, x, y);
//            if ( !tile->did_generate_noise && tile->terrain == TERRAIN_GRASS ) {
//                GenerateTileNoise(tile, x, y);
//                printf("generated tile noise at %d, %d\n", x, y);
//            }
//        }
//    }
//
//    return 0;
//}

void RenderAllGrassTextures(tile_t * world_tiles)
{
    for ( int i = 0; i < num_grass_tiles; i++ ) {
        int x = grass_tiles[i].x;
        int y = grass_tiles[i].y;

        tile_t * adjacents[NUM_DIRECTIONS];
        GetAdjacentTiles(x, y, world_tiles, adjacents);
        RenderGrassEffectTexture(grass_tiles[i].tile, adjacents, x, y);
    }
}

void LoadChunkIfNeeded(world_t * world, chunk_coord_t chunk_coord)
{
    if ( world->loaded_chunks[chunk_coord.y][chunk_coord.x] ) {
        return;
    }

    GenerateTerrainInChunk(world, chunk_coord);
    SpawnActorsInChunk(world, chunk_coord);

    world->loaded_chunks[chunk_coord.y][chunk_coord.x] = true;
    printf("loaded chunk %d, %d\n", chunk_coord.x, chunk_coord.y);
}

void LoadChunkInRegion(world_t * world, position_t center, int tile_radius)
{
    tile_coord_t center_tile = PositionToTile(center);

    // Upper left corner of load region.
    tile_coord_t min_tile = {
        center_tile.x - tile_radius,
        center_tile.y - tile_radius
    };

    // Lower right corner of load region.
    tile_coord_t max_tile = {
        center_tile.x + tile_radius,
        center_tile.y + tile_radius
    };

    chunk_coord_t min_chunk = TileToChunk(min_tile);
    chunk_coord_t max_chunk = TileToChunk(max_tile);

    chunk_coord_t chunk;
    for ( chunk.y = min_chunk.y; chunk.y <= max_chunk.y; chunk.y++ ) {
        for ( chunk.x = min_chunk.x; chunk.x <= max_chunk.x; chunk.x++ ) {
            LoadChunkIfNeeded(world, chunk);
        }
    }
}

world_t * CreateWorld(void)
{
    world_t * world = calloc(1, sizeof(*world));
    if ( world == NULL ) {
        Error("could not allocate world");
    }

    world->clock = MORNING_END_TICKS;

    memset(occupied, 0, sizeof(occupied));

    world->actors = NewArray(0, sizeof(actor_t));
    world->pending_actors = NewArray(0, sizeof(actor_t));

    // Generate tiles near the center of the world.
    PROFILE_START(spawn_generation);
    initial_generation = true;
    num_grass_tiles = 0;
    tile_coord_t center_tile = { WORLD_WIDTH / 2, WORLD_HEIGHT / 2 };
    LoadChunkInRegion(world, TileToPosition(center_tile), 32);
    initial_generation = false;
    PROFILE_END(spawn_generation);

    SpawnPlayer(world);

    printf("num actors: %d\n", world->actors->count);

    return world;
}
