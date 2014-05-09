#include <stdio.h>
#include <string.h>
#include "KMP_search.h"

int kmp_table(const char* W, int* T, int len)
{
	T[0] = -1;
	T[1] = 0;

	int pos = 1;
	int t = 0;
	while(pos < len - 1){ //NOTE: the (len - 1)!!!
		if(W[pos] == W[t]){
			pos++;//NOTE: first auto-increment, then reference
			t++;
			T[pos] = t;
		}
		else 
		{
			if(T[t] > 0){
				t = T[t]; //NOTE: back to check suffix
			}
			else{
				t = 0;
				pos++; //NOTE: first auto-increment, then reference
				T[pos] = t;
			}
		}
	}

	return 0;
}

int kmp_search(const char* sub, const char* full)
{
	if(sub == NULL || full == NULL){
		printf("sub == NULL || full == NULL\n");
		return -2;
	}

	unsigned int sub_len = strlen(sub);
	unsigned int full_len = strlen(full);

	if(sub_len > full_len){
		printf("sub_len > full_len\n");
		return -3;
	}

	int result = -1;

	int* T = new int[sub_len]; //table

	// build the table
	kmp_table(sub, T, sub_len);

	// search
	unsigned int pos = 0;//pos in full
	size_t idx = 0;
	while(pos + idx < full_len){

		//printf("idx is %d\n",idx);
		if(idx + 1 == strlen(sub)){
			printf("matched!\n");
			result = 1;
			break;
		}

		if(sub[idx] == full[pos + idx]){

			idx++;
		}
		else{
			pos = pos + idx - T[idx];
			if(T[idx] == -1){
				idx = 0;
			}
			else {
				idx = T[idx];
			}
		}
	}

	if(result == 1){
		if(full_len == sub_len){
			return 0;
		}
		else{
			return 1;
		}
	}

	return -1;
}
