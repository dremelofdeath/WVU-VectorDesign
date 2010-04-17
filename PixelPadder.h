#ifndef TRUEPERSPECTIVE_PIXELPADDER_H
#define TRUEPERSPECTIVE_PIXELPADDER_H

#include <cv.h>

class PixelPadder {
  public:
    PixelPadder();
    PixelPadder(int wh);
    ~PixelPadder();
    void initialize();
    void initialize(int wh);
    void padWithImage(IplImage* image);
    void setDimensions(int wh);
    IplImage* getImage();
  protected:
    bool areImagePropertiesChanging(IplImage* newImage);
    void createPaddingWithDimensions(int wh, IplImage* img);
    void drawImageCentered(IplImage* image);
  private:
    int _dimensions, _depth, _channels;
    IplImage* _padding;
};

#endif
