///////////////////////////////////////////////////////////////////////////////
// accumulator::statistics::percentage_effective_sample_size.hpp             //
//                                                                           //
//  Copyright 2009 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_IMPORTANCE_SAMPLING_STATISTICS_PERCENTAGE_EFFECTIVE_SAMPLE_SIZE_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_IMPORTANCE_SAMPLING_STATISTICS_PERCENTAGE_EFFECTIVE_SAMPLE_SIZE_HPP_ER_2009
#include <boost/parameter/binding.hpp>
#include <boost/mpl/placeholders.hpp>
#include <boost/mpl/apply.hpp>
#include <boost/accumulators/framework/extractor.hpp>
#include <boost/accumulators/framework/accumulator_base.hpp>
#include <boost/accumulators/framework/parameters/sample.hpp>
#include <boost/accumulators/framework/parameters/accumulator.hpp>
#include <boost/statistics/detail/importance_sampling/statistics/variance_of_mean_normalized.hpp>
#include <boost/accumulators/framework/depends_on.hpp>

namespace boost { 
namespace statistics{
namespace detail{
namespace accumulator{
namespace impl
{

    // Var(w/c) = Var(w) / c^2, where c = mean(w)
    template<typename T>
    class percentage_effective_sample_size 
            : public boost::accumulators::accumulator_base
        {
        typedef boost::accumulators::dont_care dont_care_;
    
        typedef tag::variance_of_mean_normalized tag_vmn_;
        typedef boost::accumulators::tag::accumulator tag_acc_;
    
        public:
        typedef T result_type;
        percentage_effective_sample_size(){}
        percentage_effective_sample_size(dont_care_){}
        void operator()(dont_care_)const{}

        template<typename Args>
        result_type result(const Args& args) const
        {

            typedef 
                typename boost::parameter::binding<Args,tag_acc_>::type cref_;
            cref_ acc = args[boost::accumulators::accumulator];

            T vmn = accumulators::extract_result<tag_vmn_>(acc);
            return one / (one+vmn);
        }
        
        static const T one;
    };

    template<typename T>
    const T percentage_effective_sample_size<T>::one = static_cast<T>(1);

}//impl

///////////////////////////////////////////////////////////////////////////////
// tag::percentage_effective_sample_size
namespace tag
{
    struct percentage_effective_sample_size
      : boost::accumulators::depends_on<tag::variance_of_mean_normalized>
    {
      typedef statistics::detail::accumulator::impl
        ::percentage_effective_sample_size<boost::mpl::_1> impl;
    };
}

///////////////////////////////////////////////////////////////////////////////
// extract::percentage_effective_sample_size
namespace extract
{

  template<typename AccSet>
  typename
    boost::mpl::apply<
        AccSet,
        tag::percentage_effective_sample_size
    >::type::result_type
  percentage_effective_sample_size(AccSet const& acc){
    typedef tag::percentage_effective_sample_size the_tag;
    return boost::accumulators::extract_result<the_tag>(acc);
  }

}

using extract::percentage_effective_sample_size;

}// accumulator
}// detail
}// statistics
}// boost

#endif