#ifndef VECTOR_ORIENTATION_H
#define VECTOR_ORIENTATION_H

#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/freeglut.h>
#include <cv.h>
#include <highgui.h>

#include "Renderable.h"

class Orientation : public Renderable
{
  public:
    Orientation();
    void genTexture_Ipl(IplImage *image, GLuint text);
    void render(void) const;
    void idle();
    ~Orientation();
  private:
    CvHaarClassifierCascade* _cascade;
		GLuint _frameTex;
};

#endif
