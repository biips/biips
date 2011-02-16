//////////////////////////////////////////////////////////////////////////////
// accumulator::zscore.hpp                                                  //
//                                                                          //
//  (C) Copyright 2010 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_ACCUMULATOR_ZSCORE_HPP_ER_2010           
#define BOOST_STATISTICS_DETAIL_ACCUMULATOR_ZSCORE_HPP_ER_2010      
#include <cmath>
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/variance.hpp>
#include <boost/accumulators/framework/extractor.hpp>
#include <boost/accumulators/framework/accumulator_base.hpp>
#include <boost/accumulators/framework/parameters/sample.hpp>
#include <boost/accumulators/framework/parameters/accumulator.hpp>
#include <boost/accumulators/framework/depends_on.hpp>
#include <boost/accumulators/statistics/stats.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace accumulator{

namespace impl
{
        
    template<typename T>
    struct zscore : public boost::accumulators::accumulator_base
    {
        typedef boost::accumulators::dont_care dont_care_;
            
        typedef T result_type;
            
        zscore(dont_care_){}

        void operator ()(dont_care_)const
        {
        }

        template<typename Args>
        result_type result(const Args& args)const{
        	typedef boost::accumulators::tag::accumulator tag_acc_;        
        	typedef boost::parameter::binding<Args,tag_acc_> bind1_;
        	typedef typename bind1_::type cref_acc_;
            cref_acc_ cref_acc = args[boost::accumulators::accumulator];
			T x = args[boost::accumulators::sample]; 
            T m = boost::accumulators::mean(cref_acc);
            T s = boost::accumulators::variance(cref_acc);
            s = std::sqrt(s);
            return (x - m)/s;
		}
	};

}//impl

namespace tag
{
    struct zscore
        : boost::accumulators::depends_on<
        	boost::accumulators::tag::mean,
        	boost::accumulators::tag::variance
        >
    {
        struct impl{
            template<typename T,typename W>
            struct apply{
                typedef boost::statistics::detail::accumulator::impl
                    	::zscore<T> type;    	
            };
        };
    };
}
    
namespace result_of{
        
    template<typename AccSet>
    struct zscore{
        typedef boost::statistics::detail::accumulator::tag::zscore tag_;
        typedef typename boost::accumulators::detail::extractor_result<
            AccSet,
            tag_
        >::type type;
    };
        
}
    
namespace extract
{
        
    // Usage : zscore(acc,x)
    template<typename AccSet,typename T1>
    typename boost::statistics::detail::accumulator
    		::result_of::template zscore<AccSet>::type
    zscore(AccSet const& acc,const T1& x)
    {
        typedef boost::statistics::detail::accumulator::tag::zscore the_tag;
        return boost::accumulators::extract_result<the_tag>(
            acc,
            (boost::accumulators::sample = x)
        );
    }
}
    
    using extract::zscore;

}// accumulator
}// detail
}// statistics
}// boost

#endif
