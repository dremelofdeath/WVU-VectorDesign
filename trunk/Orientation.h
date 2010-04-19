#ifndef VECTOR_ORIENTATION_H
#define VECTOR_ORIENTATION_H

#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/freeglut.h>
#include <cv.h>
#include <highgui.h>

#include "Renderable.h"
#include "PixelPadder.h"
#include "ObjectDetector.h"

class Orientation : public Renderable
{
  public:
    Orientation();
    Orientation(int deviceID);
    Orientation(int deviceID, GLuint frameTexture);
    ~Orientation();
    virtual void initialize();
    virtual void initialize(int deviceID);
    virtual void initialize(int deviceID, GLuint frameTexture);
    virtual void setDevice(int deviceID);
    virtual void pauseFaceDetection();
    virtual void resumeFaceDetection();
    virtual void uploadTexture(IplImage* img);
    virtual void render() const;
    virtual void idle(const int elapsed);
    virtual void updateRotation();
  protected:
    virtual void regenerateTexture();
    virtual void configureTextureParameters() const;
    virtual void restartCapture();
    virtual void releaseCapture();
    virtual void performRotation() const;

  private:
    int _deviceID;
    bool _usingPadding, _useSubImagePadding;
    float _aspectRatio, _paddingScaleFactor;
    bool _useFaceDetection;
    PixelPadder* _padder;
    ObjectDetector* _detector;
    CvCapture* _capture;
		GLuint _frameTex;
    CvFont _font, _smallfont;
    IplImage* _img;

	float angle;
	float x;
	float y;
	float z;
};

#endif