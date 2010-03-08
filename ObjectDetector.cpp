#include "ObjectDetector.hpp"

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

ObjectDetector& ObjectDetector::getInstance() {
  if(_singleton == 0) {
    _singleton = new ObjectDetector;
  }
  return *_singleton;
}

ObjectDetector::~ObjectDetector() {
  cvReleaseHaarClassifierCascade(&_cascade);
  cvReleaseMemStorage(_storage);
}

const CvSeq* ObjectDetector::detect(const IplImage* img) {
  cvClearMemStorage(_storage);
  if(img->height != _lastHeight && img->width != _lastWidth) {
    _lastHeight = img->height;
    _lastWidth = img->width;
    if(_scaledImage != 0) {
      cvReleaseImage(_scaledImage);
    }
    _scaledImage = cvCreateImage(cvSize(_lastWidth/2, _lastHeight/2), 8, 3);
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

void ObjectDetector::init() {
  init(_DEFAULT_SCALE_FACTOR, _DEFAULT_MIN_NEIGHBORS, cvSize(0, 0));
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
  // TODO: check if load is okay?
  _storage = cvCreateMemStorage(0);
  _faces = (CvSeq*)0;
}
