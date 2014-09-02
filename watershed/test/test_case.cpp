#include "pick_up_test_case.h"
#include "opencv2/opencv.hpp"
#include "time.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wwrite-strings"
#pragma GCC diagnostic ignored "-Wunused-variable"

using namespace std;
using namespace cv;

int my_watershed(Mat &src, std::vector<Point>);

ERIC_TEST(watershed, watershed)
{
	Mat src;
#if 0
	src.create(512,512,CV_8UC3);	
	int src_index = 2;
	if(src_index == 1){
		src.data[256*512*3 + 256*3 + 0] = 255;
		src.data[256*512*3 + 256*3 + 1] = 0;
		src.data[256*512*3 + 256*3 + 2] = 0;
	}
	if(src_index == 2){
		for(int i = 0; i < 16; i++){
			for(int j = 0; j < 16; j++){

				src.data[(256+i)*512*3 + (256+j)*3 + 0] = 255;
				src.data[(256+i)*512*3 + (256+j)*3 + 1] = 0;
				src.data[(256+i)*512*3 + (256+j)*3 + 2] = 0;
			}
		}
	}
#else
	src = imread("../CMake/fruits.jpg");
#endif

	imshow("src",src);
	//waitKey(0);



	vector<Point> marker;
	marker.push_back(Point(256,256));


	clock_t start,end;
	start = clock();
	my_watershed(src,marker);
	end = clock();	
	printf("execution time: %f [ms]\n",(end - start)*1000./CLOCKS_PER_SEC);
	waitKey(0);
}

int main(int argc,char* argv[])
{
	RUN_ERIC_CASE(argc,argv);
	return 0;
}
