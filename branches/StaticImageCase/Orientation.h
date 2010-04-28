#ifndef VECTOR_ORIENTATION_H
#define VECTOR_ORIENTATION_H

#ifdef WIN32
#include <windows.h>
#endif

//#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/freeglut.h>
#include <cv.h>
#include <highgui.h>

#include "Renderable.h"
#include "PixelPadder.h"
#include "ObjectDetector.h"
#include "ServerSocket.h"
#include "TCPSocket.h"
#include "NetworkException.h"

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
  protected:
    virtual void regenerateTexture();
    virtual void configureTextureParameters() const;
    virtual void restartCapture();
    virtual void releaseCapture();
    virtual void performRotation(const float vector[3]) const;
    void calculateFaceVector(IplImage* img, CvBox2D& face_box);
    void calculateFaceVector(IplImage* img, CvRect& face_rect);
    void calculateFaceVector(IplImage* img, float fx, float fy, float fsize);
  private:
    int _deviceID, _timeSpentTracking;
    bool _usingPadding, _useSubImagePadding;
    float _aspectRatio, _paddingScaleFactor;
    bool _useFaceDetection, _trackingEnabled;
    bool _hosting, _clienting;
    PixelPadder* _padder;
    ObjectDetector* _detector;
    CvRect _trackWindow;
    CvHistogram* _hist;
    CvCapture* _capture;
    ServerSocket* _receiver;
    TCPSocket* _caller;
		GLuint _frameTex;
    CvFont _font, _smallfont;
    IplImage* _img;
    IplImage* _chat;
    IplImage* _backproject;
    IplImage* _hsv;
    IplImage* _hue;
    IplImage* _mask;
    float _faceVector[3];
	GLuint _staticImage;
};

#endif
