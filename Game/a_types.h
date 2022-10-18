//
//  a_types.h
//  Game
//
//  Created by Thomas Foster on 9/15/22.
//

#ifndef a_types_h
#define a_types_h

typedef enum {
    ACTOR_NONE,
    ACTOR_PLAYER,
    ACTOR_HAND_STRIKE,
    ACTOR_TREE,
    ACTOR_BUSH,
    ACTOR_BUTTERFLY,

    // items
    ACTOR_LOG,
    ACTOR_LEAVES,
    ACTOR_STICKS,

    NUM_ACTOR_TYPES,
} actor_type_t;

#endif /* a_types_h */
