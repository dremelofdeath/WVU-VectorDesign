#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#include "Orientation.h"


//taken from http://blog.damiles.com/?p=9
void Orientation::genTexture_Ipl(IplImage* image, GLuint text)
{
  if (image==NULL) return; 
  glGenTextures(1, &text);

  glBindTexture( GL_TEXTURE_2D, text ); //bind the texture to it's array
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
}

void Orientation::render(void) const
{
  //OLD MAIN

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
  //    CvHaarClassifierCascade *cascade;

  //  const char *facedatafile = "haarcascade_frontalface_alt.xml";
  CvMemStorage *storage;
  CvSeq *faces;
  int iface;
  //    printf("CLOCKS_PER_SEC: %d", CLOCKS_PER_SEC);
  // cascade = (CvHaarClassifierCascade*)cvLoad(facedatafile);
  // if(!cascade) {
  //     fprintf(stderr, "DEATH: cascade failure!\n");
  //     exit(1);
  //   }
  // initlialize video capture with default device (0)
  device = 0;    

  // use for default resolution
  capture = cvCaptureFromCAM(device);

  // use for custom resolution
  //capture = cvCreateCameraCapture(device);
  //cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH, 800);
  //cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT, 600);

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
  //	cvNamedWindow("Main Window", CV_WINDOW_AUTOSIZE); 
  //	cvMoveWindow("Main Window", 100, 100);
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

  //    while(cvWaitKey(1) == -1) {
  cvClearMemStorage(storage);
  // load an image  
  last_frame = clock();
  img = cvQueryFrame(capture);
  last_frame = clock()-last_frame;
  if(!img) {
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
  faces = cvHaarDetectObjects(scaled, _cascade, storage, 1.2, 2,
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

  // show the image
  sprintf_s(text, 256, "%dx%d image, %d channels", height, width, channels);
  cvPutText(img, text, cvPoint(0, 25), &font, cvScalar(0, 255, 0));
  cvPutText(img, text2, cvPoint(1, 48), &smallfont, cvScalar(0, 255, 0));
  cvPutText(img, text3, cvPoint(1, img->height-(25*2)+10), &smallfont, cvScalar(0, 255, 0));
  cvPutText(img, text4, cvPoint(1, img->height-15), &smallfont, cvScalar(0, 255, 0));
  //      cvShowImage("Main Window", img );
  //        if(++clocki >= 5) {
  //  clocki = 0;
  // fps = 5*CLOCKS_PER_SEC/(int)(clock()-last_clock);
  //  sprintf(text2, "%d fps on device %d", fps, device, last_frame/5, last_face/5);
  //   sprintf(text3, "frame ticks wasted: %d", last_frame);
  //    sprintf(text4, "face ticks wasted: %d", last_face);
  //     last_clock = clock();
  //  } 

 
//  genTexture_Ipl(img, &_frameTex);    DO THIS IN THE INIT

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img->width, img->height, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, img->imageData);

  /*
  glPushMatrix();
  glTranslatef(0, 0, -3);
  glColor3f(1.0, 0.0, 0.0);
  glutSolidCube(3);
  glPopMatrix();
  */

  float quadWidth = 10.0f;
  float distanceOut = 30.0f;
  float quadHeight = 10.0f;

  glBegin(GL_QUADS);
  glTexCoord2f(0.0f, 1.0f);
  glVertex3f(-quadWidth, -quadHeight, -distanceOut);
  glTexCoord2f(0.0f, 0.0f);
  glVertex3f(-quadWidth, quadHeight, -distanceOut);
  glTexCoord2f(1.0f, 0.0f);
  glVertex3f(quadWidth, quadHeight, -distanceOut);
  glTexCoord2f(1.0f, 1.0f);
  glVertex3f(quadWidth, -quadHeight, -distanceOut);
  glEnd();

  //  }
  // release the image

  //END OLD MAIN

  //	printf("HI DERE");
  glutSwapBuffers();
  glutPostRedisplay();
}

void Orientation::idle() {
  static int last_time = 0;
  int time = glutGet(GLUT_ELAPSED_TIME);
  int elapsed = time-last_time;
  float delta_seconds = 0.001f*elapsed;
  last_time = time;
}

int main (int argc, char **argv) //char * const argv[]) {
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(480, 480);
  glutCreateWindow("Not Sword Adventures!");

  CreateGlutCallbacks();
  initGL();

  const char *facedatafile = "haarcascade_frontalface_alt.xml";
  CvSeq *faces;
  int iface;
  cascade = (CvHaarClassifierCascade*)cvLoad(facedatafile);
  if(!cascade) {
    fprintf(stderr, "DEATH: cascade failure!\n");
    exit(1);
  }


  glutMainLoop();

  //	cvReleaseCapture(&capture);

  //	ExitGlut();

  return 0;
}
