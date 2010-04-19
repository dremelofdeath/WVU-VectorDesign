#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#include <windows.h>
#include <GL/gl.h>

#include "nhz_common.h"
#include "ObjectDetector.h"
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
  if(_detector != NULL) {
    delete _detector;
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
  static double glVersionFloat = 0.0;

  if(glVersionFloat == 0.0) {
    glVersionFloat = atof((const char *)glGetString(GL_VERSION));
  }

  _capture = NULL;
  _padder = NULL;
  setDevice(deviceID);
  _frameTex = frameTexture;

  cvInitFont(&_font, CV_FONT_HERSHEY_DUPLEX, 1.0, 1.0, 0, 1);
  cvInitFont(&_smallfont, CV_FONT_HERSHEY_DUPLEX, 0.75, 0.75, 0, 1);

  _useFaceDetection = true;
  _detector = new ObjectDetector;

  _useSubImagePadding = false;
  _usingPadding = glVersionFloat < 2.0f;
  if(_usingPadding) {
    _padder = new PixelPadder;
    if(glVersionFloat > 1.099f) {
      _useSubImagePadding = true;
      _padder->stopDrawing();
    }
  }
  _paddingScaleFactor = 1.0f;

}

void Orientation::setDevice(int deviceID) {
  if(deviceID != _deviceID) {
    _deviceID = deviceID;
    restartCapture();
  }
}

void Orientation::pauseFaceDetection() {
  _useFaceDetection = false;
}

void Orientation::resumeFaceDetection() {
  _useFaceDetection = true;
}

//taken from http://blog.damiles.com/?p=9
void Orientation::uploadTexture(IplImage* img) {
  GLenum errorCode = GL_NO_ERROR;

  if(_usingPadding && !_useSubImagePadding) {
    _padder->padWithImage(img);
    img = _padder->getImage();
  }

  if(_useSubImagePadding && _padder->getImage() == NULL) {
    _padder->copyPropertiesFromImage(img);
    _padder->setDimensions(1024);
  }

  if(_frameTex == 0) {
    regenerateTexture();
    configureTextureParameters();

    if(_useSubImagePadding) { // no NPOT support, but we do have subimage support
      int width = _padder->getImage()->width;
      int height = _padder->getImage()->height;
      int x = (width - img->width)/2;
      int y = (height - img->height)/2;
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR_EXT,
                   GL_UNSIGNED_BYTE, _padder->getImage()->imageData);
      glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, img->width, img->height,
                      GL_BGR_EXT, GL_UNSIGNED_BYTE, img->imageData);
    } else { // NPOT support, or GL 1.0 support only
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img->width, img->height, 0,
                   GL_BGR_EXT, GL_UNSIGNED_BYTE, img->imageData);
    }

    if((errorCode = glGetError()) != GL_NO_ERROR) {
      NHZ_ERR("%s (image)\n", (const char *)gluErrorString(errorCode));
    }
  } else {
    configureTextureParameters();
    if(_useSubImagePadding) {
      int x = (_padder->getImage()->width - img->width)/2;
      int y = (_padder->getImage()->height - img->height)/2;
      glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, img->width, img->height,
                      GL_BGR_EXT, GL_UNSIGNED_BYTE, img->imageData);
    } else {
      glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, img->width, img->height,
                      GL_BGR_EXT, GL_UNSIGNED_BYTE, img->imageData);
    }

    if((errorCode = glGetError()) != GL_NO_ERROR) {
      NHZ_ERR("%s (subimage)\n", (const char *)gluErrorString(errorCode));
    }
  }
}

//perform rotation based on stored variables
void Orientation::performRotation() const
{
  //glRotatef(10, 1, 0, 0);
}

//update rotation variables
void Orientation::updateRotation()
{
}

void Orientation::render(void) const {
  static float quadWidth = 19.331f;
  static float distanceOut = -1.0f;
  static float quadHeight = 19.331f;

  char text[256] = {0};
  char text2[256] = {0};
  char text3[256] = {0};
  char text4[256] = {0};

  if(_usingPadding) {
    glScalef(_paddingScaleFactor, _paddingScaleFactor, 1.0f);
  } else {
    //quadHeight = 19.331f / _aspectRatio;
    glScalef(_aspectRatio, 1.0f, 1.0f);
  }

  glScalef(0.58f, 0.58f, 1.0f);

  configureTextureParameters();

  performRotation();

  glBegin(GL_QUADS);
  glTexCoord2f(0.0f, 1.0f);
  glVertex3f(-1.0f, -1.0f, distanceOut);
  glTexCoord2f(0.0f, 0.0f);
  glVertex3f(-1.0f, 1.0f, distanceOut);
  glTexCoord2f(1.0f, 0.0f);
  glVertex3f(1.0f, 1.0f, distanceOut);
  glTexCoord2f(1.0f, 1.0f);
  glVertex3f(1.0f, -1.0f, distanceOut);
  glEnd();
}

void Orientation::idle(const int elapsed) {
  int iface;

  _img = cvQueryFrame(_capture);
  if(!_img) {
    NHZ_ERR("Could not query frame.\n");
  }

  _aspectRatio = ((float)_img->width)/((float)_img->height);

  if(_padder != NULL && _padder->getImage() != NULL) {
    _paddingScaleFactor = sqrt(((float)(_padder->getImage()->width
                                        * _padder->getImage()->height))
                               / ((float)(_img->width * _img->height)));
  }

  // adjust the search distance based on GPU capabilities
  // (i.e., compensate for diminished speed on older machines by limiting depth)
  if(_usingPadding) { // OpenGL 1.x
    if(_useSubImagePadding) { // OpenGL 1.1+
      _detector->setMinSize(cvSize(_img->width/12, _img->height/12));
    } else { // OpenGL 1.0
      _detector->setMinSize(cvSize(_img->width/8, _img->height/8));
    }
  } else { // OpenGL 2.0+
      _detector->setMinSize(cvSize(_img->width/16, _img->height/16));
  }

  if(_useFaceDetection) {
    // detect faces
    CvSeq* faces = _detector->detect(_img);

    // draw face rects
    for(iface = 0; iface < faces->total; iface++) {
      CvRect face_rect = *(CvRect*)cvGetSeqElem(faces, iface);
      cvRectangle(_img, cvPoint(face_rect.x*2,face_rect.y*2),
          cvPoint(2*(face_rect.x+face_rect.width),
            2*(face_rect.y+face_rect.height)),
          CV_RGB(0, 255, 0), 3);
    }
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
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
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
