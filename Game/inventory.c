//
//  inventory.c
//  Game
//
//  Created by Thomas Foster on 9/26/22.
//

#include "inventory.h"

bool InventoryInsertItemInSlot(inventory_t * inventory, actor_t * item, int x, int y)
{
    item_info_t * info = &item->info.item;
    int w = info->width;
    int h = info->height;

    if ( info->sideways ) {
        SWAP(w, h);
    }

    // Make a list of the coordinates of all available grid slots.
    SDL_Point slots[MAX_GRID_SIZE * MAX_GRID_SIZE];
    int num_coords = 0;

    // for each cell in item (w * h)
    for ( int y1 = 0; y1 < h; y1++ ) {
        for ( int x1 = 0; x1 < w; x1++ ) {

            int check_x = x + x1;
            int check_y = y + y1;

            // part of the item goes off the side of the grid
            if ( check_x >= inventory->grid_width
                || check_y >= inventory->grid_height )
            {
                return false;
            }

            // something's already in that slot
            if ( inventory->grid[check_y][check_x] != EMPTY_SLOT ) {
                return false;
            }

            slots[num_coords].x = check_x;
            slots[num_coords].y = check_y;
            num_coords++;
        }
    }

    // All required slots at x, y are available. Put the item in items[]
    // and the grid:
    inventory->items[inventory->num_items] = *item;
    for ( int i = 0; i < num_coords; i++ ) {
        // put the current item's index in the grid at each
        // coordinate that was previously recorded
        inventory->grid[slots[i].y][slots[i].x] = inventory->num_items;
    }

    ++inventory->num_items;

    return true;
}

/// Insert item into first free spot in inventory.
///
/// - Return: true is the item was able to be inserted, false otherwise.
bool InventoryInsertItem(actor_t * item, inventory_t * inventory)
{
    // for each grid slot
    for ( int y = 0; y < inventory->grid_height; y++ ) {
        for ( int x = 0; x < inventory->grid_width; x++ ) {

            // try to insert at this grid slot
            if ( InventoryInsertItemInSlot(inventory, item, x, y) ) {
                return true;
            }

        }
    }

    // no space free for this item
    return false;
}

bool InventoryGetGridCell(inventory_t * inventory, int item_index, int * x, int * y)
{
    for ( int cell_y = 0; cell_y < inventory->grid_height; cell_y++ ) {
        for ( int cell_x = 0; cell_x < inventory->grid_width; cell_x++ ) {
            if ( inventory->grid[cell_y][cell_x] == item_index ) {
                *x = cell_x;
                *y = cell_y;
                return true;
            }
        }
    }

    return false;
}
