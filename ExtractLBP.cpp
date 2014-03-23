#include "ExtractLBP.h"

void PatchUniformLBP(const IplImage* src, float* lbp_uniform){
  int i, j, pos=0;
  float max=0, min=src->imageSize;
  IplImage* lbp = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);

  LBP(src, lbp);

  uchar* data = (uchar*)lbp->imageData;
  int step = src->widthStep;
  uchar* temp = (uchar*)malloc(sizeof(uchar) * (src->imageSize));
  memset(temp, 0, sizeof(uchar) * (src->imageSize));
  for (i = 1; i < src->height-1; i++) {
    for (j = 1; j < src->width-1; j++) {
      temp[pos++] = data[i*step+j]; 
    }
  }

  // Histograms! Needed optimization!
  for (i = 0; i < pos; i++) {
    //temp[i]
    for (j = 0; j < 58; j++) {
      if(temp[i] == g_uniform_lbp[j]) {
        lbp_uniform[j]++;
        break;
      }
    }
    if(j == 58)
      lbp_uniform[58]++;
  }

  // Normalize
  for (i = 0; i < 59; i++) {
    if(lbp_uniform[i] > max)
      max = lbp_uniform[i];
  }
  for (i = 0; i < 59; i++) {
    if(lbp_uniform[i] < min)
      min = lbp_uniform[i];
  }
  for (i = 0; i < 59; i++) {
    lbp_uniform[i] = (lbp_uniform[i] - min) / (max - min);
  }

  // Release
  free(temp);
  cvReleaseImage(&lbp);
}


void LBP(const IplImage *src,IplImage *dst)
{
	int tmp[8]={0};
	CvScalar s;

	IplImage * temp = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U,1);
	uchar *data=(uchar*)src->imageData;
	int step=src->widthStep;

	for (int i=1;i<src->height-1;i++)
	  for(int j=1;j<src->width-1;j++)
	  {
		  if(data[(i-1)*step+j-1]>data[i*step+j])
			tmp[0]=1;
		  else
			tmp[0]=0;
		  if(data[i*step+(j-1)]>data[i*step+j])
			tmp[1]=1;
		  else
			tmp[1]=0;
		  if(data[(i+1)*step+(j-1)]>data[i*step+j])
			tmp[2]=1;
		  else
			tmp[2]=0;
		  if (data[(i+1)*step+j]>data[i*step+j])
			tmp[3]=1;
	  else
			tmp[3]=0;
		  if (data[(i+1)*step+(j+1)]>data[i*step+j])
			tmp[4]=1;
		  else
			tmp[4]=0;
		  if(data[i*step+(j+1)]>data[i*step+j])
			tmp[5]=1;
		  else
			tmp[5]=0;
		  if(data[(i-1)*step+(j+1)]>data[i*step+j])
			tmp[6]=1;
		  else
			tmp[6]=0;
		  if(data[(i-1)*step+j]>data[i*step+j])
			tmp[7]=1;
		  else
			tmp[7]=0;	

		  //Caculate LBP code
			s.val[0]=(tmp[0]*1+tmp[1]*2+tmp[2]*4+tmp[3]*8+tmp[4]*16+tmp[5]*32+tmp[6]*64+tmp[7]*128);
		  
      cvSet2D(dst,i,j,s); // Writing to a LBP image
	  }
    cvReleaseImage(&temp);
    //free(data);
}

void MultiScaleLBP(const IplImage* src, float* lbp, int scale, int cell_num_steps){
  int i, j, k, h, pos=0;
  float cell_width_step, cell_height_step;
  float** lbp_temp = NULL;
  CvSize scale_size;
  IplImage* src_scale;
  IplImage* src_tmp;
  lbp_temp = (float**)malloc(sizeof(float*) * cell_num_steps * cell_num_steps * scale);
  for (i = 0; i < (cell_num_steps * cell_num_steps * scale); i++) {
    lbp_temp[i] = (float*)malloc(sizeof(float) * 59);
    memset(lbp_temp[i], 0, sizeof(float) * 59);
  }
  if(scale > 4){
    printf("Sorry, the scale cannot be set more than 4\n");
    exit(1);
  }
  if(cell_num_steps > 4){
    printf("Sorry, the cell_num_steps cannot be set more than 4\n");
    exit(1);
  }
  for (i = 0; i < scale; i++) {
    scale_size.width = src->width / 2;
    scale_size.height = src->height / 2;
    src_scale = cvCreateImage(scale_size, src->depth, 1);
    cvResize(src, src_scale);
    cell_width_step = src_scale->width / cell_num_steps;
    cell_height_step = src_scale->height / cell_num_steps;
    for (j = 0; j < cell_num_steps; j++) {
      for (k = 0; k < cell_num_steps; k++) {
        src_tmp = cvCreateImage(cvSize(cell_width_step, cell_height_step), src_scale->depth, 1);
        cvSetImageROI(src_scale, cvRect(j * cell_width_step, k * cell_height_step, 
                                        MIN(cell_width_step, src_scale->width - j*cell_width_step), 
                                        MIN(cell_height_step, src_scale->height - k*cell_height_step)));
        cvCopy(src_scale, src_tmp, NULL);
        // LBP extracts
        PatchUniformLBP(src_tmp, lbp_temp[i*cell_num_steps*cell_num_steps+j*cell_num_steps+k]);
        cvResetImageROI(src_scale);
        cvReleaseImage(&src_tmp);
        //printf("%d, %d, %d, OK\n", i, j, k);
        for (h = 0; h < 59; h++) {
          lbp[pos++] = lbp_temp[i*cell_num_steps*cell_num_steps+j*cell_num_steps+k][h];
        }
        free(lbp_temp[i*cell_num_steps*cell_num_steps+j*cell_num_steps+k]);
      }
    }
    cvReleaseImage(&src_scale);
  }
  free(lbp_temp);
}

// For DEBUG
/*
int main(int argc, const char *argv[])
{
  
	IplImage* face = cvLoadImage("..//face1.png",CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_ANYCOLOR);
	IplImage* Gray_face = cvCreateImage( cvSize( face->width,face->height ), face->depth, 1);
  cvCvtColor(face, Gray_face ,CV_BGR2GRAY);

  // single patch version DEBUG
  float* lbp = (float*)malloc(sizeof(float) * 59);
  memset(lbp, 0, sizeof(float)*59);
  PatchUniformLBP(Gray_face, lbp);
  for (int i = 0; i < 59; i++) {
    printf("%4f ", lbp[i]);
  }
  printf("\n\n");
  free(lbp);

  // multi-scale version DEBUG
  lbp = (float*)malloc(sizeof(float) * 3 * 3 * 2 * 59);
  MultiScaleLBP(Gray_face, lbp, 2, 3);
  for (int i = 0; i < 18*59; i++) {
    printf("%4f ", lbp[i]);
    if((1+i) % 59 == 0)
      printf("\n\n");
  }
  free(lbp);
}
*/
