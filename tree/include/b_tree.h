#ifndef B_TREE_H
#define B_TREE_H
//balance tree
//

typedef enum node_type{
	ROOT_NODE   = 01,
	L_NODE      = 010,
	R_NODE 		= 0100
}node_type;

typedef struct eric_node
{
	node_type type;

	int value;
	//unsigned char depth;

	eric_node* l_child;
	eric_node* r_child;
	eric_node* parent;
}eric_node;

class b_tree
{
public:
	b_tree();
	~b_tree();

	int insert(int value);
	int remove();
	int find();
	int reset();

	// pre-order traversal
	// root->left->right
	int print_all();

	int print_breadth_first();

	int get_depth_();

private:
	eric_node* root;

	eric_node* make_node_(int value, node_type type);

	int insert_to_(eric_node* node, int value);


	int destroy_tree_();
	int destroy_node_(eric_node* node);

	int print_node_(eric_node* node);

	bool tree_is_empty_();

	// utility for printing
	int root_indent;
	int get_indent_(eric_node* node);

	int print_and_format(int item_num, int item_value);


	int line_length;//assume the terminal length per line
	int total_node;
};
#endif
