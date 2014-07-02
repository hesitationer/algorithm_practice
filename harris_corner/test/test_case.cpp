#include "pick_up_test_case.h"
#include "opencv2/opencv.hpp"

using namespace cv;

extern int find_corners(Mat &image,std::vector<Point_<int>> &corners);
extern Point_<float> find_subpixels(Mat &src, Mat &image,Point_<int> raw_pt);

ERIC_TEST(harris_corners, find_corners)
{
	Mat image = imread("../test/corners.bmp",CV_LOAD_IMAGE_GRAYSCALE);
	//Mat image = imread("../test/corners_01.jpg",CV_LOAD_IMAGE_GRAYSCALE);
	//Mat image = imread("../test/corners_01.jpg");
	//imshow("corner",image);
	
	//waitKey(0);
	std::vector<Point_<int>> corners;
	find_corners(image,corners);
}


ERIC_TEST(harris_corners,sub_pixel)
{
	Mat src = imread("../test/corners.bmp");
	Mat gray_src;
    cvtColor(src,gray_src,COLOR_BGR2GRAY);	 

	// Expecte output: refine points: (188.921,115.613)
	// Result from opencv demo
	//** Number of corners detected: 10
	// -- raw Corner [0]  (189,115)  	-- Refined Corner [0]  (188.921,115.613)
	// -- raw Corner [1]  (220,62)      -- Refined Corner [1]  (219.417,61.4548)
	// -- raw Corner [2]  (62,60)       -- Refined Corner [2]  (62.5,61.1851)
	// -- raw Corner [3]  (187,220)     -- Refined Corner [3]  (187.542,219.248)
	// -- raw Corner [4]  (285,54)      -- Refined Corner [4]  (284.877,53.9455)
	// -- raw Corner [5]  (211,221)     -- Refined Corner [5]  (212.14,219.474)
	// -- raw Corner [6]  (236,89)      -- Refined Corner [6]  (233.958,92.8847)
	// -- raw Corner [7]  (242,39)      -- Refined Corner [7]  (242.486,39.273)
	// -- raw Corner [8]  (275,81)      -- Refined Corner [8]  (275.117,80.059)
	// -- raw Corner [9]  (49,75)       -- Refined Corner [9]  (47.0021,75.8706)
	// 
	// 
	find_subpixels(src,gray_src,Point_<int>(62,60));

}

ERIC_TEST(harris_corners, float_mat)
{
	Mat short_mat = Mat::zeros(2,2,CV_16UC1);

	short_mat.at<unsigned short>(0,0) = 255*255 + 14;
	short_mat.at<unsigned short>(0,1) = 255*255 + 16;
	short_mat.at<unsigned short>(1,0) = 255*255;
	short_mat.at<unsigned short>(1,1) = 65535;

	// get by ptr
	float h00 = *(unsigned short*)short_mat.ptr(0,0);
	float h01 = *(unsigned short*)short_mat.ptr(0,1);
	float h10 = *(unsigned short*)short_mat.ptr(1,0);
	float h11 = *(unsigned short*)short_mat.ptr(1,1);

	// get by at
	float at00 = short_mat.at<unsigned short>(0,0);
	float at01 = short_mat.at<unsigned short>(0,1);
	float at10 = short_mat.at<unsigned short>(1,0);
	float at11 = short_mat.at<unsigned short>(1,1);

	printf("GetByPtr:%f,%f,%f,%f\n",h00,h01,h10,h11);
	printf("GetByPtr:%f,%f,%f,%f\n",at00,at01,at10,at11);

	Mat float_mat = Mat::zeros(2,2,CV_32FC1);
	float_mat.at<float>(0,0) = 255 - 241;
	float_mat.at<float>(0,1) = 241 - 255;
	int a = (int)float_mat.at<float>(0,1);
	printf("float_mat(0,0):%f\n",float_mat.at<float>(0,0));
	printf("float_mat(0,1):%f\n",float_mat.at<float>(0,1));
	printf("(int)float_mat.at<float>(0,1):%d\n",a);
}

ERIC_TEST(harris_corners, operators_order)
{
	int a = 1;
	a += 5 - 4; // after executing, a is 2

	printf("a is:%d\n",a);
}

ERIC_TEST(harris_corners, float_notation)
{
	float a = 0.,b = .1,c=1e-6;
	printf("a is: %f, \n b is : %f \n", a,b);
	printf("c is: %f\n",c);

	Point_<float> p;
	p.x = 111.11;
	p.y = 222.22;
	printf("p.x is %f, (int)p.y is %d\n",p.x,(int)p.y);
}
