/*
 *  KeyboardManager.h
 *  Nebula Horizon
 *
 *  Created by Zachary Murray on 11/24/09.
 *
 */

#ifndef NHZ_KEYBOARDMANAGER_H
#define NHZ_KEYBOARDMANAGER_H

#include <climits>

class KeyboardManager {
  public:
    static KeyboardManager& getInstance();
    bool isKeyDown(unsigned char key); // used for ascii keys
    bool isSpecialKeyDown(int key); // used for special keys
    void updateKeyState(unsigned char key, bool isDown);
    void updateSpecialKeyState(int key, bool isDown);
  protected:
    KeyboardManager();
    void initialize();
  private:
    static KeyboardManager* _singleton;
    bool _keys[UCHAR_MAX+1];
    bool _functionKeys[12];
    bool _directionalKeys[9];
};

#endif
