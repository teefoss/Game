//
//  inventory.h
//  Game
//
//  Created by Thomas Foster on 9/26/22.
//

#ifndef inventory_h
#define inventory_h

#include "a_actor.h"

#define MAX_ITEMS 100
#define MAX_GRID_SIZE 10

#define INITIAL_GRID_WIDTH 3
#define INITIAL_GRID_HEIGHT 3

typedef struct inventory {
    actor_t selected; // Picked it up while in inventory
    actor_t right_hand;
    actor_t left_hand;

    int num_items;
    actor_t items[MAX_ITEMS];

    // The index of items[n] is put into slots
    int grid_width;
    int grid_height;
    int grid[MAX_GRID_SIZE][MAX_GRID_SIZE];
} inventory_t;

bool InventoryInsertItemInSlot(inventory_t * inventory, actor_t * item, int x, int y);
bool InventoryInsertItem(actor_t * item, inventory_t * inventory); // TODO: reorder params

#endif /* inventory_h */
