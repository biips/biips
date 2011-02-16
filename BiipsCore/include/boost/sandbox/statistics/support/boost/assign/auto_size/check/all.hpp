//////////////////////////////////////////////////////////////////////////////
// assign::detail::auto_size::check::all.hpp                                //
//                                                                          //
//  (C) Copyright 2010 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
// Do not put #ifndef #define directives here
#include <boost/preprocessor/cat.hpp>
#include <vector>
#include <set>
#include <list>
#include <stack>
#include <queue>
#include <boost/array.hpp>

#include <boost/assign/auto_size/check/constants.hpp>
#include <boost/assign/auto_size/check/example1.hpp>
#include <boost/assign/auto_size/check/iterator.hpp>
#include <boost/assign/auto_size/check/array.hpp>
#include <boost/assign/auto_size/check/copy_iterator.hpp>
#include <boost/assign/auto_size/check/copy_array.hpp>
#include <boost/assign/auto_size/check/rebind_array.hpp>
#include <boost/assign/auto_size/check/comparison_op.hpp>
#include <boost/assign/auto_size/check/converter.hpp>
#include <boost/assign/auto_size/check/fifo.hpp>
#include <boost/assign/auto_size/check/lifo.hpp>
#include <boost/assign/auto_size/check/chain_r.hpp>
#include <boost/assign/auto_size/check/chain_l.hpp>
#include <boost/assign/auto_size/check/example2.hpp>

#define BOOST_ASSIGN_AS_CHECK_fun_tpl(suffix)                                  \
    template<typename T> void \
    BOOST_PP_CAT(check_,BOOST_PP_CAT(BOOST_ASSIGN_AS_CHECK_infix,BOOST_PP_CAT(_,suffix))) \
/**/

namespace boost{                                                               \
namespace assign{                                                              \
namespace detail{                                                              \
namespace auto_size{                                                           \
 BOOST_ASSIGN_AS_CHECK_fun_tpl(example1)()                                     \
     BOOST_ASSIGN_AS_CHECK_example1                                            \
 BOOST_ASSIGN_AS_CHECK_fun_tpl(iterator)()                                     \
     BOOST_ASSIGN_AS_CHECK_iterator                                            \
 BOOST_ASSIGN_AS_CHECK_fun_tpl(array)()                                        \
     BOOST_ASSIGN_AS_CHECK_array                                               \
 BOOST_ASSIGN_AS_CHECK_fun_tpl(copy_iterator)()                                \
     BOOST_ASSIGN_AS_CHECK_copy_iterator                                       \
 BOOST_ASSIGN_AS_CHECK_fun_tpl(copy_array)()                                   \
     BOOST_ASSIGN_AS_CHECK_copy_array                                          \
 BOOST_ASSIGN_AS_CHECK_fun_tpl(rebind_array)()                                 \
     BOOST_ASSIGN_AS_CHECK_rebind_array                                        \
 BOOST_ASSIGN_AS_CHECK_fun_tpl(comparison_op)()                                \
     BOOST_ASSIGN_AS_CHECK_comparison_op                                       \
 BOOST_ASSIGN_AS_CHECK_fun_tpl(converter_list)()                               \
     BOOST_ASSIGN_AS_CHECK_converter(std::list<T>)                             \
 BOOST_ASSIGN_AS_CHECK_fun_tpl(converter_vector)()                             \
     BOOST_ASSIGN_AS_CHECK_converter(std::vector<T>)                           \
 BOOST_ASSIGN_AS_CHECK_fun_tpl(converter_array)()                              \
     BOOST_ASSIGN_AS_CHECK_converter_array                                     \
 BOOST_ASSIGN_AS_CHECK_fun_tpl(converter_set)()                                \
    BOOST_ASSIGN_AS_CHECK_converter_sorted(std::set<T>)                        \
 BOOST_ASSIGN_AS_CHECK_fun_tpl(adapter_stack)()                                \
    BOOST_ASSIGN_AS_CHECK_adapter_lifo(std::stack<T>)                          \
 BOOST_ASSIGN_AS_CHECK_fun_tpl(adapter_queue)()                                \
    BOOST_ASSIGN_AS_CHECK_adapter_fifo(std::queue<T>)                          \
 BOOST_ASSIGN_AS_CHECK_fun_tpl(chain_r)()                                      \
    BOOST_ASSIGN_AS_CHECK_chain_r                                              \
 BOOST_ASSIGN_AS_CHECK_fun_tpl(chain_l)()                                      \
    BOOST_ASSIGN_AS_CHECK_chain_l                                              \
 BOOST_ASSIGN_AS_CHECK_fun_tpl(example2)()                                     \
    BOOST_ASSIGN_AS_CHECK_example2                                             \
}                                                                              \
}                                                                              \
}                                                                              \
}                                                                              \
/**/

#undef BOOST_ASSIGN_AS_CHECK_fun_tpl
