/*! \file ObjectDetector.h
 *  \brief Defines the ObjectDetector Class, which provides an interface to 
 *	\brief detect objects using a haar classifier.
 *  \author Zachary Murray
 *	\author Michael McGregor
 */

#ifndef VECTOR_OBJECTDETECTOR_HPP
#define VECTOR_OBJECTDETECTOR_HPP

#include <cv.h>

class ObjectDetector {
  public:
    static ObjectDetector& getInstance();
    ~ObjectDetector();
    ObjectDetector();
    ObjectDetector(const float scaleFactor, const int minNeighbors,
                   CvSize minSize);
    ObjectDetector(const float scaleFactor, const int minNeighbors,
                   CvSize minSize, const char* cascadePath);
    void init();
    void init(const float scaleFactor, const int minNeighbors, CvSize minSize);
    void init(const float scaleFactor, const int minNeighbors, CvSize minSize, 
              const char* cascadePath);
    CvSeq* detect(const IplImage* img);
    float getScaleFactor();
    int getMinNeighbors();
    CvSize getMinSize();
    CvSeq* getFaces();
    void setScaleFactor(const float scaleFactor);
    void setMinNeighbors(const int minNeighbors);
    void setMinSize(CvSize minSize);
  private:
    static const int _DEFAULT_MIN_NEIGHBORS = 2;
    static ObjectDetector* _singleton;
    CvHaarClassifierCascade* _cascade;
    CvMemStorage* _storage;
    CvSeq* _faces;
    float _scaleFactor;
    int _minNeighbors;
    CvSize _minSize;
    int _lastHeight, _lastWidth;
    IplImage* _scaledImage;
};

#endif

/*!	\class ObjectDetector
 *	\brief Provides an interface for detecting ojects using a haar classifier.
 *
 *	This is an interface to the object detection functionality that
 *	is built into OpenCV.  It contains functions that are used to setup
 *	generic object detection, with some tunable parameters to allow some
 *	customization.  Different haar classifiers can be used to track different
 *	types of objects.
 */

/*! \fn ObjectDetector(const float scaleFactor, const int minNeighbors,
 *					   CvSize minSize, const char* cascadePath)
 *	\brief The constructor for the ObjectDetector class.
 *	\param scaleFactor 
 *	  The factor the search window is scaled by between scans.
 *	\param minNeighbors 
 *	  Minimum number (minus 1) of neighbor rectangles that make up an object,
 *	  with any groups of less than this number of neighbors being ignored.	
 *	\param minSize 
 *	  The minimum size a rectangle must be to be considered.
 *	\param cascadePath 
 *	  Filepath to the desired haar cascade.
 */

/*! \fn void init(const float scaleFactor, const int minNeighbors, 
 *				  CvSize minSize, const char* cascadePath)
 *	\brief Initializes the necessary parameters for object detection.
 */

/*! \fn CvSeq* detect(const IplImage* img)
 *	\brief Detects an object in the image (default detect a face).
 *	\param img 
 *    The image to detect objects in.
 *	\return
 *	  A sequence of rectangles for each group large enough to qualify.
 *	\memberof ObjectDetector
 */

/*!	\fn float getScaleFactor()
 *	\brief Returns the factor the search window is being scaled by each scan.
 *	\return
 *	  The current value of scaleFactor.
 *	\memberof ObjectDetector
 */

/*! \fn int getMinNeighbors()
 *	\brief Returns the min number of neighbors allowed to qualify as an object.
 *	\return
 *	  The current value of minNeighbors.
 *	\memberof ObjectDetector
 */

/*! \fn CvSize getMinSize()
 *	\brief Returns the minimum size a rectangle must be to be considered.
 *	\return
 *	  The current value of minSize.
 *	\memberof ObjectDetector
 */

/*!	\fn CvSeq* getFaces()
 *	\brief Returns the sequence of rectangles currently detected as objects.
 *	\return
 *	  The current sequence of detected objects.
 *	\memberof ObjectDetector
 */

/*!	\fn void setScaleFactor(const float scaleFactor)
 *	\brief Sets the factor the search window is being scaled by each scan.
 *	\memberof ObjectDetector
 */

/*! \fn void setMinNeighbors(const int minNeighbors)
 *	\brief Sets the min number of neighbors allowed to qualify as an object.
 *	\memberof ObjectDetector
 */

/*!	\fn void setMinSize(CvSize minSize)
 *	\brief Sets the minimum size a rectangle must be to be considered.
 *	\memberof ObjectDetector
 */