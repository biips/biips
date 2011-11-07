//////////////////////////////////////////////////////////////////////////////
// assign::detail::auto_size::check_unit_testing.hpp                        //
//                                                                          //
//  (C) Copyright 2010 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_ASSIGN_DETAIL_AUTO_SIZE_CHECK_UNIT_TESTING_ER_2010_HPP
#define BOOST_ASSIGN_DETAIL_AUTO_SIZE_CHECK_UNIT_TESTING_ER_2010_HPP
using boost::unit_test::test_suite;

#define BOOST_ASSIGN_AS_CHECK_add(suffix)                                      \
    fp = \
    BOOST_PP_CAT(check_,BOOST_PP_CAT(BOOST_ASSIGN_AS_CHECK_infix,BOOST_PP_CAT(_,suffix)))<int>; \
     test->add( BOOST_TEST_CASE( fp ) );                                       \
/**/


#define BOOST_ASSIGN_AS_CHECK_TEST_SUITE                                       \
test_suite* init_unit_test_suite( int argc, char* argv[] )                     \
{                                                                              \
    test_suite* test = BOOST_TEST_SUITE( "List Test Suite" );                  \
    using namespace boost::assign::detail::auto_size;                          \
    void (*fp)();                                                              \
    BOOST_ASSIGN_AS_CHECK_add(example1)                                        \
    BOOST_ASSIGN_AS_CHECK_add(iterator)                                        \
    BOOST_ASSIGN_AS_CHECK_add(array)                                           \
    BOOST_ASSIGN_AS_CHECK_add(copy_iterator)                                   \
    BOOST_ASSIGN_AS_CHECK_add(copy_array)                                      \
    BOOST_ASSIGN_AS_CHECK_add(rebind_array)                                    \
    BOOST_ASSIGN_AS_CHECK_add(comparison_op)                                   \
    BOOST_ASSIGN_AS_CHECK_add(converter_list)                                  \
    BOOST_ASSIGN_AS_CHECK_add(converter_vector)                                \
    BOOST_ASSIGN_AS_CHECK_add(converter_array)                                 \
    BOOST_ASSIGN_AS_CHECK_add(converter_set)                                   \
    BOOST_ASSIGN_AS_CHECK_add(adapter_stack)                                   \
    BOOST_ASSIGN_AS_CHECK_add(adapter_queue)                                   \
    BOOST_ASSIGN_AS_CHECK_add(chain_r)                                 \
    BOOST_ASSIGN_AS_CHECK_add(chain_l)                                 \
    BOOST_ASSIGN_AS_CHECK_add(example2)                                        \
                                                                               \
    return test;                                                               \
}                                                                              \
/**/

#endif
