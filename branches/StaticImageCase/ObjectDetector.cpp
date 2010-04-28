#include "nhz_common.h"
#include "ObjectDetector.h"

#include <highgui.h>

ObjectDetector* ObjectDetector::_singleton = 0;

ObjectDetector::ObjectDetector() {
  init();
}

ObjectDetector::ObjectDetector(const float scaleFactor, const int minNeighbors,
                               CvSize minSize) {
  init(scaleFactor, minNeighbors, minSize);
}

ObjectDetector::ObjectDetector(const float scaleFactor, const int minNeighbors,
                               CvSize minSize, const char* cascadePath) {
  init(scaleFactor, minNeighbors, minSize, cascadePath);
}

void ObjectDetector::init() {
  init(1.2f, _DEFAULT_MIN_NEIGHBORS, cvSize(0, 0));
}

void ObjectDetector::init(const float scaleFactor, const int minNeighbors,
                          CvSize minSize) {
  init(scaleFactor, minNeighbors, minSize, "haarcascade_frontalface_alt.xml");
}

void ObjectDetector::init(const float scaleFactor, const int minNeighbors,
                          CvSize minSize, const char* cascadePath) {
  setScaleFactor(scaleFactor);
  setMinNeighbors(minNeighbors);
  setMinSize(minSize);
  _cascade = (CvHaarClassifierCascade*)cvLoad(cascadePath);
  if(!_cascade) {
    NHZ_ERR("DEATH: cascade failure!\n");
  }
  _storage = cvCreateMemStorage(0);
  _faces = (CvSeq*)0;
  _lastHeight = -1;
  _lastWidth = -1;
  _scaledImage = NULL;
}

ObjectDetector& ObjectDetector::getInstance() {
  if(_singleton == 0) {
    _singleton = new ObjectDetector;
  }
  return *_singleton;
}

ObjectDetector::~ObjectDetector() {
  cvReleaseHaarClassifierCascade(&_cascade);
  cvReleaseMemStorage(&_storage);
}

CvSeq* ObjectDetector::detect(const IplImage* img) {
  cvClearMemStorage(_storage);
  if(img->height != _lastHeight && img->width != _lastWidth) {
    _lastHeight = img->height;
    _lastWidth = img->width;
    if(_scaledImage != NULL) {
      cvReleaseImage(&_scaledImage);
    }
    _scaledImage = cvCreateImage(cvSize(_lastWidth/2, _lastHeight/2), 8, 3);
    if(!_scaledImage) {
      NHZ_ERR("Couldn't create the scaled image.\n");
    }
  }
  cvPyrDown(img, _scaledImage);
  _faces = cvHaarDetectObjects(_scaledImage, _cascade, _storage, _scaleFactor,
                               _minNeighbors, CV_HAAR_DO_CANNY_PRUNING,
                               _minSize);
  return _faces;
}

float ObjectDetector::getScaleFactor() {
  return _scaleFactor;
}

int ObjectDetector::getMinNeighbors() {
  return _minNeighbors;
}

CvSize ObjectDetector::getMinSize() {
  return _minSize;
}

CvSeq* ObjectDetector::getFaces() {
  return _faces;
}

void ObjectDetector::setScaleFactor(const float scaleFactor) {
  // TODO: check for correct input?
  _scaleFactor = scaleFactor;
}

void ObjectDetector::setMinNeighbors(const int minNeighbors) {
  // TODO: check input
  _minNeighbors = minNeighbors;
}

void ObjectDetector::setMinSize(CvSize minSize) {
  // TODO: check input
  _minSize = minSize;
}
