#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#include <windows.h>
#include <GL/gl.h>

#include "nhz_common.h"
#include "Orientation.h"

Orientation::Orientation() {
  initialize();
}

Orientation::Orientation(int deviceID) {
  initialize(deviceID);
}

Orientation::Orientation(int deviceID, GLuint frameTexture) {
  initialize(deviceID, frameTexture);
}

Orientation::~Orientation() {
  releaseCapture();
  if(_frameTex != 0) {
    glDeleteTextures(1, &_frameTex);
  }
  if(_scaledImg != 0) {
    cvReleaseImage(&_scaledImg);
  }
}

void Orientation::initialize() {
  // we are going to lazily generate a texture so that we can use
  // glTexSubImage2D()
  initialize(0, 0);
}

void Orientation::initialize(int deviceID) {
  // again, this is a lazy generation here
  // see uploadTexture() to see what I mean
  initialize(deviceID, 0);
}

void Orientation::initialize(int deviceID, GLuint frameTexture) {
  static const char *facedatafile = "haarcascade_frontalface_alt.xml";
  _capture = 0; // please just don't touch this
  setDevice(deviceID);
  _scaledImg = 0; // or this
  _frameTex = frameTexture;

  cvInitFont(&_font, CV_FONT_HERSHEY_DUPLEX, 1.0, 1.0, 0, 1);
  cvInitFont(&_smallfont, CV_FONT_HERSHEY_DUPLEX, 0.75, 0.75, 0, 1);

  _storage = cvCreateMemStorage(0);

  _cascade = (CvHaarClassifierCascade*)cvLoad(facedatafile);
  if(!_cascade) {
    NHZ_ERR("DEATH: cascade failure!\n");
  }

  _usingPadding = atof((const char *)glGetString(GL_VERSION)) < 2.0f;
  if(_usingPadding) {
    _padder = new PixelPadder;
  }
}

void Orientation::setDevice(int deviceID) {
  if(deviceID != _deviceID) {
    _deviceID = deviceID;
    restartCapture();
  }
}

//taken from http://blog.damiles.com/?p=9
void Orientation::uploadTexture(IplImage* img) {
  GLenum errorCode = GL_NO_ERROR;
  if(_usingPadding) {
    _padder->padWithImage(img);
    img = _padder->getImage();
  }
  if(_frameTex == 0) {
    regenerateTexture();
    configureTextureParameters();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img->width, img->height, 0,
                 GL_BGR_EXT, GL_UNSIGNED_BYTE, img->imageData);
    if((errorCode = glGetError()) != GL_NO_ERROR) {
      NHZ_ERR("%s (image)\n", (const char *)gluErrorString(errorCode));
    }
  } else {
    configureTextureParameters();
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, img->width, img->height, GL_BGR_EXT,
                    GL_UNSIGNED_BYTE, img->imageData);
    if((errorCode = glGetError()) != GL_NO_ERROR) {
      NHZ_ERR("%s (subimage)\n", (const char *)gluErrorString(errorCode));
    }
  }
}

void Orientation::performRotation() const
{
}


void Orientation::render(void) const {
  char text[256] = {0};
  char text2[256] = {0};
  char text3[256] = {0};
  char text4[256] = {0};

  float quadWidth = 10.0f;
  float distanceOut = 30.0f;
  float quadHeight = 10.0f;

  if(_usingPadding) {
    glScalef(2.0f, 2.0f, 1.0f);
  }

  configureTextureParameters();

  glTranslatef(0, 0, -30);
  performRotation();
  glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-quadWidth, -quadHeight, 0);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-quadWidth, quadHeight, 0);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(quadWidth, quadHeight, 0);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(quadWidth, -quadHeight, 0);
  glEnd();
}

void Orientation::idle(const int elapsed) {
  int iface;
  _img = cvQueryFrame(_capture);
  if(!_img) {
    NHZ_ERR("Could not query frame.\n");
  }

  if(_scaledImg == 0) {
    _scaledImg = cvCreateImage(cvSize(_img->width/2, _img->height/2), 8, 3);
    if(!_scaledImg) {
      NHZ_ERR("Couldn't create the scaled image.\n");
    }
  }

  cvClearMemStorage(_storage);
  cvPyrDown(_img, _scaledImg);

  // detect faces
  CvSeq *faces = cvHaarDetectObjects(_scaledImg, _cascade, _storage, 1.2, 2,
                                     CV_HAAR_DO_CANNY_PRUNING,
                                     cvSize(_img->width/16, _img->height/16));

  // draw face rects
  for(iface = 0; iface < faces->total; iface++) {
    CvRect face_rect = *(CvRect*)cvGetSeqElem(faces, iface);
    cvRectangle(_img, cvPoint(face_rect.x*2,face_rect.y*2),
                cvPoint(2*(face_rect.x+face_rect.width),
                        2*(face_rect.y+face_rect.height)),
                CV_RGB(0, 255, 0), 3);
  }

  uploadTexture(_img);
}

void Orientation::regenerateTexture() {
  glGenTextures(1, &_frameTex);
}

void Orientation::configureTextureParameters() const {
  glBindTexture(GL_TEXTURE_2D, _frameTex); // bind the texture to its array
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
}

void Orientation::restartCapture() {
  // use for default resolution
  releaseCapture();
  _capture = cvCaptureFromCAM(_deviceID);
}

void Orientation::releaseCapture() {
  if(_capture != 0) {
    cvReleaseCapture(&_capture);
  }
}
