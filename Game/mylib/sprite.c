#include "mathlib.h"
#include "sprite.h"
#include "texture.h"
#include "video.h"

// NB: this does not handle sprite scaling, maybe it should
void DrawSprite(sprite_t * sprite, int x, int y, u8 variety)
{
    SDL_Rect src = sprite->location;
    SDL_Rect dst = { x, y, sprite->location.w, sprite->location.h };

    if ( sprite->animated ) {
        // TODO: handle fps
        // src.x += sprite->location.w * sprite->current_frame;
    } else if ( sprite->num_frames > 0 ) {
        src.x += sprite->location.w * (variety % sprite->num_frames);
    }

    if ( sprite->flip && Random(0, 1) ) {
        DrawTextureFlip(GetTexture(sprite->texture_name), &src, &dst, sprite->flip);
    } else {
        DrawTexture(GetTexture(sprite->texture_name), &src, &dst);
    }
}
