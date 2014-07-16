#ifndef B_TREE_H
#define B_TREE_H
//balance tree
//

// NOTE: start with 0 means these are Octal numbers (base 8)
typedef enum node_type{
	ROOT_NODE   = 01,
	L_NODE      = 010,
	R_NODE 		= 0100
}node_type;

typedef enum unbalance_type{
	L_L = 01,
	L_R = 010,
	R_L = 0100,
	R_R = 01000,
	L_BOTH = 010000,
	R_BOTH = 0100000,
}unbalance_type;

typedef enum rm_direction{
	L_REMOVE = 01,
	R_REMOVE = 010
}rm_direction;

typedef struct eric_node
{
	node_type type;

	char key[256];
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

	b_tree(const b_tree &copy);

	eric_node* insert(int value);
	int remove_back(int value);
	int remove(int value);
	int remove(eric_node* node_to_del);
	eric_node* find(int value);
	int reset();

	int print_breadth_first();

	// height of left-subtree subs height of right-subtree
	int get_balance_factor(eric_node* node);
	int get_max_depth_(eric_node* node);
	int get_node_depth_(eric_node* node);

	int check_and_rebalance();
	int rotate_node(eric_node* node);
	unbalance_type get_unbalance_type(eric_node* node);
	int rotate_from_L_L(eric_node* node);
	int rotate_from_L_R(eric_node* node);
	int rotate_from_R_L(eric_node* node);
	int rotate_from_R_R(eric_node* node);
	int rotate_from_L_BOTH(eric_node* node);
	int rotate_from_R_BOTH(eric_node* node);

	eric_node* get_root();

	int upgrade_right_child(eric_node* node);
	int upgrade_left_child(eric_node* node);

	int remove_node(eric_node* node, rm_direction dir);

private:

	eric_node* make_node_(int value, node_type type);
	eric_node* insert_to_(eric_node* node, int value);

	int destroy_tree_();
	int destroy_node_(eric_node* node);
	bool tree_is_empty_();

private:
	eric_node* root;

	int total_node;

	static int s_depth;// used by get_max_depth_()
	static int s_max_depth;// used by get_max_depth_()
};
#endif
