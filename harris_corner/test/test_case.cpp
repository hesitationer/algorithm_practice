#include "pick_up_test_case.h"
#include "opencv2/opencv.hpp"

using namespace cv;

extern int find_corners(Mat &image);

ERIC_TEST(harris_corners, find_corners)
{
	//Mat image = imread("../test/corners.bmp");
	Mat image = imread("../test/corners_01.jpg",CV_LOAD_IMAGE_GRAYSCALE);
	//Mat image = imread("../test/corners_01.jpg");
	//imshow("corner",image);
	
	//waitKey(0);
	find_corners(image);
}

ERIC_TEST(harris_corners, float_mat)
{
	Mat float_mat = Mat::zeros(2,2,CV_16UC1);

	float_mat.at<unsigned short>(0,0) = 255*255 + 14;
	float_mat.at<unsigned short>(0,1) = 255*255 + 16;
	float_mat.at<unsigned short>(1,0) = 255*255;
	float_mat.at<unsigned short>(1,1) = 65535;

	// get by ptr
	float h00 = *(unsigned short*)float_mat.ptr(0,0);
	float h01 = *(unsigned short*)float_mat.ptr(0,1);
	float h10 = *(unsigned short*)float_mat.ptr(1,0);
	float h11 = *(unsigned short*)float_mat.ptr(1,1);

	// get by at
	float at00 = float_mat.at<unsigned short>(0,0);
	float at01 = float_mat.at<unsigned short>(0,1);
	float at10 = float_mat.at<unsigned short>(1,0);
	float at11 = float_mat.at<unsigned short>(1,1);

	printf("GetByPtr:%f,%f,%f,%f\n",h00,h01,h10,h11);
	printf("GetByPtr:%f,%f,%f,%f\n",at00,at01,at10,at11);
}
