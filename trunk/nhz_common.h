#ifndef NHZ_NHZ_COMMON_H
#define NHZ_NHZ_COMMON_H

#include <stdio.h>

#ifdef WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif
#include <windows.h>
#endif

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#else
#include <GL/gl.h>
#include <GL/glext.h>
#ifdef WIN32
#include <GL/wglext.h>
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define NHZ_ERR_BUF_MAX 256

int gl_has_extension(const char *extension);
char *_nhzgeterr();

#ifdef WIN32
wchar_t *_nhzgetwerr();
void nhz_load_all_gl_procs();
#endif

// below contains the error and resource macros
#define _NHZERRFMT "" __FILE__ ", line %d: ", __LINE__

#ifdef WIN32
#define NHZ_RES_T(base, file) "" base "\\" file
#define NHZ_ERR(...) sprintf_s(_nhzgeterr(), NHZ_ERR_BUF_MAX, _NHZERRFMT);\
  OutputDebugString(_nhzgetwerr()); \
  sprintf_s(_nhzgeterr(), NHZ_ERR_BUF_MAX, __VA_ARGS__); \
  OutputDebugString(_nhzgetwerr())
#else
#define NHZ_RES_T(base, file) file
#define NHZ_ERR(...) fprintf(stderr, _NHZERRFMT); \
  sprintf(_nhzgeterr(), __VA_ARGS__); \
  fprintf(stderr, _nhzgeterr());
#endif

#ifdef __cplusplus
}
#endif

// extensions loading for Windows because it's not done for you
#ifdef WIN32

#define _NHZPROCCOMPLAIN(glfunc) \
  NHZ_ERR("trapped a stray call to " #glfunc " while it was NULL\n");

#define _NHZCHECKANDCALL(glfunc, ...) \
  if ( NHZ_##glfunc == NULL ) { \
    _NHZPROCCOMPLAIN(glfunc); \
  } else NHZ_##glfunc##( __VA_ARGS__ ); \

// windows gl extension prototypes go here (follow the pattern)
extern PFNGLPOINTPARAMETERFVARBPROC NHZ_glPointParameterfvARB;
#define glPointParameterfvARB(...) \
  _NHZCHECKANDCALL(glPointParameterfvARB, __VA_ARGS__ );

#endif

#endif
