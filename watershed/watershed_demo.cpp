#include "watershed_demo.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <cstdio>
#include <iostream>

#include "watershed.h"

using namespace cv;
using namespace std;
using namespace demo;

void demo::help()
{
	cout << "\nThis program demonstrates the famous watershed segmentation algorithm in OpenCV: watershed()\n"
		"Usage:\n"
		"./watershed [image_name -- default is fruits.jpg]\n" << endl;


	cout << "Hot keys: \n"
		"\tESC - quit the program\n"
		"\tr - restore the original image\n"
		"\tw or SPACE - run watershed segmentation algorithm\n"
		"\t\t(before running it, *roughly* mark the areas to segment on the image)\n"
		"\t  (before that, roughly outline several markers on the image)\n";
}
Mat markerMask, img;
cv::Mat g_my_gray_ ;
cv::Mat g_my_marker_ ;
Point prevPt(-1, -1);

void demo::onMouse( int event, int x, int y, int flags, void* )
{
	if( x < 0 || x >= img.cols || y < 0 || y >= img.rows )
		return;
	if( event == CV_EVENT_LBUTTONUP || !(flags & CV_EVENT_FLAG_LBUTTON) )
		prevPt = Point(-1,-1);
	else if( event == CV_EVENT_LBUTTONDOWN )
		prevPt = Point(x,y);
	else if( event == CV_EVENT_MOUSEMOVE && (flags & CV_EVENT_FLAG_LBUTTON) )
	{
		Point pt(x, y);
		if( prevPt.x < 0 )
			prevPt = pt;
		line( markerMask, prevPt, pt, Scalar::all(255), 5, 8, 0 );
		line( img, prevPt, pt, Scalar::all(255), 5, 8, 0 );
		prevPt = pt;
		imshow("image", img);
		//imshow("markerMask", markerMask);
	}
}

int demo::test_flooding_watershed(std::string filename)
{
	if(filename.empty()) filename = "fruits.jpg";
	Mat img0 = imread(filename, 1), imgGray;

	if( img0.empty() )
	{
		cout << "Couldn'g open image " << filename << ". Usage: watershed <image_name>\n";
		return 0;
	}
	help();
	namedWindow( "image", 1 );

	img0.copyTo(img);
	cvtColor(img, markerMask, CV_BGR2GRAY);
	cvtColor(markerMask, imgGray, CV_GRAY2BGR);

	g_my_gray_ = cv::Mat::zeros(img.size(), CV_8UC1);
	cv::cvtColor(img, g_my_gray_, CV_RGB2GRAY);
	auto type_ = g_my_gray_.type();
	auto depth_ = g_my_gray_.depth();
	auto dim_ = g_my_gray_.dims;

	markerMask = Scalar::all(0);
	imshow( "image", img );
	setMouseCallback( "image", onMouse, 0 );

	for(;;)
	{
		int c = waitKey(0);

		if( (uchar)c == 27 )
			break;

		if( (uchar)c == 'r' )
		{
			markerMask = Scalar::all(0);
			img0.copyTo(img);
			imshow( "image", img );
		}

		if( (uchar)c == 'w' || (uchar)c == ' ' )
		{
			int i, j, compCount = 0;
			vector<vector<Point> > contours;
			vector<Vec4i> hierarchy;

			findContours(markerMask, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);

			if( contours.empty() )
				continue;
			Mat markers(markerMask.size(), CV_32S);
			markers = Scalar::all(0);
			int idx = 0;
			for( ; idx >= 0; idx = hierarchy[idx][0], compCount++ )
				drawContours(markers, contours, idx, Scalar::all(compCount+1), -1, 8, hierarchy, INT_MAX);

			if( compCount == 0 )
				continue;

			vector<Vec3b> colorTab;
			for( i = 0; i < compCount; i++ )
			{
				int b = theRNG().uniform(0, 255);
				int g = theRNG().uniform(0, 255);
				int r = theRNG().uniform(0, 255);

				colorTab.push_back(Vec3b((uchar)b, (uchar)g, (uchar)r));
			}
			std::vector<Vec3b> my_colorTab;
			for (int i = 0; i < 256; ++i){
				int b = theRNG().uniform(0, 255);
				int g = theRNG().uniform(0, 255);
				int r = theRNG().uniform(0, 255);

				my_colorTab.push_back(Vec3b((uchar)b, (uchar)g, (uchar)r));
			}


			double t = (double)getTickCount();
			ws::priority_watershed( img0, markers );

			g_my_marker_ = markers.clone();
			std::string s_ = type2str(g_my_marker_.type());
			cv::Mat my_ = ws::priority_flood(g_my_gray_);
			t = (double)getTickCount() - t;
			printf( "execution time = %gms\n", t*1000./getTickFrequency() );

			Mat priority_watershed(markers.size(), CV_8UC3);
			Mat priority_flooding(g_my_marker_.size(), CV_8UC3);

			// paint the watershed image
			for( i = 0; i < markers.rows; i++ ){
				for( j = 0; j < markers.cols; j++ )
				{
					int idx = markers.at<int>(i,j);
					if( idx == -1 ){
						priority_watershed.at<Vec3b>(i,j) = Vec3b(255,255,255);}
					else if( idx <= 0 || idx > compCount ){
						priority_watershed.at<Vec3b>(i,j) = Vec3b(0,0,0);}
					else{
						priority_watershed.at<Vec3b>(i,j) = colorTab[(idx - 1) % compCount];}
				}
			}

			std::vector<int> hh(256, 0);
			for( i = 0; i < my_.rows; i++ ){
				for( j = 0; j < my_.cols; j++ )
				{
					int my_idx = my_.at<uchar>(i,j);
					hh[my_idx]++;
					if( my_idx == -1 ){
						priority_flooding.at<Vec3b>(i,j) = Vec3b(255,255,255);
					}
					else{ 
						priority_flooding.at<Vec3b>(i,j) = my_colorTab[my_idx];
					}
				}
			}

			priority_watershed = priority_watershed*0.5 + imgGray*0.5;
			priority_flooding = priority_flooding*0.5 + imgGray*0.5;
			imshow( "priority-watershed", priority_watershed );
			imshow("priority-flood", priority_flooding);
		}
	}

	return 0;
}

std::string demo::type2str(int type)
{
	string r;

	uchar depth = type & CV_MAT_DEPTH_MASK;
	uchar chans = 1 + (type >> CV_CN_SHIFT);

	switch ( depth ) {
	case CV_8U:  r = "8U"; break;
	case CV_8S:  r = "8S"; break;
	case CV_16U: r = "16U"; break;
	case CV_16S: r = "16S"; break;
	case CV_32S: r = "32S"; break;
	case CV_32F: r = "32F"; break;
	case CV_64F: r = "64F"; break;
	default:     r = "User"; break;
	}

	r += "C";
	r += (chans+'0');

	return r;
}

void demo::test_morphological_watershed(std::string filename)
{
  if (filename.empty()){
    filename = "gradient.png";
  }
  cv::Mat gradient = imread(filename, CV_LOAD_IMAGE_GRAYSCALE);
  ws::morphological_watershed(gradient);
}

