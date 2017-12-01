#include <iostream>

#include <boost/regex.hpp>
#include <boost/filesystem.hpp>
#include "boost/xpressive/match_results.hpp"

#include "class_name_index.h"
#include "find_for.h"




void main()
{
    search_all_files(L"all_cpp.txt");
}
