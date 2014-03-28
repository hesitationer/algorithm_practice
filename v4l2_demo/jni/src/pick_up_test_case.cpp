#include <string.h>
#include <stdio.h>
//store all case_name
static int case_num = 0;
static char all_case[256][64] = {0};

//store the function ptr
typedef void (*test_fun)();
static test_fun fun_ptr[256] = {0};

// Use assignment to run this function. tricky
static int PUSH_CASE(char* unit_name, char* case_name, test_fun f_ptr)
{ 
	char *temp = all_case[case_num]; 
	strncpy(temp, case_name, strlen(case_name)); 

	fun_ptr[case_num] = f_ptr;

	case_num++;

	return 22;//whatever
} 


//#define PUSH_CASE push_case


#define MAKE_NAME_IMPL(unit_name,case_name,idx) \
	_##unit_name##_##case_name##_##idx

#define MAKE_NAME(unit_name,case_name,idx) \
	MAKE_NAME_IMPL(unit_name,case_name,idx)

#define MAKE_FUNCTION(v1,v2) \
	MAKE_NAME(v1,v2, __LINE__)


//generate the function
#define ERIC_TEST(test_unit, test_case) \
	static void MAKE_FUNCTION(test_unit, test_case)(void); \
	static int MAKE_NAME(test_unit,test_case_,__LINE__ )= \
	PUSH_CASE(#test_unit,#test_case,MAKE_FUNCTION(test_unit,test_case)); \
	static void MAKE_FUNCTION(test_unit, test_case)(void) 


ERIC_TEST(unit_01,case_01)
{
	printf("this is case 01\n");
}

ERIC_TEST(unit_02,case_02)
{
	printf("this is case 02\n");
}

ERIC_TEST(never,ever)
{
	printf("this is case never_ever\n");
}

int pick_case()
{
	//RUN_ERIC_CASE(unit_01,case_01);

	printf("this is pick_case\n");
	
	return 0;
}


int main()
{

	for(int i = 0; i < case_num; ++i)
	{
		printf("case_num:%d, %s\n\n",i,all_case[i]);
		printf("---------------call:--------\n");
		fun_ptr[i]();
		printf("-----------call end:--------\n");
	}


	return 0;
}
