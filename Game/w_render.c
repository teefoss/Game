//
//  w_render.c
//  Game
//
//  Created by Thomas Foster on 9/13/22.
//
//  World rendering functions.

#include "w_world.h"
#include "game.h"
#include "sprites.h"

#include "mylib/video.h"
#include "mylib/sprite.h"
#include "mylib/texture.h"

#define HORIZONTAL_NUM_TILES ((float)GAME_WIDTH / (float)TILE_SIZE)
#define VERTICAL_NUM_TILES ((float)GAME_HEIGHT / (float)TILE_SIZE)

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

//    SetRGBA(93, 163, 42, 255); // darker shade of same grass color
//    SetRGBA(114, 201, 52, 255); // faintly lighter shade of same grass color
    for ( int py = 0; py < TILE_SIZE; py++ ) {
        for ( int px = 0; px < TILE_SIZE; px++ ) {
            int wx = tile_x * 16 + px; // world pixel coord
            int wy = tile_y * 16 + py;
            float noise = Noise2(wx, wy, 1.0f, 0.01f, 8, 1.0f, 0.5f, 2.5f);
            SeedRandom(wx * wy); // TODO: more than one prng

            if ( noise > 0.2f ) { //}|| (noise > 0.05f && Random(0, 3) == 0) ) {
                SetRGBA(78, 138, 36, 255); // darker green
                DrawPoint(px, py);
            } else if ( noise > 0.1 ) {
                SetRGBA(114, 201, 52, 255); // faintly lighter shade of same grass color
                DrawPoint(px, py);
            }

            if ( noise > 0.7f ) {
                if ( Random(0, 15) == 15 ) {
                    DrawSprite(&sprites[SPRITE_TINY_YELLOW_FLOWER], px, py, 0);
                }
            } else if ( noise > 0.45f && Random(0, 20) == 20 ) {
                DrawSprite(&sprites[SPRITE_TINY_BLUE_FLOWER], px, py, 0);
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

    for ( int i = 0; i < world->actors.num_actors; i++ ) {
        actor_t * actor = &world->actors.array[i];
        sprite_t * sprite = GetActorSprite(actor);

        if ( sprite ) {
            int screen_x = GAME_WIDTH / 2 - world->camera.x - actor->position.x;
            int screen_y = GAME_HEIGHT / 2 - world->camera.y - actor->position.y;

            DrawSprite(sprite, screen_x, screen_y, 0);
        }
    }
}
