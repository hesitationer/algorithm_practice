#include "pick_up_test_case.h"
#include "opencv2/opencv.hpp"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wwrite-strings"
#pragma GCC diagnostic ignored "-Wunused-variable"

using namespace cv;
using namespace std;

int K_means_cluster(Mat &src, int K,  Mat &labels, Mat &centers);

// test the randShuffle
// 2-D Mat, this function will shuffle by rows && cols
ERIC_TEST(temp,randShuffle)
{
	Mat points = Mat::zeros(5,5,CV_8UC1);
	for(int i = 0; i < 5; i++){

		points.at<char>(0,i) = i + 5;
		points.at<char>(i,0) = i - 5;
	}

	// output
	for(int i = 0; i < 5; i++){
		for(int j = 0; j < 5; j++){

			cout.width(4);
			cout<<(int)points.at<char>(i,j);
		}
		cout<<endl;
	}

	cout<<endl;
	// randShuffle
	RNG rng(123);

	randShuffle(points, 1, &rng);

	// output
	for(int i = 0; i < 5; i++){
		for(int j = 0; j < 5; j++){

			cout.width(4);
			cout<<(int)points.at<char>(i,j);
		}
		cout<<endl;
	}
}

ERIC_TEST(temp, at_point)
{
	Mat points = Mat::zeros(5,2,CV_32F);

	Point pt = points.at<Point>(3);

	cout<<(int)pt.x<<","<<(int)pt.y<<endl;

	points.at<Point>(3)= Point2f(1.1,2.2);

	pt = points.at<Point>(3);
	cout<<(int)pt.x<<","<<(int)pt.y<<endl;
}


ERIC_TEST(temp, accumulate)
{
	std::vector<int> v;
	for(int i = 0; i < 10; i++){
		v.push_back(i);
	}

	int sum = std::accumulate(v.begin(),v.end(),0);

	cout<<sum<<endl;
}

ERIC_TEST(temp, declare_vec)
{
	int a = 10;
	vector<int> vec(a);
	cout<<vec.size()<<endl;
}

ERIC_TEST(temp, Mat_type)
{
	cout<<"32F:"<<CV_32F<<endl;
	cout<<"CV_8UC1:"<<CV_8UC1<<endl;
	cout<<"CV_8UC3:"<<CV_8UC3<<endl;
}

ERIC_TEST(temp, float_cmp)
{
	int a = 1<<30;
	float b = 1234.5;

	cout<<a<<endl;
	cout<<b<<endl;
	if(b < a){
		printf("cmp!\n");
	}

}

ERIC_TEST(temp, init_center)
{
	// init centers
	RNG rng(12345);
	int K = 4;
	Mat centers = Mat::zeros(K,2,CV_32F);
	for(int i = 0; i < K; i++){

		centers.at<Point2f>(i) = Point2f(rng.uniform(1,500) + 0.99,
				rng.uniform(1,500) + 0.88);
	}	

	for(int i = 0; i < K; i++){

		cout<<centers.at<Point2f>(i).x<<","<<centers.at<Point2f>(i).y<<endl;;
	}	
}

// Test the value of:
// 		FLT_MAX
// 		DBL_MAX
// 		float flt = 1.f;
ERIC_TEST(temp, dbl_max)
{
	cout<<"double max is:"<<FLT_MAX<<endl;
	int a = 1<<30;
	if(a > FLT_MAX){
		cout<<"a > DBL_MAX"<<endl;
	}
	else{
		cout<<"a <= DBL_MAX "<<
			"and a is:"<<a
			<<endl;
	}

	float flt = 1.f;
	cout<<"1.f is: "<<flt<<endl;
}

ERIC_TEST(kmeans, cluster)
{

    const int MAX_CLUSTERS = 5;
    Scalar colorTab[] =
    {
        Scalar(0, 0, 255),
        Scalar(0,255,0),
        Scalar(255,100,100),
        Scalar(255,0,255),
        Scalar(0,255,255)
    };

    Mat img(500, 500, CV_8UC3);
    Mat my_img(500, 500, CV_8UC3);
    RNG rng(123);

    for(;;)
    {
        int k, clusterCount = rng.uniform(2, MAX_CLUSTERS+1);
        int i, sampleCount = rng.uniform(1, 1001);
        Mat points(sampleCount, 2, CV_32F), labels, my_labels;

        clusterCount = MIN(clusterCount, sampleCount);
        Mat centers,my_centers;

        /* generate random sample from multigaussian distribution */
        for( k = 0; k < clusterCount; k++ )
        {
            Point center;
            center.x = rng.uniform(0, img.cols);
            center.y = rng.uniform(0, img.rows);
            Mat pointChunk = points.rowRange(k*sampleCount/clusterCount,
                                             k == clusterCount - 1 ? sampleCount :
                                             (k+1)*sampleCount/clusterCount);
            rng.fill(pointChunk, CV_RAND_NORMAL, Scalar(center.x, center.y), Scalar(img.cols*0.05, img.rows*0.05));
        }

        randShuffle(points, 1, &rng);

        kmeans(points, clusterCount, labels,
               TermCriteria( CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 10, 1.0),
               3, KMEANS_PP_CENTERS, centers);

		K_means_cluster(points, clusterCount, my_labels, my_centers);


        img = Scalar::all(0);
        my_img = Scalar::all(0);

        for( i = 0; i < sampleCount; i++ )
        {
            int clusterIdx = labels.at<int>(i);
            Point ipt = points.at<Point2f>(i);
            circle( img, ipt, 2, colorTab[clusterIdx], CV_FILLED, CV_AA );

			//cout<<"ipt:"<<i<<": "<<ipt.x<<" "<<ipt.y<<"---";
			cout<<"labels:"<<i<<": "<<clusterIdx<<"---";

			int id = my_labels.at<int>(i);
			Point pt_i = points.at<Point2f>(i);
			//cout<<"pt_i:"<<i<<": "<<pt_i.x<<" "<<pt_i.y<<endl;
			cout<<"pt_i:"<<i<<": "<<id<<endl;
			circle(my_img, pt_i, 2, colorTab[id], CV_FILLED,CV_AA);
        }

		for(int i = 0; i < clusterCount; i++){

			//Point2f pt_f = centers.at<Point2f>(i);
			Point pt_i = centers.at<Point2f>(i);
			//cout<<"center:"<<pt_f.x<<","<<pt_f.y<<endl;
			//cout<<"center:"<<pt_i.x<<","<<pt_i.y<<endl;
			cout<<endl;
			circle(img,pt_i,7,cv::Scalar(222,134,124));

			Point my_pt = my_centers.at<Point2f>(i);
			circle(my_img,my_pt,7,cv::Scalar(222,134,124));
		}

        imshow("clusters", img);
        imshow("my_clusters", my_img);

        char key = (char)waitKey();
        if( key == 27 || key == 'q' || key == 'Q' ) // 'ESC'
            break;
    }

}

////////////////////////////////////////
int main(int argc, char* argv[])
{
	RUN_ERIC_CASE(argc,argv);
	return 0;
}




#pragma GCC diagnostic pop
