///////////////////////////////////////////////////////////////////////////////
// accumulator::statistics::empirical_distribution::count.hpp                //
//                                                                           //
//  Copyright 2010 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_NON_PARAMETRIC_EMPIRICAL_DISTRIBUTION_COUNT_HPP_ER_2010
#define BOOST_STATISTICS_DETAIL_NON_PARAMETRIC_EMPIRICAL_DISTRIBUTION_COUNT_HPP_ER_2010
#include <map>
#include <functional> // less

#include <boost/type_traits.hpp>

#include <boost/mpl/placeholders.hpp>
#include <boost/mpl/apply.hpp>
#include <boost/mpl/bool.hpp>

#include <boost/range.hpp>

#include <boost/accumulators/framework/extractor.hpp>
#include <boost/accumulators/framework/accumulator_base.hpp>
#include <boost/accumulators/framework/parameters/sample.hpp>
#include <boost/accumulators/framework/parameters/accumulator.hpp>
#include <boost/accumulators/framework/depends_on.hpp>
#include <boost/accumulators/statistics_fwd.hpp>

#include <boost/statistics/detail/non_parametric/empirical_distribution/ordered_sample.hpp>

namespace boost { 
namespace statistics{
namespace detail{
namespace empirical_distribution{

    // Usage:
    //     namespace ns = empirical_distribution;
    //     accumulator_set<T,stats<ns::tag::count<Cum> > > acc;
    //     acc = boost:for_each(samples,acc);
    //     ns::extract::count<Cum>(acc,x);
    // The result of the last statement is number of observations matching (less
    //  than) x in the samples if Cum = false (true).

namespace impl{

    // T can be an integer or a float
    template<typename T,bool Cum,typename Comp = std::less<T> >
	class count : public boost::accumulators::accumulator_base{
        typedef Comp comp_;
        typedef std::size_t size_;
        typedef boost::accumulators::dont_care dont_care_;

        public:

        typedef size_ result_type;
        typedef T sample_type;

        count(dont_care_){}

        void operator()(dont_care_){}
		
        template<typename Args>
        result_type result(const Args& args)const{
            sample_type key = args[boost::accumulators::sample];
            typedef typename boost::mpl::bool_<Cum> is_cum_;
            typedef boost::statistics::detail
            	::empirical_distribution::tag::ordered_sample tag_;
            return this->result_impl(
                boost::accumulators::extract_result<tag_>(
                    args[boost::accumulators::accumulator]
                ),
                key,
                is_cum_()
            ); 
        }

        private:
		
        template<typename Map>
        result_type result_impl(
            Map& map,
            const sample_type& key,
            boost::mpl::bool_<false> cum
        )const{
            return (map[key]); 
        }

        template<typename Map>
        result_type result_impl(
            Map& map, 
            const sample_type& x,
            boost::mpl::bool_<true> cum
        )const{
           return std::for_each(
                boost::const_begin(map),
                this->bound(map,x),
                accumulator()
           ).value; 
        }

        template<typename Map>
        typename boost::range_iterator<const Map>::type
        bound(
            const Map& map,
            const sample_type& x
        )const{
            return map.upper_bound(x);
        }

        struct accumulator{
            mutable size_ value;
        	
            accumulator():value(0){}
            accumulator(const accumulator& that)
            	:value(that.value){}
            
            template<typename Data>
            void operator()(const Data& data)const{
            	value += data.second;
            }
        
        };
    };
    
}

namespace tag
{
    template<bool Cum>
    struct count: boost::accumulators::depends_on<
      	boost::statistics::detail
        	::empirical_distribution::tag::ordered_sample
    >
    {
        struct impl{
            template<typename T,typename W>
            struct apply{
                typedef boost::statistics::detail::empirical_distribution
                	::impl::count<T,Cum> type;
            };
        };
    };
}
namespace result_of{

    template<bool Cum,typename AccSet>
    struct count{
    	typedef boost::statistics::detail
        	::empirical_distribution::tag::count<Cum> tag_;
        typedef typename 
            boost::accumulators::detail::template 
            	extractor_result<AccSet,tag_>::type type; 
    };

}
namespace extract
{

    // Usage : count<Cum>(acc,x)
    template<bool Cum,typename AccSet,typename T>
    typename boost::statistics::detail
    	::empirical_distribution::result_of::template count<Cum,AccSet>::type
  	count(AccSet const& acc,const T& x)
    {
    	typedef boost::statistics::detail
        	::empirical_distribution::tag::count<Cum> tag_;
        return boost::accumulators::extract_result<tag_>(
            acc,
            (boost::accumulators::sample = x)
        );
  	}

}

using extract::count;

}// empirical_distribution
}// detail
}// statistics
}// boost

#endif
