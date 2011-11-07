//////////////////////////////////////////////////////////////////////////////
// cross_validation::estimator::adaptor::nullary_predictor.hpp              //
//                                                                          //
//  (C) Copyright 2009 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef  BOOST_STATISTICS_DETAIL_CROSS_VALIDATION_ESTIMATOR_ADAPTOR_NULLARY_PREDICTOR_HPP_ER_2009
#define  BOOST_STATISTICS_DETAIL_CROSS_VALIDATION_ESTIMATOR_ADAPTOR_NULLARY_PREDICTOR_HPP_ER_2009
#include <boost/mpl/apply.hpp>
#include <boost/mpl/empty_base.hpp>
// #include <boost/typeof/typeof.hpp>
#include <boost/accumulators/accumulators.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace cross_validation{
namespace estimator{
namespace adaptor{

    // Adapts an accumulator associated with a tag to model Predictor
    //
    // See http://boost-sandbox.sourceforge.net/libs/accumulators/doc/html/accumulators/user_s_guide.html#accumulators.user_s_guide.the_accumulators_framework.concepts
    // B->const_reference() must return an accumulator
    template<
        typename A,
        typename Tag,
        typename B
    >
    class nullary_predictor : public B
    {
        public:
        nullary_predictor(const B& b):B(b){}
        nullary_predictor(const nullary_predictor& that):B(that){}

        typedef typename mpl::apply<
            A,
            Tag 
        >::type::result_type result_type;

        // This should be equivalent, up to add_reference
        //typedef BOOST_TYPEOF_TPL(
        //    boost::accumulators::extract_result<Tag>(A())
        //) result_type;

        template<typename X>
        result_type predict(const X&)const{
            return boost::accumulators::extract_result<Tag>(
                this->const_reference()
            );
        }

        private:
        nullary_predictor(){}
    };

namespace meta{
    template<typename A,typename Tag>
    struct nullary_predictor{
    
        template<typename B>
        struct apply{
            typedef adaptor::nullary_predictor<A,Tag,B> type;
        };

    };
}
    
}// adaptor
}// estimator
}// cross_validation
}// detail
    

}// statistics
}// boost

#endif