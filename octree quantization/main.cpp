#include "octree_quantization.h"

#include "opencv.hpp"
using namespace cv;
using namespace quant;

void main()
{
  octree_quantization quant;

  std::string file = "rainbow.jpg";
  cv::Mat img_src = imread("rainbow.jpg", CV_LOAD_IMAGE_COLOR);

  auto root = quant.build_tree(img_src); // 此时 g_root() 可以访问数据
  cv::Mat pallete = quant.make_pallete(root);

  Mat after = img_src.clone();
  for (int i = 0; i < after.rows; ++i){
    for (int j = 0; j < after.cols; ++j){
      auto p = after.ptr<uchar>(i, j);
      XPixel mapped = quant.map_pixel_to_leaf_node(XPixel(p[0], p[1], p[2]));
      p[0] = mapped.r_;
      p[1] = mapped.g_;
      p[2] = mapped.b_;
    }
  }

  imshow("pallete", pallete);
  imshow("before", img_src);
  imshow("after", after);
  waitKey(0);
}