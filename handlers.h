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

/*! \fn void idle()
 * The idle callback for GLUT. True Perspective's idle callback simply forwards
 * idle messages to the RenderQueue singleton, which in turn fires idle messages
 * to the Renderable objects in the queue. Also provides a safe method of
 * calculating the elapsed time between idle calls.
 */

/*! \fn void handle_keyboard(unsigned char key, int x, int y)
 * Handles keyboard events for GLUT. True Perspective simply forwards these
 * messages to the KeyboardManager singleton, which provides a unified interface
 * for querying the keyboard state.
 * \param key the key (represented as a character) that was pressed
 * \param x the x coordinate of where this keyboard event occured (ignored by
 * True Perspective)
 * \param y the y coordinate of where this keyboard event occured (ignored by
 * True Perspective)
 */

/*! \fn void handle_special_key(int key, int x, int y)
 * Handles non-character keyboard events for GLUT. True Perspective simply
 * forwards these messages to the KeyboardManager singleton, which provides a
 * unified interface for querying the keyboard state.
 * \param key the GLUT integer representation of the key that was pressed
 * \param x the x corrdinate of where this keyboard event occured (ignored by
 * True Perspective)
 * \param y the y coordinate of where this keyboard event occured (ignored by
 * True Perspective)
 */

/*! \fn void handle_mouse_event(int button, int state, int x, int y)
 * Handles mouse events (clicking, etc.) for GLUT. True Perspective simply
 * forwards these messages to the RenderQueue singleton, which allows each
 * individual Renderable instance to process the individual messages however
 * they please.
 * \param button the GLUT integer representation of the mouse button pressed
 * \param state the state (up or down) of the mouse button event
 * \param x the x corrdinate of where this mouse event occured
 * \param y the y coordinate of where this mouse event occured
 */

/*! \fn void handle_mouse_drag(int x, int y)
 * Handles mouse dragging events (left mouse button is held for a duration while
 * the mouse is moving) for GLUT. True Perspective simply forwards these
 * messages to the RenderQueue singleton, which allows each individual
 * Renderable instance to process the individual messages however they please.
 * \param x the x corrdinate of where this mouse event occured
 * \param y the y coordinate of where this mouse event occured
 */

/*! \fn void handle_mouse_motion(int x, int y)
 * Handles mouse motion events as the mouse moves over the viewport for GLUT.
 * True Perspective simply forwards these messages to the RenderQueue singleton,
 * which allows each individaul Renderable instance to process the individual
 * messgaes however they please.
 * \param x the x corrdinate of where this mouse event occured
 * \param y the y coordinate of where this mouse event occured
 */

/*! \fn void update_projection(GLsizei w, GLsizei h)
 * Updates the GL projection matrix based on the given width and height
 * parameters. Should be called when the screen is resized or the viewport
 * parameters change.
 * \param w the width of the new viewport
 * \param h the height of the new viewport
 */

/*! \fn static void reshape(GLsizei w, GLsizei h)
 * The reshape callback for GLUT. Processes the new resulting viewport (which
 * GLUT reports via this function's parameters). This message is not forwarded.
 * \param w the width of the new viewport
 * \param h the height of the new viewport
 */

/*! \fn void create_callbacks()
 * Instructs GLUT to use the callbacks defined in this file.
 */

/*! \fn void exit_callback()
 * The callback that is called at the end of program execution. This callback is
 * not handled by GLUT -- it is manually called (or should be handled by
 * atexit()).
 */

/*! \fn int create_window(const char *title, int xpos, int ypos, int ww, int wh)
 * Creates the standard viewport window for use with GLUT. All True Perspective
 * applications should use this function to create their window.
 * \param title the initial title of the window being created
 * \param xpos the x position for the new window
 * \param ypos the y position for the new window
 * \param ww the new window's initial width
 * \param wh the new window's initial height
 * \return the window ID for the newly create window
 */

/*! \fn void init_opengl()
 * Initializes the GL subsystem for use with True Perspective. Should be called
 * exactly once per execution.
 */

/*! \fn void set_window_id(int id)
 * Sets the private window ID that refers to the primary window. This function
 * should only be called once per execution, and only for the window that is
 * desired to be the primary window for the application. Do not call this
 * function multiple times when using multiple windows unless you actually want
 * to change the primary window.
 * \param id the id of the new primary window
 */

/*! \fn void main_springload()
 * The main entry point for all True Perspective applications. All True
 * Perspective applications should define a customized main_springload(). (The
 * default one provided may be customized and used as a template).
 */
