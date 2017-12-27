#ifndef watershed_h__
#define watershed_h__

#include <vector>
#include <map>
#include "opencv.hpp"

#include "connect_component.h"

namespace ws
{
	struct  q_hit_info
	{
		XConnectComponentCounter::XCnctCompoent q_CONN;
		std::map<int, XConnectComponentCounter::XCnctCompoent*> q_hit_CONN;
	};

	struct x_dialte_result
	{
		std::vector<cv::Point> dam_;
		std::vector<cv::Point> dilated_;
	};


	struct stPtHeight
	{
		cv::Point pt_; // �������
		int elevation_;   // �õ�ĻҶ�ֵ

		bool operator<(const stPtHeight& other) const{
			return elevation_ > other.elevation_;
		};

		stPtHeight()
			: pt_()
			, elevation_(0)
		{}
		stPtHeight(cv::Point x, int y)
			: pt_(x)
			, elevation_(y)
		{}
	};

	void flooding_test(cv::Mat img);
	void test_gradient();
	void test_hist();
	// ��ֵ���� Mat �����ǣ� ȷ���� ֻ��һ�����ӡ���СΪ54byte. �൱�� 14 int
	void test_mat_header();

	
	void show_pts(std::vector<cv::Point> vecPts, cv::Size size_, std::string win_title = "hello");

	// �������д��ڵĻҶ�ֵ�� [0, 255]�����ڼ���flood fill �ļ�����
	std::vector<int> gray_value(cv::Mat& img);
	std::vector<q_hit_info> hitCount(cv::Mat& Ti, std::vector<XConnectComponentCounter::XCnctCompoent>& C_all);
	void update_c_all(q_hit_info& q_i, std::vector<XConnectComponentCounter::XCnctCompoent>& C_all);
	void construct_dam(q_hit_info& q_i, cv::Mat& src_draw_dam);
	x_dialte_result dilate_pt_set(
		std::set<cv::Point, stCmpPt>& c_pts,
		cv::Mat& c_this_visited,
		cv::Mat& c_other_visited,
		cv::Mat& c_other_bk,
		cv::Mat& q_bk,
		cv::Mat& src_is_dam_img);	


	cv::Mat priority_flood(cv::Mat& gray_);
  void priority_watershed(cv::Mat& img_, cv::Mat& marker);
	void morphological_watershed(cv::Mat gradient);
}
#endif // watershed_h__
