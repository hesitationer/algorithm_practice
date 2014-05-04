#include <stdio.h>
#include <queue>
#include <stack>
#include "b_tree.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wwrite-strings"
char *prefix_width[10] = {
	"\t",
	"\t\t",
	"\t\t\t",
	"\t\t\t\t",
	"\t\t\t\t\t",
	"\t\t\t\t\t\t",
	"\t\t\t\t\t\t\t",
	"\t\t\t\t\t\t\t\t",
	"\t\t\t\t\t\t\t\t\t",
	"\t\t\t\t\t\t\t\t\t\t"
};

char line_indent[6] = {40,32,24,16,8,0};

#pragma GCC diagnostic pop

int b_tree::s_depth = 1;
int b_tree::s_max_depth = 1;

using namespace std;

b_tree::b_tree():
	root(NULL),
	root_indent(5),
	line_length(128),
	total_node(0)
{
	//init root node
	root = make_node_(-1,ROOT_NODE);
}

b_tree::~b_tree()
{
	destroy_tree_();
}

eric_node* b_tree::insert(int value)
{
	eric_node* new_node = NULL;
	new_node = insert_to_(root,value);
	if(new_node == NULL){
		printf("insert %d error\n",value);
	}

	//TODO: check and keep banlance

	total_node++;

	return new_node;
}


eric_node* b_tree::insert_to_(eric_node* node, int value)
{
	// NOTE the static key-word !!
	// It's neccessary to keep(hold to not change) the new_node value when 
	// return from the call in which insert operation happened
	static eric_node* new_node = NULL;

	//root is null
	if(tree_is_empty_())
	{
		root->value = value;
		return root;
	}

	//cur node is null
	if(node->l_child == NULL && value <= node->value)
	{
		//node->value = value;
		new_node = make_node_(value, L_NODE);
		node->l_child = new_node;//parent --> child
		new_node->parent = node; //child  --> parent


		return new_node;// this will causes return from recusively
	}
	else if(node->r_child == NULL && value > node->value)
	{
		//node->value = value;
		new_node = make_node_(value,R_NODE);
		node->r_child = new_node;//parent --> child
		new_node->parent = node; //child  --> parent

		return new_node;
	}
	else
	{
		if(value > node->value)//go right
		{
			insert_to_(node->r_child, value);
		}
		else//go left
		{
			insert_to_(node->l_child, value);
		}
	}

	return new_node;
}

eric_node* b_tree::make_node_(int value, node_type type)
{
	eric_node* temp = new eric_node;

	temp->type = type;

	temp->value = value;

	temp->parent = NULL;
	temp->l_child = NULL;
	temp->r_child = NULL;

	return temp;
}

int b_tree::destroy_tree_()
{
	destroy_node_(root);

	return 0;
}

int b_tree::destroy_node_(eric_node* node)
{
	if(node->l_child == NULL && node->r_child == NULL)
	{
		//update parent
		if(node->parent != NULL)
		{
			if(node->parent->l_child == node)
			{
				node->parent->l_child = NULL;
			}
			else if(node->parent->r_child == node)
			{
				node->parent->r_child = NULL;
			}
			else
			{
				printf("node doesn't contained by it's parent!\n");
				return -1;
			}
		}

		//delete
		delete node;
	}
	else
	{
		if(node->l_child != NULL){
			destroy_node_(node->l_child);
		}
		if(node->r_child != NULL){
			destroy_node_(node->r_child);
		}

		//try again the parent
		destroy_node_(node);
	}

	return 0;
}

int b_tree::print_all()
{
	print_node_(root);	
	return 0;
}

int b_tree::print_node_(eric_node* node)
{
	if(node->type == ROOT_NODE)
	{
		printf("%s node value: %d\n",prefix_width[root_indent],node->value);
	}
	else
	{

		printf("%s node value: %d",prefix_width[get_indent_(node)],node->value);
	}

	if(node->l_child != NULL)
	{
		print_node_(node->l_child);
	}
	if(node->r_child != NULL)
	{
		print_node_(node->r_child);
	}

	return 0;
}

bool b_tree::tree_is_empty_()
{
	return (root->value == -1 && root->l_child == NULL && root->r_child == NULL);
}

int b_tree::get_indent_(eric_node* node)
{
	if(node->type == ROOT_NODE)
	{
		return root_indent;
	}

	unsigned char l_r_order[20] = {0};
	eric_node* temp = node;
	int depth = 0;

	l_r_order[depth] = temp->type;


	while(temp->parent->type != ROOT_NODE)
	{
		temp = temp->parent;

		depth++;
		l_r_order[depth] = temp->type;
	}

	int indent = root_indent;
	for(int i = depth; i >= 0; i-- )
	{
		if(l_r_order[depth] == L_NODE) {
			indent--;
		}
		else if(l_r_order[depth] == R_NODE){
			indent++;
		} 
	}

	return indent;
}

int b_tree::print_breadth_first()
{
	queue<eric_node*> queue;

	queue.push(root);

	int pre_depth = -1;
	int now_depth = 0;

	int item_num = 1;

	printf("=============start output============\n");
	while(!queue.empty())
	{
		if(queue.front()->l_child != NULL){
			queue.push(queue.front()->l_child);
		}
		if(queue.front()->r_child != NULL){
			queue.push(queue.front()->r_child);
		}


		now_depth = get_node_depth_(queue.front());
		if(now_depth != pre_depth){
			printf("\n");
			pre_depth = now_depth;
		}

		if(queue.front()->parent == NULL){
			printf("%d  ", queue.front()->value);
		} else{
			printf("%d(%d)  ", queue.front()->value, queue.front()->parent->value);
		}
		queue.pop();

		item_num++;
	}

	printf("\n=============end output============\n");
	return 0;
}


int b_tree::get_balance_factor(eric_node* node)
{
	int l_depth = 1;
	int r_depth = 1;

	if(node->l_child != NULL){
		l_depth = get_max_depth_(node->l_child);

		//reset static member:  depth/max_depth
		s_depth = 1;
		s_max_depth = 1;
	}
	if(node->r_child != NULL){
		r_depth = get_max_depth_(node->r_child);

		//reset static member:  depth/max_depth
		s_depth = 1;
		s_max_depth = 1;
	}

	int factor = l_depth - r_depth;

	
	return factor;
}

int b_tree::get_max_depth_(eric_node* node)
{
	//update the depth
	++s_depth;
	//printf("node-depth: (%d, %d)\n", node->value, s_depth);
	if(s_depth > s_max_depth){
		s_max_depth = s_depth;
		//printf("update max: %d\n",s_max_depth);
	}

	if(node->l_child != NULL){
		get_max_depth_(node->l_child);
	}
	if(node->r_child != NULL){
		get_max_depth_(node->r_child);
	}


	// end one call of `get_max_depth_`
	--s_depth;
	//printf("end call (node,value):(%d,%d)\n\n", node->value, s_depth);

	return s_max_depth;
}

eric_node* b_tree::get_root()
{
	return root;
}

int b_tree::get_node_depth_(eric_node* node)
{
	int depth = 1;

	if(node->type == ROOT_NODE){
		return 1;
	}

	eric_node* temp = node;
	while(temp->type != ROOT_NODE){
		temp = temp->parent;
		depth++;
	}

	return depth;
}
