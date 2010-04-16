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
{   public:
        Orientation();

		GLuint frameTex;

        int loadTexture_Ipl(IplImage *image, GLuint *text);
        int fillRotationArray(double x, double y, double z, double angle, double rotArr[9]);
        void myReshape(int w, int h);
        void render(void) const;
        void idle();
        void CreateGlutCallbacks();
        void initGL();

        int main(int argc, char * const argv[]);

        ~Orientation();
};

#endif