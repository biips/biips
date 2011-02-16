///////////////////////////////////////////////////////////////////////////////
// random::gen_to_random.hpp                                                 //
//                                                                           //
//  Copyright 2009 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_RANDOM_GEN_TO_RANDOM_HPP_ER_2009
#define BOOST_RANDOM_GEN_TO_RANDOM_HPP_ER_2009
#include <ostream>
#include <boost/call_traits.hpp>

namespace boost{
namespace random{

    // Adds the interface of RandomDistribution to a (non-random) generator
    template<typename G,typename I = typename G::result_type>
    struct gen_to_random{
        public:

        typedef I                       input_type;
        typedef typename G::result_type result_type;

        //Construction
        gen_to_random();
        gen_to_random(typename call_traits<G>::param_type );
        gen_to_random(const gen_to_random&);
        gen_to_random& operator=(const gen_to_random&);

        template<typename U> result_type operator()(U& u);
        template<typename U> result_type operator()(U& u)const;
        
        // Access
        typename call_traits<G>::const_reference generator()const;

        private:
        result_type impl();
        typename call_traits<G>::value_type g_;
    };

    template<typename G,typename I>
    std::ostream& operator<<(std::ostream& os, const gen_to_random<G,I>& g);
   
    // Implementation //

    template<typename G,typename I>
    std::ostream& operator<<(std::ostream& os, const gen_to_random<G,I>& g)
    {
        return (os << "gen_to_random(" << g << ')' );
    }
        
    template<typename G,typename I>
    gen_to_random<G,I>::gen_to_random(){}
    
    template<typename G,typename I>
    gen_to_random<G,I>::gen_to_random(typename call_traits<G>::param_type g)
    :g_(g){}

    template<typename G,typename I>
    gen_to_random<G,I>::gen_to_random(const gen_to_random& that)
    :g_(that.g_){}
    
    template<typename G,typename I>
    gen_to_random<G,I>& 
    gen_to_random<G,I>::operator=(const gen_to_random& that){
        if(&that!=this){
            g_ = that.g_;
        }
        return *this;
    }


    template<typename G,typename I>
    template<typename U> 
    typename gen_to_random<G,I>::result_type 
    gen_to_random<G,I>::operator()(U& u){
        return (this->impl)();
    }

    template<typename G,typename I>
    template<typename U> 
    typename gen_to_random<G,I>::result_type 
    gen_to_random<G,I>::operator()(U& u)const{
        return (this->impl)();
    }

    template<typename G,typename I>
    typename call_traits<G>::const_reference 
    gen_to_random<G,I>::generator()const{
        return (this->g_);
    }

    template<typename G,typename I>
    typename gen_to_random<G,I>::result_type 
    gen_to_random<G,I>::impl(){
        return (this->g_)();
    }


}// random
}// boost

#endif
