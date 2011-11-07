//////////////////////////////////////////////////////////////////////////////
// assign::detail::auto_size::check_ref_csv.hpp                             //
//                                                                          //
//  (C) Copyright 2010 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_ASSIGN_DETAIL_AUTO_SIZE_CHECK_REF_CSV_ER_2010_HPP
#define BOOST_ASSIGN_DETAIL_AUTO_SIZE_CHECK_REF_CSV_ER_2010_HPP
#include <boost/assign/auto_size/ref_csv.hpp>
#include <boost/assign/auto_size/ref_rebind_csv.hpp>

#define BOOST_ASSIGN_AS_CHECK_ref1(a)  ref_csv(a)
#define BOOST_ASSIGN_AS_CHECK_ref2(a,b)  ref_csv(a,b)
#define BOOST_ASSIGN_AS_CHECK_ref3(a,b,c)  ref_csv(a,b,c)
#define BOOST_ASSIGN_AS_CHECK_ref4(a,b,c,d)  ref_csv(a,b,c,d)
#define BOOST_ASSIGN_AS_CHECK_ref5(a,b,c,d,e)  ref_csv(a,b,c,d,e)
#define BOOST_ASSIGN_AS_CHECK_ref6(a,b,c,d,e,f)  ref_csv(a,b,c,d,e,f)
#define BOOST_ASSIGN_AS_CHECK_ref7(a,b,c,d,e,f,g)  ref_csv(a,b,c,d,e,f,g)
#define BOOST_ASSIGN_AS_CHECK_ref8(a,b,c,d,e,f,g,h)  ref_csv(a,b,c,d,e,f,g,h)

#define BOOST_ASSIGN_AS_CHECK_cref1(a)  cref_csv(a)
#define BOOST_ASSIGN_AS_CHECK_cref2(a,b)  cref_csv(a,b)
#define BOOST_ASSIGN_AS_CHECK_cref3(a,b,c)  cref_csv(a,b,c)
#define BOOST_ASSIGN_AS_CHECK_cref4(a,b,c,d)  cref_csv(a,b,c,d)
#define BOOST_ASSIGN_AS_CHECK_cref5(a,b,c,d,e)  cref_csv(a,b,c,d,e)
#define BOOST_ASSIGN_AS_CHECK_cref6(a,b,c,d,e,f)  cref_csv(a,b,c,d,e,f)
#define BOOST_ASSIGN_AS_CHECK_cref7(a,b,c,d,e,f,g)  cref_csv(a,b,c,d,e,f,g)
#define BOOST_ASSIGN_AS_CHECK_cref8(a,b,c,d,e,f,g,h)  cref_csv(a,b,c,d,e,f,g,h)

#define BOOST_ASSIGN_AS_CHECK_ref_rebind3(a,b,c)  ref_rebind_csv(a,b,c)

#define BOOST_ASSIGN_AS_CHECK_infix ref_csv

#include <boost/assign/auto_size/check/all.hpp>

#undef BOOST_ASSIGN_AS_CHECK_ref1
#undef BOOST_ASSIGN_AS_CHECK_ref2
#undef BOOST_ASSIGN_AS_CHECK_ref3
#undef BOOST_ASSIGN_AS_CHECK_ref4
#undef BOOST_ASSIGN_AS_CHECK_ref5
#undef BOOST_ASSIGN_AS_CHECK_ref6
#undef BOOST_ASSIGN_AS_CHECK_ref7
#undef BOOST_ASSIGN_AS_CHECK_ref8

#undef BOOST_ASSIGN_AS_CHECK_cref1
#undef BOOST_ASSIGN_AS_CHECK_cref2
#undef BOOST_ASSIGN_AS_CHECK_cref3
#undef BOOST_ASSIGN_AS_CHECK_cref4
#undef BOOST_ASSIGN_AS_CHECK_cref5
#undef BOOST_ASSIGN_AS_CHECK_cref6
#undef BOOST_ASSIGN_AS_CHECK_cref7
#undef BOOST_ASSIGN_AS_CHECK_cref8

#undef BOOST_ASSIGN_AS_CHECK_ref_rebind3

#undef BOOST_ASSIGN_AS_CHECK_infix

#endif


