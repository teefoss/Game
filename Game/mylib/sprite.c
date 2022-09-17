#include "mathlib.h"
#include "sprite.h"
#include "texture.h"
#include "video.h"

// NB: this does not handle sprite scaling, maybe it should
void DrawSprite(sprite_t * sprite, int x, int y, u8 frame)
{
    SDL_Rect src = sprite->location;
    SDL_Rect dst = { x, y, sprite->location.w, sprite->location.h };
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
        if ( sprite->flags & SPRITE_FLAG_HORIZONTAL_FLIPPABLE && Random(0, 1) ) {
            flip |= SDL_FLIP_HORIZONTAL;
        }
        if ( sprite->flags & SPRITE_FLAG_VERTICAL_FLIPPABLE && Random(0, 1) ) {
            flip |= SDL_FLIP_VERTICAL;
        }
        DrawTextureFlip(texture, &src, &dst, flip);
    }
}
