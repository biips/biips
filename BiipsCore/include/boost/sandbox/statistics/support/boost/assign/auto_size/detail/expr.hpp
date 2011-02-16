//////////////////////////////////////////////////////////////////////////////
// assign::detail::expr.hpp                                                 //
//                                                                          //
//  (C) Copyright 2010 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_ASSIGN_AUTO_SIZE_DETAIL_EXPR_ER_2010_HPP
#define BOOST_ASSIGN_AUTO_SIZE_DETAIL_EXPR_ER_2010_HPP
#include <ostream>
#include <boost/mpl/void.hpp>
#include <boost/mpl/if.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/mpl/equal_to.hpp>
#include <boost/mpl/empty_base.hpp>
#include <boost/mpl/identity.hpp>
#include <boost/concept_check.hpp>
#include <boost/iterator/iterator_concepts.hpp>
#include <boost/next_prior.hpp>
#include <boost/type_traits.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/range.hpp>
//#include <boost/assign/auto_size/array/has_static_size.hpp>
//#include <boost/assign/auto_size/array/static_size.hpp>
#include <boost/assign/auto_size/reference_wrapper/copy.hpp>
#include <boost/assign/auto_size/reference_wrapper/rebind.hpp>
#include <boost/assign/auto_size/detail/policy.hpp>
#include <boost/assign/auto_size/detail/types.hpp>
#include <boost/assign/auto_size/detail/expr_size.hpp>
#include <boost/assign/auto_size/detail/expr_elem.hpp>
#include <boost/assign/auto_size/detail/fwd_expr.hpp>
#include <boost/assign/auto_size/array/lazy.hpp>

// Creates a collection of references by deducing the number of arguments
// at compile time. The functionality is controlled by parameter R which
// specifies a reference wrapper, and P, an arbitrary policy, usually intended
// to expose a container interface.
//
// Note:
// - In most situations, a reference wrapper that has copy rather than rebind 
// semantics for operator= is preferable. 
// - The older counterpart of this class is assign::static_generic_list<>.
//
// Acknowledgement: The idea of this class was developed in collaboration 
// with M.P.G

namespace boost{
namespace assign{
namespace detail{
namespace auto_size{
                        
    // ---- template parameters ---- //

    // expr<> keeps a reference to a new element and links to the previous 
    // collection.
    // E : previous collection 
    // T : element 
    // N : size of the collection
    // R : reference wrapper around an element
    // P : policy

    // ---- describe ---- //
    
    template<typename E,typename T,
        template<typename> class R,typename P>
	void describe(std::ostream& os,const expr<E,T,1,R,P>& e){
    	os << e.ref;
    }

    template<typename E,typename T,int N,
        template<typename> class R,typename P>
	void describe(std::ostream& os,const expr<E,T,N,R,P>& e){
    	describe(os,e.previous);
    	os << ',' << e.ref;
    }

    // ---- Traits ---- //
    
    template<typename E,typename T,int N,
        template<typename>class R,typename P>
    struct expr_size<expr<E,T,N,R,P> >
     	: boost::mpl::int_<N>{};

    template<typename E,typename T,int N,
        template<typename>class R,typename P>
    struct expr_elem<expr<E,T,N,R,P> >
    {
    	typedef T type;
    };

    // ---- Collection builder ---- //

    template<typename E,typename T,int N,
        template<typename> class R,typename P>
    class expr : public policy<P>::template apply<expr<E,T,N,R,P>,R>::type{
        typedef boost::mpl::int_<1> int_1_;
        typedef boost::mpl::int_<N> int_n_;
        typedef typename R<T>::type ref_;
        typedef typename policy<P>::template apply<expr,R>::type super_;

        template<typename E1>
        struct next{
            typedef expr<E1,T,N+1,R,P> type;
        };

        public:       

        typedef detail::assign_reference_copy<const E> ref_to_previous_;
        typedef typename boost::mpl::equal_to<int_n_,int_1_>::type is_first_;
        typedef typename boost::mpl::if_<
            is_first_,E,ref_to_previous_>::type previous_;
        typedef typename next<expr>::type result_type;

        explicit expr(const E& p,T& t):previous(p),ref(t){} 
        // Needed by csv.hpp :
        template<typename E1,typename P1>
        explicit expr(const expr<E1,T,N,R,P1>& that)
            :super_(that)
            ,previous(that.previous)            
            ,ref(that.ref){}

