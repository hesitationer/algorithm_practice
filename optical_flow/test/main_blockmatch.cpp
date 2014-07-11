// Farneback dense optical flow calculate and show in Munsell system of colors
// Author : Zouxy
// Date   : 2013-3-15
// HomePage : http://blog.csdn.net/zouxy09
// Email  : zouxy09@qq.com

// API calcOpticalFlowFarneback() comes from OpenCV, and this
// 2D dense optical flow algorithm from the following paper:
// Gunnar Farneback. "Two-Frame Motion Estimation Based on Polynomial Expansion".
// And the OpenCV source code locate in ..\opencv2.4.3\modules\video\src\optflowgf.cpp

#include <iostream>
#include <time.h>
#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;

#define UNKNOWN_FLOW_THRESH 1e9

// Color encoding of flow vectors from:
// http://members.shaw.ca/quadibloc/other/colint.htm
// This code is modified from:
// http://vision.middlebury.edu/flow/data/
void makecolorwheel(vector<Scalar> &colorwheel)
{
	int RY = 15;
	int YG = 6;
	int GC = 4;
	int CB = 11;
	int BM = 13;
	int MR = 6;

	int i;

	for (i = 0; i < RY; i++) colorwheel.push_back(Scalar(255,	   255*i/RY,	 0));
	for (i = 0; i < YG; i++) colorwheel.push_back(Scalar(255-255*i/YG, 255,		 0));
	for (i = 0; i < GC; i++) colorwheel.push_back(Scalar(0,		   255,		 255*i/GC));
	for (i = 0; i < CB; i++) colorwheel.push_back(Scalar(0,		   255-255*i/CB, 255));
	for (i = 0; i < BM; i++) colorwheel.push_back(Scalar(255*i/BM,	   0,		 255));
	for (i = 0; i < MR; i++) colorwheel.push_back(Scalar(255,	   0,		 255-255*i/MR));
}

void motionToColor(Mat flow, Mat &color)
{
	if (color.empty())
		color.create(flow.rows, flow.cols, CV_8UC3);

	static vector<Scalar> colorwheel; //Scalar r,g,b
	if (colorwheel.empty())
		makecolorwheel(colorwheel);

	// determine motion range:
	float maxrad = -1;

	// Find max flow to normalize fx and fy
	for (int i= 0; i < flow.rows; ++i) 
	{
		for (int j = 0; j < flow.cols; ++j) 
		{
			Vec2f flow_at_point = flow.at<Vec2f>(i, j);
			float fx = flow_at_point[0];
			float fy = flow_at_point[1];
			if ((fabs(fx) >  UNKNOWN_FLOW_THRESH) || (fabs(fy) >  UNKNOWN_FLOW_THRESH))
				continue;
			float rad = sqrt(fx * fx + fy * fy);
			maxrad = maxrad > rad ? maxrad : rad;
		}
	}

	printf("%.2f\n", maxrad);
	maxrad = 50;

	for (int i= 0; i < flow.rows; ++i) 
	{
		for (int j = 0; j < flow.cols; ++j) 
		{
			uchar *data = color.data + color.step[0] * i + color.step[1] * j;
			Vec2f flow_at_point = flow.at<Vec2f>(i, j);

			float fx = flow_at_point[0] / maxrad;
			float fy = flow_at_point[1] / maxrad;
			if ((fabs(fx) >  UNKNOWN_FLOW_THRESH) || (fabs(fy) >  UNKNOWN_FLOW_THRESH))
			{
				data[0] = data[1] = data[2] = 0;
				continue;
			}
			float rad = sqrt(fx * fx + fy * fy);

			float angle = atan2(-fy, -fx) / CV_PI;
			float fk = (angle + 1.0) / 2.0 * (colorwheel.size()-1);
			int k0 = (int)fk;
			int k1 = (k0 + 1) % colorwheel.size();
			float f = fk - k0;
			//f = 0; // uncomment to see original color wheel

			for (int b = 0; b < 3; b++) 
			{
				float col0 = colorwheel[k0][b] / 255.0;
				float col1 = colorwheel[k1][b] / 255.0;
				float col = (1 - f) * col0 + f * col1;
				if (rad <= 1)
					col = 1 - rad * (1 - col); // increase saturation with radius
				else
					col *= .75; // out of range
				data[2 - b] = (int)(255.0 * col);
			}
		}
	}
}

