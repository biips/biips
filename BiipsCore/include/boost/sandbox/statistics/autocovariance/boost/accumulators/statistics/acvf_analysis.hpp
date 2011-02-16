///////////////////////////////////////////////////////////////////////////////
// acvf_analysis.hpp                                                         //
//                                                                           //
//  Copyright 2008 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_ACCUMULATORS_STATISTICS_ACVF_ANALYSIS_HPP_ER_2008_04
#define BOOST_ACCUMULATORS_STATISTICS_ACVF_ANALYSIS_HPP_ER_2008_04
#include <iostream>
#include <algorithm>
#include <iterator>
#include <functional>
#include <stdexcept>
//#include <boost/assert.hpp>
#include <boost/assign/std/vector.hpp>
#include <boost/parameter/parameters.hpp>
#include <boost/parameter/keyword.hpp>
#include <boost/bind.hpp>
#include <boost/range.hpp>
#include <boost/ref.hpp>
#include <boost/assign/std/vector.hpp>
#include <boost/range/iterator_range.hpp>

#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/delay.hpp>
#include <boost/accumulators/statistics/acvf_moving_average.hpp>
#include <boost/accumulators/statistics/acvf.hpp>
#include <boost/accumulators/statistics/acf.hpp>
#include <boost/accumulators/statistics/percentage_effective_sample_size.hpp>
#include <boost/accumulators/statistics/standard_error_autocorrelated.hpp>
#include <boost/accumulators/statistics/standard_error_iid.hpp>

namespace boost{namespace accumulators{
namespace statistics{

    template<
        typename RealType,
        typename I = default_delay_discriminator
    >
    class acvf_analysis{
        typedef I discr_t;
        public:
        typedef RealType    value_type;

        private:

        typedef boost::accumulators::accumulator_set<
        value_type, boost::accumulators::stats<
            boost::accumulators::tag::mean,
            boost::accumulators::tag::acf<discr_t>,
            boost::accumulators::tag::integrated_acvf<discr_t>,
            boost::accumulators::tag::percentage_effective_sample_size<
                discr_t>,
            boost::accumulators::tag::standard_error_autocorrelated<discr_t>,
            boost::accumulators::tag::standard_error_iid<discr_t>
            >
        >  acc_type;

    public://TODO define copy and assign
        acvf_analysis(std::size_t max_lag)
        :K(max_lag),
        acc(boost::accumulators::tag::delay<discr_t>::cache_size=(K+1)){};

        void operator()(value_type x){
            return acc(x);
        }

        template<typename R>
        void operator()(
            const R& range,
            std::size_t offset,
            std::size_t stride){

            // an iterator range?
            typedef typename range_iterator<const R>::type const_iter_type;
            typedef typename range_size<R>::type size_type;
            const_iter_type i = boost::begin(range);
            const_iter_type e = boost::end(range);
            if(std::distance(i,e)>offset){
                std::advance(i,offset);
                //this has the effect of rounding to smallest
                std::size_t d = (std::distance(i,e)-1)/stride;
                d *= stride;
                e = i; std::advance(e,d+1);

                while(i<e){
                    acc(*i);
                    std::advance(i,stride);
                }
//            for_each(
//                boost::begin(range),
//                boost::end(range),
//                boost::bind<void>(boost::ref(acc),_1)
//            );
            }else{
                std::runtime_error("acvf_analysis");
            }
        }
        std::size_t max_lag()const{ return K; }
        value_type mean()const{ return accumulators::mean(acc); }
        value_type standard_error_iid()const{
            return accumulators::standard_error_iid<discr_t>(acc);
        }
        value_type standard_error_autocorrelated()const{
            return accumulators
                ::standard_error_autocorrelated<discr_t>(acc);
        }
        value_type integrated_acvf()const{
            return accumulators
                ::integrated_acvf<discr_t>(acc);
        }

        value_type percentage_effective_sample_size()const{
            return accumulators
                ::percentage_effective_sample_size<discr_t>(acc);
        }

        void print(std::ostream& os)const
        {
            //using namespace boost::accumulators;
            os  << "count : " << accumulators::count(acc)
                << "\nacf : ";
                    copy(
                        begin(accumulators::acf<discr_t>(acc)),
                        end(accumulators::acf<discr_t>(acc)),
                        std::ostream_iterator<value_type>(os," ")
                    );
            os  << "\nintegrated_acvf : "
                << integrated_acvf()
                << "\ness% : "
                << percentage_effective_sample_size()
                << "\nmean : " << mean()
                << "\nstandard error : "
                << "\n assuming iid : "
                << standard_error_iid()
                << "\n assuming acf is zero after lag "
                << max_lag() << ": "
                << standard_error_autocorrelated() << std::endl;
        };

    private:

        std::size_t K;
        acc_type acc;
    };

//TODO
//    std::ostream& operator<<(std::ostream& os,const acvf_analysis& a){
//        a.print(os);
//        return os;
//    };

}
}}
#endif
