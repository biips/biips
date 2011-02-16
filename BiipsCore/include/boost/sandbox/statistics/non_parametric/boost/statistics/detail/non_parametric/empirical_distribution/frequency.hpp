///////////////////////////////////////////////////////////////////////////////
// accumulator::statistics::empirical_distribution::frequency.hpp            //
//                                                                           //
//  Copyright 2010 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_ACCUMULATOR_STATISTICS_FREQUENCY_HPP_ER_2010
#define BOOST_STATISTICS_DETAIL_ACCUMULATOR_STATISTICS_FREQUENCY_HPP_ER_2010
#include <iostream> // tmp

#include <boost/mpl/placeholders.hpp>
#include <boost/mpl/apply.hpp>
#include <boost/mpl/if.hpp>

#include <boost/parameter/binding.hpp>

#include <boost/accumulators/framework/extractor.hpp>
#include <boost/accumulators/framework/accumulator_base.hpp>
#include <boost/accumulators/framework/parameters/sample.hpp>
#include <boost/accumulators/framework/parameters/accumulator.hpp>
#include <boost/accumulators/framework/depends_on.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/count.hpp>

#include <boost/statistics/detail/non_parametric/empirical_distribution/count.hpp>

namespace boost { 
namespace statistics{
namespace detail{
namespace empirical_distribution{

    // Usage:
    //     namespace ns = empirical_distribution;
    //     accumulator_set<T,stats<ns::tag::count<Cum> > > acc;
    //     acc = boost:for_each(samples,acc);
    //     ns::extract::count<Cum>(acc,x);
    // The result of the last statement is the % of observation matching (less 
    // than) x in the samples if Cum = false (true).

namespace impl{

    // Frequency as percentage of the sample size 
    //
    // Warning : See empirical_distribution::impl::count.
	template<typename T,bool Cum,typename T1>
	class frequency : public boost::accumulators::accumulator_base{
    	
		typedef std::size_t size_;
        typedef boost::accumulators::dont_care dont_care_;

        public:

        frequency(dont_care_){}

        typedef size_ size_type;
        typedef T sample_type;  // See accumulator_set
        typedef T1 result_type;

        void operator()(dont_care_)const{}

        template<typename Args>
        result_type result(const Args& args) const
        {
        }
    };
    
}

namespace tag
{
    template<bool Cum,typename T1 = double>
    struct frequency
      : boost::accumulators::depends_on<
      	statistics::detail::empirical_distribution::tag::count<Cum>,
        boost::accumulators::tag::count
    >
    {
        struct impl{
            template<typename T,typename W>
            struct apply{
        		typedef statistics::detail::empirical_distribution
                	::impl::frequency<T,Cum,T1> type;    	
            };
        };
    };
}

namespace result_of{

    template<bool Cum,typename T1,typename AccSet>
    struct frequency{

        typedef boost::statistics::detail
        	::empirical_distribution::tag::frequency<Cum,T1> tag_;
        typedef typename boost::accumulators
        	::detail::template extractor_result<AccSet,tag_>::type type;

    };
}

namespace extract
{

    // Usage : frequency<Cum,T1>(acc,x)
    template<bool Cum,typename T1,typename AccSet,typename T>
    typename boost::statistics::detail::empirical_distribution
    	::result_of::template frequency<Cum,T1,AccSet>::type
    frequency(AccSet const& acc,const T& x)
    {
		    typedef std::size_t size_;
        	typedef boost::accumulators::tag::count tag_n_;

            namespace ns = boost::statistics::detail::empirical_distribution;

            size_ i =  ns::extract::count<Cum>( acc, x );
            size_ n = boost::accumulators::extract_result<tag_n_>( acc );
            return static_cast<T1>(i)/static_cast<T1>(n);
    }

}

using extract::frequency;

}// empirical_distribution
}// detail
}// statistics
}// boost

#endif
