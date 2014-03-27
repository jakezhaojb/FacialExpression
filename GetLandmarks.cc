#include "GetLandmarks.h"

double* GetLandmarks(IplImage *orig){

  // Prepare the parameters
  char faceCascadeFilename [] = "haarcascade_frontalface_alt.xml";
  CvHaarClassifierCascade* cascade;
  cascade = (CvHaarClassifierCascade*)cvLoad(faceCascadeFilename, 0, 0, 0);
  if( !cascade )
  {
      printf("Couldnt load Face detector '%s'\n", faceCascadeFilename);
      exit(1);
  }

  FLANDMARK_Model * model = flandmark_init("flandmark_model.dat");
  if (model == 0)
  {
      printf("Structure model was not created. Corrupted file flandmark_model.dat?\n");
      exit(1);
  }

  int *bbox = (int*)malloc(4*sizeof(int));
  double *landmarks = (double*)malloc(2*model->data.options.M*sizeof(double));
  IplImage *input = cvCreateImage(cvSize(orig->width, orig->height), IPL_DEPTH_8U, 1);
  cvConvertImage(orig, input);
      // Smallest face size.
  CvSize minFeatureSize = cvSize(150, 150);
  int flags =  CV_HAAR_DO_CANNY_PRUNING;
  // How detailed should the search be.
  float search_scale_factor = 1.1f;
  CvMemStorage* storage;
  CvSeq* rects;
  int nFaces;
  char front_img_file_name[32];
  IplImage* regular_face = cvCreateImage(cvSize(50, 50), 8, 3);

  storage = cvCreateMemStorage(0);
  cvClearMemStorage(storage);

  // Detect all the faces in the greyscale image.
  rects = cvHaarDetectObjects(input, cascade, storage, search_scale_factor, 2, flags, minFeatureSize);
  if(rects->total != 1){
    printf("More than one faces are captured or no faces captured");
    exit(1);
  }

  CvRect *r = (CvRect*)cvGetSeqElem(rects, 0);

  bbox[0] = r->x;
  bbox[1] = r->y;
  bbox[2] = r->x + r->width;
  bbox[3] = r->y + r->height;

  //if (r->width < 150 || r->height < 150) {
  //  continue;
  //}

  flandmark_detect(input, bbox, model, landmarks);

  // display landmarks
  //cvRectangle(orig, cvPoint(bbox[0], bbox[1]), cvPoint(bbox[2], bbox[3]), CV_RGB(255,0,0) );
  //cvRectangle(orig, cvPoint(model->bb[0], model->bb[1]), cvPoint(model->bb[2], model->bb[3]), CV_RGB(0,0,255) );
  //cvCircle(orig, cvPoint((int)landmarks[0], (int)landmarks[1]), 3, CV_RGB(0, 0,255), CV_FILLED);
  for (int i = 2; i < 2*model->data.options.M; i += 2)
  {
      //cvCircle(orig, cvPoint(int(landmarks[i]), int(landmarks[i+1])), 3, CV_RGB(255,0,0), CV_FILLED);

  }

  /*
  if (nFaces > 0)
  {
      printf("Faces detected: %d; Detection of facial landmark on all faces took %d ms\n", nFaces, ms);
  } else {
      printf("NO Face\n");
  }
  */

  cvReleaseMemStorage(&storage);
  free(bbox);
  cvReleaseHaarClassifierCascade(&cascade);
  flandmark_free(model);

  return landmarks;
}


// for DEBUG
/*
int main(int argc, const char *argv[])
{
  printf("I am testing the Landmarks function~!\n");
  IplImage* img = cvLoadImage(argv[1]);
  cvNamedWindow("testing", 0);
  cvShowImage("testing", img);
  cvWaitKey(0);
  
  double* img_lm;
  int i;
  img_lm = GetLandmarks(img);
  for (i = 0; i < 16; i++) {
   printf("landmarks: %f ", img_lm[i]) ;
   if (i % 2 == 1) {
     printf("\n");
   }
  }
  cvCircle(img, cvPoint((int)img_lm[0], (int)img_lm[1]), 3, CV_RGB(0, 0,255), CV_FILLED);
  for (int i = 2; i < 16; i += 2)
  {
    cvCircle(img, cvPoint(int(img_lm[i]), int(img_lm[i+1])), 3, CV_RGB(255,0,0), CV_FILLED);
  }
  
  cvNamedWindow("testing2", 0);
  cvShowImage("testing2", img);
  cvWaitKey(0);

  cvDestroyWindow("testing2");
  cvDestroyWindow("testing");
  cvReleaseImage(&img);

  return 0;
}
*/
