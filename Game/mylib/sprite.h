#ifndef SPRITE_H
#define SPRITE_H

#include "genlib.h"
#include <stdbool.h>

// A Sprite / Sprite Sheet and relavent data
// In a sprite sheet, animations or tile varients should be
// laid out horizontally
typedef struct {
    const char * texture_name;
    u8 draw_order; // lower values are drawn first (in back)
    SDL_Rect location; // source rect in sprite sheet
    bool animated;
    u8 num_frames; // If not animated, refers to the number of varients.
    u8 current_frame;
    u8 fps;
    // For sprites with varients, will be randomly flipped.
    // Use SDL_RendererFlip values OR'd together
    u8 flip;
} sprite_t;

void DrawSprite(sprite_t * sprite, int x, int y, u8 variety);

#endif /* SPRITE_H */
