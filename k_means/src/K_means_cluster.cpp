#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;

int find_min_dis(Point2f pt, Mat &centers){

	int L = -1;
	float min_dis = 1 << 30;
	for(int i = 0; i < centers.rows; i++){
		
		Point2f pt_f = centers.at<Point2f>(i); 
		float x = pt_f.x;
		float y = pt_f.y;

		//cout<<"pt:("<<pt.x<<","<<pt.y<<")"<<endl;
		//cout<<"center:("<<x<<","<<y<<")"<<endl;
		float dis = (x - pt.x)*(x - pt.x) + (y - pt.y)*(y - pt.y);
		//cout<<"dis: "<<dis<<endl;
		if(dis < min_dis){

			min_dis = dis;
			L = i;
		}
	}

	return L;
}

int K_means_cluster(Mat &src, int K,  Mat &labels, Mat &centers)
{
	// init centers
	RNG rng(12345);
	centers = Mat::zeros(K,2,CV_32F);
	for(int i = 0; i < K; i++){

		centers.at<Point2f>(i) = Point2f(rng.uniform(1,500),rng.uniform(1,500));
	}

	// init labels
	labels = Mat::zeros(src.rows,1,CV_32F);

	int iterate_times = 0;
	while(1){

		int C_x[10] = {0};
		int C_y[10] = {0};
		int C_c[10] = {0};
		for(int i = 0; i < src.rows; i++){

			Point2f pt = src.at<Point2f>(i); 
			int L = find_min_dis(pt,centers);
			labels.at<int>(i) = L;


			C_c[L]++ ;C_x[L] += pt.x;C_y[L] += pt.y; 
			
		}	

		// compare and update center
		Mat C = centers.clone();
		float sum_diff = 0.0;
		for(int i = 0; i < K; i++){
			
			if(C_c[i] == 0){continue;}
			float x = C_x[i]/C_c[i];
			float y = C_y[i]/C_c[i];
			C.at<Point2f>(i) = Point2f(x,y);

			float x_ = centers.at<Point2f>(i).x;
			float y_ = centers.at<Point2f>(i).y;

			sum_diff += (x - x_)*(x - x_) + (y - y_)*(y - y_);

			// update
			centers.at<Point2f>(i) = Point2f(x,y);
		}

		// chance to break;
		if(iterate_times++ > 1000){
			cout<<"iterate_times > 100"<<endl;
			break;
		}
		if(sum_diff < 1e-6){
			cout<<"sum_diff < 1e-6, with iterate_times = "<<iterate_times<<endl;
			break;
		}
	}// while(1)

	return 0;
}
