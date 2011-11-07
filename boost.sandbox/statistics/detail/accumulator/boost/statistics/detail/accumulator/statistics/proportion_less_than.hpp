///////////////////////////////////////////////////////////////////////////////
// accumulator::statistics::proportion_less_than.hpp                         //
//                                                                           //
//  Copyright 2008 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_ACCUMULATOR_STATISTICS_PROPORTION_LESS_THAN_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_ACCUMULATOR_STATISTICS_PROPORTION_LESS_THAN_HPP_ER_2009
#include <boost/mpl/placeholders.hpp>
#include <boost/mpl/apply.hpp>
#include <boost/parameter/binding.hpp>
#include <boost/accumulators/framework/extractor.hpp>
#include <boost/accumulators/framework/accumulator_base.hpp>
#include <boost/accumulators/framework/parameters/sample.hpp>
#include <boost/accumulators/framework/parameters/accumulator.hpp>
#include <boost/accumulators/framework/depends_on.hpp>
#include <boost/accumulators/statistics/count.hpp>
#include <boost/accumulators/statistics_fwd.hpp>

#include <boost/statistics/detail/accumulator/statistics/count_less_than.hpp>

namespace boost { 
namespace statistics{
namespace detail{
namespace accumulator{

namespace impl
{

    template<typename T>
    class proportion_less_than : public boost::accumulators::accumulator_base
    {
        typedef boost::accumulators::dont_care dont_care_;
        typedef tag::count_less_than tag_m_;
        typedef boost::accumulators::tag::count tag_n_;
        typedef boost::accumulators::tag::accumulator tag_acc_;

        public:
    
        typedef T result_type;

        proportion_less_than(dont_care_){}
        
        void operator()(dont_care_)const{}

        template<typename Args>
        result_type result(const Args& args) const
        {
            typedef boost::parameter::binding<Args,tag_acc_> bind_;
            typedef typename bind_::type cref_;
            cref_ acc = args[boost::accumulators::accumulator];
            T res =  boost::accumulators::extract_result<tag_m_>( acc );
            res /= static_cast<T>(
                boost::accumulators::extract_result<tag_n_>( acc )
            );
            return res;
        }
    };

}//impl

///////////////////////////////////////////////////////////////////////////////
// tag::proportion_less_than
namespace tag
{
    struct proportion_less_than
      : boost::accumulators::depends_on<
        boost::accumulators::tag::count,
        tag::count_less_than
    >
    {
      typedef statistics::detail::accumulator::
      	impl::proportion_less_than<boost::mpl::_1> impl;
    };
}

///////////////////////////////////////////////////////////////////////////////
// extract::proportion_less_than
namespace extract
{

  	template<typename AccSet>
  	typename boost::mpl::apply<
    	AccSet,
        statistics::detail::accumulator::tag::proportion_less_than
	>::type::result_type
  	proportion_less_than(AccSet const& acc){
    	typedef statistics::detail::accumulator
        	::tag::proportion_less_than the_tag;
    	return boost::accumulators::extract_result<the_tag>(acc);
  	}

}

using extract::proportion_less_than;

}// accumulator
}// detail
}// statistics
}// boost

#endif