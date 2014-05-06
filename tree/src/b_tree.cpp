#include <stdio.h>
#include <queue>
#include <stack>
#include "b_tree.h"


int b_tree::s_depth = 1;
int b_tree::s_max_depth = 1;

using namespace std;

b_tree::b_tree():
	root(NULL),
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

	// Check and keep banlance
	// Now just check the grand-parent node of new-inserted
	int balance_factor = 0;
	balance_factor = get_balance_factor(root);
	if(balance_factor > 1 || balance_factor < -1)
	{
		if(new_node->parent->parent != NULL){
			rotate_node(new_node->parent->parent);
		}
	}

	total_node++;

	return new_node;
}

eric_node* b_tree::find(int value)
{
	eric_node* target = NULL;

	target = root;
	while(target != NULL){
		if(value == target->value){
			break;
		}else{
			if(value > target->value){

				target = target->r_child;
			}else{

				target = target->l_child;
			}
		}
	}

	if(target == NULL){
		printf("can't find node of given value: %d\n",value);
	}
	return  target;
}

int b_tree::remove_back(int value)
{
	//find 
	eric_node* target = NULL;
	target = find(value);
	if(target == NULL){
		printf("the node of value %d doesn't exist!\n",value);
		return -1;
	}

	// Check the height of left/right sub-tree
	int l_height = 0, r_height = 0;
	if(target->l_child != NULL){
		l_height = get_max_depth_(target->l_child);
	}
	if(target->r_child != NULL){
		r_height = get_max_depth_(target->r_child);
	}

	// Upgrade the higher sub-tree. 
	// Thus the balance factor tends to be small
	//
	if(l_height <= r_height){// upgrade the right-subtree

		upgrade_right_child(target->r_child);
	}
	else if(l_height > r_height){// upgrade the left-subthree

		upgrade_left_child(target->l_child);
	}
	return 0;
}

int b_tree::remove(int value)
{
	//find 
	eric_node* target = NULL;
	target = find(value);
	if(target == NULL){
		printf("the node of value %d doesn't exist!\n",value);
		return -1;
	}

	int l_height = 0, r_height = 0;
	if(target->l_child != NULL){
		l_height = get_max_depth_(target->l_child);

		//reset static member:  depth/max_depth
		s_depth = 1;
		s_max_depth = 1;
	}
	if(target->r_child != NULL){
		r_height = get_max_depth_(target->r_child);

		//reset static member:  depth/max_depth
		s_depth = 1;
		s_max_depth = 1;
	}

	// decide direction from which to upgrade after removing
	rm_direction dir;
	if(l_height > r_height){

		dir = L_REMOVE;
	}
	else if(l_height <= r_height){

		dir = R_REMOVE;
	}

	// remove
	remove_node(target, dir);

	return 0;
}

int b_tree::remove_node(eric_node* node, rm_direction dir)
{
	if(node->l_child == NULL && node->r_child == NULL){// leaf-node

		eric_node* parent = node->parent;
		if(parent != NULL){// has parent

			if(node->type == L_NODE){// node is left child

				parent->l_child = NULL;
			}
			else if(node->type == R_NODE){// node is right child

				parent->r_child = NULL;
			}
		}

		delete node;

		// check the balance factor of node->parent
		int factor = get_balance_factor(parent);
		if(factor > 1 || factor < -1){
			rotate_node(parent);
		}

		printf("return from if\n");
		return 0;
	}

	if(dir == L_REMOVE){

		if(node->l_child != NULL){

			node->value = node->l_child->value;
			dir = dir;// NOTE: do NOT change dir!!
			remove_node(node->l_child, dir);
		}
		else if(node->l_child == NULL && 
				node->r_child != NULL){

			node->value = node->r_child->value;
			dir = R_REMOVE;// NOTE: change dir!!
			remove_node(node->r_child, dir);
		}
	}
	else if(dir == R_REMOVE){

		if(node->r_child != NULL){

			node->value = node->r_child->value;
			dir = dir;// NOTE: do NOT change dir!!
			remove_node(node->r_child, dir);
		}
		else if(node->r_child == NULL && 
				node->l_child != NULL){

			node->value = node->l_child->value;
			dir = L_REMOVE;// NOTE: change dir!!
			remove_node(node->l_child, dir);
		}
	}



	//printf("node:l_depth:r_depth: (%d,%d,%d) l_child:r_child: (%d,%d)\n", 
			//node->value, l_height, r_height,node->l_child->value, node->r_child->value);
	//if(l_height > r_height){

	//	node->value = node->l_child->value;
	//	remove_node(node->l_child);	
	//}
	//else if(l_height <= r_height){

	//	node->value = node->r_child->value;
	//	remove_node(node->r_child);
	//}

	return 0;
}