int BlockSum(unsigned char* p, int step, int w, int h)
{
	int sum = 0;
	for(int i=0; i<h; i++){
		for(int j=0; j<w; j++){
			sum += (*p++);
		}
		p += (step-w);
	}
	return sum;
}

int BlockDiff(unsigned char* p1, int step1, 
			  unsigned char* p2, int step2,
			  int w, int h)
{
	unsigned char *tp1,*tp2;
	int sumDiff = 0;
	for(int i=0; i<h; i++){
		tp1 = p1;
		tp2 = p2;
		for(int j=0; j<w; j++){
			sumDiff += abs(*tp1 - *tp2);
			tp1++;
			tp2++;
		}
		p1 += step1;
		p2 += step2;
	}
	return sumDiff;
}

static Mat A_integral = Mat::zeros(240,320,CV_32FC1); 
static Mat B_integral = Mat::zeros(240,320,CV_32FC1);
//A_integral.create(320,240,CV_32FC1);
//B_integral.create(320,240,CV_32FC1);
int BlockDiff_v_integrate(int A_x, int A_y,
						  int B_x, int B_y,
						  int w, int h)// block size
{
	if(A_x < 0 || A_y < 0 || A_x + w >= 320 || A_y + h >= 240 ||
	   B_y < 0 || B_y < 0 || B_x + w >= 320 || B_y +h >= 240){

		printf("out of bound\n");
		return 0;
	}

	int sum_A = 0;
	int sum_B = 0;
	if(A_y == 0 && A_x == 0){
		
		sum_A = A_integral.at<int>(w,h);
	}
	else{
		sum_A = A_integral.at<int>(A_y - 1,A_x - 1) + 
				A_integral.at<int>(A_y + h - 1, A_x + w - 1) -
				A_integral.at<int>(A_y + h - 1, A_x) - 
				A_integral.at<int>(A_y, A_x + w - 1);
	}

		sum_B = B_integral.at<int>(B_y - 1,B_x - 1) + 
				B_integral.at<int>(B_y + h - 1, B_x + w - 1) -
				B_integral.at<int>(B_y + h - 1, B_x) - 
				B_integral.at<int>(B_y, B_x + w - 1);

	return abs(sum_B - sum_A);
}

int search(int B_x, int B_y, // current block left_top
		int s_w, int s_h, // search region in prev frame
		int b_w, int b_h, // block size for diff
		   int *tr, int *tc) // the output flow
{
	int minDiff = 65536;
	for(int i = -s_h/2; i < s_h/2; ++i){
		for(int j = -s_w/2; j < s_w/2; ++j){
			int diff = (int)BlockDiff_v_integrate(B_x + j, B_y + i,
											 B_x,B_y,
											 b_w,b_h);
			if(diff <= minDiff){
				minDiff = diff;
				*tr = i;
				*tc = j;
			}
		}
	}
	
	return 0;
}

typedef struct{
	int x;
	int y;
}offset_t;

const int SMALL_DIFF = 2;
int blWidth = 16;
int blHeight = 16;
int blSize = blWidth*blHeight;
float acceptlevel = blSize * SMALL_DIFF;

void search_spiral(unsigned char* pCurrBlock, 
				   int row, int col,
				   int bw, int bh,
				   unsigned char* pPrev, 
				   int ww, int wh,
				   int w, int h,
				   int* tr, int *tc,
				   offset_t *offset_table, // record the offset of point to search
				   int table_len)// the length of offset-table
{
	int minDiff = 65536;
	int step = w; // the width of image

	int samePosDiff = BlockDiff(pCurrBlock, step,
			pPrev + row*w + col, step, bw,bh);


	int ss_count = table_len;
	offset_t *ss = offset_table;

	int countMin = 1;
	int sumx = 0, sumy = 0;

	if(samePosDiff > acceptlevel){

		for(int i = 0 ; i < ss_count; i++){

			int dx = ss[i].x;
			int dy = ss[i].y;
			int Y = row + dy;
			int X = col + dx;
			int diff = BlockDiff(pCurrBlock, step, 
					pPrev+Y*step+X, step, bw, bh);

			if( diff < acceptlevel )
			{
				sumx = dx; sumy = dy;
				countMin = 1;
				break;
			}

			if( diff < samePosDiff )
			{
				samePosDiff = diff;
				sumx = dx; sumy = dy;
				countMin = 1;
			}
			else if( diff == samePosDiff )
			{
				sumx += dx; sumy += dy;
				countMin++;
			}

		}// ss_count
	}// samePosDiff

	*tc = sumx/countMin;
	*tr = sumy/countMin;
}

