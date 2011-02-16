///////////////////////////////////////////////////////////////////////////////
// kolmogorov_smirnov::statistic.hpp                                         //
//                                                                           //
//  Copyright 2010 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_KOLMOGOROV_SMIRNOV_STATISTIC_HPP_ER_2010
#define BOOST_STATISTICS_DETAIL_KOLMOGOROV_SMIRNOV_STATISTIC_HPP_ER_2010
#include <boost/type_traits.hpp>
#include <boost/range.hpp>

#include <boost/mpl/placeholders.hpp>
#include <boost/mpl/apply.hpp>
#include <boost/mpl/if.hpp>

#include <boost/foreach.hpp>
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
namespace kolmogorov_smirnov{

namespace impl{

    // Warning : See empirical_distribution::impl::count
    template<typename T>
    class kolmogorov_smirnov : public boost::accumulators::accumulator_base{
    	
        typedef std::size_t size_;
        typedef boost::accumulators::dont_care dont_care_;

        public:

        kolmogorov_smirnov(dont_care_){};

        typedef size_ size_type;
        typedef T sample_type;
        typedef void result_type;

        void operator()(dont_care_)const{}

        template<typename Args>
        result_type result(dont_care_) const
        {
        }
	};
    
}

namespace tag
{
    struct kolmogorov_smirnov
      : boost::accumulators::depends_on<
      	statistics::detail::empirical_distribution::tag::ordered_sample,
        boost::accumulators::tag::count
    >
    {
        struct impl{
            template<typename T,typename W>
            struct apply{
                typedef detail::kolmogorov_smirnov::impl::kolmogorov_smirnov<T> type;    	
            };
        };
    };
}
namespace result_of{

    template<typename T1,typename AccSet,typename D>
    struct statistic
    {
        typedef T1 type;
    };

}

    // Usage : statistic<T1>(acc,dist)
    template<typename T1,typename AccSet,typename D>
    typename kolmogorov_smirnov
        ::result_of::template statistic<T1,AccSet,D>::type
    statistic(AccSet const& acc,const D& dist)
    {
            namespace ed = boost::statistics::detail::empirical_distribution;
            namespace ks = boost::statistics::detail::kolmogorov_smirnov;
            typedef T1 val_;
            typedef std::size_t size_;
            typedef boost::accumulators::tag::count tag_n_;
            typedef ed::tag::ordered_sample tag_os_;

            typedef typename ed::result_of::ordered_sample<
                AccSet>::type ref_os_; 
            typedef typename boost::remove_const< //in case ref changed to cref
            	typename boost::remove_reference<
                	ref_os_
                >::type
            >::type os_;
            typedef typename boost::range_reference<os_>::type ref_elem_;

            ref_os_ ref_os 
                = boost::accumulators::extract_result<tag_os_>( acc );

            val_ m1 = static_cast<val_>(0);
            size_ i = 0;
            size_ n = boost::accumulators::extract::count( acc );
            
            BOOST_FOREACH(ref_elem_ e,ref_os){
                i += e.second; 
                val_ ecdf = static_cast<val_>(i) / static_cast<val_>(n);
                val_ true_cdf = cdf( dist, e.first );
                val_ m2 
                	= (true_cdf > ecdf)?(true_cdf - ecdf) : (ecdf - true_cdf);
                if(m2 > m1){ m1 = m2; } 
            }
            
            return m1;
    }

}// empirical_distribution
}// detail
}// statistics
}// boost

#endif