int b_tree::upgrade_right_child(eric_node* node)
{
	eric_node* parent = node->parent;
	if(parent == NULL){
		printf("node doesn't has parent can't upgrade!\n");
		return -1;
	}else{
		// overwrite the parent first
		parent->value = node->value;
	}


	// upgrade the r_child
	eric_node* temp = node;
	while(temp != NULL){

		if(temp->r_child != NULL){ //still has r_child

			temp->value = temp->r_child->value;
			temp = temp->r_child;
		}
		else{ //doesn't has r_child

			if(temp->l_child != NULL){

				// temp->l_child  --->  temp
				// this will lead `r_child < temp->parent`
				temp->value = temp->l_child->value; 

				//swap<temp,temp->parent>
				int swap = temp->parent->value;
				temp->parent->value = temp->value;
				temp->value = swap;

				// delete the l_child. THE END
				// Here Must promise the current l_child is leaf child
				delete temp->l_child;
				temp->l_child = NULL;
				break;

			}
			else if(temp->l_child == NULL){//the leaf node. THE END.
				
				temp->parent->r_child = NULL;
				delete temp;
				temp = NULL;
			}
		}
	}

	return 0;
}

int b_tree::upgrade_left_child(eric_node* node)
{
	if(node->parent == NULL){
		printf("node doesn't has parent can't be upgreaded!\n");
		return -1;
	}else{
		node->parent->value = node->value;
	}

	// upgrade the l_child
	eric_node* temp = node;
	while(temp != NULL){

		if(temp->l_child != NULL){

			temp->value = temp->l_child->value;
			temp = temp->l_child;
		}
		else {
			if(temp->r_child != NULL){

				// this will lead `l_child > temp->parent`
				temp->value = temp->r_child->value;

				//swap<temp, temp->parent>
				int swap = temp->parent->value;
				temp->parent->value = temp->value;
				temp->value = swap;

				// delete temp->r_child. THE END
				// which is a leaf-node
				delete temp->r_child;
				temp->r_child = NULL;
				break;
			}
			else if(temp->r_child == NULL){// leaf node. THE END

				temp->parent->l_child = NULL;
				delete temp;
				temp = NULL;
			}
		}
	}

	return 0;
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
		//if(node->type == L_NODE){
		//	node->parent->l_child == NULL;
		//}else if(node->type == R_NODE){
		//	node->parent->r_child == NULL;
		//}

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
				printf("node %d doesn't contained by it's parent!\n", node->value);
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

bool b_tree::tree_is_empty_()
{
	return (root->value == -1 && root->l_child == NULL && root->r_child == NULL);
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
		//printf("in while\n");
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
		}
		else{
			char id[2] = {0}; 
			node_type type = queue.front()->type;
			if(type == L_NODE){
				snprintf(id,2,"l");	
			}
			else if(type == R_NODE){
				snprintf(id,2,"r");	
			}
			printf("%s-%d(%d)  ", id,queue.front()->value, queue.front()->parent->value);
		}

		queue.pop();

		item_num++;
	}

	printf("\n=============end output============\n");
	return 0;
}


