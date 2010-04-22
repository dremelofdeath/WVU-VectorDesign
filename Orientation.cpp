#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#include <windows.h>
#include <GL/gl.h>
#include <GL/glext.h>

#include "nhz_common.h"
#include "KeyboardManager.h"
#include "ObjectDetector.h"
#include "Orientation.h"

//look these are some static variables and they probably don't even belong here but they're here for now, so deal with it
static int hdims = 16;
static float hranges_arr[] = {0,180};
static float* hranges = hranges_arr;

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
  if(_hsv) {
    cvReleaseImage(&_hsv);
  }
  if(_hue) {
    cvReleaseImage(&_hue);
  }
  if(_mask) {
    cvReleaseImage(&_mask);
  }
  if(_backproject) {
    cvReleaseImage(&_backproject);
  }
  if(_hist) {
    cvReleaseHist(&_hist);
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
  static const float initialFaceVector[3] = {0.0f, 0.0f, -2.0f};
  static double glVersionFloat = 0.0;

  if(glVersionFloat == 0.0) {
    glVersionFloat = atof((const char *)glGetString(GL_VERSION));
  }

  _timeSpentTracking = 0;

  _capture = NULL;
  _padder = NULL;
  setDevice(deviceID);
  _frameTex = frameTexture;

  _trackingEnabled = false;

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

  _faceVector[0] = initialFaceVector[0];
  _faceVector[1] = initialFaceVector[1];
  _faceVector[2] = initialFaceVector[2];

  _backproject = NULL;
  _hsv = NULL;
  _hue = NULL;
  _mask = NULL;
  _hist = NULL;
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

void Orientation::render(void) const {
  char text[256] = {0};
  char text2[256] = {0};
  char text3[256] = {0};
  char text4[256] = {0};

  if(_usingPadding) {
    glScalef(_paddingScaleFactor*0.667f, _paddingScaleFactor*0.667f, 1.0f);
  } else {
    //quadHeight = 19.331f / _aspectRatio;
    glScalef(_aspectRatio*0.58f, 0.58f, 1.0f);
  }

  configureTextureParameters();

  if(_trackingEnabled && !KeyboardManager::getInstance().isKeyDown('s')) {
    glScalef(1.5f, 1.5f, 1.0f);
    performRotation(_faceVector);
  }

  glBegin(GL_QUADS);
  glTexCoord2f(0.0f, 1.0f);
  glVertex3f(-1.0f, -1.0f, -1.0f);
  glTexCoord2f(0.0f, 0.0f);
  glVertex3f(-1.0f, 1.0f, -1.0f);
  glTexCoord2f(1.0f, 0.0f);
  glVertex3f(1.0f, 1.0f, -1.0f);
  glTexCoord2f(1.0f, 1.0f);
  glVertex3f(1.0f, -1.0f, -1.0f);
  glEnd();
}

void Orientation::idle(const int elapsed) {
  static const int vmin = 66, vmax = 256, smin = 66;
  static const int trimFactor = 20;
  static const int faceTimeThreshold = 3500;
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

  if(_timeSpentTracking > faceTimeThreshold || !_trackingEnabled) {
    resumeFaceDetection();
    _timeSpentTracking = 0;
  }

  if(KeyboardManager::getInstance().isKeyDown('r')) {
    resumeFaceDetection();
    _trackingEnabled = false;
  }

  // camshift initializations
  if(!_hsv) {
    _hsv = cvCreateImage(cvGetSize(_img), 8, 3);
  }
  if(!_hue) {
    _hue = cvCreateImage(cvGetSize(_img), 8, 1);
  }
  if(!_mask) {
    _mask = cvCreateImage(cvGetSize(_img), 8, 1);
  }
  if(!_backproject) {
    _backproject = cvCreateImage(cvGetSize(_img), 8, 1);
  }
  if(!_hist) {
    _hist = cvCreateHist(1, &hdims, CV_HIST_ARRAY, &hranges, 1);
  }

  cvCvtColor(_img, _hsv, CV_BGR2HSV);

  cvInRangeS(_hsv, cvScalar(0, smin, MIN(vmin, vmax), 0),
             cvScalar(180, 256, MAX(vmin, vmax), 0), _mask);
  cvSplit(_hsv, _hue, 0, 0, 0);

  // pull face detection from the haar classifier
  // we only want to do this once in awhile
  if(_useFaceDetection) {
    // detect faces
    CvSeq* faces = _detector->detect(_img);

    // draw face rects
    for(iface = 0; iface < faces->total; iface++) {
      CvRect face_rect = *(CvRect*)cvGetSeqElem(faces, iface);
      /*cvRectangle(_img, cvPoint(face_rect.x*2,face_rect.y*2),
                  cvPoint(2*(face_rect.x+face_rect.width),
                          2*(face_rect.y+face_rect.height)),
                  CV_RGB(0, 255, 0), 3);*/

      // I'm just grabbing the last element
      // later let's get the one with the biggest area
      _trackWindow.x = face_rect.x * 2 + trimFactor;
      _trackWindow.y = face_rect.y * 2 + trimFactor;
      _trackWindow.width = face_rect.width * 2 - trimFactor;
      _trackWindow.height = face_rect.height * 2 - trimFactor;

      // let's try to use camshift instead.
      //calculateFaceVector(_img, face_rect);
    }

    // don't want to do anything yet if there's no faces to choose from
    if ((!_trackingEnabled || _timeSpentTracking > faceTimeThreshold)
        && faces->total > 0) {
      float max_val = 0.0f;

      _timeSpentTracking = 0;
      _trackingEnabled = true;

      cvSetImageROI(_hue, _trackWindow);
      cvSetImageROI(_mask, _trackWindow);

      cvCalcHist(&_hue, _hist, 0, _mask);
      cvGetMinMaxHistValue(_hist, 0, &max_val, 0, 0);
      cvConvertScale(_hist->bins, _hist->bins,
                     max_val ? 255.0 / max_val : 0.0, 0);

      cvResetImageROI(_hue);
      cvResetImageROI(_mask);
    }
  }

  if(_trackingEnabled) {
    CvBox2D track_box;
    CvConnectedComp track_comp;

    //calculate back projection (do this every time we call camshift)
    cvCalcBackProject(&_hue, _backproject, _hist);
    cvAnd(_backproject, _mask, _backproject, 0);

    // ok now for the main attraction
    cvCamShift(_backproject, _trackWindow,
               cvTermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 10, 1),
               &track_comp, &track_box );

    // track_box is output, track_comp is the next piece of state information
    _trackWindow.x = track_comp.rect.x;
    _trackWindow.y = track_comp.rect.y;
    _trackWindow.width = track_comp.rect.width;
    _trackWindow.height = track_comp.rect.height;

    if( !_img->origin ) track_box.angle = -track_box.angle;

    calculateFaceVector(_img, _trackWindow);

    _timeSpentTracking += elapsed;
  }

  // Debug code. This makes backprojection visible.
  if(KeyboardManager::getInstance().isKeyDown('b')
     && KeyboardManager::getInstance().isKeyDown('p')) {
    cvCvtColor(_backproject, _img, CV_GRAY2BGR);
  }

  // More debug code. This makes the red ellipse visible.
  if(_trackingEnabled && KeyboardManager::getInstance().isKeyDown('f')
     && KeyboardManager::getInstance().isKeyDown('e')) {
      cvEllipseBox(_img, track_box, CV_RGB(255,0,0), 3, CV_AA, 0);
  }

  uploadTexture(_img);

  pauseFaceDetection();
}

