//
//  gobject.h
//  Game
//
//  Created by Thomas Foster on 9/12/22.
//

#ifndef gobject_h
#define gobject_h

#include "mylib/mathlib.h"
#include "mylib/sprite.h"

typedef enum {
    GOBJECT_BLUE_FLOWER,
} gobject_type_t;

typedef struct {
    gobject_type_t type;
    vec2_t position;
    sprite_t * sprite;
} gobject_t;

#endif /* gobject_h */
