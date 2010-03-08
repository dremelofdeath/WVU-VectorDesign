#ifndef VECTOR_OBJECTDETECTOR_HPP
#define VECTOR_OBJECTDETECTOR_HPP

#include <cv.h>

class ObjectDetector {
  public:
    static ObjectDetector& getInstance();
    ~ObjectDetector();
    const CvSeq* detect(const IplImage* img);
    float getScaleFactor();
    int getMinNeighbors();
    CvSize getMinSize();
    CvSeq* getFaces();
    void setScaleFactor(const float scaleFactor);
    void setMinNeighbors(const int minNeighbors);
    void setMinSize(CvSize minSize);
  protected:
    ObjectDetector();
    ObjectDetector(const float scaleFactor, const int minNeighbors,
                   CvSize minSize);
    ObjectDetector(const float scaleFactor, const int minNeighbors,
                   CvSize minSize, const char* cascadePath);
    void init();
    void init(const float scaleFactor, const int minNeighbors, CvSize minSize);
    void init(const float scaleFactor, const int minNeighbors, CvSize minSize, 
              const char* cascadePath);
  private:
    static ObjectDetector* _singleton;
    static const float _DEFAULT_SCALE_FACTOR = 1.2;
    static const int _DEFAULT_MIN_NEIGHBORS = 2;
    CvHaarClassifierCascade* _cascade;
    CvMemStorage* _storage;
    CvSeq* _faces;
    float _scaleFactor;
    int _minNeighbors;
    CvSize _minSize;
    int _lastHeight = -1, _lastWidth = -1;
    IplImage* _scaledImage;
}

#endif
