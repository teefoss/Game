//
//  cardinal.h
//  Game
//
//  Created by Thomas Foster on 9/30/22.
//

#ifndef cardinal_h
#define cardinal_h

#include "mylib/vector.h"

#define NUM_DIRECTIONS 4

/// Directions are in this order because sprite sheets use only NESW,
/// and the rows therein are ordered thusly.
typedef enum {
    NO_DIRECTION = -1,
    NORTH,
    SOUTH,
    EAST,
    WEST,
    // WEAST
    NORTH_EAST,
    NORTH_WEST,
    SOUTH_EAST,
    SOUTH_WEST,
} cardinal_t;

extern const int y_dirs[];
extern const int x_dirs[];

/// For sprites, NE = E, SW = W, etc.
cardinal_t SpriteDirection(cardinal_t cardinal);

cardinal_t VectorToCardinal(vec2_t velocity);

#endif /* cardinal_h */
