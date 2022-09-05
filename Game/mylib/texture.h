// -----------------------------------------------------------------------------
// Texture Loading, Storage and Retrieval
// -----------------------------------------------------------------------------
#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include <SDL.h>

/// Load all .bmp files in directory
///
/// The BPM transparency color is assumed to be #FF00FF.
/// - Parameter directoryName: directory (relative to cwd) containing
///   .bmp files to be loaded
void LoadTextures(const char * directoryName);

/// Get texture for given key.
///
/// - Parameter key: The file name of the texture.
/// - Returns: The requested texture. If no texture is found for given key,
///   the program is terminated via a cell of `Error()`.
SDL_Texture * GetTexture(const char * key);

#endif /* __TEXTURE_H__ */
