//
//  w_render.c
//  Game
//
//  Created by Thomas Foster on 9/13/22.
//
//  World rendering functions.

#include "w_world.h"
#include "game.h"
#include "m_debug.h"
#include "sprites.h"
#include "w_tile.h"

#include "mylib/video.h"
#include "mylib/sprite.h"
#include "mylib/texture.h"

#define HORIZONTAL_NUM_TILES ((float)GAME_WIDTH / (float)TILE_SIZE)
#define VERTICAL_NUM_TILES ((float)GAME_HEIGHT / (float)TILE_SIZE)

void UpdateDebugMap(tile_t * tiles,  SDL_Texture ** debug_map, vec2_t camera)
{
    // TODO: add these as tile_t property, add tile defintions
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
    vis_rect.x /= TILE_SIZE;
    vis_rect.y /= TILE_SIZE;
    vis_rect.w /= TILE_SIZE;
    vis_rect.h /= TILE_SIZE;
    DrawRect(&vis_rect);

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
    DrawSprite(s, Random(1, max.x), Random(1, max.y), sprite_variety, 1);
}

void RenderGrassEffectTexture
(   tile_t * tile,
    tile_t ** adjacent_tiles,
    int tile_x,
    int tile_y )
{
    // < 0.1 ms
//    PROFILE_START(create_texture);
    tile->effect = CreateTexture(TILE_SIZE, TILE_SIZE);
//    PROFILE_END(create_texture);
    SDL_SetTextureBlendMode(tile->effect, SDL_BLENDMODE_BLEND);
    SDL_SetRenderTarget(renderer, tile->effect);

    SetRGBA(0, 0, 0, 0);
    Clear();

    // Render moss.

    float noise_map[TILE_SIZE][TILE_SIZE];
    GetTileNoise(tile_x, tile_y, noise_map);

    // Set a unique seed for this tile ?
    //SeedRandom(tile_y * WORLD_WIDTH + tile_x);

    struct {
        int x;
        int y;
        bool blue; // if false, yellow
    } moss_flowers[TILE_SIZE * TILE_SIZE];
    int num_moss_flowers = 0;

    // ~0.4 ms:
//    SetRGBA(93, 163, 42, 255); // darker shade of same grass color
//    SetRGBA(114, 201, 52, 255); // faintly lighter shade of same grass color
    for ( int py = 0; py < TILE_SIZE; py++ ) {
        for ( int px = 0; px < TILE_SIZE; px++ ) {
//            int wx = tile_x * 16 + px; // world pixel coord
//            int wy = tile_y * 16 + py;
//            noise_map[py][px] = Noise2(wx, wy, 1.0f, 0.01f, 8, 1.0f, 0.5f, 2.5f);
//            SeedRandom(wx * wy); // TODO: more than one prng

            if ( noise_map[py][px] > 0.2f ) { //}|| (noise > 0.05f && Random(0, 3) == 0) ) {
                SetRGBA(78, 138, 36, 255); // darker green
                DrawPoint(px, py);
            } else if ( noise_map[py][px] > 0.1 ) {
                SetRGBA(114, 201, 52, 255); // faintly lighter shade of same grass color
                DrawPoint(px, py);
            }

            if ( noise_map[py][px] > 0.7f ) {
                if ( Random(0, 15) == 15 ) {
                    moss_flowers[num_moss_flowers].x = px;
                    moss_flowers[num_moss_flowers].y = py;
                    moss_flowers[num_moss_flowers].blue = false;
                    num_moss_flowers++;
                }
            } else if ( noise_map[py][px] > 0.45f && Random(0, 20) == 20 ) {
                moss_flowers[num_moss_flowers].x = px;
                moss_flowers[num_moss_flowers].y = py;
                moss_flowers[num_moss_flowers].blue = true;
                num_moss_flowers++;
            }

        }
    }

    // Render tiny moss flowers.
#if 0
    for ( int py = 0; py < TILE_SIZE; py++ ) {
        for ( int px = 0; px < TILE_SIZE; px++ ) {
            if ( noise_map[py][px] > 0.7f ) {
                if ( Random(0, 15) == 15 ) {
                    DrawSprite(&sprites[SPRITE_TINY_YELLOW_FLOWER], px, py, 0, 1);
                }
            } else if ( noise_map[py][px] > 0.45f && Random(0, 20) == 20 ) {
                DrawSprite(&sprites[SPRITE_TINY_BLUE_FLOWER], px, py, 0, 1);
            }
        }
    }
#endif
    for ( int i = 0; i < num_moss_flowers; i++ ) {
        sprite_id_t id =
        moss_flowers[i].blue ? SPRITE_TINY_BLUE_FLOWER : SPRITE_TINY_YELLOW_FLOWER;
        DrawSprite(&sprites[id], moss_flowers[i].x, moss_flowers[i].y, 0, 1);
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

    // Draw highlights at water edges.
    SetRGBA(122, 214, 56, 255);
    if ( adjacent_tiles[NORTH]->terrain <= TERRAIN_SHALLOW_WATER ) {
        SDL_RenderDrawLine(renderer, 0, 0, TILE_SIZE + 1, 0);
    }

    if ( adjacent_tiles[WEST]->terrain <= TERRAIN_SHALLOW_WATER ) {
        SDL_RenderDrawLine(renderer, 0, 0, 0, TILE_SIZE);
    }

    if ( adjacent_tiles[EAST]->terrain <= TERRAIN_SHALLOW_WATER ) {
        SDL_RenderDrawLine(renderer, TILE_SIZE - 1, 0, TILE_SIZE - 1, TILE_SIZE - 1);
    }

    SDL_SetRenderTarget(renderer, NULL);
}

static void RenderGrass
(   tile_t * tile,
    tile_t ** adjacent_tiles,
    int tile_x,
    int tile_y,
    SDL_Rect * dst )
{
    SetSpriteColorMod(&sprites[SPRITE_GRASS], tile->lighting);
    DrawSprite(&sprites[SPRITE_GRASS], dst->x, dst->y, tile->variety, DRAW_SCALE);

    // Generate effect texture for this tile if needed.
    if ( tile->effect == NULL ) {
        RenderGrassEffectTexture(tile, adjacent_tiles, tile_x, tile_y);
    }

    // overlay the effect texture
    SDL_SetTextureColorMod
    (   tile->effect,
        tile->lighting.x,
        tile->lighting.y,
        tile->lighting.z );
    DrawTexture(tile->effect, NULL, dst);
}

static void RenderVisibleTerrain(world_t * world)
{
    SDL_Rect visible_rect = GetVisibleRect(world->camera);

    SDL_Rect dst = { .w = SCALED_TILE_SIZE, .h = SCALED_TILE_SIZE, };
    SDL_Rect src = { .w = TILE_SIZE, .h = TILE_SIZE };

    SDL_Point min, max;
    GetVisibleTileRange(world, &min, &max);

    for ( int tile_y = min.y; tile_y <= max.y; tile_y++ ) {
        for ( int tile_x = min.x; tile_x <= max.x; tile_x++ ) {
            tile_t * tile = GetTile(world->tiles, tile_x, tile_y);

            tile_t * adjacent_tiles[NUM_DIRECTIONS];
            GetAdjacentTiles(tile_x, tile_y, world->tiles, adjacent_tiles);

            src.x = TILE_SIZE * (tile->variety % 4); // TODO: #define 4
            dst.x = tile_x * SCALED_TILE_SIZE - visible_rect.x;
            dst.y = tile_y * SCALED_TILE_SIZE - visible_rect.y;
//            dst.x /= DRAW_SCALE; dst.x *= DRAW_SCALE;
//            dst.y /= DRAW_SCALE; dst.y *= DRAW_SCALE;

            switch ( tile->terrain ) {
                case TERRAIN_DEEP_WATER:
                    // TODO: render deep water
                case TERRAIN_SHALLOW_WATER: {
                    sprite_t * sprite;

                    if ( adjacent_tiles[NORTH]
                        && adjacent_tiles[NORTH]->terrain != TERRAIN_SHALLOW_WATER
                        && adjacent_tiles[NORTH]->terrain != TERRAIN_DEEP_WATER )
                    {
                        sprite = &sprites[SPRITE_SHALLOW_WATER_EDGE];
                    } else {
                        sprite = &sprites[SPRITE_SHALLOW_WATER];
                    }

                    SetSpriteColorMod(sprite, tile->lighting);
                    DrawSprite(sprite, dst.x, dst.y, tile->variety, DRAW_SCALE);
                    break;
                }
                case TERRAIN_GRASS:
                case TERRAIN_FOREST:
                case TERRAIN_DARK_FOREST:
                    RenderGrass
                    (   tile,
                        adjacent_tiles,
                        tile_x,
                        tile_y,
                        &dst );
                    break;
                default:
                    break;
            }

            // debug: highlight tile under mouse
            if (show_debug_info
                && ((int)mouse_tile.x == tile_x && (int)mouse_tile.y == tile_y) )
            {
                SetRGBA(255, 90, 90, 255);
                DrawRect(&dst);
            }

            if ( show_geometry ) {
                SetRGBA(255, 255, 90, 255);
                SDL_Rect corner_dot = dst;
                corner_dot.w = corner_dot.h = DRAW_SCALE;
                FillRect(&corner_dot);
            }

            dst.x += TILE_SIZE;
        }

        dst.y += TILE_SIZE;
    }
}

void RenderVisibleActors(world_t * world, bool show_hitboxes)
{
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
            if ( visible_actors[i]->pos.y > visible_actors[j]->pos.y ) {
                SWAP(visible_actors[i], visible_actors[j]);
            }
        }
    }

    // draw actor shadow
    // TODO: make a actor shadow flag
    for ( int i = 0; i < num_visible; i++ ) {
        actor_t * actor = visible_actors[i];

        if ( GetActorSprite(actor) ) {
            SDL_Rect shadow = {
                .w = (actor->hitbox_width + 4) * DRAW_SCALE,
                .h = (actor->hitbox_height + 2) * DRAW_SCALE
            };
            shadow.x = actor->pos.x - shadow.w / 2 - visible_rect.x;
            shadow.y = actor->pos.y - shadow.h / 2 - visible_rect.y;

            SetRGBA(0, 0, 0, 64);
            FillRect(&shadow);
        }
    }

    // draw actors
    for ( int i = 0; i < num_visible; i++ ) {
        actor_t * actor = visible_actors[i];
        sprite_t * sprite = GetActorSprite(actor);

        if ( sprite ) {
            SDL_Rect r = GetActorVisibleRect(actor);
            r.x -= visible_rect.x; // convert to window space
            r.y -= visible_rect.y;

            SetSpriteColorMod(sprite, actor->lighting);
            DrawSprite(sprite, r.x, r.y, actor->current_frame, DRAW_SCALE);

            if ( show_hitboxes ) {
                SDL_FRect hitbox = ActorHitbox(actor);
                SetRGBA(90, 90, 255, 255);
                hitbox.x -= visible_rect.x;
                hitbox.y -= visible_rect.y;
                SDL_Rect hitbox_i = { hitbox.x, hitbox.y, hitbox.w, hitbox.h };
                DrawRect(&hitbox_i);
            }
        }
    }

}

void RenderWorld(world_t * world, bool show_hitboxes)
{
    int render_start = SDL_GetTicks(); // debug

    RenderVisibleTerrain(world);
    RenderVisibleActors(world, show_hitboxes);

    render_ms = SDL_GetTicks() - render_start; // debug
}
