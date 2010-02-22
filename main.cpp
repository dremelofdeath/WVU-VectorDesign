#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include "cv.h"
#include "highgui.h"


int main (int argc, char * const argv[]) {
	IplImage* img = 0; 
    char text[256] = {0};
    char text2[256] = {0};
	int height,width,step,channels,fps;
    clock_t last_clock;
	uchar *data;
    int clocki = 0;
    // load up the face detection data
    CvHaarClassifierCascade *cascade =
        (CvHaarClassifierCascade*)cvLoad("haarcascade_frontalface_alt.xml");
    if(!cascade) {
        fprintf(stderr, "DEATH: cascade failure!\n");
        exit(1);
    }
    // initlialize video capture with default device (0)
    int device = 0;
	CvCapture *capture = cvCaptureFromCAM(device);
    // initialize a font
    CvFont font, smallfont;
    cvInitFont(&font, CV_FONT_HERSHEY_DUPLEX, 1.0, 1.0, 0, 1);
    cvInitFont(&smallfont, CV_FONT_HERSHEY_DUPLEX, 0.75, 0.75, 0, 1);
	// create a window
	cvNamedWindow("Main Window", CV_WINDOW_AUTOSIZE); 
	cvMoveWindow("Main Window", 100, 100);
    last_clock = clock();
    while(cvWaitKey(1) == -1) {
        // load an image  
        img = cvQueryFrame(capture);
        if(!img){
            printf("Could not load image file: %s\n",argv[1]);
            exit(0);
        }
        // get the image data
        height    = img->height;
        width     = img->width;
        step      = img->widthStep;
        channels  = img->nChannels;
        data      = (uchar *)img->imageData;
        // detect faces
        CvMemStorage *storage = cvCreateMemStorage(0);
        CvSeq *faces;
        int iface;
        faces = cvHaarDetectObjects(img, cascade, storage, 1.2, 2,
                                    CV_HAAR_DO_CANNY_PRUNING);
        // draw face rects
        for(iface = 0; iface < faces->total; iface++) {
            CvRect face_rect = *(CvRect*)cvGetSeqElem(faces, iface);
            cvRectangle(img, cvPoint(face_rect.x,face_rect.y),
                        cvPoint((face_rect.x+face_rect.width),
                                (face_rect.y+face_rect.height)),
                        CV_RGB(0, 255, 0), 3);
        }
        // show the image
        sprintf(text, "%dx%d image, %d channels", height, width, channels);
        cvPutText(img, text, cvPoint(0, 25), &font, cvScalar(0, 255, 0));
        cvPutText(img, text2, cvPoint(1, 48), &smallfont, cvScalar(0, 255, 0));
        cvShowImage("Main Window", img );
        if(++clocki >= 5) {
            clocki = 0;
            fps = 5*CLOCKS_PER_SEC/(int)(clock()-last_clock);
            sprintf(text2, "%d fps on device %d", fps, device);
            last_clock = clock();
        }
    }
	// release the image
	cvReleaseCapture(&capture);
	return 0;
}
