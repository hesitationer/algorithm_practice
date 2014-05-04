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
	tree.print_breadth_first();

	tree.insert(5);
	tree.print_breadth_first();

	tree.insert(25);
	tree.print_breadth_first();

	tree.insert(2);
	tree.print_breadth_first();

	tree.insert(8);
	tree.print_breadth_first();

	tree.insert(30);
	tree.print_breadth_first();

	tree.insert(20);
	tree.print_breadth_first();

	tree.insert(17);
	tree.print_breadth_first();

	tree.insert(16);
	tree.print_breadth_first();

	//tree.print_breadth_first();
}

ERIC_TEST(b_tree,insert)
{
	b_tree tree;

	tree.insert(15);
	tree.insert(5);
	tree.insert(25);
	tree.insert(2);
	tree.insert(8);
	tree.insert(30);
	tree.insert(20);
	tree.insert(17);
	tree.insert(16);

}


ERIC_TEST(b_tree, balance_factor)
{
	b_tree tree;
	int root_factor = 0;

	tree.insert(15);
	root_factor = tree.get_balance_factor(tree.get_root());
	printf("balance factor of root is: %d\n",root_factor);

	tree.insert(5);
	root_factor = tree.get_balance_factor(tree.get_root());
	printf("balance factor of root is: %d\n",root_factor);

	tree.insert(25);
	root_factor = tree.get_balance_factor(tree.get_root());
	printf("balance factor of root is: %d\n",root_factor);

	tree.insert(2);
	root_factor = tree.get_balance_factor(tree.get_root());
	printf("balance factor of root is: %d\n",root_factor);

	tree.insert(8);
	root_factor = tree.get_balance_factor(tree.get_root());
	printf("balance factor of root is: %d\n",root_factor);

	tree.insert(30);
	root_factor = tree.get_balance_factor(tree.get_root());
	printf("balance factor of root is: %d\n",root_factor);

	tree.insert(20);
	root_factor = tree.get_balance_factor(tree.get_root());
	printf("balance factor of root is: %d\n",root_factor);

	tree.insert(17);
	root_factor = tree.get_balance_factor(tree.get_root());
	printf("balance factor of root is: %d\n",root_factor);

	tree.insert(16);
	root_factor = tree.get_balance_factor(tree.get_root());
	printf("balance factor of root is: %d\n",root_factor);
}

ERIC_TEST(b_tree, rotate)
{
	b_tree tree;

	tree.insert(15);
	tree.insert(5);
	tree.insert(25);
	tree.insert(2);
	tree.insert(8);
	tree.insert(30);
	tree.insert(20);
	tree.insert(17);
	tree.insert(16);
}

ERIC_TEST(b_tree, max_depth)
{
	int factor = 0;
	b_tree tree;

	tree.insert(15);
	factor = tree.get_balance_factor(tree.get_root());
	printf("factor is: 15 , %d\n",factor);

	tree.insert(5);
	factor = tree.get_balance_factor(tree.get_root());
	printf("factor is: 5 , %d\n",factor);

	tree.insert(25);
	factor = tree.get_balance_factor(tree.get_root());
	printf("factor is: 25,%d\n",factor);

	tree.insert(2);
	factor = tree.get_balance_factor(tree.get_root());
	printf("factor is: 2,%d\n",factor);

	tree.insert(8);
	factor = tree.get_balance_factor(tree.get_root());
	printf("depth is: 8,%d\n",factor);

	tree.insert(30);
	factor = tree.get_balance_factor(tree.get_root());
	printf("depth is: 30,%d\n",factor);

	tree.insert(20);
	factor = tree.get_balance_factor(tree.get_root());
	printf("depth is: 20,%d\n",factor);

	tree.insert(17);
	factor = tree.get_balance_factor(tree.get_root());
	printf("depth is: 17,%d\n",factor);

	tree.insert(16);
	factor = tree.get_balance_factor(tree.get_root());
	printf("depth is: 16,%d\n",factor);

}

ERIC_TEST(b_tree, node_depth)
{
	int depth = 0;
	eric_node* temp = NULL;
	b_tree tree;

	temp = tree.insert(15);
	printf("depth is: %d\n",tree.get_node_depth_(temp));

	temp = tree.insert(5);
	printf("depth is: %d\n",tree.get_node_depth_(temp));

	temp = tree.insert(25);
	printf("depth is: %d\n",tree.get_node_depth_(temp));

	temp = tree.insert(2);
	printf("depth is: %d\n",tree.get_node_depth_(temp));

	temp = tree.insert(8);
	printf("depth is: %d\n",tree.get_node_depth_(temp));

	temp = tree.insert(30);
	printf("depth is: %d\n",tree.get_node_depth_(temp));

	temp = tree.insert(20);
	printf("depth is: %d\n",tree.get_node_depth_(temp));

	temp = tree.insert(17);
	printf("depth is: %d\n",tree.get_node_depth_(temp));

	temp = tree.insert(16);
	printf("depth is: %d\n",tree.get_node_depth_(temp));
}
#pragma GCC diagnostic pop
