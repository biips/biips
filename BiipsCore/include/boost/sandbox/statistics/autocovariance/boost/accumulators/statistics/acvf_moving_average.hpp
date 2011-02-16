///////////////////////////////////////////////////////////////////////////////
// acvf_moving_average.hpp                                                   //
//                                                                           //
//  Copyright 2008 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_ACCUMULATORS_STATISTICS_ACVF_MOVING_AVERAGE_HPP_ER_2008_04
#define BOOST_ACCUMULATORS_STATISTICS_ACVF_MOVING_AVERAGE_HPP_ER_2008_04

#include <cmath>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <boost/call_traits.hpp>
//#include <boost/assert.hpp>
#include <boost/range.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/iterator/iterator_traits.hpp>
namespace boost { namespace accumulators{

    /// This is not an accumulator, only a formula:
    /// Under model \f$ x_t = theta_0 e_{t-0} + ... + theta_q e_{t-q} \f$,
    /// where the \f$ e_i \f$'s are independent and \f$Var[e_i]=1 \f$,
    /// \f$ acvf(h) = sum_{j=0}^{q-h} theta_j theta_{j+h},
    /// 0\leq h \leq q \f$
    /// Multiply result by \f$ Var[e_i] \f$ if it is not 1.
    template<typename R>
    class acvf_moving_average{
        typedef typename range_iterator<const R>::type iterator_type;
    public:
        typedef std::size_t                             argument_type;
        typedef typename
            boost::iterator_value<iterator_type>::type  result_type;
            acvf_moving_average(const R& coeffs_):coeffs(coeffs_){}
            acvf_moving_average(const acvf_moving_average& that)
            :coeffs(that.coeffs){}
            acvf_moving_average& operator=(const acvf_moving_average& that){
                if(&that!=this){
                        std::runtime_error("acvf_moving_average::operator=");}
                return *this;
            }
            result_type operator()(argument_type delay)const{
                typedef typename range_iterator<const R>::type iterator_type;
                result_type res = static_cast<result_type>(0);
                size_t h = delay;
                if(coeffs.size()>0){
                    std::size_t q = coeffs.size()-1;//MA(q)
                    if(!(h>q)){
                        iterator_type i = coeffs.begin();
                        iterator_type e = i; std::advance(e,q+1-h);
                        iterator_type i_shifted = i; std::advance(i_shifted,h);
                        iterator_type e_shifted = e; std::advance(e_shifted,h);
                        while(i<e){
                            res+=(*i)*(*i_shifted);
                            ++i; ++i_shifted;
                        }//TODO accumulate(make_zip_iterator(...
                    }
                }
                return res;
            }
    private:
        const R& coeffs;
    };

    template<typename R>
    acvf_moving_average<R> make_acvf_moving_average(const R& coeffs){
        return acvf_moving_average<R>(coeffs);
    };


}}

#endif
