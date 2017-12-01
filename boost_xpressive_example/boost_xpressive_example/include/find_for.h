#ifndef find_for_h__
#define find_for_h__

#include <string>
#include <unordered_map>
#include <vector>

void example_find_sub_string();
void find_for_use_earse_in_body();
std::unordered_map<int, std::wstring::const_iterator> find_for_keyword_and_linenum(std::wstring& input_str);
std::wstring read_from_file(std::wstring file);
std::vector<std::wstring> read_cpp_list(std::wstring all_cpp_list);
void search_all_files(std::wstring all_cpp_list);
void test_write_file();
#endif // find_for_h__