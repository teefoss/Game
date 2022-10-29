//
//  sprites.c
//  Game
//
//  Created by Thomas Foster on 9/12/22.
//

#include "sprites.h"
#include "g_game.h"
#include "inventory.h"

#define HELD_ITEM_ICON_ALPHA 128

#define PLAYER_SPRITE_WIDTH 8
#define PLAYER_SPRITE_HEIGHT 24

sprite_t sprites[NUM_SPRITES] = {
    [SPRITE_PLAYER_STAND] = {
        .texture_name = ""SPRITE_DIR"player.png",
        .location = { 0, 0, PLAYER_SPRITE_WIDTH, PLAYER_SPRITE_HEIGHT },
        .num_frames = 1,
    },
    [SPRITE_PLAYER_WALK] = {
        .texture_name = ""SPRITE_DIR"player.png",
        .location = { 8, 0, PLAYER_SPRITE_WIDTH, PLAYER_SPRITE_HEIGHT },
        .num_frames = 2,
        .fps = 6,
    },
    [SPRITE_PLAYER_SWING] = {
        .texture_name = SPRITE_DIR"player.png",
        .location = { 24, 0, PLAYER_SPRITE_WIDTH, PLAYER_SPRITE_HEIGHT },
        .num_frames = 1,
    },
    [SPRITE_PLAYER_STRIKE] = {
        .texture_name = SPRITE_DIR"player.png",
        .location = { 32, 0, PLAYER_SPRITE_WIDTH, PLAYER_SPRITE_HEIGHT },
        .num_frames = 1,
    },
    [SPRITE_SHALLOW_WATER] = {
        .texture_name = ""SPRITE_DIR"shallow-water.png",
        .location = { 0, 0, 16, 16 },
        .num_frames = 1,
    },
    [SPRITE_SHALLOW_WATER_EDGE] = {
        .texture_name = ""SPRITE_DIR"shallow-water.png",
        .location = { 0, 16, 16, 16 },
        .num_frames = 4,
        .flip = SDL_FLIP_HORIZONTAL,
    },
    [SPRITE_GRASS] = {
        .texture_name = ""SPRITE_DIR"grass.png",
        .location = { 0, 0, 16, 16 },
        .num_frames = 1,
    },
    [SPRITE_PLUS_FLOWER] = {
        .texture_name = ""SPRITE_DIR"grass-decoration.png",
        .location = { 0, 0, 6, 7 },
        .num_frames = 2,
    },
    [SPRITE_TINY_BLUE_FLOWER] = {
        .texture_name = ""SPRITE_DIR"grass-decoration.png",
        .location = { 0, 11, 1, 2 },
        .num_frames = 1,
    },
    [SPRITE_TINY_YELLOW_FLOWER] = {
        .texture_name = ""SPRITE_DIR"grass-decoration.png",
        .location = { 1, 11, 1, 2 },
        .num_frames = 1,
    },
    [SPRITE_GRASS_BLADES] = {
        .texture_name = ""SPRITE_DIR"grass-decoration.png",
        .location = { 0, 7, 5, 4 },
        .num_frames = 3,
        .flip = SDL_FLIP_HORIZONTAL,
    },
    [SPRITE_GRASS_BUMPS] = {
        .texture_name = ""SPRITE_DIR"grass-decoration.png",
        .location = { 0, 16, 2, 6 },
        .num_frames = 5,
        .flip = SDL_FLIP_HORIZONTAL,
    },
    [SPRITE_WHITE_FLOWERS] = {
        .texture_name = ""SPRITE_DIR"grass-decoration.png",
        .location = { 0, 13, 3, 3 },
        .num_frames = 3,
    },
    [SPRITE_TREE] = {
        .texture_name = ""SPRITE_DIR"tree.png",
        .location = { 0, 0, 16, 43 },
        .num_frames = 1,
    },
    [SPRITE_BUSH] = {
        .texture_name = ""SPRITE_DIR"tree.png",
        .location = { 16, 0, 12, 16 },
        .num_frames = 1,
    },
    [SPRITE_BUTTERFLY] = {
        .texture_name = ""SPRITE_DIR"wildlife.png",
        .location = { 0, 0, 3, 3 },
        .num_frames = 4,
        .fps = 12,
        //.flags = SPRITE_FLAG_ANIMATED,
    },
    [SPRITE_LOG_WORLD] = {
        .texture_name = ""SPRITE_DIR"items.png",
        .location = { 0, 0, SLOT_SIZE, SLOT_SIZE }
    },
    [SPRITE_LOG_INVENTORY] = {
        .texture_name = ""SPRITE_DIR"items.png",
        .location = { 0, 16, SLOT_SIZE * 2, SLOT_SIZE * 2 }
    },
    [SPRITE_LEAVES] = {
        .texture_name = ""SPRITE_DIR"items.png",
        .location = { 16, 0, SLOT_SIZE, SLOT_SIZE }
    },
    [SPRITE_STICKS_WORLD] = {
        .texture_name = ""SPRITE_DIR"items.png",
        .location = { 32, 0, SLOT_SIZE, SLOT_SIZE }
    },
    [SPRITE_STICKS_INVENTORY] = {
        .texture_name = ""SPRITE_DIR"items.png",
        .location = { 32, 16, SLOT_SIZE, SLOT_SIZE * 2 }
    },
    [SPRITE_ICON_NO_ITEM] = {
        .texture_name = ""SPRITE_DIR"icons.png",
        .location = { 0, 0, 16, 16 },
        .transparent = true,
        .alpha = HELD_ITEM_ICON_ALPHA,
    },
    [SPRITE_ICON_SWORD] = {
        .texture_name = ""SPRITE_DIR"icons.png",
        .location = { 12, 0, 12, 12 },
        .transparent = true,
        .alpha = HELD_ITEM_ICON_ALPHA,
    },
    [SPRITE_ICON_AXE] = {
        .texture_name = ""SPRITE_DIR"icons.png",
        .location = { 0, 24, 12, 12 },
        .transparent = true,
        .alpha = HELD_ITEM_ICON_ALPHA,
    },
};
