//
//  inventory.c
//  Game
//
//  Created by Thomas Foster on 9/26/22.
//

#include "inventory.h"
#include "g_game.h"
#include "w_world.h"
#include "mylib/texture.h"
#include "mylib/video.h"

// TODO: convert over to using coord_t

inventory_t * INV_GetInventory(game_t * game)
{
    actor_t * player = GetActorType(game->world->actors, ACTOR_PLAYER);
    return player->info.player.inventory;
}

static bool INV_InsertItemInSlot
(  inventory_t * inventory,
    actor_t * item,
    int x,
    int y )
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
bool INV_InsertItem(actor_t * item, inventory_t * inventory)
{
    // for each grid slot
    for ( int y = 0; y < inventory->grid_height; y++ ) {
        for ( int x = 0; x < inventory->grid_width; x++ ) {

            // try to insert at this grid slot
            if ( INV_InsertItemInSlot(inventory, item, x, y) ) {
                return true;
            }

        }
    }

    // no space free for this item
    return false;
}

/// Given an item index, get the grid cell coordinate (upper left).
static bool INV_GetGridCell(inventory_t * inventory, int item_index, int * x, int * y)
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

actor_t INV_RemoveItemFromSlot(inventory_t * inventory, int x, int y)
{
    int remove_i = inventory->grid[y][x];

    // Clear slots in the grid that item occupies.
    for ( int y1 = 0; y1 < inventory->grid_height; y1++ ) {
        for ( int x1 = 0; x1 < inventory->grid_width; x1++ ) {
            if ( inventory->grid[y1][x1] == remove_i ) {
                inventory->grid[y1][x1] = EMPTY_SLOT;
            }
        }
    }

    int last_i = inventory->num_items - 1;
    actor_t removed_actor = inventory->items[remove_i];

    // Remove from array by putting the last item in it's place.
    inventory->items[remove_i] = inventory->items[last_i];
    --inventory->num_items;

    // The last item's index has changed, update the grid.
    for ( int y1 = 0; y1 < inventory->grid_height; y1++ ) {
        for ( int x1 = 0; x1 < inventory->grid_width; x1++ ) {
            if ( inventory->grid[y1][x1] == last_i ) {
                inventory->grid[y1][x1] = remove_i;
            }
        }
    }

    return removed_actor;
}

static void INV_ButtonIndexToXY(inventory_t * inventory, int index, int * x, int *y)
{
    *x = index % inventory->grid_width;
    *y = index / inventory->grid_height;
}

static int GridXYToButtonIndex(inventory_t * inventory, int x, int y)
{
    return y * inventory->grid_width + x;
}

bool INV_ClickCell
(   inventory_t * inventory,
    int mouse_x,
    int mouse_y,
    int cell_x,
    int cell_y,
    panel_t * panel )
{
    printf("clicked on cell %d, %d\n", cell_x, cell_y);

    // Cell has an item, pick it up.
    if ( inventory->selected.type == ACTOR_NONE ) {
        int clicked_index = inventory->grid[cell_y][cell_x];

        // Get the upper left cell of this item.
        int origin_cell_x;
        int origin_cell_y;
        INV_GetGridCell(inventory,
                             clicked_index,
                             &origin_cell_x,
                             &origin_cell_y);

        // From that, get the button index, which in turn has the window
        // coordinate used to calculate the click offset.
        int button_index = GridXYToButtonIndex
        (   inventory,
            origin_cell_x,
            origin_cell_y );

        // Calculate the click offset.
        SDL_Rect button_rect = panel->button_rect(button_index);
        inventory->held_item_offset.x = mouse_x - button_rect.x;
        inventory->held_item_offset.y = mouse_y - button_rect.y;

        // Pick up item, put in hand, remove from grid/items
        inventory->selected = INV_RemoveItemFromSlot
        (   inventory,
            cell_x,
            cell_y );

        return true;
    }

    return false;
}

bool INV_ProcessControls(game_t * game, screen_t * screen)
{
    if ( game->control_state.controls[CONTROL_INVENTORY_TOGGLE] ) {
        UI_PopScreen(game);
        return true;
    }

    if ( game->control_state.controls[CONTROL_INVENTORY_SELECT] ) {
        for ( int i = 0; i < screen->num_panels; i++ ) {
            panel_t * panel = &screen->panels[i];

            for ( int j = 0; j < panel->num_buttons; j++ ) {
                SDL_Rect button_rect = panel->button_rect(j);
                SDL_Point mouse_point = {
                    game->control_state.cursor_x,
                    game->control_state.cursor_y
                };

                // Click inside a button?
                if ( SDL_PointInRect(&mouse_point, &button_rect) ) {
                    int cell_x, cell_y;
                    inventory_t * inventory = INV_GetInventory(game);
                    INV_ButtonIndexToXY(inventory, j, &cell_x, &cell_y);
                    return INV_ClickCell
                    (   inventory,
                        mouse_point.x,
                        mouse_point.y,
                        cell_x,
                        cell_y,
                        panel );
                }
            }
        }
    }

    return false;
}

void INV_Render(game_t * game, screen_t * screen)
{
    SDL_Texture * panel2x2 = GetTexture(SPRITE_DIR"/inventory-panel-2x2.png");
    SDL_Texture * panel2x3 = GetTexture(SPRITE_DIR"/inventory-panel-2x3.png");

    // Draw grid texture.

    panel_t * grid_panel = &screen->panels[PANEL_GRID];
    SDL_Texture * grid_texture = GetTexture(SPRITE_DIR"/inventory-grid-3x3.png");
    SDL_Rect grid_dst = grid_panel->panel_rect();

    V_DrawTexture(grid_texture, NULL, &grid_dst);

    // Draw backpack items over grid texture.

    inventory_t * inv = INV_GetInventory(game);

    for ( int i = 0; i < inv->num_items; i++ ) {
        int x, y;
        if ( INV_GetGridCell(inv, i, &x, &y) ) {
            actor_t * item = &inv->items[i];
            SDL_Rect cell = grid_panel->button_rect(y * 3 + x);
            DrawSprite(item->info.item.sprite, 0, 0, cell.x, cell.y, DRAW_SCALE, 0);
        }
    }

    // Draw cursor or held item.

    if ( inv->selected.type == ACTOR_NONE ) {
        SDL_Texture * cursor = GetTexture(SPRITE_DIR"/cursor.png");
        SDL_Rect cursor_dst = G_TextureSize(cursor);
        cursor_dst.x = game->control_state.cursor_x - cursor_dst.w / 2;
        cursor_dst.y = game->control_state.cursor_y - cursor_dst.h / 2;

        V_DrawTexture(cursor, NULL, &cursor_dst);
    } else {
        item_info_t * info = &inv->selected.info.item;

        DrawSprite(info->sprite,
                   0,
                   0,
                   game->control_state.cursor_x - inv->held_item_offset.x,
                   game->control_state.cursor_y - inv->held_item_offset.y,
                   DRAW_SCALE,
                   0);
    }
}
