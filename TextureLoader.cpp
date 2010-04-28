/*
 *  TextureLoader.cpp
 *  Nebula Horizon
 *
 *  Created by Zachary Murray on 11/25/09.
 *
 */

#include "TextureLoader.h"

GLuint TextureLoader::loadTexture(std::istream& input) {
  return loadTexture(input, 0, 0);
}
