///////////////////////////////////////////////////////////////////////////////
// ars::sampler.hpp                                                          //
//                                                                           //
//  Copyright 2009 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_ARS_SAMPLER_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_ARS_SAMPLER_HPP_ER_2009
#include <boost/ars/error.hpp>
#include <boost/ars/proposal_sampler.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace ars{

// This class adds a rejection steop to proposal_sampler 
//
// PropS is an instance of the class template proposal_sampler
template<typename PropS>
class sampler : public PropS{
    typedef PropS super_t;
    public:

    sampler():
    super_t(),n_max_reject_(super_t::param_::n_max_reject),n_reject_(0){}

    sampler(const sampler& that)
    :PropS(that),n_max_reject_(that.n_max_reject_),n_reject_(that.n_reject_)
    {}

    sampler& operator=(const sampler& that){
        if(&that!=this){
            PropS::operator=(that);
            n_max_reject_ = that.n_max_reject_;
            n_reject_ = that.n_reject_;
        }
        return *this;
    }

    void set_n_max_reject(unsigned n){ n_max_reject_ = n; }

    template<typename U>
    typename super_t::result_type operator()(U& u)const{
        static const char* method
            = "ars::sampler::operator()(U&) after n = %1%";
        typename super_t::result_type draw;
        for(typename super_t::size_type i = 0; i<n_max_reject(); i++){
            n_reject_ = i;
            if(this->sample(u,draw)){
                return draw;
            }
        }
        boost::format f(method); f%n_max_reject();
        throw exception(method,f.str(),*this);
    }
    typename super_t::size_type n_max_reject()const{ return n_max_reject_; }
    typename super_t::size_type n_reject()const{ return n_reject_; }

    protected:
    typename super_t::size_type n_max_reject_;
    mutable typename super_t::size_type n_reject_;
};


}// ars
}// detail
}// statistics
}// boost

#endif 