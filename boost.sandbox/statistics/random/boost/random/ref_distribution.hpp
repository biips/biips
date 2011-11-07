///////////////////////////////////////////////////////////////////////////////
// random::ref_distribution.hpp                                              //
//                                                                           //
//  Copyright 2009 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_RANDOM_REF_RANDOM_DISTRIBUTION_HPP_ER_2009
#define BOOST_RANDOM_REF_RANDOM_DISTRIBUTION_HPP_ER_2009
#include <boost/call_traits.hpp>
#include <boost/type_traits.hpp>

namespace boost{
namespace random{

    // Models RandomDistribution.
    //
    // Passing a reference_wrapper as second argument to variate_generator
    // cannot work because it lacks nested types input_type and result_type.
    // This class remedies this problem. 
    //
    // Rd = T or T& etc. and T must model RandomDistribution
    template<typename Rd>
    class ref_distribution{
        BOOST_MPL_ASSERT((
            is_reference<Rd>
        )); //If not, no point in using this class
        typedef typename remove_reference<Rd>::type  cv_d_;
        public:
        typedef typename remove_cv<cv_d_>::type distribution_type;
    
        typedef typename distribution_type::input_type input_type;
        typedef typename distribution_type::result_type result_type;

        ref_distribution(typename call_traits<Rd>::param_type d);
        ref_distribution(const ref_distribution&);
        ref_distribution& operator=(const ref_distribution&);
    
        template<typename U> result_type operator()(U& urng);
        template<typename U> result_type operator()(U& urng)const;

        typename call_traits<Rd>::reference distribution();
        
        template<typename Rd1>
        friend std::istream& operator>>(std::ostream&,ref_distribution<Rd1>&);

        private:
        template<typename U> result_type impl(U& urng);
        typename call_traits<Rd>::value_type d_;
    };

    template<typename Rd>
    std::ostream& operator<<(std::ostream&,const ref_distribution<Rd>&);

    // Implementation //

    template<typename Rd>
    std::ostream& operator<<(
        std::ostream& os,const ref_distribution<Rd>& r
    ){
        os << r.distribution();
        return os;
    }

    template<typename Rd>
    std::istream& operator>>(
        std::ostream& is,
        ref_distribution<Rd>& r
    ){
        is >> r.distribution();
        return is;
    }

    template<typename Rd>
    ref_distribution<Rd>::ref_distribution(    
        typename call_traits<Rd>::param_type d
    ):d_(d){}

    template<typename Rd>
    ref_distribution<Rd>::ref_distribution(
        const ref_distribution& that
    ):d_(that.d_){}

    template<typename Rd>
    ref_distribution<Rd>&
    ref_distribution<Rd>::operator=(const ref_distribution& that){
        if(&that!=this){
            d_ = that.d_;
        }
        return (*this);
    }
    
    template<typename Rd>
    template<typename U>
    typename ref_distribution<Rd>::result_type 
    ref_distribution<Rd>::operator()(U& urng){
        return (this->impl(urng));
    }

    template<typename Rd>
    template<typename U>
    typename ref_distribution<Rd>::result_type 
    ref_distribution<Rd>::operator()(U& urng)const{
        return (this->impl(urng));
    }

    template<typename Rd>
    template<typename U>
    typename ref_distribution<Rd>::result_type 
    ref_distribution<Rd>::impl(U& urng){
        return (this->distribution())(urng);
    }


    template<typename Rd>
    typename call_traits<Rd>::reference 
    ref_distribution<Rd>::distribution(){ return d_;}
        
    

}// random
}// boost

#endif