#include "connect_component.h"

#include <iostream>
#include <array>
#include <queue>

#include "watershed.h"
#include "watershed_demo.h"

static std::array<cv::Point, 8> dir;
static auto init_ = [=]()->int{
	dir[0] = cv::Point(-1, -1); dir[1] = cv::Point(0, -1);dir[2] = cv::Point(1, -1);
	dir[3] = cv::Point(-1, 0); /*dir[0] = cv::Point(0, 0);*/ dir[4] = cv::Point(1, 0);
	dir[5] = cv::Point(-1, 1); dir[6] = cv::Point(0, 1); dir[7] = cv::Point(1, 1);
	return 1;
}();

std::vector<cv::Point> g_all_dam;

using namespace cv;
void ws::flooding_test(cv::Mat gradient_img)
{
	imshow("gradient_", gradient_img);

	std::vector<int> gray_v = gray_value(gradient_img);
	int min_x = gray_v.front();
	int max_x = gray_v.back();

	cv::Mat flood = gradient_img.clone();
	for (size_t T = 0; T < gray_v.size(); ++T)
	{
		cv::threshold(gradient_img, flood, gray_v[T], 255, THRESH_BINARY);
		cv::imshow("flooding...", flood);
		cv::waitKey(0);
	}
}

void ws::test_hist()
{
	Mat src = imread("gradient.png", CV_LOAD_IMAGE_GRAYSCALE);
	

	Mat hist;
	int hist_size = 256;
	float range[2] =  {0, 256};
	const float* histRange = { range };
	calcHist(&src, 1, 0, Mat(), hist, 1, &hist_size, &histRange, true, false);

	auto x = hist.size();
	auto y = hist.type();
	std::vector<int> vecV;
	for (int i = 0; i < 256; ++i)
	{
		auto f = hist.at<float>(i);
		if (f > 10)
		{
			vecV.push_back(i);
		}
	}

	int hist_w = 512; int hist_h = 400;
	int bin_w = cvRound( (double) hist_w/hist_size );

	Mat histImage( hist_h, hist_w, CV_8UC3, Scalar( 0,0,0) );
	normalize(hist, hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());
	for( int i = 1; i < hist_size; i++ )
	{
		line( histImage, Point( bin_w*(i-1), hist_h - cvRound(hist.at<float>(i-1)) ) ,
			Point( bin_w*(i), hist_h - cvRound(hist.at<float>(i)) ),
			Scalar( 255, 0, 0), 2, 8, 0  );
	}

	imshow("hist", histImage);
	waitKey(0);
}

