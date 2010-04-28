/*
 *  KeyboardManager.cpp
 *  Nebula Horizon
 *
 *  Created by Zachary Murray on 11/24/09.
 *
 */

#include <cctype>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "KeyboardManager.h"

KeyboardManager* KeyboardManager::_singleton = 0;

KeyboardManager::KeyboardManager() {
  initialize();
}

KeyboardManager& KeyboardManager::getInstance() {
  if(_singleton == 0) {
    _singleton = new KeyboardManager;
  }
  return *_singleton;
}

bool KeyboardManager::isKeyDown(unsigned char key) {
  return _keys[tolower(key)];
}

bool KeyboardManager::isSpecialKeyDown(int key) {
  bool ret = false;
  if(key >= GLUT_KEY_F1 && key <= GLUT_KEY_F12) {
    ret = _functionKeys[key - GLUT_KEY_F1];
  } else if(key >= GLUT_KEY_LEFT && key <= GLUT_KEY_INSERT) {
    ret = _directionalKeys[key - GLUT_KEY_LEFT];
  }
  return ret;
}

void KeyboardManager::updateKeyState(unsigned char key, bool isDown) {
  _keys[tolower(key)] = isDown;
}

void KeyboardManager::updateSpecialKeyState(int key, bool isDown) {
  if(key >= GLUT_KEY_F1 && key <= GLUT_KEY_F12) {
    _functionKeys[key - GLUT_KEY_F1] = isDown;
  } else if(key >= GLUT_KEY_LEFT && key <= GLUT_KEY_INSERT) {
    _directionalKeys[key - GLUT_KEY_LEFT] = isDown;
  }
}

void KeyboardManager::initialize() {
  int i = 0;
  for(i = 0; i <= UCHAR_MAX; i++) {
    _keys[i] = false;
  }
  for(i = 0; i < 9; i++) {
    _functionKeys[i] = false;
  }
  for(i = 0; i < 12; i++) {
    _directionalKeys[i] = false;
  }
}
