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

int b_tree::insert(int value)
{
	insert_to_(root,value);

	total_node++;

	return 0;
}


int b_tree::insert_to_(eric_node* node, int value)
{
	//root is null
	if(tree_is_empty_())
	{
		root->value = value;
		return 0;
	}

	//cur node is null
	if(node->l_child == NULL && value <= node->value)
	{
		//node->value = value;
		eric_node* temp = make_node_(value, L_NODE);
		node->l_child = temp;//parent --> child
		temp->parent = node; //child  --> parent

		//reset
		temp = NULL;

		return 0;
	}
	else if(node->r_child == NULL && value > node->value)
	{
		//node->value = value;
		eric_node* temp = make_node_(value,R_NODE);
		node->r_child = temp;//parent --> child
		temp->parent = node; //child  --> parent

		//reset
		temp = NULL;

		return 0;
	}
	else
	{
		if(value > root->value)//go right
		{
			insert_to_(node->r_child, value);
		}
		else//go left
		{
			insert_to_(node->l_child, value);
		}
	}

	//TODO: check and keep banlance

	return 0;
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

	int item_num = 1;

	while(!queue.empty())
	{
		if(queue.front()->l_child != NULL){
			queue.push(queue.front()->l_child);
		}
		if(queue.front()->r_child != NULL){
			queue.push(queue.front()->r_child);
		}

		print_and_format(item_num, queue.front()->value);
		queue.pop();

		item_num++;
	}

	return 0;
}

int b_tree::print_and_format(int item_num,int item_value)
{
	static int depth = get_depth_();
	static int width = 5;
	static int interval = 2;
	static int max = 2 << (depth - 1);//max number in one line, e.g. last line
	static int last_line_width = max*width + (max - 1)*interval;
	static int fix_indent = (int)(line_length - last_line_width)/2;

	static int cur_depth = 1;
	if((item_num + 1) == (1 << cur_depth))//1, 3, 5... last item of per line
	{
		//printf("item_num:%d",item_num);
		cur_depth++;

		int cur_line_item = (1 << depth) - 1;
		int cur_used_width = cur_line_item*width  + (cur_line_item - 1)*interval;
		int cur_line_indent = fix_indent + (last_line_width - cur_used_width)/2;
		for(int i = 0; i < cur_line_indent; ++i){
			printf(" ");
		}
	}
	printf("%5d",item_value);
	printf("  ");//the 2-space interval

	if((item_num + 1) == (1 << (cur_depth - 1)))//1, 3, 5... last item of per line
	{
		printf("\n");
	}

	return 0;
}

int b_tree::get_depth_()
{
	if(total_node == 0)
	{
		return 0;
	}

	if(total_node == 1)
	{
		return 1;
	}

	int depth = 0;
	int temp = total_node;

	do{
		temp = temp >> 1;
		
		depth++;
	}while(temp);

	return depth;
}
