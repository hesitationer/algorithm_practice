#include "pick_up_test_case.h"

//store all case_name
int case_num = 0;
char all_case[256][64] = {0};

//store the function ptr
test_fun fun_ptr[256] = {0};

// Use assignment to run this function. tricky
static int PUSH_CASE(char* unit_name, char* case_name, test_fun f_ptr)
{ 
	char *temp = all_case[case_num]; 
	strncpy(temp, case_name, strlen(case_name)); 

	fun_ptr[case_num] = f_ptr;

	case_num++;

	return 0;//whatever
} 

static char key[32] = {};
static char v1[256] = {};
static char v2[256] = {};

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

		printf("\nkey:%s\n",key);


		//get the 1st part of value
		temp++;//move to the key start
		while(*temp != '.' && temp != NULL)
		{
			*value_1st++ = *temp++;
		}

		printf("\nvalue_1st:%s\n",v1);

		//get the 2nd part of value
		temp++;
		while(*temp != 0)
		{
			*value_2nd++ = *temp++;
		}

		printf("value_2nd:%s\n",v2);

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

int RUN_ERIC_CASE(int argc, char** argv) 
{										
	if(argc == 1)	  
	{  
		printf("no filtern");  
	}  
	else if(argc == 2)  
	{  
		parse_options(argv[1]);  
		store_options();  
  
		for(int i = 0; i < 3; ++i)  
		{  
			printf("%sn",op_filter[i]);  
		}  

		//TODO: find the function-ptr by op_filter and run the case
	}  
}										
