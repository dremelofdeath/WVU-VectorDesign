/*! \file handlers.h
 *  \brief A set of I/O handling functions
 *  \author Zachary Murray
 *	\date 11/24/09
 */

#ifdef WIN32
#include <windows.h>
#endif

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#ifndef NHZ_HANDLERS_H
#define NHZ_HANDLERS_H

#ifdef __cplusplus
extern "C" {
#endif

void display();
#ifdef WIN32
void win32_set_hInstance(HINSTANCE inst);
HINSTANCE win32_get_hInstance();
void win32_idle_handle_messages();
#endif
void idle();
void handle_keyboard(unsigned char key, int x, int y);
void handle_special_key(int key, int x, int y);
void handle_mouse_event(int button, int state, int x, int y);
void handle_mouse_drag(int x, int y);
void handle_mouse_motion(int x, int y);
void update_projection(GLsizei w, GLsizei h);
static void reshape(GLsizei w, GLsizei h);
void create_callbacks();
void exit_callback();
int create_window(const char *title, int xpos, int ypos, int ww, int wh);
void init_opengl();
void set_window_id(int id);
void main_springload();

#ifdef __cplusplus
}
#endif

#endif