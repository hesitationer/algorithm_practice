#include "pick_up_test_case.h"

//store all case_name
int case_num = 0;
char all_case[256][64] = {0};

//store the function ptr
test_fun fun_ptr[256] = {0};

// Use assignment to run this function. tricky
int PUSH_CASE(char* unit_name, char* case_name, test_fun f_ptr)
{ 
	char *temp = all_case[case_num]; 
	strncpy(temp, case_name, strlen(case_name)); 

	fun_ptr[case_num] = f_ptr;

	case_num++;

	return 0;//whatever
} 

// the parse result from cmd line
static char key[32] = {};
static char v1[256] = {};
static char v2[256] = {};

//Usage: ./my_bin --filter=xxxx.yyyy
// the filter option.
// --key: filter
// --v1:  test_unit
// --v2:  test_case
static char filter_key[32] = {};
static char filter_v1[256] = {};
static char filter_v2[256] = {};
static char* op_filter[3] = {filter_key,filter_v1,filter_v2};


int parse_options(char* filter_str)
{

	char *key_ptr = key;
	char *temp = filter_str + 2;
	char *value_1st = v1;
	char *value_2nd = v2;


	if(filter_str[0] == '-' && filter_str[1] == '-')
	{
		//get the key 
		while(*temp != '=' && temp != NULL)
		{
			*key_ptr++ = *temp++;
		}

		//get the 1st part of value
		temp++;//move to the key start
		while(*temp != '.' && temp != NULL)
		{
			*value_1st++ = *temp++;
		}

		//get the 2nd part of value
		temp++;
		while(*temp != 0)
		{
			*value_2nd++ = *temp++;
		}
	}
	else
	{
		printf("invalid option: %s!\n",filter_str);
	}


	//reset pointer
	key_ptr = NULL;
	temp = NULL;
	value_1st = NULL;
	value_2nd = NULL;	

	return 0;
}


int store_options()
{
	if(strcmp(key,"filter") == 0)
	{
		//copy
		strncpy(filter_key,key,sizeof(key));
		strncpy(filter_v1,v1,sizeof(v1));
		strncpy(filter_v2,v2,sizeof(v2));

		memset(key, 0, sizeof(key));
		memset(v1, 0, sizeof(v1));
		memset(v2, 0, sizeof(v2));

	}
	else if(strcmp(key,"repeat") == 0)
	{
	}
	else
	{
		printf("invalid options:%s\n",key);
	}

	return 0;
}

int run_selected_case()
{
	int case_index = -1;
	//find index of case in all_case[][]
	for(int i = 0; i < case_num; ++i)
	{
		if(strcmp(all_case[i], filter_v2) == 0)
		{
			case_index = i;
			break;
		}
	}

	if(case_index == -1)
	{
		printf("not found the given case: %s\n",filter_v2);
		return -1;
	}

	//call fun_ptr by index from fun_ptr[]
	fun_ptr[case_index]();
}

int RUN_ERIC_CASE(int argc, char** argv) 
{										
	if(argc == 1)	  
	{  
		printf("no filter\n");  
	}  
	else if(argc == 2)  
	{  
		parse_options(argv[1]);  
		store_options();  

		//run the selected case
  
		for(int i = 0; i < 3; ++i)  
		{  
			printf("%s\n",op_filter[i]);  
		}  

		//TODO: find the function-ptr by op_filter and run the case
		run_selected_case();
	}  
}										
