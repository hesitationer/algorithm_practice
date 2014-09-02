#include "opencv2/opencv.hpp"
#include <vector>
#include <queue>

using namespace std;
using namespace cv;

#define DIS_LEVEL  50

// 8-neighbors
Point N8[8] = {
	{-1, -1}, {0, -1}, {1, -1},

	{-1,  0}, /*   */  {1,  0},

	{-1,  1}, {0,  1}, {1,  1}
};

int cal_pt_dis(unsigned char* pt1, unsigned char* pt2)
{
	return 
		(pt1[0] - pt2[0])*(pt1[0] - pt2[0]) + 
		(pt1[1] - pt2[1])*(pt1[1] - pt2[1]) + 
		(pt1[2] - pt2[2])*(pt1[2] - pt2[2]); 
	
	
}

static vector<Vec3b> colortab;
static bool first_time = true;

int draw_point(Mat &canvas, Point pos, 
		int color_index)// seed_index used to choose color
{

	if(first_time){

		// init color-tab
		for(int i = 0 ; i < 256; i++){
			int b = theRNG().uniform(0,255);
			int g = theRNG().uniform(0,255);
			int r = theRNG().uniform(0,255);

			colortab.push_back(Vec3b((uchar)b,(uchar)g,(uchar)r));
		}

		first_time = false;
	}

	canvas.at<Vec3b>(pos.y,pos.x) = colortab[color_index%256];

	return 0;
}

static int color_index = -1;

int my_watershed(Mat &src, vector<Point> marker)
{
	// assistant data
	Mat flag = Mat::zeros(src.rows, src.cols, CV_8UC1);
	Mat result = Mat::zeros(src.rows, src.cols, src.type());

	queue<Point> queue;

	for(int i = 1; i < src.rows - 1; i++){
		for(int j = 1; j < src.cols - 1; j++){

			// color index++
			color_index++;

			//if(color_index >= src.rows*100)
			//	break;

			// push (i,j) to queue
			if(flag.data[i*flag.step + j] == 1){
				continue;
			}
			else{
				queue.push(Point(j,i));
				flag.data[i*flag.step + j] = 1;
				draw_point(result,Point(j,i),color_index);
			}


			while(!queue.empty()){

				Point temp = queue.front();
				queue.pop();

				// the neighbor
				for(int i = 0 ; i < 8; i++){

					// skip 1-pix bound
					if(temp.x == 0 || temp.y == 0 || temp.x == src.cols ||
							temp.y == src.rows){
						continue;
					}
					// have been checked
					if(flag.data[(temp.y+N8[i].y)*flag.step +
							(temp.x+N8[i].x)] == 1){
						continue;
					}

					// mark or skip
					Point N_i(temp.x + N8[i].x, temp.y + N8[i].y);
					unsigned char* C_ptr = src.data + temp.y*src.step + temp.x*3; //center
					unsigned char* N_ptr = src.data + (temp.y + N8[i].y)*src.step + (temp.x + N8[i].x)*3;//neighbor
					int dis = cal_pt_dis(C_ptr,N_ptr);
					if(dis <= DIS_LEVEL){

						queue.push(N_i);
						flag.data[(temp.y+N8[i].y)*flag.step + (temp.x+N8[i].x)] = 1; // this line!!!
						draw_point(result,N_i,color_index);
					}
				}// 8-neighbors
			}// while(!queue.empty())

		}
	}

	// show result
	imshow("result",result);

	return 0;
}
