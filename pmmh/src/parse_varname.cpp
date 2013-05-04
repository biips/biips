#include <iostream>
#include <string>
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
using namespace std;

void parse_varname(const string & to_parse,
                  string & var_name,
                  vector<size_t> lower,
                  vector<size_t> upper) {

   using boost::regex;
   using boost::algorithm::split;
   using boost::is_any_of;

   // regexp alpha sequence not beginning with
   // a number followed eventually by some characters,
   // between brackets
   regex re("(\\D.*[^\\[\\]]){1}(\\[(.+)\\])?");
   boost::cmatch matches;

   if (boost::regex_match(to_parse.c_str(), matches, re))
   {
     var_name = string(matches[1].first, matches[1].second);
     cout << var_name << endl;
     
     if(matches[3].first !=  matches[3].second){
        
        string tmp = string(matches[3].first, matches[3].second); 
        
        vector<string> result;
        split(result, tmp, is_any_of(","));
        lower.resize(result.size());
        upper.resize(result.size());
        for( auto & str : result) {
            vector<string> res2;
            split(res2, str, is_any_of(":"));
            copy(res2.begin(),res2.end(), ostream_iterator<string>(cout,"\n"));
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
