///////////////////////////////////////////////////////////////////////////////
// accumulator::statistics::empirical_distribution::ordered_sample.hpp       //
//                                                                           //
//  Copyright 2010 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_NON_PARAMETRIC_EMPIRICAL_DISTRIBUTION_ORDERED_SAMPLE_HPP_ER_2010
#define BOOST_STATISTICS_DETAIL_NON_PARAMETRIC_EMPIRICAL_DISTRIBUTION_ORDERED_SAMPLE_HPP_ER_2010
#include <map>
#include <functional>

#include <boost/mpl/placeholders.hpp>
#include <boost/mpl/apply.hpp>

#include <boost/accumulators/framework/extractor.hpp>
#include <boost/accumulators/framework/accumulator_base.hpp>
#include <boost/accumulators/framework/parameters/sample.hpp>
#include <boost/accumulators/framework/parameters/accumulator.hpp>
#include <boost/accumulators/framework/depends_on.hpp>
#include <boost/accumulators/statistics_fwd.hpp>

namespace boost { 
namespace statistics{
namespace detail{
namespace empirical_distribution{

    // Usage:
    //     namespace ns = empirical_distribution;
    //     accumulator_set<T,stats<ns::tag::ordered_sample> > acc;
    //     acc = boost:for_each(samples,acc);
    //     ns::extract::ordered_sample(acc);
    // The result of the last statement is a sorted map associating each value 
    // in the sample (of type T) with their number of occurences in the sample.

namespace impl{

    template<typename T>
    class ordered_sample : public boost::accumulators::accumulator_base{
        typedef std::less<T> comp_;
        typedef std::size_t size_;
        typedef boost::accumulators::dont_care dont_care_;
        typedef std::map<T,size_,comp_> map_;

        public:
		
        // See accumulator_set for convention naming sample_type
        typedef T 		sample_type; 
        typedef size_ 	size_type;	 

        // non-const because map::operator[](key) returns a non-const
        typedef map_& result_type;

        ordered_sample(dont_care_){}

        template<typename Args>
        void operator()(const Args& args){
        	++(this->freq[
            		static_cast<T>(
                        args[boost::accumulators::sample]
                    )
                ]
            );
        }
		
        // Returns the entire distribution, represented by a map
        result_type result(dont_care_)const{
            return (this->freq); 
        }

        private:
        mutable map_ freq;
	};
    
}// impl

namespace tag
{
    struct ordered_sample
      : boost::accumulators::depends_on<>
    {
      typedef statistics::detail::empirical_distribution::
      	impl::ordered_sample<boost::mpl::_1> impl;
    };
}// tag

namespace result_of{

    template<typename AccSet>
    struct ordered_sample : boost::accumulators::detail::extractor_result<
        AccSet,
        boost::statistics::detail::empirical_distribution::tag::ordered_sample
    >{};

}// result_of

namespace extract
{

  	template<typename AccSet>
    typename boost::statistics::detail::empirical_distribution
    	::result_of::template ordered_sample<AccSet>::type
  	ordered_sample(AccSet const& acc)
    {
    	typedef boost::statistics::detail::empirical_distribution
    		::tag::ordered_sample the_tag;
        return boost::accumulators::extract_result<the_tag>(acc);
  	}

}// extract

using extract::ordered_sample;

}// empirical_distribution
}// detail
}// statistics
}// boost

#endif