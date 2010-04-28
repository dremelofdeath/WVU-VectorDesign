/*! \file KeyboardManager.h
 *  \brief Defines a class for managing the keyboard.
 *	\author Zachary Murray
 *  \date 11/24/09
 */

#ifndef NHZ_KEYBOARDMANAGER_H
#define NHZ_KEYBOARDMANAGER_H

#include <climits>

class KeyboardManager {
  public:
    static KeyboardManager& getInstance();
    bool isKeyDown(unsigned char key);
    bool isSpecialKeyDown(int key);
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

/*! \class KeyboardManager
 *	\brief A class to manage the keyboard.
 *
 *	A class containing functions to both check and update the current value of
 *	ascii, function, and directional keys on the keyboard.  The latter two
 *	types of keys are also known as 'special keys', and will be refered to as
 *	such in this documentation and the code.
 */

/*!	\fn bool isKeyDown(unsigned char key)
 *	\brief Determines if an ascii key is pressed.
 *	\param key
 *	  The key to check.
 *	\return
 *	  The boolean status of key
 *	\memberof KeyboardManager
 */

/*!	\fn isSpecialKeyDown(int key)
 *	\brief Determines if a special key is pressed.
 *	\param key
 *	  The key to check.
 *	\return
 *	  The boolean status of key
 * \memberof KeyboardManager
 */

/*!	\fn updateKeyState(unsigned char key, bool isDown)
 *	\brief Updates the state of an ascii key.
 *	\param key
 *	  The key whose status is to be updated.
 *	\param isDown
 *	  The status to set to key.
 *	\memberof KeyboardManager
 */

/*!	\fn void updateSpecialKeyState(int key, bool isDown)
 *	\brief Updates the state of a special key.
 *	\param key
 *	  The key whose status is to be updated.
 *	\param isDown
 *	  The status to set to key.
 *	\memberof KeyboardManager
 */