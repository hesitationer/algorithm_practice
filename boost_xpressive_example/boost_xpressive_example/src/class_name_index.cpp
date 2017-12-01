#include "class_name_index.h"

#include <iostream>
#include <boost/xpressive/xpressive.hpp>

using namespace boost::xpressive;

boost::regex expression(
	"^(template[[:space:]]*<[^;:{]+>[[:space:]]*)?"
	"(class|struct)[[:space:]]*"
	"(\\<\\w+\\>([[:blank:]]*\\([^)]*\\))?"
	"[[:space:]]*)*(\\<\\w*\\>)[[:space:]]*"
	"(<[^;:{]+>[[:space:]]*)?(\\{|:[^;\\{()]*\\{)");

void IndexClasses(map_type& m, const std::string& file)
{
	std::string::const_iterator start, end;
	start = file.begin();
	end = file.end();
	boost::match_results<std::string::const_iterator> what;
	boost::match_flag_type flags = boost::match_default;
	while (regex_search(start, end, what, expression, flags))
	{
		// what[0] contains the whole string 
		// what[5] contains the class name. 
		// what[6] contains the template specialisation if any. 
		// add class name and position to map: 
		m[std::string(what[5].first, what[5].second)
			+ std::string(what[6].first, what[6].second)]
			= what[5].first - file.begin();
		// update search position: 
		start = what[0].second;
		// update flags: 
		flags |= boost::match_prev_avail;
		flags |= boost::match_not_bob;
	}
}

void what_size(){
    cmatch what;
    cregex cre = +(s1 = _w);

    // store the results of the regex_match in "what"
    if (regex_match("hellox", what, cre))
    {
        std::cout << what.size() << "\n";
        std::cout << what[0] << '\n'; // prints "hello"
        std::cout << what[1] << '\n'; // prints "o"
    }
}

void search_first_or_two()
{
    std::string str("I was born on 5/30/1973 at 7am.");
    //std::string str("I was born on 5/30/1973 at 7am 5/30/1974.");

    // define some custom mark_tags with names more meaningful than s1, s2, etc.
    mark_tag day(1), month(2), year(3), delim(4);

    // this regex finds a date
    sregex date = (month = repeat<1, 2>(_d))           // find the month ...
        >> (delim = (set = '/', '-'))            // followed by a delimiter ...
        >> (day = repeat<1, 2>(_d)) >> delim  // and a day followed by the same delimiter ...
        >> (year = repeat<1, 2>(_d >> _d));    // and the year.

    smatch what;

    auto start = str.begin();
    auto end = str.end();
    if (regex_search(start, end, what, date))
    {
        std::cout << what.size() << "\n";
        std::cout << what[0] << '\n'; // whole match
        std::cout << what[day] << '\n'; // the day
        std::cout << what[month] << '\n'; // the month
        std::cout << what[year] << '\n'; // the year
        std::cout << what[delim] << '\n'; // the delimiter
    }

    std::string left(what[0].second, end); // 参见 sub_match 的说明， second是全匹配的结束
    std::cout << left << "\n";
}

