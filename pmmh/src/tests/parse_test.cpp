#define BOOST_TEST_MODULE Parse_varname
#include <boost/test/included/unit_test.hpp>
#include "parse_varname.hpp"
#include "include/common/Error.hpp"
#include <iostream>
using namespace std;

struct nireFixture {
    string a_parser;
    string var_name;
    vector<size_t> lower_res, lower_test;
    vector<size_t> upper_res, upper_test;
};

BOOST_FIXTURE_TEST_SUITE(s, nireFixture)

// first case : one variable, no bounds
BOOST_AUTO_TEST_CASE(case1)
{
   a_parser = "x";
   parse_varname(a_parser, var_name, lower_test, upper_test);
   BOOST_CHECK_EQUAL(var_name, string("x"));
   BOOST_CHECK_EQUAL(upper_test.size(), 0);
   BOOST_CHECK_EQUAL(lower_test.size(), 0);
}

// first case : one unlegal variable name
BOOST_AUTO_TEST_CASE(case2)
{
   a_parser = "2x";
   BOOST_CHECK_THROW(parse_varname(a_parser, var_name, lower_test, upper_test), Biips::RuntimeError);
}
BOOST_AUTO_TEST_SUITE_END()
