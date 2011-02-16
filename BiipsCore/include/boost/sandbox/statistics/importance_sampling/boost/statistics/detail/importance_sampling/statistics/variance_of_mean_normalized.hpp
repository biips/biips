///////////////////////////////////////////////////////////////////////////////
// accumulator::statistics::variance_of_mean_normalized.hpp                  //
//                                                                           //
//  Copyright 2009 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_IMPORTANCE_SAMPLING_STATISTICS_VARIANCE_OF_MEAN_NORMALIZED_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_IMPORTANCE_SAMPLING_STATISTICS_VARIANCE_OF_MEAN_NORMALIZED_HPP_ER_2009
#include <boost/parameter/binding.hpp>
#include <boost/mpl/placeholders.hpp>
#include <boost/mpl/apply.hpp>
#include <boost/accumulators/framework/extractor.hpp>
#include <boost/accumulators/framework/accumulator_base.hpp>
#include <boost/accumulators/framework/parameters/sample.hpp>
#include <boost/accumulators/framework/parameters/accumulator.hpp>
#include <boost/accumulators/framework/depends_on.hpp>
#include <boost/accumulators/statistics_fwd.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/variance.hpp>

namespace boost { 
namespace statistics{
namespace detail{
namespace accumulator{

namespace impl
{

    // Var(w/c) = Var(w) / c^2, where c = mean(w)
    template<typename T>
    class variance_of_mean_normalized 
            : public boost::accumulators::accumulator_base{
        typedef boost::accumulators::dont_care dont_care_;
    
        typedef boost::accumulators::tag::variance tag_v_;
        typedef boost::accumulators::tag::mean tag_m_;
        typedef boost::accumulators::tag::accumulator tag_acc_;
    
        public:
        typedef T result_type;
        variance_of_mean_normalized(){}
        variance_of_mean_normalized(dont_care_){}
        void operator()(dont_care_)const{}

        template<typename Args>
        result_type result(const Args& args) const
        {
            typedef 
                typename boost::parameter::binding<Args,tag_acc_>::type cref_;
            cref_ acc = args[boost::accumulators::accumulator];
            T v = accumulators::extract_result<tag_v_>(acc);
            T c = accumulators::extract_result<tag_m_>(acc);
            return v / (c*c);
        }
    };

}//impl

///////////////////////////////////////////////////////////////////////////////
// tag::variance_of_mean_normalized
namespace tag
{
    struct variance_of_mean_normalized
      : boost::accumulators::depends_on<
        boost::accumulators::tag::mean,
        boost::accumulators::tag::variance
    >
    {
      typedef statistics::detail::accumulator::
      	impl::variance_of_mean_normalized<boost::mpl::_1> impl;

    };
}

///////////////////////////////////////////////////////////////////////////////
// extract::variance_of_mean_normalized
namespace extract
{

  template<typename AccSet>
  typename
    boost::mpl::apply<
        AccSet,
        tag::variance_of_mean_normalized
    >::type::result_type
    variance_of_mean_normalized(AccSet const& acc){
        typedef tag::variance_of_mean_normalized the_tag;
        return boost::accumulators::extract_result<the_tag>(acc);
    }

}

using extract::variance_of_mean_normalized;

}// accumulator
}// detail
}// statistics
}// boost

#endif