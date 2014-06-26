#include "opencv2/opencv.hpp"

using namespace cv;

const float quality_level = 0.1;

int find_corners(Mat &image)
{
	int width = image.cols;
	int height = image.rows;

	Mat diff_x  = Mat::zeros(height,width,CV_32FC1); 
	Mat diff_y  = Mat::zeros(height,width,CV_32FC1); 

	Mat diff_xx = Mat::zeros(height,width,CV_32FC1); 
	Mat diff_yy = Mat::zeros(height,width,CV_32FC1);
	Mat diff_xy = Mat::zeros(height,width,CV_32FC1); 

	Mat corner_map = Mat::zeros(height,width,CV_32FC1);

	// range: [1, w-1], [1, h - 1]
	for(int i = 1; i < height - 1; ++i){
		for(int j =1; j < width - 1; ++j){
			diff_x.at<float>(i,j) =  image.at<unsigned char>(i,j + 1) - image.at<unsigned char>(i,j-1);
			diff_y.at<float>(i,j) =  image.at<unsigned char>(i + 1,j) - image.at<unsigned char>(i - 1,j);
		}
	}

	for(int i = 1; i < height - 1; ++i){
		for(int j = 1; j < width - 1; ++j){
			diff_xx.at<float>(i,j) = pow(diff_x.at<float>(i,j),2);
			diff_yy.at<float>(i,j) = pow(diff_y.at<float>(i,j),2);

			diff_xy.at<float>(i,j) = diff_x.at<float>(i,j) * diff_y.at<float>(i,j);
		}
	}


	// smooth the diff_xx, diff_yy, diff_xy
	// Gaussian  model
	//
	// boxFilter:
	//
	//    1 1 1
	//    1 1 1
	//    1 1 1
	boxFilter(diff_xx,diff_xx,diff_xx.depth(), Size(2,2),
			Point(-1,-1),false,BORDER_DEFAULT);
	boxFilter(diff_yy,diff_yy,diff_yy.depth(), Size(2,2),
			Point(-1,-1),false,BORDER_DEFAULT);
	boxFilter(diff_xy,diff_xy,diff_xy.depth(), Size(2,2),
			Point(-1,-1),false,BORDER_DEFAULT);
	

	// calcHarris
	float max = 0.0;
	for(int i = 1; i < height - 1; ++i){
		for(int j = 1; j < width - 1; ++j){

			float h00 = diff_xx.at<float>(i,j);
			float h01 = diff_xy.at<float>(i,j); float h10 = h01;
			float h11 = diff_yy.at<float>(i,j);

			float H_det = h00*h11 - h01*h10;
			float H_trace = h00 + h11;
			float small_eigenvalue = (float)(H_det - H_trace*H_trace*0.04);

			if(small_eigenvalue > max){
				max = small_eigenvalue;
				printf("max:%f\n",max);
			}

			corner_map.at<float>(i,j) = small_eigenvalue;
		}
	}

 	float corner_epsilon = max*quality_level;
	printf("corner_epsilon:%f\n",corner_epsilon);

	// Normalizing
	Mat norm_corner_map, norm_corner_map_scaled;
	normalize(corner_map,norm_corner_map,0,255,NORM_MINMAX,CV_32FC1,Mat());
	convertScaleAbs(norm_corner_map,norm_corner_map_scaled);

	for(int i = 0; i < height - 1; ++i){
		for(int j = 0; j < width - 1; ++j){

			int value = (int)norm_corner_map.at<float>(i,j);
			if(value > 160){
				circle( norm_corner_map_scaled, Point(j,i), 5, CV_RGB(0,255,0), 2, 8, 0 );			
			}
		}
	}

	imshow("diff_x",diff_x);
	imshow("diff_y",diff_y);
	imshow("diff_xx",diff_xx);
	imshow("diff_yy",diff_yy);
	imshow("diff_xy",diff_xy);
	namedWindow("find_corners",CV_WINDOW_NORMAL);
	imshow("dst_norm_scaled",norm_corner_map_scaled);
	waitKey(0);

	return 0;
}