void Orientation::regenerateTexture() {
  glGenTextures(1, &_frameTex);
}

void Orientation::configureTextureParameters() const {
  glBindTexture(GL_TEXTURE_2D, _frameTex); // bind the texture to its array
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
  if(!_usingPadding) {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  }
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

#define FLOAT_PI 3.14159f
#ifndef DEG2RAD
#define TRUEPERSPECTIVE_ORIENTATION_DEG2RADDEFINED 1
#define DEG2RAD(theta) ((theta)*180.0f/FLOAT_PI)
#endif

void Orientation::performRotation(const float vec[3]) const {
  const bool is_x_zero = vec[0] > -0.00001f && vec[0] < 0.00001f;
  float theta_xz = 180.0f + DEG2RAD(-atan(vec[2]/vec[0]));
  //float theta_xy = is_x_zero ? 0.0f : DEG2RAD(atan(vec[1]/vec[0]));
  theta_xz += vec[0] >= 0.0f ? -90.0f : 90.0f;
  glTranslatef(0.0f, 0.0f, -0.95f);
  //if(!is_x_zero) glRotatef(theta_xz, 0.0f, 1.0f, 0.0f);
  glRotatef((45.f/_aspectRatio)*vec[0], 0.0f, 1.0f, 0.0f);
  glRotatef(-45.0f*vec[1], 1.0f, 0.0f, 0.0f);
  glTranslatef(0.0f, 0.0f, 0.95f);
}

#ifdef TRUEPERSPECTIVE_ORIENTATION_DEG2RADDEFINED
#undef DEG2RAD
#endif
#undef FLOAT_PI

void Orientation::calculateFaceVector(IplImage* img, CvBox2D& face_box) {
  float fsize = face_box.size.width * face_box.size.height;
  calculateFaceVector(img, (float)face_box.center.x,
                      (float)face_box.center.y, fsize);
}

void Orientation::calculateFaceVector(IplImage* img, CvRect& face_rect) {
  int x_mid_rect = face_rect.x+face_rect.width/2;
  int y_mid_rect = face_rect.y+face_rect.height/2;
  float rect_area = (float)(face_rect.width*face_rect.height);
  calculateFaceVector(img, (float)x_mid_rect, (float)y_mid_rect, rect_area);
}

void Orientation::calculateFaceVector(IplImage* img, float fx,
                                      float fy, float fsize) {
  float x_mid_img = (float)img->width/2;
  float y_mid_img = (float)img->height/2;
  float img_area = (float)(img->width*img->height);
  _faceVector[0] = (fx-x_mid_img)/x_mid_img;
  _faceVector[1] = (fy-y_mid_img)/y_mid_img;
  _faceVector[2] = sqrt(img_area/fsize);
}
