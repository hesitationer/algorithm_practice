#include "opencv2/opencv.hpp"

using namespace cv;

const float quality_level = 0.1;

int find_corners(Mat &image)
{
	int width = image.cols;
	int height = image.rows;

	Mat show_corners = Mat::zeros(height,width,CV_8UC3);

	Mat diff_x  = Mat::zeros(height,width,CV_8UC1); 
	Mat diff_y  = Mat::zeros(height,width,CV_8UC1); 

	Mat diff_xx = Mat::zeros(height,width,CV_16UC1); 
	Mat diff_yy = Mat::zeros(height,width,CV_16UC1);
	Mat diff_xy = Mat::zeros(height,width,CV_16UC1); 

	Mat corner_map = Mat::zeros(height,width,CV_16UC1);

	// range: [1, w-1], [1, h - 1]
	for(int i = 1; i < height - 1; ++i){
		for(int j =1; j < width - 1; ++j){
			*diff_x.ptr(i,j) =  *image.ptr(i,j + 1) - *image.ptr(i,j-1);
			*diff_y.ptr(i,j) =  *image.ptr(i + 1,j) - *image.ptr(i - 1,j);
		}
	}

	for(int i = 1; i < height - 1; ++i){
		for(int j = 1; j < width - 1; ++j){
			*(unsigned short*)diff_xx.ptr(i,j) = pow(*diff_x.ptr(i,j),2);
			*(unsigned short*)diff_yy.ptr(i,j) = pow(*diff_y.ptr(i,j),2);

			*(unsigned short*)diff_xy.ptr(i,j) = (*diff_x.ptr(i,j)) * ( *diff_y.ptr(i, j));
		}
	}

	// Gaussian  model
	//
	//   1  2  1
	//   2  4  2
	//   1  2  1
	unsigned short max = 0;
	for(int i = 1; i < height - 1; ++i){
		for(int j = 1; j < width - 1; ++j){

			Mat H = Mat::zeros(2,2,CV_16UC1);

			// size of scan window is 3
			unsigned short h00 = *(unsigned short*)H.ptr(0,0) = 
			   (*(unsigned short*)diff_xx.ptr(i - 1,j)*2 + *(unsigned short*)diff_xx.ptr(i - 1,j - 1)*1 + *(unsigned short*)diff_xx.ptr(i - 1,j+1)*1 +
				*(unsigned short*)diff_xx.ptr(i + 0,j)*4 + *(unsigned short*)diff_xx.ptr(i + 0,j - 1)*2 + *(unsigned short*)diff_xx.ptr(i + 0,j+1)*2 +
				*(unsigned short*)diff_xx.ptr(i + 1,j)*2 + *(unsigned short*)diff_xx.ptr(i + 1,j - 1)*1 + *(unsigned short*)diff_xx.ptr(i + 1,j+1)*1)/16.0;

			unsigned short h01 = *(unsigned short*)H.ptr(0,1) = 
			   (*(unsigned short*)diff_xy.ptr(i - 1,j)*2 + *(unsigned short*)diff_xy.ptr(i - 1,j - 1)*1 + *(unsigned short*)diff_xy.ptr(i - 1,j+1)*1 +
				*(unsigned short*)diff_xy.ptr(i + 0,j)*4 + *(unsigned short*)diff_xy.ptr(i + 0,j - 1)*2 + *(unsigned short*)diff_xy.ptr(i + 0,j+1)*2 +
				*(unsigned short*)diff_xy.ptr(i + 1,j)*2 + *(unsigned short*)diff_xy.ptr(i + 1,j - 1)*1 + *(unsigned short*)diff_xy.ptr(i + 1,j+1)*1)/16.0;

			//float h10 = *H.ptr(1,0) = (float)
			//	(*diff_xy.ptr(i - 1,j)*2 + *diff_xy.ptr(i - 1,j - 1)*1 + *diff_xy.ptr(i - 1,j+1)*1 +
			//	*diff_xy.ptr(i + 0,j)*4 + *diff_xy.ptr(i + 0,j - 1)*2 + *diff_xy.ptr(i + 0,j+1)*2 +
			//	*diff_xy.ptr(i + 1,j)*2 + *diff_xy.ptr(i + 1,j - 1)*1 + *diff_xy.ptr(i + 1,j+1)*1)/16;
			
			unsigned short h10 = h01;

			unsigned short h11 = *(unsigned short*)H.ptr(1,1) = 
			   (*(unsigned short*)diff_yy.ptr(i - 1,j)*2 + *(unsigned short*)diff_yy.ptr(i - 1,j - 1)*1 + *(unsigned short*)diff_yy.ptr(i - 1,j+1)*1 +
				*(unsigned short*)diff_yy.ptr(i + 0,j)*4 + *(unsigned short*)diff_yy.ptr(i + 0,j - 1)*2 + *(unsigned short*)diff_yy.ptr(i + 0,j+1)*2 +
				*(unsigned short*)diff_yy.ptr(i + 1,j)*2 + *(unsigned short*)diff_yy.ptr(i + 1,j - 1)*1 + *(unsigned short*)diff_yy.ptr(i + 1,j+1)*1)/16.0;

			//printf("hxx:%f,%f,%f,%f\n",h00,h01,h10,h11);

			unsigned short H_det = h00*h11 - h01*h10;
			unsigned short H_trace = h00 + h11;
			unsigned short small_eigenvalue = (unsigned short)((float)H_det - H_trace*H_trace*0.4);

			if(small_eigenvalue > max){
				max = small_eigenvalue;
				printf("max:%d\n",max);
			}

			*(unsigned short*)corner_map.ptr(i,j) = small_eigenvalue;
		}
	}

 	unsigned short corner_epsilon = max*quality_level;
	printf("corner_epsilon:%d\n",corner_epsilon);

	for(int i = 0; i < height - 1; ++i){
		for(int j = 0; j < width - 1; ++j){
			unsigned short value = corner_map.at<unsigned short>(i,j);
			//printf("value:%d\n",value);
			if(value > corner_epsilon){
				circle( image, Point(j,i), 1, CV_RGB(0,255,0), -1 );			
				circle( show_corners, Point(j,i), 1, CV_RGB(0,255,0), 1 );			
			}
		}
	}


	imshow("diff_x",diff_x);
	imshow("diff_y",diff_y);
	imshow("diff_xx",diff_xx);
	imshow("diff_yy",diff_yy);
	imshow("diff_xy",diff_xy);
	imshow("show_corners",show_corners);
	namedWindow("find_corners",CV_WINDOW_NORMAL);
	imshow("find_corners",image);
	waitKey(0);

	return 0;
}
