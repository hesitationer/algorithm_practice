#include "opencv2/opencv.hpp"

using namespace cv;

const float EPS = 1.0e-6;

// This function fill roi with the value:
//
//   x = (1 - dx) * [(1-dy)*p00 + dy*p10]
//   y =    dx    * [(1-dy)*p01 + dy*p11]
//
//   in which:
//   dx = center.x - floor(center.x)
//   dy = center.x - floor(center.y)
//
//
//  Four  ---> One
//  | p00 p01 ...|             | P00  ...   |
//  | 			 |             | 			|
//  | p10 p11 ...|   ----->    |     ...    |
//  |			 |             |			|
//  | ...        |             |     ...    |
int get_roi_in_subpixel_form(Mat &src, Mat &dst_roi, Point_<float> now_center, 
		int radius_w, int radius_h)
{
	int roi_w = 2*radius_w + 1;
	int roi_h = 2*radius_h + 1;

	double left_top_x = now_center.x - radius_w;
	double left_top_y = now_center.y - radius_h;

	Rect temp(left_top_x,left_top_y,roi_w + 1,roi_h + 1);
	Mat roi_int = src(temp);
	dst_roi.create(roi_h,roi_w,CV_32FC1);

	double dx = now_center.x - floor(now_center.x);
	double dy = now_center.y - floor(now_center.y);

	for(int i = 0; i < roi_h; ++i){
		for(int j = 0; j < roi_w; ++j){

			int p00 = roi_int.at<int>(i,j);
			int p01 = roi_int.at<int>(i,j + 1);
			int p10 = roi_int.at<int>(i + 1,j);
			int p11 = roi_int.at<int>(i + 1,j + 1);

			// two parts: prev_col + next_col
			double prev = (1 - dx)*((1-dy)*p00 +      dy *p10);
			double next =      dx *(   dy *p01 + (1 - dy)*p11);
			dst_roi.at<float>(i,j) = prev + next;
		}
	}

	return 0;
}

Point_<float> find_subpixels(Mat &src,Mat &gray_img, Point_<int> raw_pt)
{
	int radius = 5;
	int roi_w = 2*5 + 1;
	int roi_h = 2*5 + 1;

	// mark the given point
	circle(src,raw_pt,4,CV_RGB(255,0,0),-1,8,0);

	// input check
	imshow("input color",src);
	imshow("input gray",gray_img);

	// make mask for roi
	float coeff = 1./(radius*radius);
	Mat mask = Mat::zeros(roi_h,roi_w,CV_32FC1);
	for(int i = -radius; i <= radius; ++i){
		for(int j = -radius; j <= radius; ++j){

			float a = (float)exp(-i*i*coeff)*exp(-j*j*coeff);
			mask.at<float>(j + radius,i + radius) = a;
		}
	}

	// calculate subpixle

	Mat roi;
	int count = 0;
	float n_x = 0., n_y = 0.;
	while(true){

		Point_<float> now_center(raw_pt.x,raw_pt.y);
		// get the sub_rect of now-center
		float roi_x = now_center.x - radius;
		float roi_y = now_center.y - radius;
		//get_roi_in_subpixel_form(gray_img,roi, now_center,
		//		radius,radius);
		roi = gray_img(Rect(roi_x,roi_y,roi_w,roi_h));

		// sobel to get devrivate(gradient)
		Mat Dx, Dy;
		Sobel(roi,Dx,CV_32F,1,0);
		Sobel(roi,Dy,CV_32F,0,1);

		// get the dxx,dxy,dyy
		float dx,dy,dxx = 0.,dxy = 0.,dyy = 0.,px,py;
		float b0 = 0.,b1 = 0.;
		for(int i = 0; i < roi.rows; ++i){

			py = i - radius;
			for(int j = 0; j < roi.cols; ++j){

				dx = Dx.at<float>(j,i);
				dy = Dy.at<float>(j,i);

				double m = mask.at<float>(j,i);
				dxx += dx*dx*m;
				dxy += dx*dy*m;
				dyy += dy*dy*m;

				px = j - radius;

				b0 += dx*dx*px + dx*dy*py;
				b1 += dx*dy*px + dy*dy*py;
			}
		}

		// get the A^-1
		// (A^-1) = (A^*) / (det)
		//
		//
		//		|  dxx   dxy |
		// A  = |		     |
		// 		|  dxy   dyy |
		//
		//
		//		|  dyy   -dxy |
		// A^* =|			  |
		// 		| -dxy    dxx |
		//
		float det = dxx*dyy - dxy*dxy;
		float scale = 1./det;

		float bb0 =  dyy*scale*b0 - dxy*scale*b1;
		float bb1 = -dxy*scale*b0 + dxx*scale*b1;


		// resolve to get new center
		n_x = raw_pt.x + bb0;
		n_y = raw_pt.y + bb1;

		now_center.x = n_x;
		now_center.y = n_y;

		if((bb0*bb0 + bb1*bb1) < EPS){
			break;
		}
		if( ++count > 10){
			break;
		}
	}

	Point_<float> sub_pixel_point(n_x,n_y);

	printf("(%d,%d)-->(%f,%f)\n",raw_pt.x,raw_pt.y,n_x,n_y);

	waitKey(0);
	return sub_pixel_point;
}
