#include "pick_up_test_case.h"
#include "mean_shift.h"
#include "opencv2/opencv.hpp"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wwrite-strings"
#pragma GCC diagnostic ignored "-Wunused-variable"

using namespace cv;

Mat image;

bool selectObject = false;
Point origin;
int trackObject = 0;
Rect selection;
int vmin = 10, vmax = 256, smin = 30;
static void onMouse(int event, int x, int y, int, void*)
{
	if(selectObject){

		selection.x = MIN(x,origin.x);
		selection.y = MIN(y,origin.y);
		selection.width  = std::abs(x - origin.x);
		selection.height = std::abs(y - origin.y);

		selection &= Rect(0, 0, image.cols, image.rows);
	}

	switch(event){
		case CV_EVENT_LBUTTONDOWN:
			origin = Point(x,y);
			selection = Rect(x,y,0,0);
			selectObject = true;
			break;
		case CV_EVENT_LBUTTONUP:
			selectObject = false;
			if( selection.width > 0 && selection.height > 0){

				trackObject = -1;
				break;
			}
	}
}

ERIC_TEST(mean_shift, tracking)
{
	MeanShiftTracker t;

	Mat frame;
	Rect track_window(0,0,55,15);
	int hsize = 16;
	float hranges[] = {0,180};
	const float* phranges = hranges;
	//VideoCapture capture(0);
	VideoCapture capture("../CMake/mean_shift_video.avi");
	
	//capture.open(0);

	if(!capture.isOpened()){
		printf("open camera failed!\n");
		return;
	}

	// set mouse callback
	namedWindow("mean_shift", CV_WINDOW_AUTOSIZE);
	setMouseCallback("mean_shift", onMouse, 0);

	bool paused = false;
	Mat hsv, hue, mask, hist, histimg = Mat::zeros(200,320,CV_8UC3), backproj;
	Mat roi, maskroi;
	while(1){

		// capture frame
		if(!paused){
			capture >> frame;
		}

		frame.copyTo(image);

		if(!paused){

			cvtColor(image,hsv,COLOR_BGR2HSV);
			imshow("hsv",hsv);


			if(trackObject){

				int _vmin = vmin, _vmax = vmax;
				inRange(hsv, Scalar(0,smin,MIN(_vmin,_vmax)),
						Scalar(180,256,MAX(_vmin,_vmax)), mask);
				imshow("mask",mask);

				int from_to[] = {0,0};
				hue.create(hsv.size(),hsv.depth());
				mixChannels(&hsv, 1, &hue, 1, from_to, 1);



				if(trackObject < 0){// update selection

					track_window = selection;
					trackObject = 1;

					roi = hue(selection);
					maskroi = mask(selection);
					calcHist(&roi, 1, 0, maskroi, hist, 1, &hsize, &phranges);
					normalize(hist,hist,0,255,CV_MINMAX);
					imshow("roi",roi);

					//draw the hist
					histimg = Scalar::all(0);
                    int binW = histimg.cols / hsize;
                    Mat buf(1, hsize, CV_8UC3);
                    for( int i = 0; i < hsize; i++ )
                        buf.at<Vec3b>(i) = Vec3b(saturate_cast<uchar>(i*180./hsize), 255, 255);
                    cvtColor(buf, buf, CV_HSV2BGR);

                    for( int i = 0; i < hsize; i++ )
                    {
                        int val = saturate_cast<int>(hist.at<float>(i)*histimg.rows/255);
                        rectangle( histimg, Point(i*binW,histimg.rows),
                                   Point((i+1)*binW,histimg.rows - val),
                                   Scalar(buf.at<Vec3b>(i)), -1, 8 );
                    }

					imshow("histimg",histimg);
				}// if(trackObject < 0)

				// prepare probability image
				calcBackProject(&hue, 1, 0, hist, backproj, &phranges);
				backproj &= mask;
				imshow("backproj",backproj);

				// mean-shift
				
				clock_t start, end;
				start = clock();
				track_window = t.MeanShift(backproj,track_window);
				end = clock();
				printf("mean-shift time:%f[ms]\n",(float)(end - start)*1000/CLOCKS_PER_SEC);
			}
		}// if(!paused)
		else if( trackObject < 0){

			paused = false;
		}

		rectangle(frame,Point(track_window.x,track_window.y),
				Point(track_window.x + track_window.width,
					track_window.y + track_window.height),
				CV_RGB(0,255,0));
		imshow("mean_shift",frame);

		// chance to break
		char c = waitKey(33);
		if(c == 27){
			break;
		}
		if(c == 'p'){
			paused = !paused;
		}

	}// while(1)
}

