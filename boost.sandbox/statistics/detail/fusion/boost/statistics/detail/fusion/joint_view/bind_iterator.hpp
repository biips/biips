///////////////////////////////////////////////////////////////////////////////
// statistics::detail::fusion::joint_view::bind_iterator.hpp               	 //
//                                                                           //
//  Copyright 2009 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_FUSION_JOINT_VIEW_BINDER_ITERATOR_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_FUSION_JOINT_VIEW_BINDER_ITERATOR_HPP_ER_2009
#include <boost/utility/result_of.hpp>
#include <boost/range.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/type_traits/add_const.hpp>
#include <boost/type_traits/add_reference.hpp>
#include <boost/iterator/iterator_traits.hpp>
#include <boost/iterator/transform_iterator.hpp>
#include <boost/statistics/detail/fusion/joint_view/binder.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace fusion{
namespace joint_view{

namespace result_of{

namespace impl{

	// Binder = joint_view::binder<>
	template<typename Binder,typename It>
    struct bind_iterator
    {
        typedef typename iterator_value<It>::type 				seq2_;
        typedef typename boost::add_const<seq2_>::type 			c_;
        typedef typename boost::add_reference<c_>::type 		cref_;
        typedef typename boost::result_of<Binder(cref_)>::type 	res_;
        typedef boost::function<res_(cref_)> 					fun_; 
        typedef boost::transform_iterator<fun_,It>  			type;
        
        static type call(const Binder& binder,It it)
        {
            return type(it,fun_(binder));
        }
    };
}// impl

	// transforms *it to joint_view(seq1,(*it))
	template<typename Seq1,typename It, bool is_left = true>
    struct bind_iterator
    {
		typedef detail::fusion::joint_view::binder<Seq1,is_left> binder_;
        typedef detail::fusion::joint_view::result_of::impl::bind_iterator<
        	binder_,It
        > meta_;
        typedef typename meta_::type type;
        static type call(const Seq1& seq1,It it)
        {
        	binder_ binder(seq1);
            return meta_::call(binder,it);
        }
    };

	template<typename Seq1,typename R,bool is_left = true>
    struct bind_range
    {

        typedef typename boost::range_iterator<const R>::type it_;
		typedef detail::fusion::joint_view::result_of::bind_iterator<
        	Seq1,it_,is_left
        > impl_;
        typedef typename boost::iterator_range<typename impl_::type> type;
            
        static type call(const Seq1& seq1,const R& r)
        {
            return type(
                impl_::call(seq1,boost::begin(r)),
                impl_::call(seq1,boost::end(r))
            );
        }
    };
                                

}// result_of

    template<typename Seq1,typename It>
    typename detail::fusion::joint_view::result_of::bind_iterator<Seq1,It>::type
    bind_iterator(const Seq1& seq1,It it)
    {
    	typedef detail::fusion::joint_view::result_of::bind_iterator<
        	Seq1,
            It
        > meta_;
    	return meta_::call(seq1,it);
    }
    
    template<typename Seq1,typename R>
    typename detail::fusion::joint_view::result_of::bind_range<Seq1,R>::type
    bind_range(const Seq1& seq1,const R& r)
    {
    	typedef detail::fusion::joint_view::result_of::bind_range<
        	Seq1,
            R
        > meta_;
    	return meta_::call(seq1,r);
    }

    

}// joint_view
}// fusion
}// detail
}// statistics
}// boost

#endif