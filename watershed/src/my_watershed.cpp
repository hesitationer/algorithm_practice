#include "opencv2/opencv.hpp"
#include <vector>
#include <queue>

using namespace std;
using namespace cv;

#define DIS_LEVEL 20*20

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

int draw_point(Mat &canvas, Point pos, 
		int seed_index)// seed_index used to choose color
{

	int w = canvas.step;
	canvas.data[pos.y*w + pos.x*3 + 0] = 0;
	canvas.data[pos.y*w + pos.x*3 + 1] = 0;
	canvas.data[pos.y*w + pos.x*3 + 2] = 255;

	return 0;
}

int my_watershed(Mat &src, vector<Point> marker)
{
	// assistant data
	Mat flag = Mat::zeros(src.rows, src.cols, CV_8UC1);
	Mat result = Mat::zeros(src.rows, src.cols, src.type());
	int seed_index = 0;

	vector<Point>::iterator iter;	
	for(iter = marker.begin(); iter != marker.end(); iter++,seed_index++){

		Point seed = *iter;

		// search from the marker
		queue<Point> queue;
		queue.push(seed);
		draw_point(result,seed,seed_index);


		while(!queue.empty()){

			Point temp = queue.front();
			queue.pop();
			flag.data[temp.y*flag.step + temp.x] = 1;

			// the neighbor
			for(int i = 0 ; i < 8; i++){

				// have been checked
				if(flag.data[(temp.y+N8[i].y)*flag.step + (temp.x+N8[i].x)] == 1){
					continue;
				}

				// mark or skip
				Point N_i(temp.x + N8[i].x, temp.y + N8[i].y);
				unsigned char* C_ptr = src.data + temp.y*src.step + temp.x*3; //center
				unsigned char* N_ptr = src.data + (temp.y + N8[i].y)*src.step + (temp.x + N8[i].x)*3;//neighbor
				int dis = cal_pt_dis(C_ptr,N_ptr);
				if(dis <= DIS_LEVEL){

					queue.push(N_i);
					draw_point(result,N_i,seed_index);
				}
			}// 8-neighbors
		}// while(!queue.empty())
	}// all markers

	// show result
	imshow("result",result);

	return 0;
}
