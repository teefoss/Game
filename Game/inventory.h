//
//  inventory.h
//  Game
//
//  Created by Thomas Foster on 9/26/22.
//

#ifndef inventory_h
#define inventory_h

#include "a_actor.h"
#include "g_game.h"
#include "ui_screen.h"

#define MAX_ITEMS 100
#define MAX_GRID_SIZE 10
#define EMPTY_SLOT 0xFF

#define INITIAL_GRID_WIDTH 3
#define INITIAL_GRID_HEIGHT 3

#define SLOT_SIZE 16

typedef enum {
    PANEL_GRID,
    PANEL_RIGHT_HAND,
    PANEL_LEFT_HAND,
    PANEL_HEAD,
    PANEL_LEGS,
    PANEL_FEET,
    NUM_INVENTORY_PANELS,
} inventory_panel_t;

typedef struct game game_t;

typedef struct inventory {
    actor_t selected; // Picked it up while in inventory.
    window_coord_t held_item_offset;

    actor_t right_hand;
    actor_t left_hand;

    u8 num_items;
    actor_t items[MAX_ITEMS];

    // The index of items[n] is put into slots
    // If a grid slot is empty its value is EMPTY_SLOT
    u8 grid_width;
    u8 grid_height;
    u8 grid[MAX_GRID_SIZE][MAX_GRID_SIZE];
} inventory_t;

bool INV_InsertItem(actor_t * item, inventory_t * inventory); // TODO: reorder params

bool INV_ProcessControls(game_t * game, screen_t * screen);
void INV_Render(game_t * game, screen_t * screen);

#endif /* inventory_h */
