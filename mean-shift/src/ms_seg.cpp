#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;


Point_<int> N8[8] = {
	{-1,-1},  {0,-1}, {1,-1},

	{-1, 0},  /*cc*/  {1, 0},

	{-1, 1},  {0, 1}, {1, 1}

}; // 8-neighbors

int cal_rgb_dis_square(unsigned char* p0, unsigned char* p1)
{
	return 
			(p0[0] - p1[0])*(p0[0] - p1[0]) + 
			(p0[1] - p1[1])*(p0[1] - p1[1]) + 
			(p0[2] - p1[2])*(p0[2] - p1[2]);  

}


//This colors the segmentations
static void floodFillPostprocess( Mat& img, const Scalar& colorDiff=Scalar::all(1) )
{
    CV_Assert( !img.empty() );
    RNG rng = theRNG();
    Mat mask( img.rows+2, img.cols+2, CV_8UC1, Scalar::all(0) );
    for( int y = 0; y < img.rows; y++ )
    {
        for( int x = 0; x < img.cols; x++ )
        {
            if( mask.at<uchar>(y+1, x+1) == 0 )
            {
                Scalar newVal( rng(256), rng(256), rng(256) );
                floodFill( img, mask, Point(x,y), newVal, 0, colorDiff, colorDiff );
            }
        }
    }
}

#define RGB_DIS_LEVEL_SQUARE  20*20
void mean_shift_seg(Mat src, Mat dst)
{
	int h = src.rows;
	int w = src.cols;

	int size = 15;
	// Filter
	// skip the 2-pixs bound for (3,3)
	for(int i = size; i < h - size; i++){
		for(int j = size; j < w - size; j++){

			
			unsigned char *center = src.data + i*w*3 + j*3;
			int sum_r = 0, sum_g = 0, sum_b = 0;
			int good_neighbor = 0;

			if(size == 3){
				for(int k = 0; k < 8; k++){

					// k-th neighbor
					unsigned char *Nk = src.data + (i+N8[k].y)*w*3 + (j+N8[k].x)*3; 
					int dis = cal_rgb_dis_square(center,Nk);
					if(dis < RGB_DIS_LEVEL_SQUARE){
						sum_r += Nk[0];sum_g += Nk[1];sum_b += Nk[2];

						good_neighbor++;
					}
				}	
			}else{
				for(int k = -size; k <= size; k++){
					for(int q = -size; q <= size; q++){

						unsigned char *Nk = src.data + (i+k)*w*3 + (j+q)*3; 
						int dis = cal_rgb_dis_square(center,Nk);
						if(dis < RGB_DIS_LEVEL_SQUARE){
							sum_r += Nk[0];sum_g += Nk[1];sum_b += Nk[2];

							good_neighbor++;
						}
					}
				}
			}

			// mean
			unsigned char *dst_center = dst.data + i*w*3 + j*3;
			if(good_neighbor > 0){
				//printf("mean\n");
				dst_center[0] = sum_r/good_neighbor;
				dst_center[1] = sum_g/good_neighbor;
				dst_center[2] = sum_b/good_neighbor;
			}else{
				dst_center[0] = center[0];
				dst_center[1] = center[1];
				dst_center[2] = center[2];
			}
			
		}
	}// for(int i = 2; i < h; i++)

	// color the result
	floodFillPostprocess(dst,Scalar::all(2));

}
