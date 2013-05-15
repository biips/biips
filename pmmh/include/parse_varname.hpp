#ifndef __PARSE_VARNAME__HPP__
#define __PARSE_VARNAME__HPP__
#include <string>
#include <vector>

using namespace std;

void parse_varname(const string & to_parse,
                  string & var_name,
                  vector<size_t> & lower,
                  vector<size_t> & upper);

                  
#endif
