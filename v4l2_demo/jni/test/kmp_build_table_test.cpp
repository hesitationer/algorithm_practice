#include "pick_up_test_case.h"

ERIC_TEST(kmp, table)
{
	printf("test kmp:table\n");

	char w[13] = {'A','A','A','B','C','D','A','A','A','A','B','C','D'};
	int T1[13] = {0};
	int T2[13] = {0};
	int len = 13;

	build_kmp_table_my(w,T1,13);
	build_kmp_table_wiki(w,T2,13);

	printf("|");
	for(int i = 0; i < 13; ++i)
	{
		printf("%2c | ",w[i]);
	}
	printf("\n");

	printf("|");
	for(int i = 0; i < 13; ++i)
	{
		printf("%2d | ",T1[i]);
	}
	printf("\n");

	printf("|");
	for(int i = 0; i < 13; ++i)
	{
		printf("%2d | ",T2[i]);
	}
	printf("\n");

}

ERIC_TEST(KMP,search)
{
	char sub[16];
	char full[256];

	snprintf(sub,16,"xxxyzyyz");
	snprintf(full,256,"sdfd2wsdfmzsdfjdflsfjeofdsfsxxxyzyyzsdfsdferererwxcvlaldfuiwfdf");
	//snprintf(sub,16,"abc");
	//snprintf(full,256,"abcdefg");

	for(size_t i = 0; i < strlen(full); ++i){
		if(i%9 == 0){
			printf("\n");
		}
		printf("%c |",full[i]);
	}
	printf("\n");

	int ret = match_or_part_match_KMP_v1(sub,full);
	if(ret == 0){
		printf("exactly same!\n");
	}
	else if(ret == 1){
		printf("partion matched!\n");
	}
	else if(ret == -1){
		printf("NOT matched!\n");
	}
}
