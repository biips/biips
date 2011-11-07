///////////////////////////////////////////////////////////////////////////////
// importance_sampling::prepare_weights.hpp                                  //
//                                                                           //
//  Copyright 2009 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_IMPORTANCE_SAMPLING_WEIGHTS_PREPARE_WEIGHTS_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_IMPORTANCE_SAMPLING_WEIGHTS_PREPARE_WEIGHTS_HPP_ER_2009
#include <iterator>
#include <functional>
#include <boost/format.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/iterator/filter_iterator.hpp>
#include <boost/iterator/iterator_traits.hpp>
#include <boost/statistics/detail/importance_sampling/weights/apply_exp_offset.hpp>
#include <boost/statistics/detail/importance_sampling/weights/scale_to_finite_sum.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace importance_sampling{

    // Warning: read side effects carefully.
    template<typename T>
    class prepare_weights{
        public:
        typedef T value_type;
        typedef std::size_t size_type;

        prepare_weights()
        :max_log_(default_max_log),offset_(zero),scaling_factor_(zero){}

        prepare_weights(value_type max_log)
        :max_log_(max_log),offset_(zero),scaling_factor_(zero){}

        // Default copy/assign

        // [ Input ]
        // max_log controls precision hence raising it should decr pc_lt_eps
        // but also incr risk that cum_sum isinf. 
        value_type max_log()const{ return this->max_log_; }  
 
        // [ Output ]
        // lw <- lw + offset, max{lw}+offset = max_log
        value_type offset()const{ return this->offset_; }  
        // w <- w/c such that sum{w/c}<inf
        value_type scaling_factor()const{ return this->scaling_factor_; }
        value_type count()const{ return this->n_; } 
        
        // [ Side effect ] 
        // 1) w <- exp(lw+offset)
        // 2) if needed, w <- w/c such that sum{w} < inf
        template<typename ItW>
        void operator()(ItW b_w,ItW e_w) // log( unnormalized weights )
        {
            this->offset_ = apply_exp_offset(
                b_w,
                e_w,
                this->max_log()
            );

            // if max_log is small enough (which costs precision), this does not 
            // nothing i.e. scaling_factor = 1
            this->scaling_factor_ = scale_to_finite_sum(
                b_w,
                e_w
            ); 
            
            this->n_ = std::distance(b_w,e_w);
        }

        static const char* header;
        private:
        value_type max_log_;
        value_type offset_;
        value_type scaling_factor_;
        size_type n_;
        static const value_type zero;
        static const value_type default_max_log;

    };

    template<typename T>
    std::ostream& operator<<(
        std::ostream& out, 
        const prepare_weights<T>& that
    ){
        out << 
            (
                boost::format("(%1%,%2%,%3%)")
                % that.count()
                % that.offset()
                % that.scaling_factor()
            ).str();
        return out;
    }

    template<typename T>
    const char* prepare_weights<T>::header 
        = "(count, offset,scaling_factor)";

    template<typename T>
    const typename prepare_weights<T>::value_type
    prepare_weights<T>::default_max_log = static_cast<value_type>(0);

    template<typename T>
    const typename prepare_weights<T>::value_type
    prepare_weights<T>::zero = static_cast<value_type>(0);

}// importance_weights
}// detail
}// statistics
}// boost

#endif