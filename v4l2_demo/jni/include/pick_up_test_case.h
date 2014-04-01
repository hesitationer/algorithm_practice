#include <string.h>
#include <stdio.h>

typedef void (*test_fun)();
int PUSH_CASE(char* unit_name, char* case_name, test_fun f_ptr);

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

// check if:
// 1. strcmp(sub_string, target_string) == 0
// 2. sub_string is part of target_string
//
// return: 
// 	0  exactly equal
// 	1  sub is part of target 
// 	-1 not either
int match_or_part_match(char *sub_string, char* target_string);
// the KMP version of fuzzy-match
int match_or_part_match_KMP_v1(const char* sub_string, const char* target_string);

int run_selected_case();
int RUN_ERIC_CASE(int argc, char** argv);


extern int case_num;
extern char all_case[256][64];
extern test_fun fun_ptr[256];
