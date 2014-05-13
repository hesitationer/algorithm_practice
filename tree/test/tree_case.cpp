#include "pick_up_test_case.h"
#include "b_tree.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wwrite-strings"
#pragma GCC diagnostic ignored "-Wunused-variable"
ERIC_TEST(b_tree,instance)
{
	b_tree tree;
}

ERIC_TEST(b_tree,print_breadth_first)
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

ERIC_TEST(b_tree, L_L_rotate)
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

	tree.print_breadth_first();
}


ERIC_TEST(b_tree, L_R_rotate)
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
	tree.insert(18);

	tree.print_breadth_first();
}

ERIC_TEST(b_tree, R_R_rotate)
{
	b_tree tree;

	tree.insert(15);
	tree.insert(5);
	tree.insert(25);
	tree.insert(2);
	tree.insert(8);
	tree.insert(30);
	tree.insert(20);
	tree.insert(22);
	tree.insert(24);

	tree.print_breadth_first();
}

ERIC_TEST(b_tree, R_L_rotate)
{
	b_tree tree;

	tree.insert(15);
	tree.insert(5);
	tree.insert(25);
	tree.insert(2);
	tree.insert(8);
	tree.insert(30);
	tree.insert(20);
	tree.insert(24);
	tree.insert(22);

	tree.print_breadth_first();
}

ERIC_TEST(b_tree, find)
{
	b_tree tree;

	tree.insert(15);
	tree.insert(5);
	tree.insert(25);
	tree.insert(2);
	tree.insert(8);
	tree.insert(30);
	tree.insert(20);
	tree.insert(24);
	tree.insert(22);

	tree.print_breadth_first();

	int to_find = 5;
	eric_node* result = tree.find(to_find);
	printf("to find is %d \n",to_find);

	if(result == NULL){
		printf("find failed\n");
	}else{

		if(result->parent != NULL){
			printf("parent is %d\n", result->parent->value);
		}
		if(result->l_child != NULL){
			printf("l_child is %d\n", result->l_child->value);
		}
		if(result->r_child != NULL){
			printf("r_child is %d\n", result->r_child->value);
		}
	}
}

ERIC_TEST(b_tree, remove_R_both_LR)
{
	b_tree tree;

	tree.insert(15);
	tree.insert(5);
	tree.insert(25);
	tree.insert(2);
	tree.insert(8);
	tree.insert(30);
	tree.insert(20);
	tree.insert(24);
	tree.insert(22);
	tree.insert(27);
	tree.insert(33);

	printf("before remove\n");
	tree.print_breadth_first();

	tree.remove(15);
	printf("after remove\n");
	tree.print_breadth_first();
}

ERIC_TEST(b_tree, remove_R_R)
{
	b_tree tree;

	tree.insert(15);
	tree.insert(5);
	tree.insert(25);
	tree.insert(2);
	tree.insert(8);
	tree.insert(30);
	tree.insert(20);
	tree.insert(24);
	tree.insert(22);
	tree.insert(33);

	printf("before remove\n");
	tree.print_breadth_first();

	tree.remove(15);
	printf("after remove\n");
	tree.print_breadth_first();
}

ERIC_TEST(b_tree, remove_R_L)
{
	b_tree tree;

	tree.insert(15);
	tree.insert(5);
	tree.insert(25);
	tree.insert(2);
	tree.insert(8);
	tree.insert(30);
	tree.insert(20);
	tree.insert(24);
	tree.insert(22);
	tree.insert(27);

	printf("before remove\n");
	tree.print_breadth_first();

	tree.remove(15);
	printf("after remove\n");
	tree.print_breadth_first();
}


ERIC_TEST(b_tree, remove_L_R)
{
	b_tree tree;

	tree.insert(15);
	tree.insert(5);
	tree.insert(25);
	tree.insert(2);
	tree.insert(8);
	tree.insert(30);
	tree.insert(20);
	tree.insert(24);
	tree.insert(22);

	printf("before remove\n");
	tree.print_breadth_first();

	tree.remove(15);
	printf("after remove\n");
	tree.print_breadth_first();
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

// 1. check the value like: 010;
// 2. check if L_L == ROOT_NODE
#pragma GCC diagnostic ignored "-Wenum-compare" 
ERIC_TEST(b_tree, enum_case)
{
	printf("ROOT_NODE is: %d\n", ROOT_NODE);
	printf("L_L is: %d\n", L_L);
	printf("L_R is: %d\n", L_R);
	printf("R_L is: %d\n", R_L);
	printf("R_R is: %d\n", R_R);

	int type = 010;

	if(ROOT_NODE == L_L){
		printf("ROOT_NODE == L_L\n");
	}else{
		printf("ROOT_NODE != L_L\n");
	}

	if(type == L_NODE && type == L_R){
		printf("type == L_NODE && type == L_R\n");
	}
}

ERIC_TEST(b_tree, if_delete_zero_out)
{
	b_tree *t1 = new b_tree();
	t1->insert(5);
	t1->insert(7);

	eric_node* temp = t1->get_root()->r_child;
	printf("%d\n",temp->value);
	delete temp;

	if(temp == NULL){
		printf("temp == NULL\n");
	}else if(temp != NULL){
		printf("temp != NULL\n");
	}
}

ERIC_TEST(b_tree, delete_char_array)
{
	char *array = new char[10];

	snprintf(array,10,"hello");

	printf("%s\n", array);

	delete[] array;
	if(array != NULL){
		printf("array != NULL\n");
	}
}


#pragma GCC diagnostic pop
