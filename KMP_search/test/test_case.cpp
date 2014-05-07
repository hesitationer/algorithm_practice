#include <stdio.h>
#include "pick_up_test_case.h"

#include "KMP_search.h"


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wwrite-strings"
#pragma GCC diagnostic ignored "-Wunused-variable"
ERIC_TEST(KMP,kmp)
{
	char sub[16];
	char full[256];

	snprintf(sub,16,"xxxyzyyz");
	snprintf(full,256,"sdfd2wsdfmzsdfjdflsfjeofdsfs\
			xxxyzyyz\
			sdfsdferererw.xcvlaldfuiwfdf");

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
