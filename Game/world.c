//
//  world.c
//  Game
//
//  Created by Thomas Foster on 9/11/22.
//

#include "world.h"
#include "game.h"
#include "sprites.h"

#include "mylib/genlib.h"
#include "mylib/input.h"
#include "mylib/texture.h"
#include "mylib/video.h"

#include <stdlib.h>

#define HORIZONTAL_NUM_TILES ((float)GAME_WIDTH / (float)TILE_SIZE)
#define VERTICAL_NUM_TILES ((float)GAME_HEIGHT / (float)TILE_SIZE)

static float terrain_elevations[NUM_TERRAIN_TYPES] = {
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

static tile_t * GetTile(tile_t * tiles, int x, int y)
{
    if ( x < 0 || x >= WORLD_WIDTH ) {
        return NULL;
    }

    if ( y < 0 || y >= WORLD_HEIGHT ) {
        return NULL;
    }

    return &tiles[y * WORLD_HEIGHT + x];
}

static void GetAdjacentTiles
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

world_t * CreateWorld(void)
{
    world_t * world = malloc(sizeof(*world));
    if ( world == NULL ) {
        Error("could not allocate world");
    }

    GenerateTerrain(world);

    return world;
}

// Determine the maximum point at which rect 'inner' can
// be placed in rect 'outer', accounting for a margin
static SDL_Point RectInRectMaxPoint
(   const SDL_Rect * inner,
    const SDL_Rect * outer,
    int margin )
{
    SDL_Point pt;
    pt.x = outer->w - inner->w - margin;
    pt.y = outer->h - inner->h - margin;

    return pt;
}

// Used by RenderGrassEffectTexture().
// Render flowers n stuff onto effect texture.
static void RenderGrassDecoration(sprite_id_t id, u8 sprite_variety)
{
    sprite_t * s = &sprites[id];
    SDL_Rect area = { .w = TILE_SIZE, .h = TILE_SIZE };
    SDL_Point max = RectInRectMaxPoint(&s->location, &area, 1);
    DrawSprite(s, Random(1, max.x), Random(1, max.y), sprite_variety);
}

static void RenderGrassEffectTexture(tile_t * tile, int tile_x, int tile_y)
{
    tile->effect = CreateTexture(TILE_SIZE, TILE_SIZE);
    SDL_SetTextureBlendMode(tile->effect, SDL_BLENDMODE_BLEND);
    SDL_SetRenderTarget(renderer, tile->effect);

    SetRGBA(0, 0, 0, 0);
    Clear();

    // Render moss.
    SetRGBA(78, 138, 36, 255);
    for ( int py = 0; py < TILE_SIZE; py++ ) {
        for ( int px = 0; px < TILE_SIZE; px++ ) {
            int wx = tile_x * 16 + px; // world pixel coord
            int wy = tile_y * 16 + py;
            float noise = Noise2(wx, wy, 1.0f, 0.01f, 8, 1.0f, 0.5f, 2.0f);
            SeedRandom(wx * wy); // TODO: more than one prng
            if ( noise > 0.1f || (noise > 0.05f && Random(0, 3) == 0) ) {
                DrawPoint(px, py);
            }
        }
    }

    // Sprinkle some foliage.
    // Most tiles have grass, occasionally a flower.
    if ( Random(0, 1) == 1 ) {
        if ( Random(0, 12) == 12 ) {
            RenderGrassDecoration(SPRITE_PLUS_FLOWER, tile->variety);
        } else {
            RenderGrassDecoration(SPRITE_GRASS_BLADES, tile->variety);
        }
    }

    SDL_SetRenderTarget(renderer, NULL);
}

static void RenderGrass
(   tile_t * tile,
    tile_t ** adjacent_tiles,
    int x,
    int y,
    SDL_Texture * grass_texture,
    SDL_Rect * dst )
{
    SDL_Rect src = {
        .y = 0,
        .w = TILE_SIZE,
        .h = TILE_SIZE
    };

    if ( tile->variety % 12 == 0 ) {
        src.x = TILE_SIZE * (tile->variety < 128 ? 1 : 2);
    } else {
        src.x = 0;
    }

    DrawTexture(GetTexture("grass.png"), &src, dst);

    // Generate effect texture for this tile if needed.
    if ( tile->effect == NULL ) {
        RenderGrassEffectTexture(tile, x, y);
    }

    DrawTexture(tile->effect, NULL, dst);

    // Draw highlights at water edges.
    SetRGBA(122, 214, 56, 255);
    if ( adjacent_tiles[NORTH]->terrain <= TERRAIN_SHALLOW_WATER ) {
        SDL_RenderDrawLine(renderer, dst->x, dst->y, dst->x + TILE_SIZE + 1, dst->y);
    }

    if ( adjacent_tiles[WEST]->terrain <= TERRAIN_SHALLOW_WATER ) {
        SDL_RenderDrawLine(renderer, dst->x, dst->y, dst->x, dst->y + TILE_SIZE);
    }

    if ( adjacent_tiles[EAST]->terrain <= TERRAIN_SHALLOW_WATER ) {
        SDL_RenderDrawLine
        (   renderer,
            dst->x + TILE_SIZE - 1,
            dst->y,
            dst->x + TILE_SIZE - 1,
            dst->y + TILE_SIZE - 1 );
    }
}

static void RenderVisibleTerrain(world_t * world)
{
    // Find the upper left visible tile.
    int corner_tile_x = (int)world->camera.x - HORIZONTAL_NUM_TILES / 2.0f;
    int corner_tile_y = (int)world->camera.y - VERTICAL_NUM_TILES / 2.0f;
    //printf("ul corner tile: %d, %d\n", corner_tile_x, corner_tile_y);

    SDL_Texture * grass_texture = GetTexture("grass.png");
    SDL_Texture * water_texture = GetTexture("shallow-water.png");

    vec2_t fraction = {
        .x = world->camera.x - (int)world->camera.x,
        .y = world->camera.y - (int)world->camera.y
    };

    SDL_Rect dst = {
        .y = -fraction.y * (float)TILE_SIZE,
        .w = TILE_SIZE,
        .h = TILE_SIZE,
    };

    for (int tile_y = corner_tile_y;
         tile_y <= corner_tile_y + VERTICAL_NUM_TILES + 1;
         tile_y++ ) {

        dst.x = -fraction.x * (float)TILE_SIZE;

        for (int tile_x = corner_tile_x;
             tile_x <= corner_tile_x + HORIZONTAL_NUM_TILES + 1;
             tile_x++ ) {

            tile_t * tile = GetTile(world->tiles, tile_x, tile_y);
            tile_t * adjacent_tiles[NUM_DIRECTIONS];
            GetAdjacentTiles(tile_x, tile_y, world->tiles, adjacent_tiles);

            SDL_Rect src = {
                .x = TILE_SIZE * (tile->variety % 4), // TODO: #define 4
                .w = TILE_SIZE,
                .h = TILE_SIZE
            };

            // TODO: this is an awful, awful mess
            switch ( tile->terrain ) {
                case TERRAIN_DEEP_WATER:
                    // TODO: render deep water
                case TERRAIN_SHALLOW_WATER: {
                    if ( adjacent_tiles[NORTH]
                        && adjacent_tiles[NORTH]->terrain != TERRAIN_SHALLOW_WATER
                        && adjacent_tiles[NORTH]->terrain != TERRAIN_DEEP_WATER )
                    {
                        src.y = TILE_SIZE * 1;
                    } else {
                        src.y = 0;
                    }

                    DrawTexture(water_texture, &src, &dst);
                    break;
                }
                case TERRAIN_BEACH:
                case TERRAIN_GRASS:
                case TERRAIN_FOREST:
                case TERRAIN_DARK_FOREST:
                    RenderGrass
                    (   tile,
                        adjacent_tiles,
                        tile_x,
                        tile_y,
                        grass_texture,
                        &dst );
                    break;
                default:
                    break;
            }

            dst.x += TILE_SIZE;
        }

        dst.y += TILE_SIZE;
    }

}

void RenderWorld(world_t * world)
{
    RenderVisibleTerrain(world);
}

void DestroyWorld(world_t * world)
{
    for ( int i = 0; i < WORLD_WIDTH * WORLD_HEIGHT; i++ ) {
        if ( world->tiles[i].effect ) {
            SDL_DestroyTexture(world->tiles[i].effect);
        }
    }
    
    free(world);
}
