#include "ExtractLBP.h"

uchar g_uniform_lbp[58] = {0, 1, 2, 3, 4, 6, 7, 8, 12, 14, 15, 16, 24, 28, 30, 31, 32, 48, \
                          56, 60, 62, 63, 64, 96, 112, 120, 124, 126, 127, 128, 129, 131, \
                          135, 143, 159, 191, 192, 193, 195, 199, 207, 223, 224, 225, 227,\
                          231, 239, 240, 241, 243, 247, 248, 249, 251, 252, 253, 254, 255};
//uchar g_not_uniform_lbp[] = {5,   9,  10,  11,  13,  17,  18,  19,  20,  21,  22,  23,  25,\
                            26,  27,  29,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,\
                            43,  44,  45,  46,  47,  49,  50,  51,  52,  53,  54,  55,  57,\
                            58,  59,  61,  65,  66,  67,  68,  69,  70,  71,  72,  73,  74,\
                            75,  76,  77,  78,  79,  80,  81,  82,  83,  84,  85,  86,  87,\
                            88,  89,  90,  91,  92,  93,  94,  95,  97,  98,  99, 100, 101,\
                           102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 113, 114, 115,\
                           116, 117, 118, 119, 121, 122, 123, 125, 130, 132, 133, 134, 136,\
                           137, 138, 139, 140, 141, 142, 144, 145, 146, 147, 148, 149, 150,\
                           151, 152, 153, 154, 155, 156, 157, 158, 160, 161, 162, 163, 164,\
                           165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177,\
                           178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190,\
                           194, 196, 197, 198, 200, 201, 202, 203, 204, 205, 206, 208, 209,\
                           210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222,\
                           226, 228, 229, 230, 232, 233, 234, 235, 236, 237, 238, 242, 244,\
                           245, 246, 250};

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
  
	IplImage* face = cvLoadImage("src/face1.png",CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_ANYCOLOR);
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
