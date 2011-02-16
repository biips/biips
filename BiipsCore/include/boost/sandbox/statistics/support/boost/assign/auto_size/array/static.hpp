//////////////////////////////////////////////////////////////////////////////
// assign::detail::array::static.hpp                                        //
//                                                                          //
//  (C) Copyright 2010 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_ASSIGN_AUTO_SIZE_DETAIL_ARRAY_STATIC_ER_2010_HPP
#define BOOST_ASSIGN_AUTO_SIZE_DETAIL_ARRAY_STATIC_ER_2010_HPP
#include <boost/assign/auto_size/array/interface.hpp>
#include <boost/assign/auto_size/array/ref.hpp>
#include <boost/assign/auto_size/array/tag.hpp>

// Declarations for write_to_array and the class that derives from static_array
// must precede inclusion of this class.

namespace boost{
namespace assign{
namespace detail{
namespace auto_size{
 
 
    // static_array has the functionality of of array_interface<> and is 
    // allocated at construction. It is suitable as the result from a csv
    // function.
    
    template<typename T,int N,template<typename> class R>
    class static_array; 

    template<>
    struct csv_policy<tag::array>
    {
        template<typename T,int N,template<typename> class R>
        struct apply{
            typedef static_array<T,N,R> type;
        };
    };        
 

    template<typename T,int N,template<typename> class R>
    class static_array 
    	: public array_interface<T,N,R,static_array<T,N,R> >
    {

        typedef typename auto_size::ref_array<T,N,R>::type ref_array_;
                
        public:

        static_array(){}	

        template<typename E>
        static_array(const E& coll){
            write_to_array<0>(this->ref_array,coll);
        }	
        
        ref_array_& ref_array_impl(){ 
            return this->ref_array;
        }

        const ref_array_& ref_array_impl()const{ 
            return this->ref_array;
        }
        
        private:
        mutable ref_array_ ref_array;
        
    };


}// auto_size  
}// detail      
}// assign
}// boost

#endif

