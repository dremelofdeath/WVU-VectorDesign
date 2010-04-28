#include <cstring>

#include "nhz_common.h"

// wglGetProcAddress function prototypes
#ifdef WIN32
PFNGLPOINTPARAMETERFVPROC NHZ_glPointParameterfvARB = NULL;
#endif

static char *gl_extensions_supported = NULL;
static char _NHZ_ERR_STRING[NHZ_ERR_BUF_MAX];

#ifdef WIN32
static wchar_t _NHZ_ERR_WSTRING[NHZ_ERR_BUF_MAX];
#endif

int gl_has_extension(const char *extension) {
  int ret = 0;
  if(gl_extensions_supported == NULL) {
    gl_extensions_supported = (char *)glGetString(GL_EXTENSIONS);
  }
  ret = std::strstr(gl_extensions_supported, extension) != NULL;
  return ret;
}

char *_nhzgeterr() {
  return (char *)_NHZ_ERR_STRING;
}

#ifdef WIN32
wchar_t *_nhzgetwerr() {
  MultiByteToWideChar(CP_ACP, NULL, _nhzgeterr(), -1, _NHZ_ERR_WSTRING, 
                      NHZ_ERR_BUF_MAX);
  return (wchar_t *)_NHZ_ERR_WSTRING;
}
#endif

// wglGetProcAddress functions
#ifdef WIN32

#define NHZPROCLOAD(proctype, procname) \
  if ( NHZ_##procname == NULL ) { \
    NHZ_##procname = ( proctype )wglGetProcAddress( #procname ); \
    if ( NHZ_##procname == NULL ) { \
      NHZ_ERR( #procname " couldn't be loaded\n"); \
      NHZ_ERR("windows system error code %d\n", GetLastError()); \
    } \
  } else { \
    NHZ_ERR( #procname " already loaded\n" ); \
  }

#define IF_EXTENSION_IS_SUPPORTED(ext) \
  if(!gl_has_extension( ext )) { \
    NHZ_ERR( ext " is not supported; ignoring\n"); \
  } else \

void nhz_load_all_gl_procs() {
  IF_EXTENSION_IS_SUPPORTED("GL_ARB_point_parameters") {
    NHZPROCLOAD(PFNGLPOINTPARAMETERFVARBPROC, glPointParameterfvARB);
  }
}

#undef IF_EXTENSION_IS_SUPPORTED
#undef NHZPROCLOAD
#endif
