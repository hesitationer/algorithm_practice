#include "opencv2/opencv.hpp"
#include "/usr/include/opencv2/legacy/legacy.hpp"

using namespace cv;
void use_cv_API_BM()
{
	// Video
	VideoCapture capture("../../mean-shift/CMake/mean_shift_video.avi");

	if(!capture.isOpened()){
		printf("open camera failed!\n");
		return;
	}

	
	int width  = 640;
	int height = 480;
	CvSize b_size = {16,16}; // block_size
	CvSize s_size = {16,16}; // shift_size
	CvSize search_radius = {64,64}; 

	int vsize_w = (width - b_size.width + s_size.width) / s_size.width;
	int vsize_h = (height - b_size.height + s_size.height) / s_size.height;

	Mat frame;
	Mat A = Mat::zeros(height,width,CV_8UC1);
	Mat B = Mat::zeros(height,width,CV_8UC1);

	int usePrev = 1;

	Mat vx = Mat::zeros(vsize_h,vsize_w,CV_32FC1);
	Mat vy = Mat::zeros(vsize_h,vsize_w,CV_32FC1);

	int framecount = 0;
	while(true){

		//src to gray 
		capture >> frame;
		cvtColor(frame, A, COLOR_BGR2GRAY);


		// treatment for first frame 
		if(0 == framecount){
			B = A.clone();

			framecount++; continue;
		}

		// frame prev and now
		CvMat cvA = CvMat(A), cvB = CvMat(B), cv_vx = CvMat(vx), cv_vy = CvMat(vy);
		cvCalcOpticalFlowBM(&cvA,&cvB,b_size,s_size,search_radius,usePrev,&cv_vx,&cv_vy);	

		// vision vx vy
		imshow("src",Mat(&cvA));
		imshow("vx",Mat(&cv_vx));
		imshow("vy",Mat(&cv_vy));

		// chance to break
		char c = waitKey(33);
		if(27 == c){
			break;
		}

		// update A and count
		B = A;
		framecount++;
	}
}
