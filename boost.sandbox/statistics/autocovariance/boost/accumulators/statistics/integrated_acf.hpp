///////////////////////////////////////////////////////////////////////////////
// integrated_acf.hpp                                                        //
//                                                                           //
//  Copyright 2008 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_ACCUMULATORS_STATISTICS_INTEGRATED_ACF_HPP_ER_2008_04
#define BOOST_ACCUMULATORS_STATISTICS_INTEGRATED_ACF_HPP_ER_2008_04
#include <cmath>
#include <vector>
#include <algorithm>
#include <boost/mpl/int.hpp>
#include <boost/mpl/size_t.hpp>
#include <boost/mpl/assert.hpp>
#include <boost/mpl/placeholders.hpp>

#include <boost/call_traits.hpp>
#include <boost/array.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/type_traits/add_const.hpp>

#include <boost/accumulators/framework/extractor.hpp>
#include <boost/accumulators/framework/accumulator_base.hpp>
#include <boost/accumulators/framework/extractor.hpp>
#include <boost/accumulators/numeric/functional.hpp>
#include <boost/accumulators/framework/parameters/sample.hpp>
#include <boost/accumulators/framework/depends_on.hpp>
#include <boost/accumulators/statistics_fwd.hpp>

#include <boost/accumulators/statistics/acv0.hpp>
#include <boost/accumulators/statistics/integrated_acvf.hpp>

namespace boost { namespace accumulators
{


namespace impl
{
    ////////////////////////////////////////////////////////////////////////////
    // integrated_acf_impl
    template<typename T,typename I>
    class integrated_acf_impl
      : public accumulator_base
    {
    public:
        typedef T                              result_type;

        integrated_acf_impl(dont_care):val(static_cast<T>(0)){}

        template<typename Args>
        void operator ()(Args const &args)
        {
            T iacvf = integrated_acvf<I>(args[accumulator]);
            T acv0_val = acv0<I>(args[accumulator]);
            if(acv0_val>static_cast<T>(0)){
                val = iacvf/acv0_val;
            }else{
                val = static_cast<T>(0);
            }
		}

        result_type result(dont_care) const{ 
            return val;
        }

    private:
	   T  val;

    };

} // namespace impl

///////////////////////////////////////////////////////////////////////////////
// tag::integrated_acf
//

namespace tag
{

    template <typename I = default_delay_discriminator>
    struct integrated_acf
      : depends_on<acv0<I>,integrated_acvf<I> >
    {
        /// INTERNAL ONLY
      typedef
        accumulators::impl::integrated_acf_impl<mpl::_1,I> impl;

    };
}



///////////////////////////////////////////////////////////////////////////////
// extract::integrated_acf
//

namespace extract
{

  template<typename I,typename AccumulatorSet>
  typename mpl::apply<
    AccumulatorSet,tag::integrated_acf<I> >::type::result_type
  integrated_acf(AccumulatorSet const& acc){
    typedef tag::integrated_acf<I> the_tag;
    return extract_result<the_tag>(acc);
  }

}

using extract::integrated_acf;


}} // namespace boost::accumulators

#endif
