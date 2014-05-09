#include <stdio.h>
#include "pick_up_test_case.h"

#include "KMP_search.h"


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wwrite-strings"
#pragma GCC diagnostic ignored "-Wunused-variable"
ERIC_TEST(KMP,table)
{
	char W[18] = {0};
	int  T[16] = {0};	

	//snprintf(W,16,"xxxyzyyz");
	//snprintf(W, 18, "AAAABCDAAAAABCGN");
	snprintf(W, 18, "ababababca");



	kmp_table(W, T, strlen(W));

	//print
	printf("P: |");
	for(int i = 0; i < 16; ++i){
		printf("%4d |",i);
	}
	printf("\n");

	printf("W: |");
	for(int i = 0; i < 16; ++i){
		printf("%4c |",W[i]);
	}
	printf("\n");

	printf("T: |");
	for(int i = 0; i < 16; ++i){
		printf("%4d |",T[i]);
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

	int ret = kmp_search(sub,full);
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
#pragma GCC diagnostic pop
