#include <iostream>
#include <string>
#include <boost/regex.hpp>
#include <boost/algorithm/string_regex.hpp>
using namespace std;

void parse_varname(const string & to_parse,
                  string & var_name,
                  vector<size_t> lower,
                  vector<size_t> upper) {

   // regexp alpha sequence not beginning with
   // a number followed eventually by some characters,
   // between bracketds
   using boost::regex;
   using boost::regex_match;
   using boost::cmatch;

   regex re("(\\D.*[^\\[\\]]){1}(\\[(.+)\\])?");
   cmatch matches;

   if (regex_match(to_parse.c_str(), matches, re))
   {
     var_name = string(matches[1].first, matches[1].second);
     cout << var_name << endl;
     if(matches[3].first !=  matches[3].second){
        string tmp = string(matches[3].first, matches[3].second); 
        cmatch tmp_match;
        if (regex_match(tmp.c_str(), tmp_match, regex(".*,.*"))) {
           vector<string> result;
           boost::algorithm::split_regex( result, tmp, regex(",") );
           copy( result.begin(), result.end(), ostream_iterator<string>( cout, "\n" ) ) ;
        }
        else if (regex_match(tmp.c_str(), tmp_match, regex("\\d+"))) {
             cout << string(tmp_match[1].first, tmp_match[1].second)  << endl;
        }
     }

   }
   else
   {
      cout << "The regexp \"" << re << "\" does not match \"" << to_parse << "\"" << endl;
   }
}                  

int main( ) {

   string s, var;
   cin >> s;
   vector<size_t> lower,upper;
   parse_varname(s, var, lower, upper);
   return 0;
}   
