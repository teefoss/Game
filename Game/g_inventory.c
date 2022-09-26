//
//  g_inventory.c
//  Game
//
//  Created by Thomas Foster on 9/24/22.
//

#include <stdio.h>

#include "a_actor.h"

#define MAX_ITEMS 100
#define MAX_GRID_SIZE 10

typedef struct {
    // Size in inventory cells
    int width;
    int height;

    bool sideways; // If true, width <-> height
} item_t;

typedef struct {
    actor_t selected; // Picked it up while in inventory
    actor_t right_hand;
    actor_t left_hand;
    actor_t items[MAX_ITEMS];

    // The index of items[n] is put into slots
    int slots[MAX_GRID_SIZE][MAX_GRID_SIZE];
} inventory_t;
