#ifndef octree_quantization_h__
#define octree_quantization_h__

#include <memory>
#include <array>
#include <set>
#include <list>
#include <vector>

namespace cv
{
  class Mat;
}

namespace quant
{

  void test_oct_quantization();

  class XNode;
  class XPixel;
  typedef std::shared_ptr<XNode> XNodePtr;
  typedef std::array<XNodePtr, 8> Childs8;
  typedef std::array<std::list<XNodePtr>, 8> LevelList; // 存储每一层的节点信息

  class octree_quantization
  {
  public:
    octree_quantization();

    XNodePtr build_tree(cv::Mat& img);
    cv::Mat make_pallete(XNodePtr root);
    XPixel map_pixel_to_leaf_node(XPixel pix);

  private:
    XNodePtr root();
    std::list<XNodePtr>& get_level_list(int idx);

    void insert(XNodePtr root, XPixel pix);
    void reduce_node(XNodePtr root);
    XNodePtr find_node_to_collapse();

    int leaf_count();
    void find_all_leafs(XNodePtr root, std::vector<XNodePtr>& all_leafs);
  private:
    XNodePtr root_node_;
    LevelList level_list_;
  };

  class XNode
  {
  public:
    int pixel_cnt_;
    int r_sum_;
    int g_sum_;
    int b_sum_;
    int level_; // 当前节点所处的level start from 1. 0 是 root
    Childs8 childs8_;


    XNode(int Lvl = 0);
    XNode(XPixel pix, int Lvl = 0);

    int r_();
    int g_();
    int b_();
    // 加上另一个节点，得到另一个节点的数据
    // this 节点变为 leaf_node.
    void merge(XNodePtr other);

    // 遍历所有子节点，逐个执行 merge
    // 递归操作
    // 返回减少的节点数
    int merge_child();

    // 判断当前节点是否叶子节点
    bool is_leaf();
  };

  class XPixel
  {
  public:
    typedef unsigned char uchar;
    uchar r_;
    uchar g_;
    uchar b_;

  public:
    XPixel(uchar r, uchar g, uchar b);

    // 返回第 i  个下标
    int idx(int i);
    uchar ith_bit(uchar x, int i);
  };

}

#endif // octree_quantization_h__


