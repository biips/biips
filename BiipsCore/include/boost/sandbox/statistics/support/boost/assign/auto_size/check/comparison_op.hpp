//////////////////////////////////////////////////////////////////////////////
// assign::detail::auto_size::check_comparison_op.hpp                       //
//                                                                          //
//  (C) Copyright 2010 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_ASSIGN_DETAIL_AUTO_SIZE_CHECK_COMPARISON_OP_ER_2010_HPP
#define BOOST_ASSIGN_DETAIL_AUTO_SIZE_CHECK_COMPARISON_OP_ER_2010_HPP

#ifndef BOOST_ASSIGN_CHECK_EQUAL
#error
#endif

#define BOOST_ASSIGN_AS_CHECK_comparison_op                                    \
{                                                                              \
    typedef T val_;                                                            \
    using namespace check_constants;                                           \
    val_ a1 = 0,  b1 = 1,  c1 = 2;                                             \
    typedef boost::array<T,3> ar_;                                             \
    ar_ ar1; ar1[0] = a1; ar1[1] = b1; ar1[2] = c1;                            \
    ar_ ar2; ar2[0] = a1; ar2[1] = b1; ar2[2] = c1;                            \
    BOOST_AUTO(tmp1,BOOST_ASSIGN_AS_CHECK_cref3(a1,b1,c1));                    \
    BOOST_AUTO(tmp2,BOOST_ASSIGN_AS_CHECK_cref3(ar2[0],ar2[1],ar2[2]));        \
    BOOST_ASSIGN_CHECK_EQUAL( (ar1  ==  tmp2), true  );                        \
    BOOST_ASSIGN_CHECK_EQUAL( (tmp1 ==  ar2),  true  );                        \
    BOOST_ASSIGN_CHECK_EQUAL( (tmp1 ==  tmp2), true  );                        \
    BOOST_ASSIGN_CHECK_EQUAL( (ar1  !=  tmp2), false );                        \
    BOOST_ASSIGN_CHECK_EQUAL( (tmp1 !=  ar2),  false );                        \
    BOOST_ASSIGN_CHECK_EQUAL( (tmp1 !=  tmp2), false );                        \
    BOOST_ASSIGN_CHECK_EQUAL( (ar1  >   tmp2), false );                        \
    BOOST_ASSIGN_CHECK_EQUAL( (tmp1 >   ar2),  false );                        \
    BOOST_ASSIGN_CHECK_EQUAL( (tmp1 >   tmp2), false );                        \
    BOOST_ASSIGN_CHECK_EQUAL( (ar1  <   tmp2), false );                        \
    BOOST_ASSIGN_CHECK_EQUAL( (tmp1 <   ar2),  false );                        \
    BOOST_ASSIGN_CHECK_EQUAL( (tmp1 <   tmp2), false );                        \
    BOOST_ASSIGN_CHECK_EQUAL( (ar1  <=  tmp2), true  );                        \
    BOOST_ASSIGN_CHECK_EQUAL( (tmp1 <=  ar2),  true  );                        \
    BOOST_ASSIGN_CHECK_EQUAL( (tmp1 <=  tmp2), true  );                        \
    BOOST_ASSIGN_CHECK_EQUAL( (ar1  >=  tmp2), true  );                        \
    BOOST_ASSIGN_CHECK_EQUAL( (tmp1 >=  ar2),  true  );                        \
    BOOST_ASSIGN_CHECK_EQUAL( (tmp1 >=  tmp2), true  );                        \
    ar2[1] = 2;                                                                \
    BOOST_ASSIGN_CHECK_EQUAL( (ar1  ==  tmp2), false );                        \
    BOOST_ASSIGN_CHECK_EQUAL( (tmp1 ==  ar2),  false );                        \
    BOOST_ASSIGN_CHECK_EQUAL( (tmp1 ==  tmp2), false );                        \
    BOOST_ASSIGN_CHECK_EQUAL( (ar1  !=  tmp2), true  );                        \
    BOOST_ASSIGN_CHECK_EQUAL( (tmp1 !=  ar2),  true  );                        \
    BOOST_ASSIGN_CHECK_EQUAL( (tmp1 !=  tmp2), true  );                        \
    BOOST_ASSIGN_CHECK_EQUAL( (ar1  >   tmp2), false );                        \
    BOOST_ASSIGN_CHECK_EQUAL( (tmp1 >   ar2),  false );                        \
    BOOST_ASSIGN_CHECK_EQUAL( (tmp1 >   tmp2), false );                        \
    BOOST_ASSIGN_CHECK_EQUAL( (ar1  <   tmp2), true  );                        \
    BOOST_ASSIGN_CHECK_EQUAL( (tmp1 <   ar2),  true  );                        \
    BOOST_ASSIGN_CHECK_EQUAL( (tmp1 <   tmp2), true  );                        \
    BOOST_ASSIGN_CHECK_EQUAL( (ar1  <=  tmp2), true  );                        \
    BOOST_ASSIGN_CHECK_EQUAL( (tmp1 <=  ar2),  true  );                        \
    BOOST_ASSIGN_CHECK_EQUAL( (tmp1 <=  tmp2), true  );                        \
    BOOST_ASSIGN_CHECK_EQUAL( (ar1  >=  tmp2), false );                        \
    BOOST_ASSIGN_CHECK_EQUAL( (tmp1 >=  ar2),  false );                        \
    BOOST_ASSIGN_CHECK_EQUAL( (tmp1 >=  tmp2), false );                        \
}                                                                              \
/**/

#endif