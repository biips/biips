///////////////////////////////////////////////////////////////////////////////
// weighted_count.hpp                                                        //
//                                                                           //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_ACCUMULATORS_STATISTICS_DETAIL_WEIGHTED_COUNT_HPP_ER_2010
#define BOOST_ACCUMULATORS_STATISTICS_DETAIL_WEIGHTED_COUNT_HPP_ER_2010

#include <boost/mpl/placeholders.hpp>
#include <boost/accumulators/framework/accumulator_base.hpp>
#include <boost/accumulators/framework/extractor.hpp>
#include <boost/accumulators/numeric/functional.hpp>
#include <boost/accumulators/framework/parameters/sample.hpp>
#include <boost/accumulators/framework/parameters/weight.hpp>
#include <boost/accumulators/framework/accumulators/external_accumulator.hpp>
#include <boost/accumulators/framework/depends_on.hpp>
#include <boost/accumulators/statistics_fwd.hpp>

// This file is a modification of Boost.Accumulator's count.hpp by Eric Niebler. 

namespace boost { namespace accumulators
{

namespace impl
{
    ///////////////////////////////////////////////////////////////////////////////
    // weighted_count_impl
    template<typename Weight>
    struct weighted_count_impl
      : accumulator_base
    {
        typedef Weight weighted_sample;

        // for boost::result_of
        typedef weighted_sample result_type;

        template<typename Args>
        weighted_count_impl(Args const &args)
          : weighted_count_(
                numeric::zero<Weight>::value
            )
        {
        }

        template<typename Args>
        void operator ()(Args const &args)
        {
            // what about overflow?
            this->weighted_count_ += args[ weight ];
        }

        result_type result(dont_care) const
        {
            return this->weighted_count_;
        }

    private:

        weighted_sample weighted_count_;
    };

} // namespace impl

namespace tag
{
    struct weighted_count
      : depends_on<>
    {
        typedef accumulators::impl::weighted_count_impl<mpl::_2> impl;
    };

/*
    template<typename VariateType, typename VariateTag>
    struct weighted_count_of_variates
      : depends_on<>
    {
        /// INTERNAL ONLY
        ///
        typedef accumulators::impl::weighted_count_impl<VariateType, mpl::_2, VariateTag> impl;
    };

    struct abstract_weighted_count_of_variates
      : depends_on<>
    {
    };
*/
}

namespace extract
{
    extractor<tag::weighted_count> const weighted_count = {};
//    extractor<tag::abstract_weighted_count_of_variates> const weighted_count_of_variates = {};

    BOOST_ACCUMULATORS_IGNORE_GLOBAL(weighted_count)
//    BOOST_ACCUMULATORS_IGNORE_GLOBAL(weighted_count_of_variates)
}

using extract::weighted_count;
//using extract::weighted_count_of_variates;

/*
template<typename VariateType, typename VariateTag>
struct feature_of<tag::weighted_count_of_variates<VariateType, VariateTag> >
  : feature_of<tag::abstract_weighted_count_of_variates>
{
};
*/

}} // namespace boost::accumulators

#endif
