///////////////////////////////////////////////////////////////////////////////
// statistics::detail::fusion::joint_view::binder.hpp                        //
//                                                                           //
//  Copyright 2009 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_FUSION_JOINT_VIEW_BINDER_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_FUSION_JOINT_VIEW_BINDER_HPP_ER_2009
#include <boost/mpl/bool.hpp>
#include <boost/mpl/identity.hpp>
#include <boost/mpl/empty_base.hpp>
#include <boost/mpl/assert.hpp>

#include <boost/type_traits.hpp>
#include <boost/call_traits.hpp>
#include <boost/utility/result_of.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/fusion/include/pair.hpp>
#include <boost/fusion/include/map.hpp>
#include <boost/fusion/include/joint_view.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace fusion{
namespace joint_view{


	// binds a sequence of type Seq1 to a sequence of arbitrary type. 
	template<
    	typename Seq1, 
        bool is_left = true
    >
	class binder
    {
    	typedef boost::is_reference<Seq1> meta_is_ref_;
		BOOST_MPL_ASSERT(( boost::mpl::not_< meta_is_ref_ > ));

		typedef typename boost::add_const<Seq1>::type c_seq1_;

		template<typename T0,typename T1,template<typename,typename> class F>
        struct meta_
         : boost::mpl::eval_if_c< 
        	is_left,
            boost::mpl::identity< F<T0,T1> >,
            boost::mpl::identity< F<T1,T0> >
        >{};

		template<typename Seq2>
        struct meta_view_ : meta_<
        	c_seq1_,
        	typename boost::add_const<Seq2>::type,
            boost::fusion::joint_view
        >{};
    
		public:

		typedef Seq1 seq1_type;

		template<typename S>
        struct result{};

		template<typename F,typename Seq2>
        struct result<F(const Seq2&)> : meta_view_<Seq2>{};

		binder(){}
		binder(const Seq1& seq1) : s1(seq1){}    	
        binder(const binder& that) : s1 (that.s1){}

		binder& 
        operator=(const binder& that)
        {
        	if(&that!=this)
            {
            	this->s1 = that.s1;
            }
        	return (*this);
        }

		template<typename Seq2>
		typename boost::lazy_enable_if_c<
        	is_left,
			meta_view_<Seq2>
        >::type
        operator()(const Seq2& seq2)const
        {
        	typedef typename meta_view_<Seq2>::type view_;
        	return view_(this->s1,seq2);
		}

		template<typename Seq2>
		typename boost::lazy_disable_if_c<
        	is_left,
			meta_view_<Seq2>
        >::type
        operator()(const Seq2& seq2)const
        {
        	typedef typename meta_view_<Seq2>::type view_;
        	return view_(seq2,this->s1);
		}

        private:
		Seq1 s1;
    };

}// joint_view
}// fusion
}// detail
}// statistics
}// boost

#endif