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
        .location = { 0, 0, 8, 24 },
        .num_frames = 1,
    },
    [SPRITE_PLAYER_WALK] = {
        .texture_name = "player.png",
        .location = { 8, 0, 8, 24 },
        .num_frames = 2,
        .fps = 10,
    },
    [SPRITE_SHALLOW_WATER] = {
        .texture_name = "shallow-water.png",
        .location = { 0, 0, 16, 16 },
        .num_frames = 1,
    },
    [SPRITE_SHALLOW_WATER_EDGE] = {
        .texture_name = "shallow-water.png",
        .location = { 0, 16, 16, 16 },
        .num_frames = 4,
        .flip = SDL_FLIP_HORIZONTAL,
    },
    [SPRITE_GRASS] = {
        .texture_name = "grass.png",
        .location = { 0, 0, 16, 16 },
        .num_frames = 1,
    },
    [SPRITE_PLUS_FLOWER] = {
        .texture_name = "grass-decoration.png",
        .location = { 0, 0, 6, 7 },
        .num_frames = 2,
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
        .flip = SDL_FLIP_HORIZONTAL,
    },
    [SPRITE_GRASS_BUMPS] = {
        .texture_name = "grass-decoration.png",
        .location = { 0, 16, 2, 6 },
        .num_frames = 5,
        .flip = SDL_FLIP_HORIZONTAL,
    },
    [SPRITE_WHITE_FLOWERS] = {
        .texture_name = "grass-decoration.png",
        .location = { 0, 13, 3, 3 },
        .num_frames = 3,
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
        //.flags = SPRITE_FLAG_ANIMATED,
    },
    [SPRITE_LOG] = {
        .texture_name = "items.png",
        .location = { 0, 0, 12, 12 }
    }
};