int SearchDisp(unsigned char* pCurrBlock, 
		int row, int col,
		int bw, int bh,
		unsigned char* pPrev, 
		int ww, int wh,
		int w, int h,
		int* tr, int *tc)
{
	int minDiff = 65536;
	for(int i=-wh/2; i<wh/2; i++){
		for(int j=-ww/2; j<ww/2; j++){
			int diff = BlockDiff(pCurrBlock, w, 
					pPrev+(row+i)*w+(col+j), w, bw, bh);
			if(diff <= minDiff){
				minDiff = diff;
				*tr = i;
				*tc = j;
			}
		}
	}

	return 0;
}


int integral_buffer(unsigned char *data, int *dst, int w, int h)
{
	dst[0] = data[0];
	for(int i = 1; i < w; i++){
		dst[i] = data[i] + dst[i - 1];
	}
	for(int j = 1; j < h; j++){
		dst[j*w] = data[j*w] + dst[(j-1)*w];
	}

	for(int i = 1; i < h; i++){
		for(int j = 1; j < w; j++){

			dst[i*w + j] =  data[i*w + j] + 
				dst[i*w + (j-1)] + 
				dst[(i-1)*w + j]  -
				dst[(i-1)*w + (j-1)];
		}
	}

	return 0;
}

void get_offset_table(int sw, // width of search region
		int sh, // height of search region 
		offset_t* ss)// len(ss) = (sw + 1)*(sh + 1))
{
	// w/h --> radius
	sw = sw/2;
	sh = sh/2;
	// Calculate scanning scheme
	int min_count = std::min(sw, sh);
	int ss_count = 0;

	// use spiral search pattern
	//
	//     9 10 11 12
	//     8  1  2 13
	//     7  *  3 14
	//     6  5  4 15
	//... 20 19 18 17
	//

	int i, j;
	for( i = 0; i < min_count; i++ )
	{
		// four cycles along sides
		int x = -i-1, y = x;

		// upper side
		for( j = -i; j <= i + 1; j++, ss_count++ )
		{
			ss[ss_count].x = ++x;
			ss[ss_count].y = y;
		}

		// right side
		for( j = -i; j <= i + 1; j++, ss_count++ )
		{
			ss[ss_count].x = x;
			ss[ss_count].y = ++y;
		}

		// bottom side
		for( j = -i; j <= i + 1; j++, ss_count++ )
		{
			ss[ss_count].x = --x;
			ss[ss_count].y = y;
		}

		// left side
		for( j = -i; j <= i + 1; j++, ss_count++ )
		{
			ss[ss_count].x = x;
			ss[ss_count].y = --y;
		}
	}

	// the rest part
	if( sw < sh )
	{
		int xleft = -min_count;

		// cycle by neighbor rings
		for( i = min_count; i < sh; i++ )
		{
			// two cycles by x
			int y = -(i + 1);
			int x = xleft;

			// upper side
			for( j = -sw; j <= sw; j++, ss_count++, x++ )
			{
				ss[ss_count].x = x;
				ss[ss_count].y = y;
			}

			x = xleft;
			y = -y;
			// bottom side
			for( j = -sw; j <= sw; j++, ss_count++, x++ )
			{
				ss[ss_count].x = x;
				ss[ss_count].y = y;
			}
		}
	}
	else if( sw > sh )
	{
		int yupper = -min_count;

		// cycle by neighbor rings
		for( i = min_count; i < sw; i++ )
		{
			// two cycles by y
			int x = -(i + 1);
			int y = yupper;

			// left side
			for( j = -sh; j <= sh; j++, ss_count++, y++ )
			{
				ss[ss_count].x = x;
				ss[ss_count].y = y;
			}

			y = yupper;
			x = -x;
			// right side
			for( j = -sh; j <= sh; j++, ss_count++, y++ )
			{
				ss[ss_count].x = x;
				ss[ss_count].y = y;
			}
		}
	}
}

