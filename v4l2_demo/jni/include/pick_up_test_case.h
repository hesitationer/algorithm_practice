#include <string.h>
#include <stdio.h>

typedef void (*test_fun)();
static int PUSH_CASE(char* unit_name, char* case_name, test_fun f_ptr);

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

int parse_options(char* filter_str);
int store_options();

int RUN_ERIC_CASE(int argc, char** argv);


extern int case_num;
extern char all_case[256][64];
extern test_fun fun_ptr[256];
