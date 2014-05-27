#include "mean_shift.h"
#include <cmath>

using namespace std;

Rect MeanShiftTracker::MeanShift(Mat probImg, Rect track_window)
{
	int h_roi = track_window.height;
	int w_roi = track_window.width;

	for(int iterate_count  = 0;iterate_count<10;iterate_count++){

		// relative coordinate
		int old_x = track_window.width/2;
		int old_y = track_window.height/2;

		Mat roi = probImg(track_window);

		float numerator_x = 0.0, denominator_x=0.0;
		float numerator_y = 0.0, denominator_y=0.0;

		for(int row = 0; row < h_roi; ++row){
			for(int col = 0; col < w_roi; ++col){

				int weight = roi.at<unsigned char>(row,col);
				//printf("weight:%d\n",weight);

				float kernel_value_x = std_gaussian_1d(abs(row - old_x));
				numerator_x += row*kernel_value_x;
				denominator_x +=  kernel_value_x;

				float kernel_value_y = std_gaussian_1d(abs(col - old_y));
				numerator_y += row*kernel_value_y;
				denominator_y +=  kernel_value_y;
			}
		}

		//printf("(A/B):(%f,%f)\n",numerator_y,denominator_y);
		int delta_x = (int)(numerator_x/denominator_x);
		int delta_y = (int)(numerator_y/denominator_y);
		//printf("(%f,%f)\n",delta_x,delta_y);

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

float MeanShiftTracker::std_gaussian_1d(float x)
{
	return exp(-0.5*pow(x,2));
}
