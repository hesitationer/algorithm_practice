#include "mean_shift.h"
#include <cmath>

using namespace std;

MeanShiftTracker::MeanShiftTracker():
	sigma_x(0),
	sigma_y(0)
{
}

Rect MeanShiftTracker::MeanShift(Mat &probImg, Rect track_window)
{
	int h_roi = track_window.height;
	int w_roi = track_window.width;

	caculate_variance(probImg,0,0);

	for(int iterate_count  = 0;iterate_count<10;iterate_count++){

		// relative coordinate
		int old_x = track_window.width/2;
		int old_y = track_window.height/2;

		printf("old_x,old_y:(%d,%d)\n",old_x,old_y);

		Mat roi = probImg(track_window);

		float numerator_x = 0.0, denominator_x=0.0;
		float numerator_y = 0.0, denominator_y=0.0;

		for(int row = 0; row < h_roi; ++row){
			for(int col = 0; col < w_roi; ++col){

				float weight = (int)roi.ptr(row,col);
				printf("weight:%d\n",weight);

				float kernel_value_x = kernel_value(old_x, row, sigma_x);

				numerator_x += row*kernel_value_x*weight;
				denominator_x +=  kernel_value_x;

				float kernel_value_y = kernel_value(old_y, col, sigma_y);
				numerator_y += row*kernel_value_y*weight;
				denominator_y +=  kernel_value_y;
			}
		}

		//printf("(A/B):(%f,%f)\n",numerator_y,denominator_y);
		int kernel_x = (int)(numerator_x/denominator_x);
		int kernel_y = (int)(numerator_y/denominator_y);
		printf("kernel_x/y:(%d,%d)\n",kernel_x,kernel_y);

		int delta_x = kernel_x - old_x;
		int delta_y = kernel_y - old_y;
		printf("delta_x/y:(%d,%d)\n",delta_x,delta_y);

		int n_x = track_window.x + delta_x;
		int n_y = track_window.y + delta_y;

		if(n_x < 0){
			n_x = 0;
		}
		else if(n_x + track_window.width > probImg.cols){
			n_x = probImg.cols - track_window.width;
		}
		if(n_y < 0){
			n_y = 0;
		}
		else if(n_y + track_window.height > probImg.rows){
			n_y = probImg.rows - track_window.height;
		}

		delta_x = n_x - track_window.x;
		delta_y = n_y - track_window.y;
		printf("(d_x,d_y):(%d,%d)\n",delta_x,delta_y);
		// update track-window
		track_window.x += delta_x;
		track_window.y += delta_y;

		// break chance
		if((delta_x*delta_x + delta_y*delta_y ) < 1){
			break;
		}

	}


	return track_window;
}

float MeanShiftTracker::scotts_factor(int N,int dims)
{
	return pow(N,-1./(dims + 4));
}

float MeanShiftTracker::kernel_value(int value, int pos,float sigma)
{

	float factor = 2*sigma*pow(h,2);
	float energy = (pos - value)*(pos - value)/factor;
	float result = exp(-energy);

	float pi = 3.141592653589793;
	float coefficient = sqrt(2*pi*sigma)*h;

	return result/coefficient;
}

void MeanShiftTracker::caculate_variance(Mat &roi, int offset_x, int offset_y)
{
	int w = roi.cols;
	int h = roi.rows;

	printf("w,h:%d,%d\n",w,h);

	// sigma_x
	int sum_x = 0;
	for(int i = 0 + offset_x; i < w + offset_x; i++){
		sum_x += i;
	}

	float mean_x = sum_x / w; 
	printf("mean_x:%d\n",mean_x);

	sigma_x = 0.0;
	for(int i = 0 + offset_x; i < w + offset_x; i++){
		float delta = i - mean_x;
		sigma_x += pow(delta,2);		
	}

	sigma_x = (sigma_x*h) / (w*h-1); // h rows, w*h points

	// sigma_y
	int sum_y = 0;
	for(int i = 0 + offset_y; i < h + offset_y; i++){
		sum_y += i;
	}

	float mean_y = sum_y / h;

	sigma_y = 0.0;
	for(int i = 0 + offset_y; i < h + offset_y; i++){
		float delta = i - mean_y;
		sigma_y += pow(delta,2);		
	}

	sigma_y = (sigma_y*w) / (h*w-1); // w cols, w*h points
}
