#ifndef __PARSE_VARNAME__HPP__
#define __PARSE_VARNAME__HPP__
#include <string>
#include <vector>

using namespace std;

void parse_one_varname(const string & to_parse,
                       string & var_name,
                       vector<size_t> & lower,
                       vector<size_t> & upper);

void parse_varnames(const vector<string> & to_parse,
                   vector<string> & var_names,
                   vector<vector<size_t> > & lower,
                   vector<vector<size_t> > & upper); 
                  
#endif
