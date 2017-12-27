#include "connect_component.h"

#include <array>
#include <stack>

namespace cv
{
	bool operator<(const cv::Point& a, const cv::Point& b)
	{
		if (a.x != b.x) {
			return a.x < b.x;
		}
		else{
			return a.y < b.y;
		}
	}
}

using namespace cv;

static std::array<cv::Point, 8> dir;
static auto init_ = [=]()->int{
	dir[0] = cv::Point(-1, -1); dir[1] = cv::Point(0, -1);dir[2] = cv::Point(1, -1);
	dir[3] = cv::Point(-1, 0); /*dir[0] = cv::Point(0, 0);*/ dir[4] = cv::Point(1, 0);
	dir[5] = cv::Point(-1, 1); dir[6] = cv::Point(0, 1); dir[7] = cv::Point(1, 1);
	return 1;
}();


XConnectComponentCounter::XConnectComponentCounter(cv::Mat& img)
{
	cv::Mat b; b.create(img.size(), CV_8UC1);

	//imshow("xyz", img);
	//cv::waitKey(0);

	cv::threshold(img, b, 100, 255, THRESH_BINARY);

	//imshow("xyz", b);
	//cv::waitKey(0);


	countConnectComponent(b);
}

void XConnectComponentCounter::countConnectComponent(cv::Mat& bin_img)
{
	cv::Mat visited = Mat::zeros(bin_img.size(), CV_8UC1);

	for (int i = 2; i < bin_img.rows - 2; ++i) {
		for (int j = 2; j < bin_img.cols - 2; ++j) {
			if (visited.at<uchar>(i, j) == 1) {// unvisited 
				continue;
			}
			
			XCnctCompoent CONN(bin_img);
			std::stack<cv::Point> s; auto& _pts = CONN.allPts_;
			s.push(cv::Point(i, j)); CONN.type_ = static_cast<XCnctCompoent::XCnctType>(bin_img.at<uchar>(i, j));
			visited.at<uchar>(i, j) = 1;
			while (!s.empty()) {
				auto top = s.top();
				s.pop(); 
				for (int k = 0; k < 8; ++k) {
					auto zz = top + dir[k]; if (zz.x < 0 || zz.x >= bin_img.rows || zz.y < 0 || zz.y >= bin_img.cols) {
						continue;
					}
					if (visited.at<uchar>(zz.x, zz.y) == 0 
						&& bin_img.at<uchar>(zz.x, zz.y) == bin_img.at<uchar>(top.x, top.y)) {
						s.push(zz); visited.at<uchar>(zz.x, zz.y)= 1;
						_pts.insert(zz); CONN.bk_.at<uchar>(zz.x, zz.y) = CONN.type_;
					}
				}

				//imshow("bk", CONN.bk_);
				//cv::waitKey(1);
			} // find once

			vecConnectCmpnt_.push_back(CONN);
		}
	}
}

void XConnectComponentCounter::test()
{
	for (size_t i = 0; i < vecConnectCmpnt_.size(); ++i)
	{
		std::string arr[10] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9"}; 
		std::string name_ = arr[i] + std::string("--cnn--") + ((vecConnectCmpnt_[i].type_ == XCnctCompoent::black) ? std::string("black") : std::string("white"));
		imshow(name_, vecConnectCmpnt_[i].bk_);
		cv::waitKey(0);
	}
}

std::vector<XConnectComponentCounter::XCnctCompoent>& XConnectComponentCounter::allCnnt()
{
	for (auto it = vecConnectCmpnt_.begin(), E = vecConnectCmpnt_.end(); it != E; ++it)
	{
		if ((*it).type_ == XCnctCompoent::black)
		{
			hole_.push_back(*it);
		}
	}

	return hole_;
}


