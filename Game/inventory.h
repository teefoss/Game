//
//  inventory.h
//  Game
//
//  Created by Thomas Foster on 9/26/22.
//

#ifndef inventory_h
#define inventory_h

#include "a_types.h"
#include <stdbool.h>

#define MAX_ITEMS 100
#define MAX_GRID_SIZE 10

typedef struct {
    actor_type_t type;

    // Size in inventory cells
    int width;
    int height;

    bool sideways; // If true, width <-> height
} item_t;

typedef struct {
    item_t selected; // Picked it up while in inventory
    item_t right_hand;
    item_t left_hand;
    item_t items[MAX_ITEMS];

    // The index of items[n] is put into slots
    int slots[MAX_GRID_SIZE][MAX_GRID_SIZE];
} inventory_t;

#endif /* inventory_h */
