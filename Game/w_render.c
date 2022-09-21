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

void UpdateDebugMap(tile_t * tiles,  SDL_Texture ** debug_map, vec2_t camera)
{
    static const SDL_Color layer_colors[] = {
        { 0x00, 0x00,  160, 0xFF },
        {   32,   32,  200, 0xFF },
        { 0xD2, 0xC2, 0x90, 0xFF },
        { 0x22, 0x8B, 0x22, 0xFF },
        { 0x11, 0x60, 0x11, 0xFF },
        {   80,   80,   90, 0xFF },
        {  248,  248,  248, 0xFF },
    };

    if ( *debug_map == NULL ) {
        *debug_map = CreateTexture(WORLD_WIDTH, WORLD_HEIGHT);
    }

    SDL_SetRenderTarget(renderer, *debug_map);

    for ( int y = 0; y < WORLD_HEIGHT; y++ ) {
        for ( int x = 0; x < WORLD_WIDTH; x++ ) {
            tile_t * tile = GetTile(tiles, x, y);

            // render pixel to debug texture for this tile's terrain type
            SetColor(layer_colors[tile->terrain]);
            DrawPoint(x, y);
        }
    }

    SetGray(255);
    SDL_Rect vis_rect = GetVisibleRect(camera);
    DrawRect(vis_rect);

    SDL_SetRenderTarget(renderer, NULL);
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
    SDL_Rect visible_rect = GetVisibleRect(world->camera);

    // Find the upper left visible tile.
    int corner_tile_x = visible_rect.x / TILE_SIZE;
    int corner_tile_y = visible_rect.y / TILE_SIZE;
    //printf("ul corner tile: %d, %d\n", corner_tile_x, corner_tile_y);

    SDL_Texture * grass_texture = GetTexture("grass.png");
    SDL_Texture * water_texture = GetTexture("shallow-water.png");

    SDL_Rect dst = {
        .w = TILE_SIZE,
        .h = TILE_SIZE,
    };

    SDL_Rect src = {
        .w = TILE_SIZE,
        .h = TILE_SIZE
    };

    for (int tile_y = corner_tile_y;
         tile_y <= corner_tile_y + VERTICAL_NUM_TILES + 1;
         tile_y++ ) {

        for (int tile_x = corner_tile_x;
             tile_x <= corner_tile_x + HORIZONTAL_NUM_TILES + 1;
             tile_x++ ) {

            tile_t * tile = GetTile(world->tiles, tile_x, tile_y);
            tile_t * adjacent_tiles[NUM_DIRECTIONS];
            GetAdjacentTiles(tile_x, tile_y, world->tiles, adjacent_tiles);

            src.x = TILE_SIZE * (tile->variety % 4); // TODO: #define 4
            dst.x = tile_x * TILE_SIZE - visible_rect.x;
            dst.y = tile_y * TILE_SIZE - visible_rect.y;

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

void RenderWorld(world_t * world, bool show_hitboxes)
{
    RenderVisibleTerrain(world);

    SDL_Rect visible_rect = GetVisibleRect(world->camera);

    // Filter the world actor array: only visible actors
    actor_t * visible_actors[MAX_ACTORS] = { 0 };
    int num_visible = 0;
    for ( int i = 0; i < world->num_actors; i++ ) {
        actor_t * actor = &world->actors[i];

        if ( GetActorSprite(actor)
            && RectsIntersect(visible_rect, GetActorVisibleRect(actor)) )
        {
            visible_actors[num_visible++] = actor;
        }
    }

    // Sort the visible list by y position.
    for ( int i = 0; i < num_visible; i++ ) {
        for ( int j = i + 1; j < num_visible; j++ ) {
            if ( visible_actors[i]->position.y > visible_actors[j]->position.y ) {
                SWAP(visible_actors[i], visible_actors[j]);
            }
        }
    }

    for ( int i = 0; i < num_visible; i++ ) {
        actor_t * actor = visible_actors[i];
        sprite_t * sprite = GetActorSprite(actor);

        if ( sprite ) {
            SDL_Rect r = GetActorVisibleRect(actor);
            r.x -= visible_rect.x;
            r.y -= visible_rect.y;
            DrawSprite(sprite, r.x, r.y, actor->current_frame);

            if ( show_hitboxes ) {
                SDL_FRect hitbox = ActorHitbox(actor);
                SetRGBA(90, 90, 255, 255);
                hitbox.x -= visible_rect.x;
                hitbox.y -= visible_rect.y;
                SDL_Rect hitbox_i = { hitbox.x, hitbox.y, hitbox.w, hitbox.h };
                DrawRect(hitbox_i);
            }
        }
    }
}
