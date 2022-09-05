//
//  texture.c
//  Space Invaders
//
//  Created by Thomas Foster on 8/26/22.
//

#include "texture.h"
#include "genlib.h"
#include "video.h"
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
    puts("clean up textures");
    for ( int i = 0; i < numTextures; i++ ) {
        free(textures[i].key);
        SDL_DestroyTexture(textures[i].texture);
    }
}

void LoadTextures(const char * directoryName)
{
    DIR * dir = opendir(directoryName);
    if ( dir == NULL ) {
        Error("could not open directory '%s'", directoryName);
    }

    numTextures = 0;
    struct dirent * entry;
    while (( entry = readdir(dir) )) {
        const char * file = entry->d_name;

        if ( strcmp(".", file) == 0 || strcmp("..", file) == 0 ) {
            continue;
        }

        char * path = calloc
        (   strlen(directoryName) + strlen(file) + 2,
            sizeof(*path) );

        strcat(path, directoryName);
        if ( directoryName[strlen(directoryName) - 1] != '/' ) {
            strcat(path, "/");
        }
        strcat(path, file);

        SDL_Surface * surface = SDL_LoadBMP(path);
        if ( surface == NULL ) {
            CleanupTextures();
            Error("failed to create surface (%s)", file);
        }

        Uint32 key = SDL_MapRGB(surface->format, KEY_R, KEY_G, KEY_B);
        SDL_SetColorKey(surface, SDL_TRUE, key);

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
        directoryName );

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
