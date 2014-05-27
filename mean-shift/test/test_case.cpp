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
	Mat frame;
	Rect track_window(0,0,55,15);
	int hsize = 16;
	float hranges[] = {0,180};
	const float* phranges = hranges;
	VideoCapture capture(0);
	
	capture.open(0);

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
				//printf("backproj: d,w,h: (%d,%d,%d)\n",backproj.dims,
				//		   	backproj.cols,
				//		   	backproj.rows);
				track_window = MeanShiftTracker::MeanShift(backproj,track_window);
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

	}// while(1)
}

#pragma GCC diagnostic pop
