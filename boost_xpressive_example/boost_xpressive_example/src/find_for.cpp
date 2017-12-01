#include "find_for.h"

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <boost/xpressive/xpressive.hpp>
#include "boost/xpressive/xpressive_static.hpp"
#include <boost/xpressive/regex_actions.hpp>


using namespace boost::xpressive;

namespace client
{
    std::unordered_map<int, std::string> g_mapRes; 

    struct push_impl
    {
        // Result type, needed for tr1::result_of
        typedef void result_type;

        template<typename HashMap, typename LineNum, typename ForString>
        void operator()(HashMap &mmap, LineNum const &line_num, ForString const &str) const
        {
            mmap.insert(std::make_pair(*line_num, str));
        }
    };

    function<push_impl>::type const store = { {} };
}

std::vector<std::wstring> read_cpp_list(std::wstring all_cpp_list)
{
    std::vector<std::wstring> vecCpps;
    std::wifstream in(all_cpp_list, std::ios::in | std::ios::binary);
    if (in)
    {
        std::wstring line;
        while(std::getline(in, line)){
            std::wstring x(line.begin(), line.end() - 1);
            vecCpps.push_back(x);
        }
    }
    
    return vecCpps;
}

std::wstring read_from_file(std::wstring file)
{
    std::wifstream in(file, std::ios::in | std::ios::binary);
    if (in)
    {
        std::wostringstream contents;
        contents << in.rdbuf();
        in.close();

        return contents.str();
    }
    
    return L"";
}

wsregex get_line_with_action(std::shared_ptr<int> ptrInt)
{
    value<std::shared_ptr<int>> v_ptrInt(ptrInt);
    wsregex L = _ln[++*v_ptrInt];
    return L;
}

wsregex gen_regex_for_statment()
{
    wsregex parentheses;
    parentheses                          // A balanced set of parentheses ...
        = '('                            // is an opening parenthesis ...
        >>                           // followed by ...
        *(                          // zero or more ...
        keep(+~(set = '(', ')'))  // of a bunch of things that are not parentheses ...
        |                          // or ...
        by_ref(parentheses)      // a balanced set of parentheses
        )                          //   (ooh, recursion!) ...
        >>                           // followed by ...
        ')'                            // a closing parenthesis
        ;

    wsregex for_stat;
    for_stat = as_xpr("for")
        >>
        *_s 
        >> parentheses// space
        ;
    return for_stat;
}

wsregex gen_regex_curve_block(
    )
{

    wsregex curve_bracket;
    {
        curve_bracket                        // A balanced set of parentheses ...
            = '{'                          // is an opening parenthesis ...
            >>                             // followed by ...
            *(                             // zero or more ...
            keep(s1= +~(set = '{', '}'))      // of a bunch of things that are not parentheses ...
            |                              // or ...
            by_ref(curve_bracket)            // a balanced set of parentheses
            )                               //   (ooh, recursion!) ...
            >>                             // followed by ...
            '}'                            // a closing parenthesis
            ;

        return curve_bracket;
    }
}

wsregex gen_regex_for_block( )
{
    

    wsregex for_block;
    for_block = gen_regex_for_statment() 
        >> *_s // NOTE: this is important. 不能换成任意字符
        >>  gen_regex_curve_block();

    return for_block;
}

std::wofstream& g_output_file()
{
    static std::wofstream ostream("output_xueyy.txt", std::ios::out);
    return ostream;
}

void test_write_file()
{
    g_output_file() << "hello world\n";
    g_output_file().close();
}

void find_for_use_earse_in_body(std::wstring cpp_name)
{
    std::wstring file_contents = read_from_file(cpp_name);

    auto for_map = find_for_keyword_and_linenum(file_contents);

    wsmatch what;
    auto rex_for_body = gen_regex_curve_block();
    auto end = file_contents.cend();
    for (auto it = for_map.cbegin(), E = for_map.cend(); it != E; ++it)
    {
        auto start = (*it).second;
        if (regex_search(start, end, what, rex_for_body))
        {
            std::wstring& x = what.str(0);
            auto t = x.find(L"erase");
            if (t != std::wstring::npos)
            {
                auto next_line_break = x.find(L'\n', t);
                std::wstring sub;
                if (next_line_break != std::wstring::npos) {
                    sub = std::wstring(x.begin() + t, x.begin() + next_line_break);
                }
                else {
                    sub = x;
                }
                //std::wcout << cpp_name << L": Found " << (*it).first << L":\n";
                //std::wcout << sub << L"\n";
                //std::wcout << L"================================================================================\n";
                g_output_file() << cpp_name << L" " << (*it).first << L":\n";
                g_output_file() << sub << L"\n";
                g_output_file() << L"================================================================================\n";
            }
        }
        else
        {
            //std::cout << "invalid for-keyword \n";
            //std::wcout << L"================================================================================\n";
        }
    }
}


void example_find_sub_string()
{
    std::wstring str(L"This is his face.");

    // find a whole word
    wsregex token = +alnum;

    wsregex_iterator cur(str.begin(), str.end(), token);
    wsregex_iterator end;

    for (; cur != end; ++cur)
    {
        wsmatch const &what = *cur;
        std::wcout << what[0] << L'\n';
    }
};

// 返回 for 的 iterator 与行号！ 
// 此 iterator 有效，因为使用得是 wstring 的引用
std::unordered_map<int, std::wstring::const_iterator> find_for_keyword_and_linenum(std::wstring& input_str)
{
    std::unordered_map<int, std::wstring::const_iterator> for_map;

    int line_num = 0;
    wsregex line_rex = _ln[++ref(line_num)]; // 记录行号
    wsregex for_rex = as_xpr("for");
    wsregex rich_for_rex = for_rex | line_rex;


    wsmatch what;
    auto start = input_str.cbegin();
    auto end = input_str.cend();
    while(regex_search(start, end, what, rich_for_rex))
    {
        auto x = what[0];
        if (x == L"for") {
            for_map[line_num + 1] = what[0].first;
        }

        start = what[0].second;
    }

    return for_map;
}

void search_all_files(std::wstring all_cpp_list)
{
    auto s = clock();
    std::vector<std::wstring> vecCpps = read_cpp_list(all_cpp_list);
    for (size_t i = 0; i < vecCpps.size(); ++i)
    {
        //std::cout << "find: " << i << "\n";
        find_for_use_earse_in_body(vecCpps[i]);
    }
    auto e = clock();
    std::cout << "used: " <<(e - s) / CLOCKS_PER_SEC << "s\n";

    g_output_file().close();
}