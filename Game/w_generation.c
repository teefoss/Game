//
//  w_generation.c
//  Game
//
//  Created by Thomas Foster on 9/13/22.
//
//  World generation code

#include "w_world.h"
#include "mylib/video.h"

static vec2_t GetTileCenter(int x, int y)
{
    return (vec2_t){
        x * TILE_SIZE + TILE_SIZE / 2,
        y * TILE_SIZE + TILE_SIZE / 2
    };
}

static const float terrain_elevations[NUM_TERRAIN_TYPES] = {
    -1.00, // deep ocean
    -0.45, // shallow ocean
    -0.20, // beach
    -0.15, // grass
     0.05, // forest
     0.30, // dark forest
     1.00,
};

// Used by GenerateTerrain() to initialize and set up a world tile at x, y, and
// assign its properties according a noise value for the tile.
static void
SetUpTile
(   int x,
    int y,
    tile_t * tile,
    tile_t * world_tiles,
    float tile_noise )
{
    tile_t * adjacent_tiles[NUM_DIRECTIONS];
    GetAdjacentTiles(x, y, world_tiles, adjacent_tiles);

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

// Used by CreateWorld() to generate all terrain.
static void GenerateTerrain(world_t * world)
{
    RandomizeNoise(0);

    float half_width = WORLD_WIDTH / 2.0f;
    float half_height = WORLD_HEIGHT / 2.0f;

    for ( int y = 0; y < WORLD_HEIGHT; y++ ) {
        for ( int x = 0; x < WORLD_WIDTH; x++ ) {

            // get distance from this point to center of world
            float distance = DISTANCE(half_width, half_height, x, y);

            // Use a gradient to apply a circular mask to the world.
            // Its radius is equal to WORLD_HEIGHT / 2.
            float gradient;

            float noise;

            // set noise and gradient
            if ( distance < half_height ) {
                // A gradient is applied around the world center: the farther
                // out a tile is, the more it's elevation is lowered.
                gradient = MAP(distance, 0.0f, half_height, 0.0f, 1.0f);
                noise = Noise2(x, y, 1.0f, 0.01f, 6, 1.0f, 0.5f, 2.0f) - gradient;
            } else {
                // Outside the circular mask, land is removed entirely.
                noise = -1.0f;
            }

            SetUpTile(x, y, GetTile(world->tiles, x, y), world->tiles, noise);
        }
    }

    world->camera.x = half_width + 40.5f;
    world->camera.y = half_height - 30.5f;
}

// track occupied tiles during generation
static bool occupied[WORLD_HEIGHT][WORLD_WIDTH];

void SpawnPlayer(world_t * world)
{
    // make a list of grass tiles ordered by how close they are to the
    // the center of the world and select one at random

    int center_x = WORLD_WIDTH / 2;
    int center_y = WORLD_HEIGHT / 2;

    int num_potentials = 0;
    struct potential_tile {
        int x;
        int y;
        int distance;
    } potentials[WORLD_WIDTH * WORLD_HEIGHT];

    // fill the list
    for ( int y = 0; y < WORLD_HEIGHT; y++ ) {
        for ( int x = 0; x < WORLD_WIDTH; x++ ) {
            tile_t * tile = GetTile(world->tiles, x, y);
            if ( tile->terrain == TERRAIN_GRASS ) {
                potentials[num_potentials].x = x;
                potentials[num_potentials].y = y;
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

    // select one of the 50 grass tiles closest to the center of world.
    Randomize();
    int i = Random(0, spawn_tile_count - 1);
    occupied[potentials[i].y][potentials[i].x] = true;

    vec2_t position = GetTileCenter(potentials[i].x, potentials[i].y);
    SpawnActor(ACTOR_PLAYER, position, world);

    world->camera = position;
}

world_t * CreateWorld(void)
{
    world_t * world = calloc(1, sizeof(*world));
    if ( world == NULL ) {
        Error("could not allocate world");
    }

    memset(occupied, 0, sizeof(occupied));

    GenerateTerrain(world);
    SpawnPlayer(world);

    // testing: spawn trees
    for ( int y = 0; y < WORLD_HEIGHT; y++ ) {
        for ( int x = 0; x < WORLD_WIDTH; x++ ) {
            if ( occupied[y][x] ) {
                continue;
            }

            tile_t * tile = GetTile(world->tiles, x, y);
            if ( tile->terrain == TERRAIN_GRASS
                && !occupied[y][x]
                && Random(0, 10) == 10 )
            {
                vec2_t v = GetTileCenter(x, y);
                v.x += RandomFloat(-TILE_SIZE / 2.0f, TILE_SIZE / 2.0f);
                v.y += RandomFloat(-TILE_SIZE / 2.0f, TILE_SIZE / 2.0f);
                SpawnActor(ACTOR_TREE, v, world);
                occupied[y][x] = true;
            }
        }
    }

    return world;
}
