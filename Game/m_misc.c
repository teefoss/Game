//
//  m_misc.c
//  Game
//
//  Created by Thomas Foster on 9/20/22.
//

#include "m_misc.h"

cardinal_t VelocityToDirection(vec2_t velocity)
{
    if ( fabsf(velocity.x) > fabsf(velocity.y) ) {
        return velocity.x < 0 ? WEST : EAST;
    } else {
        return velocity.y < 0 ? NORTH : SOUTH;
    }
}
