extern "C"{

int kmp_table(const char* src, int* table, int len);
int kmp_search(const char* sub, const char* full);

}

