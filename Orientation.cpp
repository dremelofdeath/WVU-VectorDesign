#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#include <windows.h>
#include <GL/gl.h>
#include <GL/glext.h>

#include "nhz_common.h"
#include "ObjectDetector.h"
#include "Orientation.h"

//look these are some static variables and they probably don't even belong here but they're here for now, so deal with it
static IplImage *image = 0, *hsv = 0, *hue = 0, *mask = 0, *backproject = 0, *histimg = 0;
static CvRect track_window;
static CvBox2D track_box;
static CvConnectedComp track_comp;
static CvHistogram *hist = 0;
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

  _faceVector[0] = initialFaceVector[0];
  _faceVector[1] = initialFaceVector[1];
  _faceVector[2] = initialFaceVector[2];
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

  glScalef(1.5f, 1.5f, 1.0f);

  configureTextureParameters();

  performRotation(_faceVector);

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

//straight up highway robbed out of camshiftdemo.c
CvScalar hsv2rgb( float hue )
{
    int rgb[3], p, sector;
    static const int sector_data[][3]=
        {{0,2,1}, {1,2,0}, {1,0,2}, {2,0,1}, {2,1,0}, {0,1,2}};
    hue *= 0.033333333333333333333333333333333f;
    sector = cvFloor(hue);
    p = cvRound(255*(hue - sector));
    p ^= sector & 1 ? 255 : 0;

    rgb[sector_data[sector][0]] = 255;
    rgb[sector_data[sector][1]] = 0;
    rgb[sector_data[sector][2]] = p;

    return cvScalar(rgb[2], rgb[1], rgb[0],0);
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
      calculateFaceVector(_img, face_rect);
    }

	//this definitely doesn't go here, in fact I think we only need to do it once
	//so I'll move it to initialize when I'm feeling less lazy
    image = cvCreateImage( cvGetSize(_img), 8, 3 );
    image->origin = _img->origin;
    hsv = cvCreateImage( cvGetSize(_img), 8, 3 );
    hue = cvCreateImage( cvGetSize(_img), 8, 1 );
    mask = cvCreateImage( cvGetSize(_img), 8, 1 );
    backproject = cvCreateImage( cvGetSize(_img), 8, 1 );
    hist = cvCreateHist( 1, &hdims, CV_HIST_ARRAY, &hranges, 1 );
    histimg = cvCreateImage( cvSize(320,200), 8, 3 );
//    cvZero( histimg );
	//end horrifying memory leak section

    



	//camshift stuff
	//calculate back projection (do this every time we call camshift)

	
	cvCalcBackProject( &hue, backproject, hist );
    cvAnd( backproject, mask, backproject, 0 );

	
	if (faces->total > 0) //don't want to do anything yet if there's no faces to choose from
	{
		//I'm just grabbing the 0th element, later let's get the one with the biggest area
		track_window = *(CvRect*)cvGetSeqElem(faces, 0);


		//here's a big chunk of somewhat modified camshiftdemo code we need to appropriate

		int _vmin = 67, _vmax = 256, smin = 105; //these values are subject to change based on how much they suck

	    cvInRangeS( hsv, cvScalar(0,smin,MIN(_vmin,_vmax),0),
					cvScalar(180,256,MAX(_vmin,_vmax),0), mask );
					cvSplit( hsv, hue, 0, 0, 0 );

		float max_val = 0.f;
		cvSetImageROI( hue, track_window );
		cvSetImageROI( mask, track_window );
	
		cvCalcHist( &hue, hist, 0, mask );
		cvGetMinMaxHistValue( hist, 0, &max_val, 0, 0 );
		cvConvertScale( hist->bins, hist->bins, max_val ? 255. / max_val : 0., 0 );
		cvResetImageROI( hue );
		cvResetImageROI( mask );
	//	track_window = selection; dont need this
	//	track_object = 1; or this
	
	//probably don't need anything to do with histimg but honestly who knows
	//cvZero( histimg );
    //bin_w = histimg->width / hdims;
		int i;
		for( i = 0; i < hdims; i++ )
		{
			int val = cvRound( cvGetReal1D(hist->bins,i)*histimg->height/255 );
			CvScalar color = hsv2rgb(i*180.f/hdims);
	//		cvRectangle( histimg, cvPoint(i*bin_w,histimg->height),
	//					cvPoint((i+1)*bin_w,histimg->height - val),
	//					color, -1, 8, 0 );
		}

		//end big chunk of camshiftdemo code


		//ok now for the main attraction
		cvCamShift( backproject, 
		track_window,
		//don't know what the hell all this is, don't think we need to touch it
		cvTermCriteria( CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 10, 1 ),
		//track_box is output, track_comp is the next piece of state information (see the next line)
		&track_comp, &track_box );

        if( !_img->origin ) track_box.angle = -track_box.angle;


        cvEllipseBox(_img, track_box, CV_RGB(255,0,0), 3, CV_AA, 0 );

	}
	
	track_window = track_comp.rect;


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
  glRotatef(60.0f*vec[0], 0.0f, 1.0f, 0.0f);
  glRotatef(-60.0f*vec[1], 1.0f, 0.0f, 0.0f);
  glTranslatef(0.0f, 0.0f, 0.95f);
}

#ifdef TRUEPERSPECTIVE_ORIENTATION_DEG2RADDEFINED
#undef DEG2RAD
#endif
#undef FLOAT_PI

void Orientation::calculateFaceVector(IplImage* img, CvRect& face_rect) {
  int x_mid_rect = face_rect.x+face_rect.width/2;
  int y_mid_rect = face_rect.y+face_rect.height/2;
  int x_mid_img = img->width/4;
  int y_mid_img = img->height/4;
  float rect_area = (float)(face_rect.width*face_rect.height);
  float img_area = ((float)(img->width*img->height))/4;
  _faceVector[0] = (float)(x_mid_rect-x_mid_img)/(float)x_mid_img;
  //_faceVector[0] = pow(_faceVector[0], 3.0f);
  //_faceVector[0] *= -2.0f;
  _faceVector[1] = (float)(y_mid_rect-y_mid_img)/(float)y_mid_img;
  _faceVector[2] = sqrt(img_area/rect_area);
}
