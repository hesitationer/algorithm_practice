#ifndef class_name_index_h__
#define class_name_index_h__

#include <string>
#include <map>
#include <boost/regex.hpp>

// purpose: 
// takes the contents of a file in the form of a string 
// and searches for all the C++ class definitions, storing 
// their locations in a map of strings/int's 
typedef std::map<std::string, int, std::less<std::string> > map_type;

void IndexClasses(map_type& m, const std::string& file);
void what_size();
void search_first_or_two();
#endif // class_name_index_h__



