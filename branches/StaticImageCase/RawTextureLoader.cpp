/*
 *  RawTextureLoader.cpp
 *  Nebula Horizon
 *
 *  Created by Zachary Murray on 11/25/09.
 *
 */

#include <stdexcept>
#include <iostream>
#include <cstring>

#ifdef WIN32
#include <windows.h>
#endif

#include "RawTextureLoader.h"
#include "nhz_common.h"

#ifdef __APPLE__
#include <OpenGL/glext.h>
#else
#include <GL/glext.h>
#endif

GLuint RawTextureLoader::loadTexture(std::istream& input) {
  throw std::logic_error("must specify width and height for RAW format");
  return 0;
}

GLuint RawTextureLoader::loadTexture(std::istream& input,
                                     unsigned int w, unsigned int h) {
  static int x = 0;
  GLuint texture;
  char databuf[1024];
  char *data = (char *)malloc(sizeof(char)*w*h*3);
  size_t bytes_read = 0;
  ++x;
  if(input.bad()) {
    NHZ_ERR("bad input\n");
  };
  if(input.fail()) {
    NHZ_ERR("fail input: %d\n", x);
  };
  if(input.eof()) {
    NHZ_ERR("eof input\n");
  };
  while(bytes_read < w*h*3 && !input.eof() && !input.fail() && !input.bad()) {
    input.read(databuf, 1024);
    memcpy(data + bytes_read, databuf, 1024);
    bytes_read += 1024;
  }
  if(input.bad()) {
    NHZ_ERR("stream went bad while reading file (after %d bytes)\n",bytes_read);
  }
  if(input.fail()) {
    NHZ_ERR("failed to read entire file (after reading %d bytes)\n",bytes_read);
  }
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0,
               GL_RGB, GL_UNSIGNED_BYTE, data);
  free(data);
  return texture;
}
