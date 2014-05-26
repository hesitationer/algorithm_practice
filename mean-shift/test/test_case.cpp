#include "pick_up_test_case.h"
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
	VideoCapture capture(0);
	
	capture.open(0);

	if(!capture.isOpened()){
		printf("open camera failed!\n");
		return;
	}

	// set mouse callback
	namedWindow("mean_shift", 0);
	setMouseCallback("mean_shift", onMouse, 0);

	bool paused = false;
	while(1){

		// capture frame
		if(!paused){
			capture >> frame;
		}

		frame.copyTo(image);

		//if(!paused){
		//	if(trackObject){

		//		printf("mean_shift...\n");
		//	}
		//}
		//else if( trackObject < 0){

		//	paused = false;
		//}


		// prepare probability image

		// mean-shift
		
		// update and draw the bbox
		track_window = selection;
		rectangle(frame,Point(track_window.x,track_window.y),
				Point(track_window.x + track_window.width, track_window.y + track_window.height),
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
