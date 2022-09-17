//
//  w_generation.c
//  Game
//
//  Created by Thomas Foster on 9/13/22.
//
//  World generation code

#include "w_world.h"
#include "mylib/video.h"

static const float terrain_elevations[NUM_TERRAIN_TYPES] = {
    -1.00, // deep ocean
    -0.45, // shallow ocean
    -0.20, // beach
    -0.15, // grass
     0.05, // forest
     0.30, // dark forest
     1.00,
};

// for debug texture
static SDL_Color layer_colors[] = {
    { 0x00, 0x00,  160, 0xFF },
    {   32,   32,  200, 0xFF },
    { 0xD2, 0xC2, 0x90, 0xFF },
    { 0x22, 0x8B, 0x22, 0xFF },
    { 0x11, 0x60, 0x11, 0xFF },
    {   80,   80,   90, 0xFF },
    {  248,  248,  248, 0xFF },
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

            // render pixel to debug texture for this tile's terrain type
            SetColor(layer_colors[i]);
            DrawPoint(x, y);

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

    world->debug_texture = CreateTexture(WORLD_WIDTH, WORLD_HEIGHT);
    SDL_SetRenderTarget(renderer, world->debug_texture);

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

    SDL_SetRenderTarget(renderer, NULL);

    world->camera.x = half_width + 40.5f;
    world->camera.y = half_height - 30.5f;
}

void SpawnPlayer(world_t * world)
{
    // make a list of grass tiles ordered by how close they are to the
    // the center of the world and selector

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

    // sort the list
    for ( int i = 0; i < num_potentials; i++ ) {
        for ( int j = i + 1; j < num_potentials; j++ ) {
            if ( potentials[j].distance < potentials[i].distance) {
                SWAP(potentials[i], potentials[j]);
            }
        }
    }

    if ( num_potentials < 50 ) {
        Error("Somehow there are < 50 grass tiles in the world!");
    }

    int i = Random(0, 49);
    vec2_t position = { potentials[i].x, potentials[i].y };
    position = AddVectors(position, (vec2_t){ 0.5f, 0.5f }); // center in tile
    SpawnActor(ACTOR_PLAYER, position, &world->actors);
}

world_t * CreateWorld(void)
{
    world_t * world = malloc(sizeof(*world));
    if ( world == NULL ) {
        Error("could not allocate world");
    }

    GenerateTerrain(world);
    SpawnPlayer(world);

    return world;
}