///////////////////////////////////////////////////////////////////////////////
// kernel::functional::rp_visitor.hpp                                        //
//                                                                           //
//  Copyright 2009 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
/////////////////////////////////////////////////////////////////////////////// 
#ifndef BOOST_STATISTICS_DETAIL_KERNEL_ESTIMATION_DENSITY_VISITOR_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_KERNEL_ESTIMATION_DENSITY_VISITOR_HPP_ER_2009
#include <algorithm>
#include <boost/concept/assert.hpp>
#include <boost/concept_check.hpp>
#include <boost/call_traits.hpp>
#include <boost/call_traits.hpp>
#include <boost/type_traits/is_reference.hpp>
#include  <boost/type_traits/is_convertible.hpp>
#include <boost/mpl/nested_type.hpp>
#include <boost/mpl/assert.hpp>
#include <boost/statistics/detail/kernel/estimation/detail/mean_accumulator.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace kernel{

    // A type that models Trainer and Predictor
    // See https://svn.boost.org/svn/boost/sandbox/statistics/cross_validation/boost/statistics/detail/cross_validation/estimator/concept/ 
    //
    // R:   range of a data range 
    // V:   visitor 
    // K:   kernel
    // A:   accumulator
    //
    // V                                      ->     data range of type R
    // rp_visitor                             ->     {x[i]:i=1,...,n}
    // meta_nw_visitor_unary<F1,F2>::apply    ->     {(x,y)[i]:i=1,...,n}
    //
    // It is recommended that R derives from iterator_range because it is cheap
    // to copy.
    template<
        typename R, 
        template<typename,typename,typename> class V,
        typename K,
        typename A = typename 
            detail::mean_accumulator<typename K::result_type>::type 
    >
    class estimator{
    
        public:
        typedef R training_dataset_type;
        typedef K kernel_type;
        typedef A accumulator_type;

        template<typename X>
        struct visitor
        {
            typedef V<K,const X&,A> arg_;
            typedef typename mpl::nested_type<arg_>::type type;
        };

        template<typename X>
        struct result_of_predict
        {
            typedef typename visitor<X>::type v_;
            typedef typename v_::result_type type;
        };
    
        // Constructor
        estimator(){}
        estimator(K k)
            :k_(k){}
        estimator(K k,const A& a)
            :k_(k),a_(a){}
        estimator(const estimator& that)
            :td_(that.td_),k_(that.k_),a_(that.a_){}
        estimator& operator=(const estimator& that){
            if(&that = this){
                this->td_ = that.td_;
                this->k_ = that.k_;
                this->a_ = that.a_;
            }
            return *this;
        }
    
        // Access
        const R& training_dataset()const{
            return this->td_;
        }
        const K& kernel()const{
            return this->k_; 
        }
        const A& accumulator()const{
            return this->a_;
        }

        void train(const R& training_dataset){
            BOOST_CONCEPT_ASSERT((
                boost::Assignable<R>
            ));
            this->td_ = training_dataset;
        }

        template<typename X> 
        typename visitor<X>::type 
        visit(const X& x)const{
            typedef estimator this_type;
            typedef typename visitor<X>::type v_;
    
            return std::for_each(
                boost::begin( this->training_dataset() ),
                boost::end( this->training_dataset() ),
                v_( this->kernel(), x, this->a_)
            );
        }
        
        template<typename X> 
        typename result_of_predict<X>::type 
        predict(const X& x)const{
            return visit(x).estimate();
        }

    protected:
        R td_;
        K k_;
        A a_; // Initialized accumulator 
    };

}// kernel
}// detail
}// statistics
}// boost

#endif