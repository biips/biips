//////////////////////////////////////////////////////////////////////////////
// assign::detail::array::lazy.hpp                                          //
//                                                                          //
//  (C) Copyright 2010 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_ASSIGN_AUTO_SIZE_DETAIL_ARRAY_LAZY_ER_2010_HPP
#define BOOST_ASSIGN_AUTO_SIZE_DETAIL_ARRAY_LAZY_ER_2010_HPP
#include <boost/shared_ptr.hpp>
#include <boost/assign/auto_size/array/interface.hpp>
#include <boost/assign/auto_size/array/ref.hpp>
#include <boost/assign/auto_size/detail/policy.hpp>
#include <boost/assign/auto_size/array/tag.hpp>

// Declarations for write_to_array and the class that derives from lazy_array
// must precede inclusion of this class.
 
namespace boost{
namespace assign{
namespace detail{
namespace auto_size{

    template<typename T,int N,template<typename> class R,typename D>
    class lazy_array; 

    template<typename E> struct expr_size;
    template<typename E> struct expr_elem;

    template<>
    struct policy<tag::array>
    {
        template<typename E,template<typename> class R>
        struct apply{
            typedef typename expr_size<E>::type size_;
            typedef typename expr_elem<E>::type elem_;
            typedef lazy_array<elem_,size_::value,R,E> type;
        };
   };        
                   
   // Postones allocation until it is necessary.
   // D must interoperate with write_to_array.
   //
   // D is typically the result of nested compile time expressions each contain-
   // ing a reference. Only the last of these expression needs to expose an 
   // friendly interface, hence 'lazy_array'.
   template<typename T,int N,template<typename> class R,typename D>
   class lazy_array 
    	: public array_interface<T,N,R,lazy_array<T,N,R,D> >
    {
        typedef typename auto_size::ref_array<T,N,R>::type ref_array_;
                
        void alloc_if()const{
            if(!this->ptr){
               return this->alloc();
            }
        }

        void alloc()const{ 
            this->ptr = smart_ptr_(new ref_array_);
            write_to_array<0>(*this->ptr,static_cast<const D&>(*this));	
        }
		
        public:

        void force_alloc(){ this->alloc(); } // TODO remove

        ref_array_& ref_array_impl(){ 
            this->alloc_if();
            return (*this->ptr);
        }

        const ref_array_& ref_array_impl()const{ 
            this->alloc_if();
            return (*this->ptr);
        }
        
        private:
        
        typedef boost::shared_ptr<ref_array_> smart_ptr_;
        mutable smart_ptr_ ptr;

    };

}// auto_size  
}// detail      
}// assign
}// boost

#endif
