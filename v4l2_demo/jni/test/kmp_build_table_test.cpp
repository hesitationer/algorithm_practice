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
