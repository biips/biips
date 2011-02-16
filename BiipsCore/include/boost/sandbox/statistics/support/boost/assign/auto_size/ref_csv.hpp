//////////////////////////////////////////////////////////////////////////////
// assign::ref_csv.hpp                                                      //
//                                                                          //
//  (C) Copyright 2010 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_ASSIGN_AUTO_SIZE_REF_CSV_ER_2010_HPP
#define BOOST_ASSIGN_AUTO_SIZE_REF_CSV_ER_2010_HPP

// Usage:
// ref_csv(a,b,c);
// cref_csv(a,b,c);
// ref_csv<P>(a,b,c);
// cref_csv<P>(a,b,c);

#define BOOST_ASSIGN_CSV_ref boost::assign::detail::auto_size::ref_copy
#include <boost/assign/auto_size/detail/csv.hpp>
BOOST_ASSIGN_CSV(ref_csv)
#undef BOOST_ASSIGN_CSV_ref

#endif
