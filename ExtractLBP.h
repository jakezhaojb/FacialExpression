#ifndef _EXTRACT_LBP_
#define _EXTRACT_LBP_

#include <stdio.h>
#include <stdlib.h>
#include <opencv2/opencv.hpp>
#include <cv.h> 
#include <highgui.h>
#include <cxcore.h>
 
using namespace std;
using namespace cv;
 
void LBP (const IplImage *src, IplImage *dst);
void PatchUniformLBP(const IplImage* src, float* lbp_uniform);
void MultiScaleLBP(const IplImage* src, float* lbp, int scale, int cell_num_steps);

#endif