int b_tree::get_balance_factor(eric_node* node)
{
	if(node == NULL){
		printf("node is null in get_balance_factor\n");
		return -1;
	}
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

int b_tree::rotate_node(eric_node* node)
{
	unbalance_type type = get_unbalance_type(node);

	if(type == L_L){

		rotate_from_L_L(node);
	}
	else if(type == L_R){
		
		rotate_from_L_R(node);
	}
	else if(type == R_L){

		rotate_from_R_L(node);
	}
	else if(type == R_R){

		rotate_from_R_R(node);
	}
	else if(type == L_BOTH){

		rotate_from_L_BOTH(node);
	}
	else if(type == R_BOTH){

		rotate_from_R_BOTH(node);
	}
	return 0;
}

unbalance_type b_tree::get_unbalance_type(eric_node* node)
{
	unbalance_type type;

	if(node->l_child != NULL){
		if(node->l_child->l_child != NULL && 
				node->l_child->r_child == NULL){ // L-L

			type = L_L;
		}
		else if(node->l_child->r_child != NULL && 
				node->l_child->l_child == NULL){// L-R

			type = L_R;
		}
		else if(node->l_child->l_child != NULL &&
				node->l_child->r_child != NULL){// L-(LR)

			type = L_BOTH;
		}
	}
	else if(node->r_child != NULL){
		if(node->r_child->r_child != NULL && 
				node->r_child->l_child == NULL){// R_R

			type = R_R;
		}
		else if(node->r_child->l_child != NULL && 
				node->r_child->r_child == NULL){// R_L

			type = R_L;	
		}
		else if(node->r_child->l_child != NULL &&
				node->r_child->r_child != NULL){// R_(LR)

			type = R_BOTH;
		}
	}

	return type;
}


int b_tree::rotate_from_L_L(eric_node* up_node)
{
	eric_node* center_node = up_node->l_child; 

	// process the parent of up_node
	if(up_node->type == L_NODE){
		up_node->parent->l_child = center_node;
	}else if(up_node->type == R_NODE){
		up_node->parent->r_child = center_node;
	}

	// process center
	center_node->r_child = up_node; //center --> up
	center_node->parent = up_node->parent;

	// process the 'up_node', now it's child of center
	//
	up_node->parent = center_node;  //up --> center
	up_node->l_child = NULL;




	return 0;
}
int b_tree::rotate_from_L_R(eric_node* up_node)
{
	// FIRST: rotate to L_L type
	eric_node* center_node = up_node->l_child;
	eric_node* down_node   = center_node->r_child;

	// up_node
	up_node->l_child = down_node;

	// down_node (after change will be center)
	down_node->parent = up_node;
	down_node->l_child = center_node;

	// center_node(after change will be down_node)
	center_node->parent = down_node;
	center_node->r_child = NULL;
		
	// SECOND: process as L_L
	rotate_from_L_L(up_node);

	return 0;
}
int b_tree::rotate_from_R_L(eric_node* up_node)
{
	// FIRST: rotate to R_R type
	eric_node* center_node = up_node->r_child;
	eric_node* down_node = center_node->l_child;

	// up_node
	up_node->r_child = down_node;

	// down_node (after change, will be center-node)
	down_node->parent = up_node;
	down_node->r_child = center_node;

	// center_node (after change, will be down-node)
	center_node->parent = down_node;
	center_node->l_child = NULL;	

	// SECOND: process as R_R
	rotate_from_R_R(up_node);

	return 0;
}
int b_tree::rotate_from_R_R(eric_node* up_node)
{
	eric_node* center_node = up_node->r_child;

	// process the parent of up_node
	if(up_node->type == L_NODE){
		up_node->parent->l_child = center_node;
	}else if(up_node->type == R_NODE){
		up_node->parent->r_child = center_node;
	}

	// process the center-node
	center_node->l_child = up_node;
	center_node->parent = up_node->parent;

	// process the up-node(now it's the l_child of center-node)
	up_node->parent = center_node;
	up_node->r_child = NULL;

	return 0;
}

int b_tree::rotate_from_L_BOTH(eric_node* up_node)
{
	/*
	 *			A					B
	 *		  B  	     ===>    C     D 
	 *		C    D						 A
	 * */

	eric_node* A = up_node;

	eric_node* B = A->l_child;

	eric_node* D = B->r_child;

	// A->parent
	if(A->type == L_NODE){
		A->parent->l_child = B;
	}
	else if(A->type == R_NODE){
		A->parent->r_child = B;
	}

	// B
	B->parent = A->parent;

	// D
	D->r_child = A;

	// A
	A->parent = D;
	A->l_child = NULL;

	return 0;
}

int b_tree::rotate_from_R_BOTH(eric_node* up_node)
{
	/*
	 *			A					B
	 *		      B     ===>      C   D 
	 *		    C   D			A
	 * */

	eric_node* A = up_node;

	eric_node* B = A->r_child;

	eric_node* C = B->l_child;

	// A->parent
	if(A->type == R_NODE){

		A->parent->r_child = B;
	}
	else if(A->type == L_NODE){

		A->parent->l_child = B;
	}

	// B
	B->parent = A->parent;

	// C
	C->r_child = A;

	// A
	A->parent = C;
	A->r_child = NULL;

	return 0;
}
