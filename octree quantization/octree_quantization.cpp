#include "octree_quantization.h"

#include <opencv.hpp>
using namespace cv;

namespace quant {

  XNodePtr build_tree(cv::Mat& img);

  // level start from 1. [1, 8]
  std::list<XNodePtr>& octree_quantization::get_level_list(int idx)
  {
    if (idx >= 1 && idx <=8){
      return level_list_[idx-1];
    } else{
      throw std::runtime_error("bad idx!");
    }
  }

  bool XNode::is_leaf()
  {
    return pixel_cnt_ > 0;
  }

  XNodePtr octree_quantization::root()
  {
    if (root_node_ == nullptr){
      root_node_ = XNodePtr(new XNode(0));
    }
    return root_node_;
  }

  void octree_quantization::find_all_leafs(XNodePtr root, std::vector<XNodePtr>& all_leafs)
  {
    for (int i = 0; i < 8; ++i){
      if (root->childs8_[i] == nullptr){
        continue;
      }

      if (root->childs8_[i]->is_leaf()){
        all_leafs.push_back(root->childs8_[i]);
      }
      else{
        find_all_leafs(root->childs8_[i], all_leafs);
      }
    }
  }

  // count the leaf-node
  int octree_quantization::leaf_count()
  {
    std::vector<XNodePtr> all_leafs;
    find_all_leafs(root_node_, all_leafs);

    return all_leafs.size();
  }

  cv::Mat octree_quantization::make_pallete(XNodePtr root)
  {
    std::vector<XNodePtr> all_leafs;
    find_all_leafs(root, all_leafs);

    cv::Mat pallete = Mat::zeros(256, 256, CV_8UC3);

    for (int i = 0; i < 256; ++i){
      for (int j = 0; j < 256; ++j){
        size_t x = i / 16; size_t y = j / 16; size_t z = x + y * 16;
        if (z < all_leafs.size()){
          XNodePtr node = all_leafs[z];

          auto p = pallete.ptr<uchar>(i, j);
          p[0] = node->r_();
          p[1] = node->g_();
          p[2] = node->b_();
        }
      }
    }

    return pallete;
  }

  // 从第7层开始，逐个节点返回
  XNodePtr octree_quantization::find_node_to_collapse()
  {
    // 从第七层开始找
    for (int L = 7; L >=1 ; --L){
      auto& Li_List = get_level_list(L);
      if (!Li_List.empty()){
        auto p = Li_List.front();
        Li_List.pop_front();
        return p;
      }
    }

    return nullptr;
  }
  // prune the tree to 256-leaf-node
  // start from the level-7
  void octree_quantization::reduce_node(XNodePtr root){
    int xyz = leaf_count();
    while (xyz > 256)
    {
      auto p_new_leaf = find_node_to_collapse();
      xyz -= p_new_leaf->merge_child();
    }
  }

  // 向树中插入 pix，非递归，固定插入8次！
  // 记录 Node 的 level-number
  void octree_quantization::insert(XNodePtr root, XPixel pix)
  {
    auto node = root;
    for (int i = 0; i < 8; ++i){
      int child_idx = pix.idx(i); auto& chd = node->childs8_[child_idx];
      int lvl = i + 1;
      if (chd == nullptr){ // 空位置，新节点
        auto p = XNodePtr(new XNode(pix, lvl)); 
        node->childs8_[child_idx] = p;
        // store
        get_level_list(i+1).push_back(p);

      }

      node = node->childs8_[child_idx]; // move the ith-level
      if (lvl == 8){
        node->r_sum_ += pix.r_;
        node->g_sum_ += pix.g_;
        node->b_sum_ += pix.b_ ;
        node->pixel_cnt_ += 1;
      }
    } // for
  } // insert

  // 构造树. 返回根结点
  XNodePtr octree_quantization::build_tree(cv::Mat& img)
  {
    for (int i = 0; i < img.rows; ++i){
      for (int j = 0; j < img.cols; ++j){
        auto p = img.ptr(i, j);
        uchar r = p[0];
        uchar g = p[1];
        uchar b = p[2];

        insert(root(), XPixel(r, g, b));
      }
    }

    reduce_node(root());

    return root();
  }

  // 遍历树，
  // 给定一个XPixel，找到其对应的 leaf_node. 
  // 读取 leaf_node 的颜色，转为返回XPixle
  XPixel octree_quantization::map_pixel_to_leaf_node(XPixel pix)
  {
    XNodePtr pNode = root();
    for (int i = 0; i < 8; ++i){
      int idx_ = pix.idx(i);
      if (pNode->childs8_[idx_] == nullptr){
        continue;
      }
      pNode = pNode->childs8_[idx_];
      if (pNode->is_leaf()){
        return XPixel(pNode->r_(), pNode->g_(), pNode->b_());
      }
    }
    
    return XPixel(0, 0, 0);
  }

  XPixel::XPixel(uchar r, uchar g, uchar b)
    : r_(r)
    , g_(g)
    , b_(b)
  {

  }

  int XPixel::idx(int i)
  {
    uchar index = 0;
    {
      uchar mask = 0x80 >> i;
      if (r_ & mask)
        index |= 4;
      if (g_ & mask)
        index |= 2;
      if (b_ & mask)
        index |= 1;
    }

    return index;
  }

  void XNode::merge(XNodePtr other)
  {
    if (other == nullptr){return;}

    this->pixel_cnt_ += other->pixel_cnt_; // pix_cnt_ 不为0.
    this->r_sum_ += other->r_sum_;
    this->g_sum_ += other->g_sum_;
    this->b_sum_ += other->b_sum_;
  }

  XNode::XNode(int Lvl /*= 0*/) : pixel_cnt_(0)
    , r_sum_(0)
    , g_sum_(0)
    , b_sum_(0)
    , level_(Lvl)
  {
    childs8_.fill(nullptr);
  }

  XNode::XNode(XPixel pix, int Lvl /*= 0*/) 
    : pixel_cnt_(0)
    , r_sum_(0)
    , g_sum_(0)
    , b_sum_(0)
    , level_(Lvl)
  {
    childs8_.fill(nullptr);
  }

  int XNode::r_()
  {
    return r_sum_ / pixel_cnt_;
  }

  int XNode::g_()
  {
    return g_sum_ / pixel_cnt_;
  }

  int XNode::b_()
  {
    return b_sum_ / pixel_cnt_;
  }

  int XNode::merge_child()
  {
    int N = 0;
    for (int i = 0; i < 8; ++i){
      if (childs8_[i] == nullptr){
        continue;
      }

      this->merge(childs8_[i]); 
      N++;
      this->childs8_[i] = nullptr; 
    }

    return N - 1;
  }

} // namespace oct_quantizatiimg_sr

quant::octree_quantization::octree_quantization()
  :root_node_(new XNode(0))
{

}

