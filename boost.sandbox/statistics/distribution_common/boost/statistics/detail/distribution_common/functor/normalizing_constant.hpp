///////////////////////////////////////////////////////////////////////////////
// distribution::common::functor::normalizing_constant.hpp                   				 //
//                                                                           //
//  Copyright 2009 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_DISTRIBUTION_COMMON_FUNCTOR_NORMALIZING_CONSTANT_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_DISTRIBUTION_COMMON_FUNCTOR_NORMALIZING_CONSTANT_HPP_ER_2009
#include <boost/mpl/assert.hpp>
#include <boost/statistics/detail/distribution_common/meta/value.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace distribution{
namespace functor{

    template<typename D>
    class normalizing_constant_
    {

        BOOST_MPL_ASSERT(( boost::mpl::not_< boost::is_reference<D> >)); 
        // use reference::wrapper if a ref is needed

        public:
        typedef D distribution_type;
        typedef typename common::meta::value<D>::type result_type;

        normalizing_constant_(){}
        normalizing_constant_(const D& d):d_(d){}
        normalizing_constant_(const normalizing_constant_& that):d_(that.d_){}
        normalizing_constant_& operator=(const normalizing_constant_& that)
        {
            if(&that!=this)
            {
                d_ = that.d_;
            }
            return (*this);
        }

        template<typename X>
        result_type operator()(const X& x)const{
            return normalizing_constant( this->distribution(), x );
        }
    
        const D& distribution()const{ return this->d_; }
    
        private:
        D d_;
    
    };

}// functor
}// distribution
}// detail
}// statistics
}// boost

#endif
