//
//  texture.c
//  Space Invaders
//
//  Created by Thomas Foster on 8/26/22.
//

#include "texture.h"
#include "genlib.h"
#include "video.h"

#include <SDL_image.h>
#include <dirent.h> // TODO: check if this is portable

#define MAX_TEXTURES    1024
#define KEY_R           0xFF
#define KEY_G           0x00
#define KEY_B           0xFF

typedef struct {
    char * key;
    SDL_Texture * texture;
} texture_t;

int numTextures;
texture_t textures[MAX_TEXTURES];

static void CleanupTextures(void)
{
    for ( int i = 0; i < numTextures; i++ ) {
        free(textures[i].key);
        SDL_DestroyTexture(textures[i].texture);
    }
}

void LoadTextures(const char * directory_name, const char * file_extension)
{
    if ( strcmp(file_extension, "png") == 0 ) {
        if ( IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG ) {
            Error("failed to init SDL_Image");
        }
    }

    DIR * dir = opendir(directory_name);
    if ( dir == NULL ) {
        Error("could not open directory '%s'", directory_name);
    }

    numTextures = 0;
    struct dirent * entry;
    while (( entry = readdir(dir) )) {
        const char * file = entry->d_name;

        // skip the '.' and '..'
        if ( strcmp(".", file) == 0 || strcmp("..", file) == 0 ) {
            continue;
        }

        // skip files with non-matching extension
        const char * this_extension = GetExtension(file);
        if ( strcmp(this_extension, file_extension) != 0 ) {
            continue;
        }

        // make storage for the path of the image file to be
        // opened (directory + file name)
        char * path = calloc
        (   strlen(directory_name) + strlen(file) + 2,
            sizeof(*path) );

        // create the path
        strcat(path, directory_name);
        if ( directory_name[strlen(directory_name) - 1] != '/' ) {
            strcat(path, "/");
        }
        strcat(path, file);

        // load the image as a surface
        SDL_Surface * surface;
        if ( strcmp(file_extension, "bmp") == 0 ) {
            surface = SDL_LoadBMP(path);
            Uint32 key = SDL_MapRGB(surface->format, KEY_R, KEY_G, KEY_B);
            SDL_SetColorKey(surface, SDL_TRUE, key);
        } else if ( strcmp(file_extension, "png") == 0 ) {
            surface = IMG_Load(path);
        } else {
            Error("unsupported image format: %s", file_extension);
        }

        if ( surface == NULL ) {
            CleanupTextures();
            Error("failed to create surface (%s)", file);
        }

        // create a texture from the suface, its file name is the key.
        textures[numTextures].key = SDL_strdup(file);
        textures[numTextures].texture
            = SDL_CreateTextureFromSurface(renderer, surface);

        if (textures[numTextures].texture == NULL ) {
            CleanupTextures();
            Error("failed to create texture (%s)\n", file);
        }

        ++numTextures;
        free(path);
    }

    printf
    (   "LoadTextures: loaded %d textures from '%s'\n",
        numTextures,
        directory_name );

    atexit(CleanupTextures);
    closedir(dir);
}

SDL_Texture * GetTexture(const char * name)
{
    for ( int i = 0; i < numTextures; i++ ) {
        if ( strcmp(name, textures[i].key) == 0 ) {
            return textures[i].texture;
        }
    }

    Error("texture '%s' not found", name);
    return NULL;
}
