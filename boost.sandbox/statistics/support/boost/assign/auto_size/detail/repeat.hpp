//////////////////////////////////////////////////////////////////////////////
// repeat.hpp                                                               //
//                                                                          //
//  (C) Copyright 2010 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_ASSIGN_DETAIL_AUTO_SIZE_DETAIL_REPEAT_ER_2010_HPP
#define BOOST_ASSIGN_DETAIL_AUTO_SIZE_DETAIL_REPEAT_ER_2010_HPP
#include <boost/mpl/int.hpp>
#include <boost/mpl/divides.hpp>
#include <boost/mpl/modulus.hpp>
#include <boost/assign/auto_size/detail/csv.hpp>

namespace boost{
namespace assign{
namespace detail{
namespace auto_size{
namespace repeat_aux{

    template<int N,typename T>
    struct result{
        typedef auto_size::tag::array pol_;
        typedef typename auto_size::csv_policy<pol_>::template  
            apply<
                T,
                N,
                boost::assign::detail::auto_size::ref_copy
            >::type type;
    }; 

    template<int N,typename T>
    struct caller{};

    template<typename T>
    struct caller<1,T>
    {
        typedef typename auto_size::repeat_aux::result<1,T> result_;
        typedef typename result_::type type;

        static type call(T& t){
            return boost::assign::detail::auto_size::make_first_expr_no_policy<
                boost::assign::detail::auto_size::ref_copy,T>(t); 
        }
        
    };

    template<typename T>
    struct caller<2,T>
    {
        typedef typename auto_size::repeat_aux::result<2,T> result_;
        typedef typename result_::type type;

        static type call(T& t){
            return boost::assign::detail::auto_size::make_first_expr_no_policy<
                boost::assign::detail::auto_size::ref_copy,T>(t)(t); 
        }
        
    };

    template<typename T>
    struct caller<3,T>
    {
        typedef typename auto_size::repeat_aux::result<3,T> result_;
        typedef typename result_::type type;

        static type call(T& t){
            return boost::assign::detail::auto_size::make_first_expr_no_policy<
                boost::assign::detail::auto_size::ref_copy,T>(t)(t)(t); 
        }
        
    };

}// repeat_aux
}// auto_size
}// detail

    template<unsigned K,typename T>
    typename detail::auto_size::repeat_aux::result<K,T>::type
    repeat(T& t){
        namespace ns = detail::auto_size;
        typedef ns::repeat_aux::caller<K,T> caller_;
        return caller_::call(t);
    }

    template<unsigned K,typename T>
    typename detail::auto_size::repeat_aux::result<K,const T>::type
    c_repeat(const T& t){
        namespace ns = detail::auto_size;
        typedef ns::repeat_aux::caller<K,const T> caller_;
        return caller_::call( t );
    }

}// assign
}// boost

#endif