// This example shows that: 
//       Mat.ptr() 
// is much intuitive than
//       Mat.at()
ERIC_TEST(mean_shift, mat_ptr)
{
	Mat frame;
	VideoCapture capture(0);

	capture.open(0);


	capture >> frame;
	capture >> frame;
	capture >> frame;
	capture >> frame;
	capture >> frame;

	imshow("cur frame",frame);
	waitKey(0);


	for(int i = 0; i < 5; ++i){
		for(int j = 0; j < 5; ++j){

			int data0 = frame.ptr(i,j)[0];
			int data1 = frame.ptr(i,j)[1];
			int data2 = frame.ptr(i,j)[2];
			printf("frame(%d,%d):(%d %d %d)  ", i,j,data0,data1,data2);
		}
		printf("\n");
	}
}

ERIC_TEST(mean_shift, mat_value)
{
	Mat my_mat_1(Size(3,4),CV_8UC1);
	Mat my_mat_3(Size(3,5),CV_8UC3);

	int count = 0;
	for(int i = 0; i < my_mat_1.rows; ++i){
		for(int j = 0; j < my_mat_1.cols; ++j){
			*my_mat_1.ptr(i,j) = count++;
		}
	}

	for(int i = 0; i < my_mat_1.rows; ++i){
		for(int j = 0; j < my_mat_1.cols; ++j){
			printf("%d ", *my_mat_1.ptr(i,j));
		}
		printf("\n");
	}

	printf("(dims,channels,rows,cols):%d,%d,%d,%d\n",my_mat_1.dims,
			my_mat_1.channels(),
			my_mat_1.rows,
			my_mat_1.cols);

	printf("(dims,channels,rows,cols):%d,%d,%d,%d\n",my_mat_1.dims,
			my_mat_3.channels(),
			my_mat_3.rows,
			my_mat_3.cols);
}

ERIC_TEST(mean_shift, variance)
{
	Mat empty = Mat::zeros(24,20,CV_8UC1);

	MeanShiftTracker t;
	float s_x = 0, s_y = 0;
	t.caculate_variance(empty, s_x, s_y);

	printf("%f,%f\n",s_x,s_y);
}

ERIC_TEST(mean_shift,weighted_variance)
{
	Mat diag_mat = Mat::zeros(5,6,CV_8UC1);

	int w = diag_mat.cols;
	int h = diag_mat.rows;

	for(int i = 0; i < h; ++i){
		for(int j = 0; j < w; ++j){
			if((rand() % 20) > 10){
				*diag_mat.ptr(i,j) = 1;
			}
			printf("%2d ", *diag_mat.ptr(i,j));
		}
		printf("\n");
	}

	MeanShiftTracker t;
	t.weighted_sample_variance(diag_mat, 0, 0);
	printf("s_x,s_y:%f,%f\n",t.sigma_x,t.sigma_y);// compare with the result of np.cov() :-P
}

ERIC_TEST(mean_shift, mat_type)
{
	printf("%d\n",CV_8UC1);
	printf("%d\n",CV_8UC2);
	printf("%d\n",CV_8UC3);
	printf("%d\n",CV_8UC4);


	printf("%d\n",CV_8SC1);
	printf("%d\n",CV_8SC2);
	printf("%d\n",CV_8SC3);
	printf("%d\n",CV_8SC4);


	printf("%d\n",CV_16UC1);
	printf("%d\n",CV_16UC2);
	printf("%d\n",CV_16UC3);
	printf("%d\n",CV_16UC4);
}

ERIC_TEST(mean_shift, record_video)
{
	VideoCapture capture(0);
	Mat frame;

	bool record = false;

	cv::VideoWriter writer;

	while(1){
		capture >> frame;

		if(record){

			if(! writer.isOpened()){

				std::cout<<"Frame size:"<<frame.cols<<"x"<<frame.rows <<std::endl;

				if(! writer.open("mean_shift_video.avi",
							CV_FOURCC('X','V','I','D'),
							25,
							frame.size()
							)){

					return;
				}
			}

			// write
			writer.write(frame);
		}

		imshow("frame",frame);

		char c = waitKey(33);
		if(27 == c){
			break;
		}
		else if('s' == c){
			record = !record;
		}

	}
}

#pragma GCC diagnostic pop
