#include "mathlib.h"
#include "sprite.h"
#include "texture.h"
#include "video.h"

void DrawSprite(sprite_t * sprite, int x, int y, u8 frame, int scale)
{
    SDL_Rect src = sprite->location;
    SDL_Rect dst = { x, y, sprite->location.w * scale, sprite->location.h * scale };
    SDL_Texture * texture = GetTexture(sprite->texture_name);

    if ( sprite->flags & SPRITE_FLAG_ANIMATED ) {
        src.x += sprite->location.w * frame;
        DrawTexture(texture, &src, &dst);
    } else {
        if ( sprite->flags & SPRITE_FLAG_VARIETY ) {
            // select a random varient
            src.x += sprite->location.w * frame % sprite->num_frames;
        }

        SDL_RendererFlip flip = 0;
        if ( sprite->flags & SPRITE_FLAG_HORIZONTAL_FLIPPABLE && frame < 128 ) {
            flip |= SDL_FLIP_HORIZONTAL;
        }
        if ( sprite->flags & SPRITE_FLAG_VERTICAL_FLIPPABLE && frame < 128 ) {
            flip |= SDL_FLIP_VERTICAL;
        }
        DrawTextureFlip(texture, &src, &dst, flip);
    }
}

void SetSpriteColorMod(sprite_t * sprite, vec3_t color_mod)
{
    SDL_Texture * texture = GetTexture(sprite->texture_name);
    SDL_SetTextureColorMod(texture, color_mod.x, color_mod.y, color_mod.z);
}
