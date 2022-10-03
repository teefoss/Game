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

    // save all available grid slots
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
            if ( inventory->grid[check_y][check_x]) {
                return false;
            }

            slots[num_coords].x = check_x;
            slots[num_coords].y = check_y;
            num_coords++;
        }
    }

    // all required slots at x, y are available, put item in items[]
    // and the grid:

    inventory->items[inventory->num_items++] = *item;
    for ( int i = 0; i < num_coords; i++ ) {
        // put the current item's index in the grid at each
        // coordinate that was previously recorded
        inventory->grid[slots[i].y][slots[i].x] = inventory->num_items;
    }

    return true;
}

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
