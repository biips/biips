#define BOOST_TEST_MODULE Parse_varname
#include <boost/test/included/unit_test.hpp>
#include "parse_varname.hpp"
#include "include/common/Error.hpp"
#include <iostream>
using namespace std;

struct nireFixture {
    string a_parser;
    string var_name;
    vector<size_t> lower_test;
    vector<size_t> upper_test;
};

BOOST_FIXTURE_TEST_SUITE(s, nireFixture)

// one character variable, no bounds
BOOST_AUTO_TEST_CASE(case1)
{
   a_parser = "x";
   parse_one_varname(a_parser, var_name, lower_test, upper_test);
   BOOST_CHECK_EQUAL(var_name, string("x"));
   BOOST_CHECK_EQUAL(upper_test.size(), 0);
   BOOST_CHECK_EQUAL(lower_test.size(), 0);
}

// illegal variable name (beginning with number)
BOOST_AUTO_TEST_CASE(case2)
{
   a_parser = "2x";
   BOOST_CHECK_THROW(parse_one_varname(a_parser, var_name, lower_test, upper_test), Biips::RuntimeError);
}

// three character variable name, 2 coordinates 
BOOST_AUTO_TEST_CASE(case3)
{
   a_parser = "aba[1,2]";
   parse_one_varname(a_parser, var_name, lower_test, upper_test);
   BOOST_CHECK_EQUAL(var_name, string("aba"));
   BOOST_CHECK_EQUAL(upper_test.size(), 2);
   BOOST_CHECK_EQUAL(lower_test.size(), 2);
   size_t upper[] =  { 1, 2 };
   size_t lower[] =  { 1, 2 };
   BOOST_CHECK_EQUAL_COLLECTIONS(upper_test.begin(), upper_test.end(), upper, upper + 2);
   BOOST_CHECK_EQUAL_COLLECTIONS(lower_test.begin(), lower_test.end(), lower, lower + 2);
}

//  variable name containing a number, bounds (bounded range, unbounded range, one composante)
BOOST_AUTO_TEST_CASE(case4)
{
   a_parser = "ho2ho[1:8,,5]";
   parse_one_varname(a_parser, var_name, lower_test, upper_test);
   BOOST_CHECK_EQUAL(var_name, string("ho2ho"));
   BOOST_CHECK_EQUAL(upper_test.size(), 3);
   BOOST_CHECK_EQUAL(lower_test.size(), 3);
   size_t lower[] =  { 1, 0, 5 };
   size_t upper[] =  { 8, 0, 5 };
   BOOST_CHECK_EQUAL_COLLECTIONS(lower_test.begin(), lower_test.end(), lower, lower + 3);
   BOOST_CHECK_EQUAL_COLLECTIONS(upper_test.begin(), upper_test.end(), upper, upper + 3);
}

// illegal variable name (brackets contain nothing)
BOOST_AUTO_TEST_CASE(case5)
{
   a_parser = "x[]";
   BOOST_CHECK_THROW(parse_one_varname(a_parser, var_name, lower_test, upper_test), Biips::RuntimeError);
}

// illegal variable name (one domaine is inconsistent)
BOOST_AUTO_TEST_CASE(case6)
{
   a_parser = "aa[8:1,]";
   BOOST_CHECK_THROW(parse_one_varname(a_parser, var_name, lower_test, upper_test), Biips::RuntimeError);
}

// illegal variable name (two brackets domain)
BOOST_AUTO_TEST_CASE(case7)
{
   a_parser = "gogo[1][2,]";
   BOOST_CHECK_THROW(parse_one_varname(a_parser, var_name, lower_test, upper_test), Biips::RuntimeError);
}

// illegal variable name (bounded range with lacking bound)
BOOST_AUTO_TEST_CASE(case8)
{
   a_parser = "gogo[1,1:]";
   BOOST_CHECK_THROW(parse_one_varname(a_parser, var_name, lower_test, upper_test), Biips::RuntimeError);
}

BOOST_AUTO_TEST_SUITE_END()
