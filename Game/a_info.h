//
//  a_info.h
//  Game
//
//  Created by Thomas Foster on 10/22/22.
//

#ifndef a_info_h
#define a_info_h

#include "mylib/genlib.h"
#include "mylib/sprite.h"

typedef struct inventory inventory_t;

typedef struct {
    inventory_t * inventory;

    /// No movement buttons are pressed.
    bool stopping_x;
    bool stopping_y;
    bool strike_button_down;
} player_info_t;

typedef struct {
    // Size in inventory cells
    int width;
    int height;

    bool sideways; // If true, width <-> height
    sprite_t * sprite; // inventory sprite
} item_info_t;

typedef struct {
    u8 level; // damage "rating", e.g. hand = 0 (weak)
    u8 amount;
} damage_t;

typedef struct {
    u8 amount;

    // what damage level or greater is required to affect health
    u8 minimum_damage_level;
} health_t;

typedef struct {
    u8 quantity;
    actor_type_t actor_type;
} drop_t;

#endif /* a_info_h */
