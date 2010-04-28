/*
 *  RawTextureLoader.h
 *  Nebula Horizon
 *
 *  Created by Zachary Murray on 11/25/09.
 *
 */

#ifndef NHZ_RAWTEXTURELOADER_H
#define NHZ_RAWTEXTURELOADER_H

#include "TextureLoader.h"

class RawTextureLoader : public TextureLoader {
  public:
    // invalid operation: must specify width and height
    GLuint loadTexture(std::istream& input);
    GLuint loadTexture(std::istream& input, unsigned int w, unsigned int h);
};

#endif
