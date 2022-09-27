//
//  m_misc.c
//  Game
//
//  Created by Thomas Foster on 9/20/22.
//

#include "m_misc.h"

cardinal_t VelocityToDirection(vec2_t velocity)
{
    if ( velocity.y && !velocity.x ) {
        return velocity.y < 0 ? NORTH : SOUTH;
    } else {
        return velocity.x < 0 ? WEST : EAST;
    }
}
