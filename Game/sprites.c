//
//  sprites.c
//  Game
//
//  Created by Thomas Foster on 9/12/22.
//

#include "sprites.h"
#include "world.h"

sprite_t sprites[NUM_SPRITES] = {
    [SPRITE_GRASS] = {
        .texture_name = "grass.png",
        .location = { 0, 0, 16, 16 },
        .num_frames = 3,
    },
    [SPRITE_PLUS_FLOWER] = {
        .texture_name = "grass-decoration.png",
        .location = { 0, 0, 6, 7 },
        .num_frames = 4,
    },
    [SPRITE_GRASS_BLADES] = {
        .texture_name = "grass-decoration.png",
        .location = { 0, 7, 5, 4 },
        .num_frames = 3,
        .flip = SDL_FLIP_HORIZONTAL,
    }
};
