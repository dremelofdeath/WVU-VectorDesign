#ifdef WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif
#include <windows.h>
#include <atlbase.h>
#include <atlconv.h>
#include "TruePerspective.h"
#define MAX_LOADSTRING 100
#endif

#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <fstream>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#endif

#include "handlers.h"
#include "KeyboardManager.h"
#include "RenderQueue.h"
#include "nhz_common.h"
#include "Orientation.h"

static GLsizei winWidth = 800, winHeight = 600;
static int window_id_main = 0;

void display() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glPushMatrix();
  RenderQueue::getInstance()->render();
  glPopMatrix();
  glutSwapBuffers();
}

#ifdef WIN32
static HINSTANCE hInstance = NULL;
static HHOOK nextHook;

void win32_set_hInstance(HINSTANCE inst) {
  hInstance = inst;
}

HINSTANCE win32_get_hInstance() {
  return hInstance;
}

void win32_idle_handle_messages() {
  static HACCEL hAccelTable = NULL;
  MSG msg;
  if(!hAccelTable && !win32_get_hInstance()) {
    hAccelTable = LoadAccelerators(win32_get_hInstance(),
                                   MAKEINTRESOURCE(IDC_RENDERQUEUE));
  }
  if(hAccelTable) {
    while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
      if(!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
      }
    }
  }
}

LRESULT CALLBACK CallWndProc(int code, WPARAM wParam, LPARAM lParam) {
  if(code < 0) {
    return CallNextHookEx(nextHook, code, wParam, lParam);
  }
  switch(code) {
    case HC_ACTION:
      PCWPSTRUCT cwp = (PCWPSTRUCT)lParam;
      switch(cwp->message) {
        case WM_MOVING:
        case WM_GETICON:
        case WM_MOVE:
          idle();
          RedrawWindow(cwp->hwnd, NULL, NULL, RDW_ERASE | RDW_INVALIDATE);
          break;
      }
      break;
  }
  return CallNextHookEx(nextHook, code, wParam, lParam);
}
#endif

void idle() {
  static int last_time = 0;
  int time = glutGet(GLUT_ELAPSED_TIME);
  // if we overflow, last_time will be greater than time
  int elapsed = last_time <= time ? time-last_time : time+last_time;
  last_time = time;
  RenderQueue::getInstance()->idle(elapsed);
#ifdef WIN32
  win32_idle_handle_messages();
#endif
  glutPostRedisplay();
}

void handle_keyboard(unsigned char key, int x, int y) {
  KeyboardManager::getInstance().updateKeyState(key, true);
}

void handle_keyboard_up(unsigned char key, int x, int y) {
  KeyboardManager::getInstance().updateKeyState(key, false);
}

void handle_special_key(int key, int x, int y) {
  KeyboardManager::getInstance().updateSpecialKeyState(key, true);
}

void handle_special_key_up(int key, int x, int y) {
  KeyboardManager::getInstance().updateSpecialKeyState(key, false);
}

void handle_mouse_event(int button, int state, int x, int y) {
  RenderQueue::getInstance()->handleMouseEvent(button, state, x, y);
}

void handle_mouse_drag(int x, int y) {
  RenderQueue::getInstance()->handleMouseDrag(x, y);
}

void handle_mouse_motion(int x, int y) {
  RenderQueue::getInstance()->handleMouseMotion(x, y);
}

void update_projection(GLsizei w, GLsizei h) {
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60.0, (double)w/(double)h, 0.1, 1000);
}

static void reshape(GLsizei w, GLsizei h) {
  winWidth = w;
  winHeight = h;
  update_projection(w, h);
  glViewport(0, 0, w, h);
  glutPostRedisplay();
}

void create_callbacks() {
  glutDisplayFunc(&display);
  glutIdleFunc(&idle);
  glutReshapeFunc(&reshape);
  glutIgnoreKeyRepeat(1);
  glutKeyboardFunc(&handle_keyboard);
  glutKeyboardUpFunc(&handle_keyboard_up);
  glutSpecialFunc(&handle_special_key);
  glutSpecialFunc(&handle_special_key_up);
  glutMouseFunc(&handle_mouse_event);
  glutMotionFunc(&handle_mouse_drag);
  glutPassiveMotionFunc(&handle_mouse_motion);
}

void exit_callback() {
  RenderQueue::getInstance()->clear();
  glutDestroyWindow(window_id_main);
}

int create_window(const char *title, int xpos, int ypos, int ww, int wh) {
  static const GLfloat light0_pos[4] = {-1.0f, 0.0f, 0.0f, 1.0f};
  static const GLfloat ambient0[4] = {0.85f, 0.85f, 0.95f, 1.0f};
  static const GLfloat diffuse0[4] = {0.9f, 0.9f, 0.9f, 1.0f};
  static const GLfloat specular0[4] = {1.0f, 1.0f, 1.0f, 1.0f};
  int ret = 0;
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
  glutInitWindowPosition(xpos, ypos);
  glutInitWindowSize(ww, wh);
  ret = glutCreateWindow(title);
  /*glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
  glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
  glEnable(GL_NORMALIZE);*/
  glDepthMask(GL_FALSE);
  glDisable(GL_DEPTH_TEST); 
  glDisable(GL_LIGHTING); 
  glEnable(GL_TEXTURE_2D);
  return ret;
}

void init_opengl() {
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  update_projection(winWidth, winHeight);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  //gluLookAt(0.0, 0.0, 2.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

void set_window_id(int id) {
  window_id_main = id;
}

void main_springload() {
#ifdef WIN32
  TCHAR szTitle[MAX_LOADSTRING];
  USES_CONVERSION;
  LoadString(win32_get_hInstance(), IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
  const char *title = T2A(szTitle);
#else
  const char *title = "True Perspective";
#endif
  set_window_id(create_window(title, 40, 40, 800, 600));
  atexit(&exit_callback);
  create_callbacks();
#ifdef WIN32
  nextHook = SetWindowsHookEx(WH_CALLWNDPROC, CallWndProc,
                              GetModuleHandle(NULL), GetCurrentThreadId());
#endif
  init_opengl();
  
  //put standard main code here
  Orientation orient;
  RenderQueue::getInstance()->enqueue(orient);

  glutMainLoop();
}
