//////////////////////////////////////////////////////////////////////////////
// assign::detail::array::interface.hpp                                     //
//                                                                          //
//  (C) Copyright 2010 Erwann Rogard - Manuel Peinado Gallego               //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_ASSIGN_AUTO_SIZE_DETAIL_ARRAY_INTERFACE_ER_2010_HPP
#define BOOST_ASSIGN_AUTO_SIZE_DETAIL_ARRAY_INTERFACE_ER_2010_HPP
#include <algorithm>
#include <boost/mpl/bool.hpp>
#include <boost/mpl/int.hpp>
#include <boost/array.hpp>
#include <boost/range.hpp>
#include <boost/typeof/typeof.hpp>
//#include <boost/assign/list_of.hpp> // converter<>
#include <boost/assign/auto_size/comparison_op/range.hpp>
#include <boost/assign/auto_size/reference_wrapper/has_copy_semantics.hpp> //needed?
#include <boost/assign/auto_size/array/ref.hpp>
#include <boost/assign/auto_size/range/converter.hpp>

namespace boost{
namespace assign{
namespace detail{
namespace auto_size{

    template<typename T,int N,template<typename> class R>
    struct array_interface_traits{
        typedef typename R<T>::type ref_;
        typedef typename ref_array<T,N,R>::type ref_array_;
        typedef ref_ value_type;
        typedef typename 
        	boost::range_iterator<ref_array_>::type iterator;
        typedef typename boost::range_iterator<
        	const ref_array_>::type const_iterator;
        #ifndef BOOST_MSVC
		    typedef typename 
        	    boost::range_size<ref_array_>::type size_type;
        #endif
        #ifdef BOOST_MSVC 
//c:\users\erwann\reposetories\boost\trunk\boost\range\size_type.hpp(66) : error C2872: 'detail' : ambiguous symbol
//        could be 'boost::detail'  or 'boost::assign::detail'
            typedef std::size_t size_type; 
        #endif


        typedef typename boost::range_difference<
            ref_array_>::type difference_type;
        typedef typename ref_array_::reference reference;
        typedef typename 
        	ref_array_::const_reference const_reference;

    };
                   
    // Used as a base class of D, adds the array and conversion interface
    //
    // Requirements: let d1 and d2 instances of D and const D, respectively
    // Valid expression			Result
    // d1.ref_array_impl()		array_interface::ref_array_&
    // d2.ref_array_impl()		array_interface::const ref_array_&
    template<typename T,std::size_t N,template<typename> class R,typename D>
    class array_interface 
      :  
        public range_comparison_op::base_of< 
            array_interface<T,N,R,D>
          >::type
    {
    
        typedef array_interface_traits<T,N,R> traits;
        typedef typename traits::ref_ ref_;
        typedef typename traits::ref_array_ ref_array_;
        typedef array_interface<T,N,R,D> this_;

        public:
        typedef ref_ value_type;
        typedef typename traits::iterator iterator;
        typedef typename traits::const_iterator const_iterator;
        typedef typename traits::size_type size_type;
        typedef typename traits::difference_type difference_type;
        typedef typename traits::reference reference;
        typedef typename traits::const_reference const_reference;
                
        BOOST_STATIC_CONSTANT(size_type, static_size = N);
                                            
        iterator begin() { return boost::begin(this->ref_array()); }
        iterator end() { return boost::end(this->ref_array()); }

        const_iterator begin()const { return boost::begin(this->ref_array()); }
        const_iterator end()const { return boost::end(this->ref_array()); }
        size_type size() const{ return this->ref_array().size(); }
        bool empty() const{ return this->ref_array().empty(); }
                
        reference operator[](size_type i){ return (this->ref_array())[i]; }
        const_reference operator[](size_type i)const{ 
             return (this->ref_array())[i]; }
			
        reference front(){ return (this->ref_array()).front(); }
        const_reference front() const{ return (this->ref_array()).front(); }
        reference back(){ return (this->ref_array()).back(); }
        const_reference back() const{ return (this->ref_array()).back(); }
        
        void swap(array_interface& other){ 
            typedef boost::mpl::int_<N> int_n_;
            this->swap_impl(other,int_n_());
            // Previously :
            //    std::swap(this->ref_array(),other.ref_array());
            // did not pass check_copy_array.
        }

        void assign(const T& val){ 
            typedef has_copy_semantics<ref_> pred_;
            return this->assign(val,pred_());
        }

        protected:
        //typedef assign_detail::converter<this_,const_iterator> converter_;
        //friend class assign_detail::converter<this_,const_iterator>;

        public:

        BOOST_ASSIGN_AS_CONVERTER(this_)

        private:
        typedef boost::mpl::bool_<false> false_;
        typedef boost::mpl::bool_<true> true_;

        void swap_impl(array_interface& other,boost::mpl::int_<0>){}

        template<int N1>
        void swap_impl(array_interface& other,boost::mpl::int_<N1>){
            reference lhs = (*this)[N1-1];
            reference rhs = (other)[N1-1];
            lhs.swap(rhs); 
            typedef boost::mpl::int_<N1-1> next_int_;
            this->swap_impl(other,next_int_());
        }
        
        void assign(const T& val,true_ /*copy semantics*/){ 
            // Force copy semantics. Suggested by M.P.G on Feb 28th, 2010.
            ref_array_& ra = this->ref_array();
            std::fill(ra.begin(), ra.end(), val);
        }

        void assign(const T& val,false_ /*copy semantics*/){ 
            this->ref_array().assign(val);
        }
        
        ref_array_& ref_array(){ 
            return static_cast<D&>(*this).ref_array_impl();
        }

        const ref_array_& ref_array()const{ 
            return static_cast<const D&>(*this).ref_array_impl();
        }

    };


}// auto_size  
}// detail      
}// assign
}// boost

#endif
