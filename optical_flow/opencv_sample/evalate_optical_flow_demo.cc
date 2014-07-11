#include <iostream>
#include <fstream>

#include "opencv2/video/tracking.hpp"
#include "opencv2/legacy/legacy.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace cv;
using namespace std;

inline bool isFlowCorrect(Point2f u)
{
	return !cvIsNaN(u.x) && !cvIsNaN(u.y) && fabs(u.x) < 1e9 && fabs(u.y) < 1e9;
}

static Vec3b computeColor(float fx, float fy)
{
	static bool first = true;

	// relative lengths of color transitions:
	// these are chosen based on perceptual similarity
	// (e.g. one can distinguish more shades between red and yellow
	//  than between yellow and green)
	const int RY = 15;
	const int YG = 6;
	const int GC = 4;
	const int CB = 11;
	const int BM = 13;
	const int MR = 6;
	const int NCOLS = RY + YG + GC + CB + BM + MR;
	static Vec3i colorWheel[NCOLS];

	if (first)
	{
		int k = 0;

		for (int i = 0; i < RY; ++i, ++k)
			colorWheel[k] = Vec3i(255, 255 * i / RY, 0);

		for (int i = 0; i < YG; ++i, ++k)
			colorWheel[k] = Vec3i(255 - 255 * i / YG, 255, 0);

		for (int i = 0; i < GC; ++i, ++k)
			colorWheel[k] = Vec3i(0, 255, 255 * i / GC);

		for (int i = 0; i < CB; ++i, ++k)
			colorWheel[k] = Vec3i(0, 255 - 255 * i / CB, 255);

		for (int i = 0; i < BM; ++i, ++k)
			colorWheel[k] = Vec3i(255 * i / BM, 0, 255);

		for (int i = 0; i < MR; ++i, ++k)
			colorWheel[k] = Vec3i(255, 0, 255 - 255 * i / MR);

		first = false;
	}

	const float rad = sqrt(fx * fx + fy * fy);
	const float a = atan2(-fy, -fx) / (float)CV_PI;

	const float fk = (a + 1.0f) / 2.0f * (NCOLS - 1);
	const int k0 = static_cast<int>(fk);
	const int k1 = (k0 + 1) % NCOLS;
	const float f = fk - k0;

	Vec3b pix;

	for (int b = 0; b < 3; b++)
	{
		const float col0 = colorWheel[k0][b] / 255.f;
		const float col1 = colorWheel[k1][b] / 255.f;

		float col = (1 - f) * col0 + f * col1;

		if (rad <= 1)
			col = 1 - rad * (1 - col); // increase saturation with radius
		else
			col *= .75; // out of range

		pix[2 - b] = static_cast<uchar>(255.f * col);
	}

	return pix;
}

static void drawOpticalFlow(const Mat_<Point2f>& flow, Mat& dst, float maxmotion = -1)
{
	dst.create(flow.size(), CV_8UC3);
	dst.setTo(Scalar::all(0));

	// determine motion range:
	float maxrad = maxmotion;

	if (maxmotion <= 0)
	{
		maxrad = 1;
		for (int y = 0; y < flow.rows; ++y)
		{
			for (int x = 0; x < flow.cols; ++x)
			{
				Point2f u = flow(y, x);

				if (!isFlowCorrect(u))
					continue;

				maxrad = max(maxrad, sqrt(u.x * u.x + u.y * u.y));
			}
		}
	}

	for (int y = 0; y < flow.rows; ++y)
	{
		for (int x = 0; x < flow.cols; ++x)
		{
			Point2f u = flow(y, x);

			if (isFlowCorrect(u))
				dst.at<Vec3b>(y, x) = computeColor(u.x / maxrad, u.y / maxrad);
		}
	}
}

// binary file format for flow data specified here:
// http://vision.middlebury.edu/flow/data/
static void writeOpticalFlowToFile(const Mat_<Point2f>& flow, const string& fileName)
{
	static const char FLO_TAG_STRING[] = "PIEH";

	ofstream file(fileName.c_str(), ios_base::binary);

	file << FLO_TAG_STRING;

	file.write((const char*) &flow.cols, sizeof(int));
	file.write((const char*) &flow.rows, sizeof(int));

	for (int i = 0; i < flow.rows; ++i)
	{
		for (int j = 0; j < flow.cols; ++j)
		{
			const Point2f u = flow(i, j);

			file.write((const char*) &u.x, sizeof(float));
			file.write((const char*) &u.y, sizeof(float));
		}
	}
}

// Adapt from D:\OpenCV\opencv-2.4.6.1\modules\legacy\test\test_optflow.cpp
Mat copnvert2flow(const Mat& velx, const Mat& vely)
{
	Mat flow(velx.size(), CV_32FC2);
	for(int y = 0 ; y < flow.rows; ++y)
		for(int x = 0 ; x < flow.cols; ++x)
			flow.at<Point2f>(y, x) = Point2f(velx.at<float>(y, x), vely.at<float>(y, x));
	return flow;
}