int demo_main()
{
	VideoCapture cap;
	//cap.open(0);
	cap.open("../../mean-shift/CMake/mean_shift_video.avi");

	if(!cap.isOpened()){
		return -1;
	}


	int bw = 16, bh = 16;
	int ww = 48, wh = 48; // search region
	int w = 320, h = 240;
	int table_len = (ww + 1)*(wh + 1); // 24 -- 1 -- 24
	offset_t *search_table = (offset_t*)malloc(table_len*sizeof(offset_t));
	get_offset_table(ww,wh,search_table);

	Mat currFrame, preFrame, tmpFrame, showFrame, subFrame;
	clock_t start, end;
	for(;;){
		cap >> tmpFrame;
		resize(tmpFrame, showFrame, cvSize(320, 240), 0, 0, 0);
		cvtColor(showFrame, currFrame, CV_BGR2GRAY);
		cvtColor(showFrame, subFrame, CV_BGR2GRAY);

		if(preFrame.data)
		{
			int tr = 0, tc = 0;

			for(int i=0; i<h; i++){
				for(int j=0; j<w; j++){
					*(subFrame.data+i*w+j) = 
						abs(*(currFrame.data+i*w+j) - *(preFrame.data+i*w+j));
				}
			}

			//start = clock();
			const double start = (double)getTickCount();

			// caculate the integral-image
			integral_buffer(preFrame.data,(int*)A_integral.data,w,h);
			integral_buffer(currFrame.data,(int*)B_integral.data,w,h);
			for(int i=wh/2; i<h-wh/2; i+=bh){
				for(int j=ww/2; j<w-ww/2; j+=bw){
					// 					if(BlockSum(subFrame.data+i*w+j, w, bw,bh) < 5*bw*bh){
					// 						tr = 0;
					// 						tc = 0;
					// 					}else
					{
						int use = 1;
						if(1 == use){
							SearchDisp(currFrame.data+i*w+j, i, j, bw, bh,
								preFrame.data, ww, wh, w, h, &tr, &tc);
						}
						if(2 == use){
							search_spiral(currFrame.data+i*w+j,i,j,bw,bh,
									preFrame.data,ww,wh,w,h,&tr,&tc,
									search_table,
									table_len);
						}
							

					}
					line(showFrame, cvPoint(j, i), cvPoint(j+tc, i+tr),
							cvScalar(0,0,255), 2, 2);
				}
			}
			//end = clock();
			const double timeSec = (getTickCount() - start) / getTickFrequency();
			printf("execution time:%f [s]\n",timeSec);

		}

		imshow("curr", currFrame);
		imshow("Z-search", showFrame);
		imshow("sub", subFrame);

		resize(currFrame, preFrame, cvSize(320, 240), 0, 0, 0);
		char c = waitKey(33);
		printf("%d\n",c);
		if(c == 27){
			printf("break?\n");
			break;
		}
	}
}
// int main(int, char**)
// {
// 	VideoCapture cap;
// 	cap.open(0);
// 	//cap.open("test_02.wmv");
// 
// 	if( !cap.isOpened() )
// 		return -1;
// 
// 	Mat prevgray, gray, flow, cflow, frame, subframe;
// 	namedWindow("flow", 1);
// 
// 	Mat motion2color, motion2color2;
// 
// 	for(;;)
// 	{
// 		double t = (double)cvGetTickCount();
// 
// 		cap >> frame;
// 		resize(frame, subframe, cvSize(320, 240), 0, 0, 0);
// 		cvtColor(subframe, gray, CV_BGR2GRAY);
// 		imshow("original", frame);
// 
// 		if( prevgray.data )
// 		{
// 			calcOpticalFlowFarneback(prevgray, gray, flow, 0.5, 3, 15, 3, 5, 1.2, 0);
// 			motionToColor(flow, motion2color);
// 			resize(motion2color, motion2color2, cvSize(640, 480), 0, 0, 0);
// 			imshow("flow", motion2color2);
// 		}
// 		if(waitKey(10)>=0)
// 			break;
// 		std::swap(prevgray, gray);
// 
// 		t = (double)cvGetTickCount() - t;
// 		cout << "cost time: " << t / ((double)cvGetTickFrequency()*1000.) << endl;
// 	}
// 	return 0;
// }
