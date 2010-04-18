#include "PixelPadder.h"

PixelPadder::PixelPadder() {
  initialize();
}

PixelPadder::PixelPadder(int wh) {
  initialize(wh);
}

PixelPadder::~PixelPadder() {
  if(_padding) {
    cvReleaseImage(&_padding);
  }
}

void PixelPadder::initialize() {
  initialize(1024);
}

void PixelPadder::initialize(int wh) {
  _padding = 0;
  _depth = 0;
  _channels = 0;
  _isDrawing = true;
  setDimensions(wh);
}

void PixelPadder::stopDrawing() {
  _isDrawing = false;
}

void PixelPadder::resumeDrawing() {
  _isDrawing = true;
}

bool PixelPadder::getDrawingState() {
  return _isDrawing;
}

void PixelPadder::padWithImage(IplImage* image) {
  if(getDrawingState()) {
    if(areImagePropertiesChanging(image)) {
      createPaddingWithDimensions(_dimensions, image);
    }
    drawImageCentered(image);
  }
}

void PixelPadder::setDimensions(int wh) {
  _dimensions = wh;
  createPaddingWithDimensions(wh, NULL);
}

void PixelPadder::copyPropertiesFromImage(IplImage* img) {
    _depth = img->depth;
    _channels = img->nChannels;
}

IplImage* PixelPadder::getImage() {
  return _padding;
}

bool PixelPadder::areImagePropertiesChanging(IplImage* newImage) {
  return newImage->nChannels != _channels || newImage->depth != _depth;
}

void PixelPadder::createPaddingWithDimensions(int wh, IplImage* img) {
  if(_padding != 0) {
    cvReleaseImage(&_padding);
  }

  if(img != NULL) {
    copyPropertiesFromImage(img);
  }

  if(_depth != 0 && _channels != 0) {
    _padding = cvCreateImage(cvSize(wh, wh), _depth, _channels);
    cvFloodFill(_padding, cvPoint(0, 0), cvScalar(0.0, 0.0, 0.0));
  }
}

void PixelPadder::drawImageCentered(IplImage* image) {
  // TODO: this code assumes that the image retrieved is smaller than 1024x1024
  // TODO: or at least that size(image) < size(_padding)
  int x = (_padding->width - image->width)/2;
  int y = (_padding->height - image->height)/2;
  for(int ix=0; ix < image->width; ix++) {
    for(int iy=0; iy < image->height; iy++) {
      double r = cvGet2D(image, iy, ix).val[2];
      double g = cvGet2D(image, iy, ix).val[1];
      double b = cvGet2D(image, iy, ix).val[0];
      CvScalar bgr = cvScalar(b, g, r);
      cvSet2D(_padding, iy+y, ix+x, bgr);
    }
  }
}
