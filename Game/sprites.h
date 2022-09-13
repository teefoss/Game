//
//  sprites.h
//  Game
//
//  Created by Thomas Foster on 9/12/22.
//

#ifndef sprites_h
#define sprites_h

#include "mylib/sprite.h"

typedef enum {
    DRAW_ORDER_BACKGROUND,
    DRAW_ORDER_FOREGROUND,
} draw_order_t;

typedef enum {
    SPRITE_GRASS,
    SPRITE_PLUS_FLOWER,
    SPRITE_GRASS_BLADES,
    NUM_SPRITES,
} sprite_id_t;

extern sprite_t sprites[NUM_SPRITES];

#endif /* sprites_h */
