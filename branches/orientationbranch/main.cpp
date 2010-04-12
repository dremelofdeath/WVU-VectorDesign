#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include "cv.h"
#include "highgui.h"


int fillRotationArray(double x, double y, double z, double angle, double rotArr[9])
{
	rotArr[0] = 1 + (1-cos(angle))*(x*x-1);
	rotArr[1] = -z*sin(angle)+(1-cos(angle))*x*y;
	rotArr[2] = y*sin(angle)+(1-cos(angle))*x*z;
	rotArr[3] = z*sin(angle)+(1-cos(angle))*x*y;
	rotArr[4] = 1 + (1-cos(angle))*(y*y-1);
	rotArr[5] = -x*sin(angle)+(1-cos(angle))*y*z;
	rotArr[6] = -y*sin(angle)+(1-cos(angle))*x*z;
	rotArr[7] = x*sin(angle)+(1-cos(angle))*y*z;
	rotArr[8] = 1 + (1-cos(angle))*(z*z-1);
	return 0;
}

int main (int argc, char * const argv[]) {
	IplImage* img = 0;	
    IplImage* scaled = 0; 
    char text[256] = {0};
    char text2[256] = {0};
    char text3[256] = {0};
    char text4[256] = {0};
	int height,width,step,channels,fps;
    clock_t last_clock, last_frame=0, last_face=0;
    int clocki = 0;
	uchar *data;
    int device;
    CvCapture *capture;
    CvFont font, smallfont;
    // load up the face detection data
    CvHaarClassifierCascade *cascade;
    const char *facedatafile = "haarcascade_frontalface_alt.xml";
    CvMemStorage *storage;
    CvSeq *faces;
    int iface;
    printf("CLOCKS_PER_SEC: %d", CLOCKS_PER_SEC);
    cascade = (CvHaarClassifierCascade*)cvLoad(facedatafile);
    if(!cascade) {
        fprintf(stderr, "DEATH: cascade failure!\n");
        exit(1);
    }
    // initlialize video capture with default device (0)
    device = 0;    

	// use for default resolution
	capture = cvCaptureFromCAM(device);
    
	// use for custom resolution
	/*capture = cvCreateCameraCapture(device);
	cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH, 800);
	cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT, 600);*/

	// initialize a font
    cvInitFont(&font, CV_FONT_HERSHEY_DUPLEX, 1.0, 1.0, 0, 1);
    cvInitFont(&smallfont, CV_FONT_HERSHEY_DUPLEX, 0.75, 0.75, 0, 1);
    storage = cvCreateMemStorage(0);
    img = cvQueryFrame(capture);
    scaled = cvCreateImage(cvSize(img->width/2, img->height/2), 8, 3);
	//DEDIT
	//create clone of image for src - use for rotations
	IplImage* src;
	src = cvCloneImage(img);
	//END DEDIT
	// create a window
	cvNamedWindow("Main Window", CV_WINDOW_AUTOSIZE); 
	cvMoveWindow("Main Window", 100, 100);
    last_clock = clock();


	//DEDIT
	
	//x, y, and z represent a unit vector
	//angle is the angle of rotation around this unit vector
	double x, y, z, angle;
	x = 0.0;
	y = 0.0;
	z = 0.0;
	angle = 45.0;

	//BUILD ROTATION MATRIX:
	//0 1 2
	//3 4 5
	//6 7 8

	const double TRANX = 300; //img->width/10; //-100.0;
	const double TRANY = 300; //img->height/10; //-100.0;
	const double TRANZ = 0; //-100.0;


	double transArr[9] = {1, 0, TRANX,
						  0, 1, TRANY, //lol trany
						  0, 0, 1};


	/*
	double transArr[9] = {1, 0, TRANX,
						  0, 1, TRANY, //lol trany
						  0, 0, TRANZ};
						  */

	double revTransArr[9] = {1, 0, -TRANX,
						     0, 1, -TRANY, //lol trany
						     0, 0, 1};

	CvMat translationMatrix = cvMat(3, 3, CV_64FC1, transArr);

	CvMat revTranslationMatrix = cvMat(3, 3, CV_64FC1, revTransArr);

	double rotArr[9] = {1 + (1-cos(angle))*(x*x-1), -z*sin(angle)+(1-cos(angle))*x*y, y*sin(angle)+(1-cos(angle))*x*z,
						z*sin(angle)+(1-cos(angle))*x*y, 1 + (1-cos(angle))*(y*y-1), -x*sin(angle)+(1-cos(angle))*y*z,
						-y*sin(angle)+(1-cos(angle))*x*z, x*sin(angle)+(1-cos(angle))*y*z, 1 + (1-cos(angle))*(z*z-1)};


	//create the rotation matrix so I can orient later
	CvMat rotationMatrix = cvMat(3, 3, CV_64FC1, rotArr);


	//center of rotation
//	CvPoint2D32f center = cvPoint2D32f(img->width/2, img->height/2); //which iplimage? img or scaled?

	//END DEDIT


	//DEDIT
	//perform orientation
//	cvWarpAffine(scaled, dst, rotationMatrix);
	//END DEDIT


	int increasingY = 0;

    while(cvWaitKey(1) == -1) {
        cvClearMemStorage(storage);
        // load an image  
        last_frame = clock();
        img = cvQueryFrame(capture);
        last_frame = clock()-last_frame;
        if(!img){
            printf("Could not query frame");
            exit(0);
        }
        cvPyrDown(img, scaled);
        // get the image data
        height    = img->height;
        width     = img->width;
        step      = img->widthStep;
        channels  = img->nChannels;
        data      = (uchar *)img->imageData;
        // detect faces
        last_face = clock();
        faces = cvHaarDetectObjects(scaled, cascade, storage, 1.2, 2,
                                    CV_HAAR_DO_CANNY_PRUNING,
                                    cvSize(img->width/16, img->height/16));
        last_face = clock()-last_face;
        // draw face rects
        for(iface = 0; iface < faces->total; iface++) {
            CvRect face_rect = *(CvRect*)cvGetSeqElem(faces, iface);
            cvRectangle(img, cvPoint(face_rect.x*2,face_rect.y*2),
                        cvPoint(2*(face_rect.x+face_rect.width),
                                2*(face_rect.y+face_rect.height)),
                        CV_RGB(0, 255, 0), 3);
        }


	//DEDIT

		//questions:
		//do warp affine before or after rotation? think after - it's what actually applies it
		//which iplimage - scaled or img? what should dst be a clone of?

		//img is what is actually being shown!

	src = cvCloneImage(img);
	
	//update rotation matrix
	if (increasingY) x = x + 0.0003;
	else x = x - 0.0003;
	if (x >= 0.001) increasingY = 0;
	else if (x <= -0.001) increasingY = 1;

	fillRotationArray(x, y, z, angle, rotArr);
	rotationMatrix = cvMat(3, 3, CV_64FC1, rotArr);	


//	translationMatrix = cvMat(3, 3, CV_64FC1, transArr);	

	//before rotation we need to adjust img with translate so that we are rotating around the midpoint
//	cvWarpPerspective(src, img, &rotationMatrix);
//	cvWarpPerspective(src, img, &translationMatrix);	
//	src = cvCloneImage(img);
	//adjust img - do rotation
	cvWarpPerspective(src, img, &rotationMatrix);
//	cvWarpPerspective(src, img, &translationMatrix);	
//	src = cvCloneImage(img);
	//before rotation we need to adjust img with translate so that we are rotating around the midpoint
//	cvWarpPerspective(src, img, &revTranslationMatrix);	

	cvReleaseImage(&src);

	//END DEDIT


        // show the image
        sprintf(text, "%dx%d image, %d channels", height, width, channels);
        cvPutText(img, text, cvPoint(0, 25), &font, cvScalar(0, 255, 0));
        cvPutText(img, text2, cvPoint(1, 48), &smallfont, cvScalar(0, 255, 0));
        cvPutText(img, text3, cvPoint(1, img->height-(25*2)+10), &smallfont, cvScalar(0, 255, 0));
        cvPutText(img, text4, cvPoint(1, img->height-15), &smallfont, cvScalar(0, 255, 0));
        cvShowImage("Main Window", img );
        if(++clocki >= 5) {
            clocki = 0;
            fps = 5*CLOCKS_PER_SEC/(int)(clock()-last_clock);
            sprintf(text2, "%d fps on device %d", fps, device, last_frame/5, last_face/5);
            sprintf(text3, "frame ticks wasted: %d", last_frame);
            sprintf(text4, "face ticks wasted: %d", last_face);
            last_clock = clock();
        }


    }
	// release the image
	cvReleaseCapture(&capture);
	return 0;
}
