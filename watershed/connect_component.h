#ifndef connect_component_h__
#define connect_component_h__

#include "opencv.hpp"
#include <vector>
#include <set>

struct stCmpPt
{
	bool operator()(const cv::Point& a, const cv::Point& b)
	{
		if (a.x != b.x) {
			return a.x < b.x;
		}
		else{
			return (a.y < b.y);
		}
		return false;
	}

};

namespace cv
{
	bool operator<(const cv::Point& a, const cv::Point& b);
}

class XConnectComponentCounter
{

public:
	class XCnctCompoent
	{
	public:
		enum XCnctType{black = 0, white};

	public:
		XCnctCompoent()
		{

		}
		XCnctCompoent(cv::Mat& src)
		{
			bk_.create(src.size(), CV_8UC1);
			bk_.setTo(cv::Scalar(127));
		}

		cv::Mat bk_;
		std::set<cv::Point, stCmpPt> allPts_;
		XCnctType type_;	
	};
public:
	XConnectComponentCounter(cv::Mat& img);
	std::vector<XCnctCompoent>& allCnnt();

	void test();


private:
	void countConnectComponent(cv::Mat& bin_img);

private:
	std::vector<XCnctCompoent> vecConnectCmpnt_;
	std::vector<XConnectComponentCounter::XCnctCompoent> hole_;
};

#endif // connect_component_h__
