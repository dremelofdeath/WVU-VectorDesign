/*
 *  TextureLoader.h
 *  Nebula Horizon
 *
 *  Created by Zachary Murray on 11/25/09.
 *
 */

#ifndef NHZ_TEXTURELOADER_H
#define NHZ_TEXTURELOADER_H

#include <istream>

#ifdef WIN32
#include <windows.h>
#endif

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

class TextureLoader {
  public:
    virtual GLuint loadTexture(std::istream& input) = 0;
    virtual GLuint loadTexture(std::istream& input,
                               unsigned int w, unsigned int h) = 0;
};

#endif
