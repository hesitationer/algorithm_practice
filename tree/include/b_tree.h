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

	eric_node* insert(int value);
	int remove();
	int find();
	int reset();

	// pre-order traversal
	// root->left->right
	int print_all();

	int print_breadth_first();

	// height of left-subtree subs height of right-subtree
	int get_balance_factor(eric_node* node);
	int get_max_depth_(eric_node* node);
	int get_node_depth_(eric_node* node);


	eric_node* get_root();
private:
	eric_node* root;

	eric_node* make_node_(int value, node_type type);

	eric_node* insert_to_(eric_node* node, int value);


	int destroy_tree_();
	int destroy_node_(eric_node* node);

	int print_node_(eric_node* node);

	bool tree_is_empty_();

	// utility for printing
	int root_indent;
	int get_indent_(eric_node* node);

	static int s_depth;// used by get_max_depth_()
	static int s_max_depth;// used by get_max_depth_()


	int line_length;//assume the terminal length per line
	int total_node;
};
#endif
