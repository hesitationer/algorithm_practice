#include "malloc.h"

#include "pick_up_test_case.h"

extern "C"
{
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

int build_kmp_table_my(const char* w, int* table, int len)
{
	if(w == NULL)
	{
		printf("w == NULL!\n");
		return -1;
	}

	table[0] = -1;
	for(int i = 1; i < len; ++i)
	{
		if(w[i] != w[0])
		{
			table[i] = 0;
			continue;
		}

		int matched_before_cur = 0;
		const char *p1 = w;
		const char *p2 = w + i;
		while(*p1++ == *p2++)
		{
			table[i++] = matched_before_cur;
			matched_before_cur++;
		}

		table[i] = matched_before_cur;
	}

	return 0;
}

int build_kmp_table_wiki(const char* w, int* table, int len)
{
	if(w == NULL || table == NULL)
	{
		printf("w == NULL || table == NULL\n");
		return -1;
	}

	table[0] = -1; table[1] = 0;
	int pos = 2,cnd = 0;

	while(pos < len)
	{
		if(w[pos - 1] == w[cnd])
		{
			cnd++;
			table[pos] = cnd;
			pos++;
		}	
		else if(cnd > 0)
		{
			cnd = table[cnd];
		}
		else
		{
			table[pos] = 0;
			pos++;
		}
	}

	return 0;
}

int match_or_part_match_KMP_v1(const char* sub_string, const char* target_string)
{
	printf("\n======enter KMP====\n");
	printf("sub: %s\n",sub_string);
	printf("target: %s\n",target_string);

	//input check
	if(sub_string == NULL || target_string == NULL)
	{
		printf("sub_string or target_string is NULL!\n");
		return -2;
	}

	int sub_len = strlen(sub_string);
	int target_len = strlen(target_string);
	if(sub_len > target_len)
	{
		printf("sub_len > target_len\n");
		return -1;
	}

	//KMP

	//KMP PART 1 ---Build the table
	int *kmp_table = (int*)malloc(sub_len*sizeof(int));
	build_kmp_table_wiki(sub_string,kmp_table,sub_len);


	//KMP Part 2 ---Start the Search
	int m = 0; //cur pos in target_string
	int i = 0;	//cur pos in sub_string 

	int ret = -1;//suppose neither equal nor part at start
	while(m + i < target_len)
	{
		if(target_string[m + i] == sub_string[i])
		{
			if(i == sub_len - 1)//get to the end of sub_string
			{
				ret = 1;
				break;
			}

			i++;
		}
		else
		{
			//update m and i
			m = m + i - kmp_table[i]; 
			if(kmp_table[i] == -1)//the first time
			{
				i = 0;
			}
			else
			{
				i = kmp_table[i];
			}
		}
	}

	return ret;
}

int match_or_part_match(char *sub_string, char* target_string)
{
	//input check
	if(sub_string == NULL || target_string == NULL)
	{
		printf("sub_string or target_string is NULL!\n");
		return -2;
	}

	int sub_len = strlen(sub_string);
	int target_len = strlen(target_string);

	int ret = strcmp(sub_string, target_string);
	if(ret == 0)
	{
		return 0;
	}
	else if(sub_len >= target_len)//no chance to be part
	{
		return -1;
	}
	else
	{
		//check if A is part B
		//char *p1 = sub_string;
		//char *p2 = target_string;

		// suppose:
		// strlen(A) is 2
		// strlen(B) is 3
		// the valide range is [0, 3-2] 
		for(int i = 0; i <= target_len - sub_len; ++i)
		{
			if(target_string[i] != sub_string[0])
			{
				continue;
			}


			//compare from target_string[i]
			char *p1 = sub_string;
			char *p2 = target_string + i;
			while(*p1 != 0)//till the end of sub_string
			{
				if(*p1 == *p2)
				{
					p1++;
					p2++;
				}
				else
				{
					break;
				}
			}//end while(*p1 != 0)

			if(*p1 == 0)
			{
				return 1;
			}
		}// end for(i = 0; i < target_len - sub_len; ++i)

		return -1;

	}// end for else
}

int run_selected_case()
{
	int case_index = -1;
	//find index of case in all_case[][]
	for(int i = 0; i < case_num; ++i)
	{
		if(match_or_part_match_KMP_v1(filter_v2, all_case[i]) >= 0)
		//if(match_or_part_match(filter_v2, all_case[i]) >= 0)
		{
			printf("\nselect case: %s--%s\n",filter_v2,all_case[i]);
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

	return 0;
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

	return 0;
}										

}//end extern "C"
