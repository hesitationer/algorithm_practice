#include "pick_up_test_case.h"
#include "b_tree.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wwrite-strings"
#pragma GCC diagnostic ignored "-Wunused-variable"
ERIC_TEST(b_tree,instance)
{
	b_tree tree;
}

ERIC_TEST(b_tree,print_all)
{
	b_tree tree;

	tree.insert(15);
	tree.insert(5);
	tree.insert(25);
	tree.insert(2);
	tree.insert(8);
	tree.insert(30);
	tree.insert(20);

	//tree.print_all();
	tree.print_breadth_first();
}

ERIC_TEST(b_tree,print_insert)
{
	b_tree tree;
	tree.insert(2);
	tree.insert(1);
	tree.insert(3);
	//tree.print_all();
	tree.print_breadth_first();
}

ERIC_TEST(b_tree, get_depth)
{
	b_tree tree;

	tree.insert(15);
	tree.insert(5);
	tree.insert(25);
	tree.insert(2);
	tree.insert(8);
	tree.insert(30);
	tree.insert(20);

	int depth = tree.get_depth_();
	printf("depth: %d\n",depth);
}
#pragma GCC diagnostic pop
