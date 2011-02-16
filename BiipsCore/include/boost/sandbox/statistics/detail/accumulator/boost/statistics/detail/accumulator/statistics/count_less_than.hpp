///////////////////////////////////////////////////////////////////////////////
// accumulator::statistics::count_less_than.hpp                              //
//                                                                           //
//  Copyright 2008 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_ACCUMULATOR_STATISTICS_COUNT_LESS_THAN_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_ACCUMULATOR_STATISTICS_COUNT_LESS_THAN_HPP_ER_2009
#include <boost/mpl/placeholders.hpp>
#include <boost/mpl/apply.hpp>
#include <boost/accumulators/framework/extractor.hpp>
#include <boost/accumulators/framework/accumulator_base.hpp>
#include <boost/accumulators/framework/parameters/sample.hpp>
#include <boost/accumulators/framework/parameters/accumulator.hpp>
#include <boost/accumulators/framework/depends_on.hpp>

#include <boost/statistics/detail/accumulator/statistics/keyword/threshold.hpp>

namespace boost { 
namespace statistics{
namespace detail{
namespace accumulator{

namespace impl
{

    template<typename T>
    struct count_less_than : public boost::accumulators::accumulator_base
    {
        typedef boost::accumulators::dont_care dont_care_;
    
        typedef std::size_t result_type;

        template<typename Args>
        count_less_than(const Args& args)
            : cnt(0), t(args[keyword::threshold|static_cast<T>(0)]){}

        count_less_than(const count_less_than& that)
            : cnt(that.cnt), t(that.t){}

        template<typename Args>
        void operator ()(const Args& args)
        {
            if(args[boost::accumulators::sample]<this->t)
            {
                ++this->cnt;
            }
        }

        result_type result(dont_care_) const
        {
            return this->cnt;
        }

    private:
        result_type cnt;
        T t;
    };

}//impl

namespace tag
{
    struct count_less_than
      : boost::accumulators::depends_on<>
    {
      typedef statistics::detail::accumulator::
      	impl::count_less_than<boost::mpl::_1> impl;

    };
}

namespace extract
{

  	template<typename AccumulatorSet>
  	typename 
  	boost::mpl::apply<
    	AccumulatorSet,
        statistics::detail::accumulator::tag::count_less_than
	>::type::result_type
  	count_less_than(AccumulatorSet const& acc){
    	typedef statistics::detail::accumulator::tag::count_less_than the_tag;
    	return boost::accumulators::extract_result<the_tag>(acc);
  	}

}

using extract::count_less_than;

}// accumulator
}// detail
}// statistics
}// boost

#endif