void calcOpticalFlowBM( const Mat& prev, const Mat& curr, Size bSize, Size shiftSize, Size maxRange, int usePrevious, Mat& flow )
{
	Size sz((curr.cols - bSize.width + shiftSize.width)/shiftSize.width, (curr.rows - bSize.height + shiftSize.height)/shiftSize.height);
	Mat velx(sz, CV_32F), vely(sz, CV_32F);

	CvMat cvvelx = velx;    CvMat cvvely = vely;
	CvMat cvprev = prev;    CvMat cvcurr = curr;
	cvCalcOpticalFlowBM( &cvprev, &cvcurr, bSize, shiftSize, maxRange, usePrevious, &cvvelx, &cvvely);
	flow = copnvert2flow(velx, vely);
}


int main(int argc, const char* argv[])
{
	//if (argc < 3)
	//{
	//	cerr << "Usage : " << argv[0] << "<frame0> <frame1> [<output_flow>]" << endl;
	//	return -1;
	//}

	// Read images
	Mat frame0 = imread("../CMake/prev.png");
	Mat frame1 = imread("../CMake/next.png");
	//VideoCapture capture("../../mean-shift/CMake/mean_shift_video.avi");
	//Mat frame0;
	//Mat frame1;
	//int framecount = 0;
	//int pos = 100;
	//while(1){
	//	printf("framecount:%d\n",framecount);
	//	if(framecount++ == pos){
	//		printf("-------------\n");
	//		capture >> frame0;
	//		imwrite("A.jpg",frame0);
	//	}
	//	if(framecount++ == (pos + 221)){
	//		printf("-*-*-*-*-*-*-*\n");
	//		capture >> frame1;
	//		imwrite("B.jpg",frame1);
	//		break;
	//	}
	//}

	//return 0;

	// Check images
	if (frame0.empty())
	{
		cerr << "frame0 is emtpy" << "]" << endl;
		return -1;
	}
	if (frame1.empty())
	{
		cerr << "frame1 is emtpy" << "]" << endl;
		return -1;
	}

	if (frame1.size() != frame0.size())
	{
		cerr << "Images should be of equal sizes" << endl;
		return -1;
	}

	// Calculate gray images
	Mat frame0_gray, frame1_gray;
	cvtColor(frame0, frame0_gray, CV_BGR2GRAY);
	cvtColor(frame1, frame1_gray, CV_BGR2GRAY);



	// Calculate optical flow
	Mat_<Point2f> flow;

	int selected = 2;
	if(1 == selected){

		//************** BM method ******************//
		const double start = (double)getTickCount();
		calcOpticalFlowBM( frame0_gray, frame1_gray, Size(15, 15), Size(1, 1), Size(15, 15), 0, flow);
		const double timeSec = (getTickCount() - start) / getTickFrequency();
		cout << "calcOpticalFlowBM : " << timeSec << " sec" << endl;
	}
	else if(2 == selected){

		//************** Farneback method ******************//
		const double start = (double)getTickCount();
		calcOpticalFlowFarneback(frame0_gray, frame1_gray, flow, 0.5, 3, 15, 3, 5, 1.2, 0);
		const double timeSec = (getTickCount() - start) / getTickFrequency();
		cout << "calcOpticalFlowFarneback : " << timeSec << " sec" << endl;
	}
	else if(3 == selected){

		//************** Simple flow method ******************//
		if (frame0.type() != 16 || frame1.type() != 16) {
			printf("Images should be of equal type CV_8UC3\n");
			exit(1);
		}
		const double start = (double)getTickCount();
		calcOpticalFlowSF(frame0, frame1,
				flow,
				3,   // layers
				2,   // averaging_block_size
				4,   // max flow
				4.1, // sigma_dist
				25.5,// sigma_color
				18,  // postprocess_windows
				55.0,// sigma_dist_fix
				25.5,// sigma_color_fix
				0.35,// occ_thr
				18,  // upscale_averaging_radius
				55.0, //upscale_sigma_dist
				25.5, //upscale_sigma_color
				10);  //speed_up_thr
		const double timeSec = (getTickCount() - start) / getTickFrequency();
		cout << "calcOpticalFlowSF : " << timeSec << " sec" << endl;
	}
	else if(4 == selected){

		//************** DualTVL1 method ******************//
		Ptr<DenseOpticalFlow> tvl1 = createOptFlow_DualTVL1();
		const double start = (double)getTickCount();
		tvl1->calc(frame0_gray, frame1_gray, flow);
		const double timeSec = (getTickCount() - start) / getTickFrequency();
		cout << "calcOpticalFlowDual_TVL1 : " << timeSec << " sec" << endl;
	}
	else{
		printf("??????????????????????????????????????????\n");
	}

	// Show flow with color map
	Mat out;
	drawOpticalFlow(flow, out);
	imshow("Flow", out);
	waitKey();
	// <DEBUG> save images [7/4/2014 Leo.Li]
	imwrite("flow2.jpg", out);

	// Write flow if needed
	if (argc == 4)
		writeOpticalFlowToFile(flow, argv[3]);

	return 0;
}
