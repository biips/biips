///////////////////////////////////////////////////////////////////////////////
// ars::function::adaptor.hpp                                                //
//                                                                           //
//  Copyright 2009 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_ARS_FUNCTION_ADAPTOR_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_ARS_FUNCTION_ADAPTOR_HPP_ER_2009
#include <boost/type_traits.hpp>
#include <boost/call_traits.hpp>

//  Make sure to precede this file by
// #include <boost/statistics/detail/distribution_toolkit/distributions/D.hpp>
// #include <boost/statistics/detail/distribution_toolkit/fwd_math/cdf.hpp>
// if D is defined in boost::math

namespace boost{
namespace statistics{
namespace detail{
namespace ars{
namespace function{

// Adapts a distribution in sandbox/distribution_toolkit/distributions to the 
// required signature.
template<typename D>
class adaptor{

    typedef typename remove_const<
        typename remove_reference<
            D
        >::type
    >::type dist_t;

    public:
    typedef typename dist_t::value_type      value_type;
    typedef typename dist_t::policy_type     policy_type;
    typedef adaptor<dist_t>    base_type;

    adaptor(typename call_traits<D>::param_type d):d_(d){}
    adaptor(const adaptor& that):d_(that.d_){}

    adaptor&
    operator=(const adaptor& that){
        if(&that!=this){
            d_ = that.d_;
        }
        return *this;
    }

    void operator()(
        const value_type& x,
        value_type& log_pdf,
        value_type& dlog_pdf
    )const{
        log_pdf = log_unnormalized_pdf(
            this->distribution(),
            x
        );
            
        dlog_pdf = derivative_log_unnormalized_pdf(
            this->distribution(),
            x
        );
    }

    typename call_traits<D>::const_reference distribution()const{
        return d_;
    }

    private:
        typename call_traits<D>::value_type d_;

};

}
}// ars
}// detail
}// statistics
}// boost

#endif