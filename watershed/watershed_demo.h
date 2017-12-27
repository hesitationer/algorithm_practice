#ifndef demo_h__
#define demo_h__

#include <string>

namespace demo
{
	int test_flooding_watershed(std::string filename);
  void test_morphological_watershed(std::string filename);


	void help();
	void onMouse( int event, int x, int y, int flags, void* );
	std::string type2str(int type);
}

#endif // demo_h__
