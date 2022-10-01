//
//  cardinal.c
//  Game
//
//  Created by Thomas Foster on 9/30/22.
//

#include "cardinal.h"
#include "mylib/mathlib.h"

const int y_dirs[] = {
    [NORTH] = -1,
    [SOUTH] = 1,
    [EAST] = 0,
    [WEST] = 0,
    [NORTH_EAST] = -1,
    [NORTH_WEST] = -1,
    [SOUTH_EAST] = 1,
    [SOUTH_WEST] = 1
};

const int x_dirs[] = {
    [NORTH] = 0,
    [SOUTH] = 0,
    [EAST] = 1,
    [WEST] = -1,
    [NORTH_EAST] = 1,
    [NORTH_WEST] = -1,
    [SOUTH_EAST] = 1,
    [SOUTH_WEST] = -1
};

cardinal_t SpriteDirection(cardinal_t cardinal)
{
    switch ( cardinal ) {
        case NORTH_EAST: case SOUTH_EAST:
            return EAST;
        case NORTH_WEST: case SOUTH_WEST:
            return WEST;
        default:
            return cardinal;
    }
}

cardinal_t VectorToCardinal(vec2_t vec)
{
    if ( vec.x == 0.0f && vec.y == 0.0f ) {
        return NO_DIRECTION;
    }

    float degrees = RAD2DEG(Vec2Angle(vec)) + 180.0f;
    float octant = roundf(degrees / 45.0f);
//    printf("%f degrees, octant: %f\n", degrees, octant);

    switch ( (int)octant ) {
        case 0:
        case 8: return WEST;
        case 1: return SOUTH_WEST;
        case 2: return SOUTH;
        case 3: return SOUTH_EAST;
        case 4: return EAST;
        case 5: return NORTH_EAST;
        case 6: return NORTH;
        case 7: return NORTH_WEST;
        default:
            printf("weird octant! (%d)\n", (int)octant);
            return NO_DIRECTION;
    }
}
