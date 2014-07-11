#include "pick_up_test_case.h"
#include "opencv2/opencv.hpp"

#include "main_blockmatch.cpp"

using namespace cv;

extern void use_cv_API_BM();

ERIC_TEST(op,block_match)
{
	use_cv_API_BM();
}

ERIC_TEST(op,mat_ch)
{
	Mat m32f(18,18,CV_32F);
	Mat m32fc1(18,18,CV_32FC1);

	printf("m32f.type:%d\n",m32f.channels());
	printf("m32f.type:%d\n",m32fc1.channels());
}

ERIC_TEST(op, demo)
{
	demo_main();
}

ERIC_TEST(op, integral_buffer)
{
	VideoCapture capture;
	capture.open("../../mean-shift/CMake/mean_shift_video.avi");

	Mat img,gray;
	capture >> img;
	capture >> img;
	capture >> img;
	capture >> img;
	capture >> img;
	capture >> img;
	capture >> img;

	cvtColor(img,gray,CV_BGR2GRAY);

	Mat integral = Mat::zeros(img.rows,img.cols,CV_32FC1);

	integral_buffer(gray.data,(int*)integral.data,img.cols,img.rows);

	int w = img.cols;
	for(int count = 0; count < 5; ++count){
		int rand_x = (rand() + 319)%320;
		int rand_y = (rand() + 239)%240;
		//int rand_x = 0;
		//int rand_y = 0;
		int step = 2;
		printf("start_point:%d,%d\n",rand_x,rand_y);

		for(int i = rand_x; i < rand_x + step; ++i){
			for(int j = rand_y; j < rand_y + step; ++j){

				printf("%d ",gray.data[i*w + j]);
			}
			printf("\n");
		}

		printf("\n");

		int idx = 0;
		long int ret = 0;
		for(int i = rand_x; i < rand_x + step; ++i){
			for(int j = rand_y; j < rand_y + step; ++j){

				int *temp = (int*)integral.data;
				printf("%d ",temp[i*w + j]);

				if(idx == 0){
					ret += temp[i*w + j];
				}
				if(idx == 1){
					ret -= temp[i*w + j];
				}
				if(idx == 2){
					ret -= temp[i*w + j];
				}
				if(idx == 3){
					ret += temp[i*w + j];
				}

				idx++;
				
			}
			printf("\n");
		}

		printf("result:%d\n",ret);
		printf("\n");
	}
}

ERIC_TEST(op, integral_v1)
{
	printf("sizeof(long int):%d\n",sizeof(long int));
	Mat input = Mat::zeros(5,5,CV_8UC1);
	for(int i = 0; i < 5; i++){
		for(int j = 0; j < 5; j++){

			int temp = (rand() + 9)%10;
			input.at<unsigned char>(i,j) = temp;
		}
	}

	Mat result = Mat::zeros(5,5,CV_32FC1);
	integral_buffer(input.data,(int*)result.data,5,5);

	// output
	for(int i = 0; i < 5; ++i){
		for(int j = 0; j < 5; ++j){
			printf("%d ", input.at<unsigned char>(i,j));
		}
		printf("\n");
	}

	printf("\n");

	for(int i = 0; i < 5; ++i){
		for(int j = 0; j < 5; ++j){
			printf("%d ", result.at<int>(i,j));
		}
		printf("\n");
	}
}
