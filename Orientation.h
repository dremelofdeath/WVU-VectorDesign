#include <GL/gl.h>

#include "Renderable.h"

class Orientation : public Renderable
{   public:
        Orientation();

        int loadTexture_Ipl(IplImage *image, GLuint *text);
        int fillRotationArray(double x, double y, double z, double angle, double rotArr[9]);
        void myReshape(int w, int h);
        void render(void);
        void idle();
        void CreateGlutCallbacks();
        void initGL();

        int main(int argc, char * const argv[]);

        ~Orientation();
};