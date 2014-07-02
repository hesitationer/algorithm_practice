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

	Rect temp(floor(left_top_x) - 1,floor(left_top_y) - 1,roi_w + 2,roi_h + 2);
	Mat roi_int = src(temp);

	double dx = (double)now_center.x - floor(now_center.x);
	dx = fmax(dx,0.0001f);
	double dy = (double)now_center.y - floor(now_center.y);

	int k = 0;
	for(int i = 0; i < dst_roi.rows; ++i){
		for(int j = 0; j < dst_roi.cols; ++j,k++){

			double p00 = (double)roi_int.at<unsigned char>(i,j);
			double p01 = (double)roi_int.at<unsigned char>(i,j + 1);
			double p10 = (double)roi_int.at<unsigned char>(i + 1,j);
			double p11 = (double)roi_int.at<unsigned char>(i + 1,j + 1);

			// two parts: prev_col + next_col
			double prev = (1 - dx)*((1-dy)*p00 + dy * p10);
			double next =      dx *((1-dy)*p01 + dy * p11);
			printf("prev + next[%d]: %f\n",k,prev + next);
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
	double coeff = 1./(radius*radius);
	Mat mask = Mat::zeros(roi_h,roi_w,CV_32FC1);

	float mask_x[11];
	for(int i = -radius, k = 0; i <= radius; i++,k++){
		mask_x[k] = (float)exp(-i*i*coeff); 
	}
	float *mask_y = mask_x;

	for(int i = 0; i < roi_h; ++i){
		for(int j = 0; j <= roi_w; ++j){

			float a = mask_x[j] * mask_y[i];
			mask.at<float>(i,j) = a;
		}
	}

	// calculate subpixle

	Mat roi;
	roi.create(roi_h + 2,roi_w + 2,CV_32FC1);
	int count = 0;
	Point_<float> now_center(raw_pt.x,raw_pt.y);
	float n_x = raw_pt.x, n_y = raw_pt.y;
	while(true){

		// get the sub_rect of now-center
		now_center.x = n_x;
		now_center.y = n_y;
		get_roi_in_subpixel_form(gray_img,roi, now_center,
				radius,radius);

		// sobel to get devrivate(gradient)
		Mat Dx, Dy;
		Dx.create(roi_h,roi_w,CV_32FC1);
		Dy.create(roi_h,roi_w,CV_32FC1);
		for(int i = 0; i < Dx.rows; ++i){
			for(int j = 0; j < Dx.cols; ++j){
				
				Dx.at<float>(i,j) = roi.at<float>(i + 1, j + 2) - roi.at<float>(i + 1,j);
				Dy.at<float>(i,j) = roi.at<float>(i + 2, j + 1) - roi.at<float>(i,j + 1);
			}
		}

		// get the dxx,dxy,dyy
		double dx,dy,dxx = 0.,dxy = 0.,dyy = 0.,px,py;
		double a, b, c, bb1,bb2;
		a = b = c = bb1 = bb2 = 0.;
		for(int i = 0; i < roi_h; ++i){

			py = i - radius;
			for(int j = 0; j < roi_w; ++j){

				double m = mask.at<float>(i,j);

				dx = Dx.at<float>(i,j);
				dy = Dy.at<float>(i,j);
				dxx = dx*dx*m;
				dxy = dx*dy*m;
				dyy = dy*dy*m;

				px = j - radius;

				a += dxx;
				b += dxy;
				c += dyy;

				bb1 += dxx*px + dxy*py;
				bb2 += dxy*px + dyy*py;
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
		double det = a*c - b*b;

		float temp_x = 0.0;
		float temp_y = 0.0;
		if( fabs(det) > DBL_EPSILON*DBL_EPSILON){

			double scale = 1./det;
			temp_x = (float)(n_x + c*scale*bb1 - b*scale*bb2);
			temp_y = (float)(n_y - b*scale*bb1 + a*scale*bb2);
			printf("update temp_x,temp_y\n");
		}
		else{
			temp_x = n_x;
			temp_y = n_y;
			printf("det is %f\n",det);
			printf("--------\n");
		}


		// chance to break
		double err = (temp_x - n_x)*(temp_x - n_x) + (temp_y - n_y)*(temp_y - n_y);
		// update n_x, n_y
		n_x = temp_x;
		n_y = temp_y;
		if(err < EPS || ++count > 10){
			printf("break with err is %f and count is %d\n",err,count);
			break;
		}
	}// while(true)

	Point_<float> result;
	printf("(%d,%d)-->(%f,%f)\n",raw_pt.x,raw_pt.y,n_x,n_y);
	if(fabs(n_x - raw_pt.x) <= radius || fabs(n_y - raw_pt.y) <= radius){

		result.x = n_x;	
		result.y = n_y;	
	}
	else{
		result.x = raw_pt.x;	
		result.y = raw_pt.y;	
	}

	printf("(%d,%d)-->(%f,%f)\n",raw_pt.x,raw_pt.y,result.x,result.y);

	waitKey(0);
	return result;
}
