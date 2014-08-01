#include "pick_up_test_case.h"
#include "mean_shift.h"
#include "opencv2/opencv.hpp"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wwrite-strings"
#pragma GCC diagnostic ignored "-Wunused-variable"

void floodFillPostprocess( Mat& img, const Scalar& colorDiff=Scalar::all(1) );
void mean_shift_seg(Mat src, Mat dst);

class my_class{
public:
	my_class(){
		printf("ctor~\n");
		a = 4;
	};
	int a;
};

ERIC_TEST(ms_seg, seg)
{
	Mat src = imread("../CMake/fruits.jpg");
	Mat dst = Mat::zeros(src.rows, src.cols, CV_8UC3);

	mean_shift_seg(src,dst);


	imshow("dst",dst);
	imshow("src",src);
	waitKey(0);
}

ERIC_TEST(seg,pyrdown)
{
	Mat src = imread("../CMake/fruits.jpg");

	int h = src.rows;
	int w = src.cols;

	Mat half = Mat::zeros(src.rows/2,src.cols/2,src.type());
	pyrDown(src,half,half.size());

	imshow("src",src);
	imshow("half",half);
	waitKey(0);
}

ERIC_TEST(ms_seg,array_of_class)
{
	my_class *array_of_class = new my_class[10];

	for(int i = 0; i < 10; i++){
		printf("%d\n",array_of_class[i].a);
	}

	delete [] array_of_class;
}	


ERIC_TEST(seg, mat_zeros)
{
	Mat tmp = Mat::zeros(240,320,CV_8UC1);

	int row = tmp.rows;
	int col = tmp.cols;
	printf("%d,%d\n",row,col);
}
ERIC_TEST(seg, mask)
{
	Mat src = imread("../CMake/fruits.jpg");

	int h = src.rows;
	int w = src.cols;

	Mat src_dst = Mat::zeros(h,w,src.type());

	Mat half = Mat::zeros(h/2,w/2,src.type());
	pyrDown(src,half,half.size());

	// half 
	Mat half_dst = Mat::zeros(half.rows,half.cols,half.type());
	mean_shift_seg(half,half_dst);

	// pyrUp   half_dst ---> src_dst
	pyrUp(half_dst,src_dst,src.size());

	// mask
	Mat mask0 = Mat::zeros(src.rows,src.cols,CV_8UC1);
	Mat m(src.rows,src.cols,CV_8UC1,mask0.data);

	uchar *dptr = half_dst.data + (int)half_dst.step + 3; //处理后小图
	int mstep = (int)m.step; //原图mask的宽度
	int dstep = (int)half.step; // 小图的宽度

	uchar *mask = 0;
	mask = m.data + mstep; // 指向矩阵m，与mask0共享data内存
	m.setTo(Scalar::all(0));

	int tab[768];
	for(int i = 0; i < 768; i++){
		tab[i] = (i-255)*(i-255);
	}
	int isr22 = 20*20;
#define cdiff(ofs0) (tab[c0-dptr[ofs0]+255] + \
		tab[c1-dptr[(ofs0)+1]+255] + tab[c2-dptr[(ofs0)+2]+255] >= isr22)

	for(int i = 1; i < half.rows - 1; ++i){
		for(int j = 1; j < half.cols - 1; ++j){

			int c0 = dptr[0],c1= dptr[1], c2 = dptr[2];
			mask[j*2 - 1] = cdiff(-3) || cdiff(3) || cdiff(-dstep-3) || cdiff(-dstep) ||
				cdiff(-dstep+3) || cdiff(dstep-3) || cdiff(dstep) || cdiff(dstep+3);

			// move ptr
			dptr += 3;

		}
		dptr += (int)half_dst.step - (half_dst.cols - 2)*3; //减去2，是左右各省去一列。乘以3，是通道。
		mask += mstep*2;
	}

	// dialate
	dilate(m,m,cv::Mat());
	mask = m.data; // rest to start

	
	// full 
	Mat src_final_dst = Mat::zeros(src.rows,src.cols,src.type());
	mean_shift_seg(src_dst,src_final_dst);

	// flood fill
	floodFillPostprocess(src_final_dst,Scalar::all(2));
	

	imshow("half",half);
	imshow("half_dst",half_dst);
	imshow("src_dst_pryUp",src_dst);
	imshow("src_dst_final",src_final_dst);

	waitKey(0);
}

ERIC_TEST(ms_seg, mat_3c_visit)
{
	Mat mat3c = Mat::zeros(4,4,CV_8UC3);

	int step = 3*4;
	*(mat3c.data + 2*step + 2*3 + 0) = 1;
	*(mat3c.data + 2*step + 2*3 + 1) = 11;
	*(mat3c.data + 2*step + 2*3 + 2) = 111;

	for(int i = 0; i < 4; i++){
		for(int j = 0; j < 4; j++){

			printf("(%d,%d,%d) ",*(mat3c.data + i*4*3 + j*3 + 0),
								 *(mat3c.data + i*4*3 + j*3 + 1),
								 *(mat3c.data + i*4*3 + j*3 + 2)
					);
		}
		printf("\n");
	}
}
#pragma GCC diagnostic pop