// 生成gradient效果图
// 目前效果无法令人满意
void ws::test_gradient()
{
	Mat src = imread("fruits.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	imshow("src", src);

	Mat gradient_ = src.clone();
	size_t k_size = 3;
	cv::Sobel(src, gradient_, CV_8UC1, 1, 0, k_size);
	imshow("x-1", gradient_);
	waitKey(0);

	Mat x2 = src.clone();
	cv::Sobel(src, x2, CV_8UC1, 0, 1, k_size);
	imshow("y-1", x2);
	waitKey(0);

	Mat x3;
	x3 = gradient_ + x2;
	imshow("mag", x3);
	waitKey(0);
}

// Implement the watershed segmentation from Digital Image Process 3rd Version
// Chapter 10: Image Segmentation
//     10.5 Image Segmentation Using Morphological Watersheds
//  Remark:
//  This algorithm looks hard. i need to find the connect-component by myself, no api-calling.
//  I have trouble in constructing the dam, the result looks not like single-pixel-witdth.
void ws::morphological_watershed(cv::Mat gradient)
{
	std::vector<int> gray_v = gray_value(gradient);
	int min_x = gray_v.front();
	int max_x = gray_v.back();

	Mat Ci = gradient.clone();
	cv::threshold(gradient, Ci, min_x, 255, THRESH_BINARY);
	XConnectComponentCounter Cn(Ci);
	std::vector<XConnectComponentCounter::XCnctCompoent> C_all = Cn.allCnnt();

	Mat Ti = gradient.clone();
	for (size_t i = 0; i<= gray_v.size(); ++i)
	{
		cv::threshold(gradient, Ti, gray_v[i], 254, THRESH_BINARY);

		//imshow("c_all", Ci);
		//imshow("Ti", Ti);
		//waitKey(0);

		auto vec_hi_ = hitCount(Ti, C_all);

		for (auto info_ = vec_hi_.begin(), E = vec_hi_.end(); info_ != E; ++info_)
		{
			auto& q_i = (*info_);
			auto n_hit = q_i.q_hit_CONN.size();
			if (n_hit == 0 || n_hit == 1)
			{
				// update the C_all. Add the q to C_all
				update_c_all(q_i, C_all);
			}
			else if (n_hit == 2)
			{
				// construct dam
				construct_dam(q_i, gradient);
			}
			else
			{
				// do nothing
			}
		}
	}

  imshow("input gradient", gradient);
	show_pts(g_all_dam, gradient.size(), "final-dam");
}

std::vector<int> ws::gray_value(cv::Mat& img)
{
	Mat hist;
	int hist_size = 256;
	float range[2] =  {0, 256};
	const float* histRange = { range };
	calcHist(&img, 1, 0, Mat(), hist, 1, &hist_size, &histRange, true, false);

	auto x = hist.size();
	auto y = hist.type();
	std::vector<int> vecV;
	for (int i = 0; i < 256; ++i)
	{
		auto f = hist.at<float>(i);
		if (f > 10)
		{
			vecV.push_back(i);
		}
	}

	return vecV;
}

std::vector<ws::q_hit_info> ws::hitCount(cv::Mat& Ti, std::vector<XConnectComponentCounter::XCnctCompoent>& C_all)
{

	auto q_pt_hit_C = [&](cv::Point& a_)->int
	{
		int nCnt = 0;
		for (auto c = C_all.begin(), E = C_all.end(); c != E; ++c)
		{
			auto c_pts = (*c).allPts_;
			auto search_ = std::find(c_pts.begin(), c_pts.end(), a_);
			if (search_ != c_pts.end())
			{
				nCnt++;
			}
		}

		return nCnt >= 0;
	};

	auto q_pts_AND_c_pts = [&](q_hit_info& hi)->bool{
		auto q_pts = hi.q_CONN.allPts_;
		for(auto c = C_all.begin(), E = C_all.end(); c != E; ++c)	{
			auto& ref_c = *c;
			auto& c_pts = ref_c.allPts_;


			std::set<cv::Point, stCmpPt> cnt_;
			std::set_intersection(q_pts.begin(), q_pts.end(),
								  c_pts.begin(), c_pts.end(),
								  std::inserter(cnt_, cnt_.begin()));
			
			bool b = !cnt_.empty();
			if (b) {
				hi.q_hit_CONN.insert(std::make_pair(c - C_all.begin(), &ref_c));
			}
		}

		return hi.q_hit_CONN.empty() ? false : true;
	};


	XConnectComponentCounter xc(Ti);
	std::vector<XConnectComponentCounter::XCnctCompoent>& v = xc.allCnnt();
	std::vector<q_hit_info> vecQ_Hit_Info;
	for (size_t q = 0; q < v.size(); ++q)
	{
		q_hit_info hi;
		hi.q_CONN = v[q];
		Mat q_bk = v[q].bk_;

		q_pts_AND_c_pts(hi);

		vecQ_Hit_Info.push_back(hi);
	}
	return std::move(vecQ_Hit_Info);
}

void ws::test_mat_header()
{
	Mat m = Mat::zeros(400, 400, CV_8UC1);
	Mat m1 = Mat::zeros(1024, 1024, CV_8UC1);

	std::cout << sizeof(Mat) << "\n";
	std::cout << sizeof(Mat) << "\n";
}

void ws::construct_dam(q_hit_info& q_i, cv::Mat& src_draw_dam)
{
	auto q_conn = q_i.q_CONN;
	auto q_bk = q_i.q_CONN.bk_;

	auto& c_1 = q_i.q_hit_CONN.at(0);
	auto& c_2 = q_i.q_hit_CONN.at(1);

	Mat c1_visit = Mat::zeros(c_1->bk_.size(), CV_8UC1);
	Mat c2_visit = Mat::zeros(c_2->bk_.size(), CV_8UC1);

	std::vector<cv::Point> q_pts_clone(q_conn.allPts_.begin(), q_conn.allPts_.end());
	
	std::vector<Point> v_dam;
	while (!q_pts_clone.empty())
	{
		/// dilate C1
		auto d1_info = dilate_pt_set(c_1->allPts_, c1_visit, c2_visit, c_2->bk_, q_bk, src_draw_dam);

		/// dilate C2
		auto d2_info = dilate_pt_set(c_2->allPts_, c2_visit, c1_visit, c_1->bk_, q_bk, src_draw_dam);

		/// remove valid pos from q.
		auto pos_ = std::remove_if(q_pts_clone.begin(), q_pts_clone.end(),
			[&](const cv::Point& p_)->bool{
				return c1_visit.at<uchar>(p_.x, p_.y) == 1
					|| c2_visit.at<uchar>(p_.x, p_.y) == 1;
		});
		q_pts_clone.erase(pos_, q_pts_clone.end());
		/// update c1_pt and c2_pt
		c_1->allPts_.insert(d1_info.dilated_.begin(), d1_info.dilated_.end());
		c_2->allPts_.insert(d2_info.dilated_.begin(), d2_info.dilated_.end());

		// update bk of c1, c2
		for (auto it = d1_info.dilated_.begin(), E = d1_info.dilated_.end(); it != E; ++it)
		{
			c_1->bk_.at<uchar>((*it).x, (*it).y) = 0; // expand the c1
		}

		for (auto it = d2_info.dilated_.begin(), E = d2_info.dilated_.end(); it != E; ++it)
		{
			c_2->bk_.at<uchar>((*it).x, (*it).y) = 0; // expand the c1
		}

		/// collect dam
		v_dam.insert(v_dam.end(), d1_info.dam_.begin(), d1_info.dam_.end());
		v_dam.insert(v_dam.end(), d2_info.dam_.begin(), d2_info.dam_.end());

		g_all_dam.insert(g_all_dam.end(), d1_info.dam_.begin(), d1_info.dam_.end());
		g_all_dam.insert(g_all_dam.end(), d2_info.dam_.begin(), d2_info.dam_.end());

		/// remove the dam pts.
		for (auto it = g_all_dam.begin(), E = g_all_dam.end(); it != E; ++it)
		{
			auto x = std::remove(q_pts_clone.begin(), q_pts_clone.end(), *it);
			q_pts_clone.erase(x, q_pts_clone.end());
		}

		//show_pts(g_all_dam, src_draw_dam.size(), "temp");
	}
}

void ws::update_c_all(q_hit_info& q_i, std::vector<XConnectComponentCounter::XCnctCompoent>& C_all)
{
	auto& q_conn = q_i.q_CONN;
	auto& hit_map = q_i.q_hit_CONN; // <int, CONN>, 序号, 以及对应的连通域
	for (auto it = hit_map.begin(), E = hit_map.end(); it != E; ++it)
	{
		int k = it->first;
		auto& c_conn = C_all[k];

		cv::bitwise_and(c_conn.bk_, q_conn.bk_, c_conn.bk_); // 做一次与操作,即可
	}
}

ws::x_dialte_result ws::dilate_pt_set(std::set<cv::Point, stCmpPt>& c_pts,
	cv::Mat& c_this_visit,
	cv::Mat& c_other_visit,
	cv::Mat& c_other_bk, 
	cv::Mat& q_bk,
	cv::Mat& src_is_dam_img)
{
	std::vector<Point> c_dilated;
	std::vector<Point> c_dam;
	for (auto c1 = c_pts.begin(), E = c_pts.end(); c1 != E; ++c1) {
		Point top_ = *c1;
		if (c_this_visit.at<uchar>(top_.x, top_.y) == 1){  // has visited 
			continue;
		}

		c_this_visit.at<uchar>(top_.x, top_.y) = 1; // set to visited
		for (int d = 0; d < 8; ++d) {
			Point nb_ = top_ + dir[d]; if (nb_.x < 0 || nb_.x >= c_this_visit.rows || nb_.y < 0 || nb_.y >= c_this_visit.cols) {
				continue;
			}
			if (q_bk.at<uchar>(nb_.x, nb_.y) != 0) { continue; }// constraint in q!
			//if (std::find(g_all_dam.begin(), g_all_dam.end(), nb_) != g_all_dam.end()) { continue; } // meet dam. skip current center

#if 0
			if (c_other_bk.at<uchar>(nb_.x, nb_.y) == 0) {// connect to C2  via nb_. so, nb_ is the dam.
				c_dam.push_back(nb_); // note: here is nb_ 
				src_is_dam_img.at<uchar>(nb_.x, nb_.y) = 255; // 应该是256的,假定没有最大值
			}
			else {
				c_dilated.push_back(nb_);				
			}
#else

			bool bMerged = false;
			for (int dx = 0; dx < 8; ++dx) {
				Point nb_nb = nb_ + dir[dx]; if (nb_nb.x < 0 || nb_nb.x >= c_this_visit.rows || nb_nb.y < 0 || nb_nb.y >= c_this_visit.cols) {
					continue;
				}
				if (q_bk.at<uchar>(nb_nb.x, nb_nb.y) != 0) { continue; }// constraint in q!

				if (c_other_bk.at<uchar>(nb_nb.x, nb_nb.y) == 0) {// connect to C2  via nb_. so, nb_ is the dam.
					c_dam.push_back(nb_); // note: here is nb_ 
					src_is_dam_img.at<uchar>(nb_.x, nb_.y) = 255; // 应该是256的,假定没有最大值
					//c_other_visit.at<uchar>(nb_nb.x, nb_nb.y) = 1; // mark the 'dam' neighbor
					bMerged = true; 
				}
				else {
					// do nothing
				}
			} // nb_nb
			if (!bMerged)
			{
				c_dilated.push_back(nb_);
			}
#endif
		}
	}

	x_dialte_result st;
	st.dam_ = std::move(c_dam);
	st.dilated_ = std::move(c_dilated);

	return st;
}

void ws::show_pts(std::vector<cv::Point> vecPts, cv::Size size_, std::string win_title)
{
	Mat xyz = Mat::zeros(size_, CV_8UC1);
	for (auto it = vecPts.begin(), E = vecPts.end(); it != E; ++it)
	{
		auto& pt = *it;
		xyz.at<uchar>(pt.x, pt.y) = 255;
	}

	imshow(win_title, xyz);
	waitKey(0);
}

// implement the priority-flooding without seed. start from the edge.
// Ref: 
//     Barnes, Lehman, Mulla. \Priority-Flood: An Optimal Depression-Filling and Watershed-
// Labeling Algorithm for Digital Elevation Models". Computers & Geosciences. Vol 62, Jan 2014, pp 117{127.
// doi: \10.1016/j.cageo.2013.04.024".
// It's the *Algorithm 5*
cv::Mat ws::priority_flood(cv::Mat& gray_)
{
	int lable_ = 1;
	cv::Mat DEM = gray_.clone();
	cv::Mat visited = cv::Mat::zeros(gray_.size(), CV_8UC1);

	cv::Mat LABEL_ = cv::Mat::zeros(gray_.size(), CV_8UC1); // candidate = 0;
	int CANDIDATE_0 = 0;
	int QUEUED_255 = 255;
	int MAX_H_255 = 255;

	std::priority_queue<stPtHeight, std::vector<stPtHeight>> OPEN_;
	std::queue<stPtHeight> PIT_;
	std::vector<int> hist_m(1024, 0);

	// fill the edge
	for (int i = 0; i < DEM.rows; ++i) {
		for (int j = 0; j < DEM.cols; ++j) {
			if (i != 0 && i != DEM.rows - 1 && j != 0 && j != DEM.cols - 1){
				continue;
			}

			uchar c = DEM.at<uchar>(i, j); // 取边缘 
			stPtHeight s(cv::Point(i, j), DEM.at<uchar>(i, j));
			OPEN_.push(s);
			LABEL_.at<uchar>(i, j) = QUEUED_255;
		}
	}

	while (!OPEN_.empty() || !PIT_.empty()) {
		stPtHeight top;
		if (!PIT_.empty()){
			top = PIT_.front();
			PIT_.pop();
		}
		else if (!OPEN_.empty()){
			top = OPEN_.top();
			OPEN_.pop();
		}
		else{
			break;
		}

		bool bHit = false;
		if (LABEL_.at<uchar>(top.pt_.x, top.pt_.y) == QUEUED_255){
			LABEL_.at<uchar>(top.pt_.x, top.pt_.y) = lable_;
			lable_++;
			bHit = true;
		}

		for (int i = 0; i < 8; ++i){
			Point nb_nb = top.pt_ + dir[i]; 
			if (nb_nb.x < 0 || nb_nb.x >= LABEL_.rows || nb_nb.y < 0 || nb_nb.y >= LABEL_.cols) {
				continue;
			}
			if ((int)LABEL_.at<uchar>(nb_nb.x, nb_nb.y) > CANDIDATE_0){
				continue;
			}

			
			LABEL_.at<uchar>(nb_nb.x, nb_nb.y) = LABEL_.at<uchar>(top.pt_.x, top.pt_.y);

			int DEM_c = (int)DEM.at<uchar>(top.pt_.x, top.pt_.y), DEM_n = (int)DEM.at<uchar>(nb_nb.x, nb_nb.y);
			if (DEM_n <= DEM_c){
				PIT_.push(stPtHeight(nb_nb, DEM_c));
			}
			else{
				OPEN_.push(stPtHeight(nb_nb, DEM_n));
			}
			//imshow("dynamic_DEM", DEM);
			//waitKey(1);
		}
	}

	std::string xs = demo::type2str(LABEL_.type());
	return LABEL_;
}

// re-implementation of the opencv-default version in:
// X:\OpenCV2.4\opencv\build\include\opencv2\imgproc\imgproc_c.h
// X:\OpenCV2.4\opencv\modules\imgproc\src\segmentation.cpp
//     CV_IMPL void
//     cvWatershed( const CvArr* srcarr, CvArr* dstarr )
// Remark:
//    Use user-input `marker` as the 'flooding-seed'
void ws::priority_watershed(cv::Mat& img_, cv::Mat& marker)
{
  // key: diff(r,g,b)
  // value:  pixel-pos
  static std::priority_queue<stPtHeight, std::vector<stPtHeight>> OPen;
  const int WSHED = -1;
  const int VISITED = -2;
  const int FRESH = 0;

  auto max_diff_rgb = [&](uchar* p1, uchar* p2)->int
  {
    auto r_diff = std::abs(p1[0] - p2[0]);
    auto g_diff = std::abs(p1[1] - p2[1]);
    auto b_diff = std::abs(p1[2] - p2[2]);

    return std::max(r_diff, std::max(g_diff, b_diff));
  };

  // init 1-pixel-border
  for (int c = 0; c < marker.cols; ++c){
    marker.at<int>(0, c) = WSHED; // NOTE: 取int
    marker.at<int>(marker.rows - 1, c) = WSHED;
  }

  for (int r = 0; r < marker.rows; ++r){
    marker.at<int>(r, 0) = WSHED; // NOTE: 取int
    marker.at<int>(r, marker.cols-1) = WSHED;
  }

  // init pts-near-marker
  for (int i = 1; i < marker.rows - 1; ++i){
    for (int j = 1; j < marker.cols - 1; ++j){
      auto& c = marker.at<int>(i, j);
      if (c != FRESH){
        if (c < FRESH){
          c = 0;
        }
        continue;
      }
      auto& n1 = marker.at<int>(i,j-1); auto& n2 = marker.at<int>(i, j+1);
      auto& n3 = marker.at<int>(i-1,j); auto& n4 = marker.at<int>(i+1,j);
      if (n1 <= 0 && n2 <= 0 && n3 <= 0 && n4 <= 0){
        continue;
      }
      cv::Point val(i, j);
      int key = 256;
      if (n1 > FRESH) {
        int x = max_diff_rgb(img_.ptr<uchar>(i,j), img_.ptr<uchar>(i, j-1));
        key = std::min(key, x);
      }
      if (n2 > FRESH) {
        int x = max_diff_rgb(img_.ptr<uchar>(i,j), img_.ptr<uchar>(i, j+1));
        key = std::min(key, x);
      }
      if (n3 > FRESH) {
        int x = max_diff_rgb(img_.ptr<uchar>(i,j), img_.ptr<uchar>(i-1, j));
        key = std::min(key, x);
      }
      if (n4 > FRESH) {
        int x = max_diff_rgb(img_.ptr<uchar>(i,j), img_.ptr(i+1, j));
        key = std::min(key, x);
      }

      assert(0 <= key && key <= 255);
      OPen.push(stPtHeight(val, key));
      marker.at<int>(i,j) = VISITED;
    }
  }



  // priority flooding
  while(!OPen.empty()){
    auto top = OPen.top();
    OPen.pop();

    int i = top.pt_.x; int j = top.pt_.y;
    auto& n1 = marker.at<int>(i,j-1); auto& n2 = marker.at<int>(i, j+1);
    auto& n3 = marker.at<int>(i-1,j); auto& n4 = marker.at<int>(i+1,j);
    int lab = 0;
    if (n1 > FRESH){
      lab = n1;
    }

    if (n2 > FRESH){
      if (lab == 0){ lab = n2; }
      else if (lab != n2){
        lab = WSHED;
      }
    }

    if (n3 > FRESH){
      if (lab == 0){ lab = n3; }
      else if (lab != n3){
        lab = WSHED;
      }
    }

    if (n4 > FRESH){
      if (lab == 0){ lab = n4; }
      else if (lab != n4){
        lab = WSHED;
      }
    }

    assert(lab != 0);
    marker.at<int>(i, j) = lab;

    if (lab == WSHED){
      continue;
    }

    if (n1 == FRESH){
      int key = max_diff_rgb(img_.ptr(i,j), img_.ptr(i, j-1));
      cv::Point val(i, j-1);
      OPen.push(stPtHeight(val, key));
      marker.at<int>(i, j-1) = VISITED;
    }
    if (n2 == FRESH){
      int key = max_diff_rgb(img_.ptr(i,j), img_.ptr(i, j+1));
      cv::Point val(i, j+1);
      OPen.push(stPtHeight(val, key));
      marker.at<int>(i, j+1) = VISITED;
    }
    if (n3 == FRESH){
      int key = max_diff_rgb(img_.ptr(i,j), img_.ptr(i-1, j));
      cv::Point val(i-1, j);
      OPen.push(stPtHeight(val, key));
      marker.at<int>(i-1, j) = VISITED;
    }
    if (n4 == FRESH){
      int key = max_diff_rgb(img_.ptr(i,j), img_.ptr(i+1, j));
      cv::Point val(i+1, j);
      OPen.push(stPtHeight(val, key));
      marker.at<int>(i+1, j) = VISITED;
    }
  }
}

