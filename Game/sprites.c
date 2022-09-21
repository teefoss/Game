//
//  sprites.c
//  Game
//
//  Created by Thomas Foster on 9/12/22.
//

#include "sprites.h"

sprite_t sprites[NUM_SPRITES] = {
    [SPRITE_PLAYER_STAND] = {
        .texture_name = "player.png",
        .location = { 0, 0, 6, 24 },
        .num_frames = 1,
    },
    [SPRITE_GRASS] = {
        .texture_name = "grass.png",
        .location = { 0, 0, 16, 16 },
        .num_frames = 3,
        .flags = SPRITE_FLAG_VARIETY|SPRITE_FLAG_HORIZONTAL_FLIPPABLE,
    },
    [SPRITE_PLUS_FLOWER] = {
        .texture_name = "grass-decoration.png",
        .location = { 0, 0, 6, 7 },
        .num_frames = 2,
        .flags = SPRITE_FLAG_VARIETY,
    },
    [SPRITE_TINY_BLUE_FLOWER] = {
        .texture_name = "grass-decoration.png",
        .location = { 0, 11, 1, 2 },
        .num_frames = 1,
    },
    [SPRITE_TINY_YELLOW_FLOWER] = {
        .texture_name = "grass-decoration.png",
        .location = { 1, 11, 1, 2 },
        .num_frames = 1,
    },
    [SPRITE_GRASS_BLADES] = {
        .texture_name = "grass-decoration.png",
        .location = { 0, 7, 5, 4 },
        .num_frames = 3,
        .flags = SPRITE_FLAG_VARIETY|SPRITE_FLAG_HORIZONTAL_FLIPPABLE,
    },
    [SPRITE_TREE] = {
        .texture_name = "tree.png",
        .location = { 0, 0, 12, 28 },
        .num_frames = 1,
    },
    [SPRITE_BUTTERFLY] = {
        .texture_name = "wildlife.png",
        .location = { 0, 0, 3, 3 },
        .num_frames = 4,
        .fps = 12,
        .flags = SPRITE_FLAG_ANIMATED,
    },
};