        result_type operator()(T& t)const{ return result_type(*this,t); }
        
        mutable previous_ previous;
        mutable ref_ ref;

        private:
        expr();

    };

    // ---- write_to_array ---- //
	
    template<int Nshift,typename A,typename E,typename T,int N,
    	template<typename> class R,typename P>
    void write_to_array(A& a,const expr<E,T,N,R,P>& e,false_ /*exit*/){
        a[Nshift+N-1].rebind( e.ref.get_ref() ) ; 
        write_to_array<Nshift>(a,e.previous.get_ref());
    }
            
    template<int Nshift,typename A,typename E,typename T,int N,
    	template<typename> class R,typename P>
    void write_to_array(A& a,const expr<E,T,N,R,P>& e,true_ /*exit*/){
        a[Nshift+N-1].rebind( e.ref.get_ref() ) ;
    }

    template<int Nshift,typename A,typename E,typename T,int N,
    	template<typename> class R,typename P>
    void write_to_array(A& a,const expr<E,T,N,R,P>& e){
        typedef expr<E,T,N,R,P> expr_;
        typedef typename expr_::is_first_ exit_;
        write_to_array<Nshift>(a,e,exit_());
    }

    // ---- ref wrappers ---- //

    template<typename T>
    struct ref_copy{
        typedef boost::assign::detail::assign_reference_copy<T> type;
    };

    template<typename T>
    struct ref_rebind{
        typedef boost::assign::detail::assign_reference_rebind<T> type;
    };

    // ---- first expr ---- //

    template<typename T,template<typename> class R,typename P>
    struct first_expr{
        typedef detail::auto_size::expr<detail::auto_size::top_,T,1,R,P> type;   
        static type call(T& a){ return type(top_(),a); }
    };

    template<typename T,template<typename> class R>
    struct first_expr_no_policy : first_expr<T,R,no_policy>{};

    template<template<typename> class R,typename T>
    typename first_expr_no_policy<T,R>::type 
    make_first_expr_no_policy(T& a){
    	return first_expr_no_policy<T,R>::call(a);
    }

    template<template<typename> class R,typename T>
    typename first_expr_no_policy<const T,R>::type 
    make_first_expr_no_policy(const T& a){
    	return first_expr_no_policy<const T,R>::call(a);
    }

    template<typename T,typename P = default_policy>
    struct first_copy : first_expr<T,ref_copy,P>{};

    template<typename T,typename P = default_policy>
    struct first_rebind : first_expr<T,ref_rebind,P>{};

    // ---- n_th_expr ---- //

namespace meta{	
namespace impl{
    template<int N>
    struct n_th_expr{
    
    	template<typename T,template<typename> class R,typename P>
        struct apply
        {
            typedef impl::n_th_expr<N-1> meta_;
            typedef typename meta_::template apply<T,R,P> apply_;
            typedef typename apply_::type::result_type type;
    	};
        
    };
    template<>
    struct n_th_expr<1>{
    
    	template<typename T,template<typename> class R,typename P>
        struct apply : first_expr<T,R,P>{};
        
    };
    
}//impl

    template<typename T,template<typename> class R,typename P = default_policy>    
    struct n_th_expr{
    
    	template<int N>
        struct apply : impl::n_th_expr<N>::template apply<T,R,P>
        {};
        
    };

    template<typename T,typename P = default_policy>
    struct n_th_expr_copy : meta::n_th_expr<T,ref_copy,P>{};

    template<typename T,typename P = default_policy>
    struct n_th_expr_rebind : meta::n_th_expr<T,ref_rebind,P>{};

}// meta

    template<typename T,template<typename> class R,int N, 
        typename P = default_policy>
    struct n_th_expr 
        : meta::n_th_expr<T,R,P>::template apply<N>{};

    template<typename T,int N, typename P = default_policy>
    struct n_th_expr_copy 
        : meta::n_th_expr_copy<T,P>::template apply<N>{};

    template<typename T,int N, typename P = default_policy>
    struct n_th_expr_rebind 
        : meta::n_th_expr_rebind<T,P>::template apply<N>{};

}// auto_size  
}// detail      
}// assign
}// boost

#endif

