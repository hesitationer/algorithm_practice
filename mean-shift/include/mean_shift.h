#ifndef MEAN_SHIFT_H
#define MEAN_SHIFT_H

#include "opencv2/opencv.hpp"

using namespace cv;

class MeanShiftTracker
{
public:
	static Rect MeanShift(Mat probImg, Rect track_window);

	static float std_gaussian_1d(float x);
};
#endif
