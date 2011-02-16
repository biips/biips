///////////////////////////////////////////////////////////////////////////////
// boost::statistics::detail::functor::range_visitor.hpp                     //
//                                                                           //
//  Copyright 2009 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_FUNCTOR_RANGE_VISITOR_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_FUNCTOR_RANGE_VISITOR_HPP_ER_2009
#include <iterator>
#include <boost/range.hpp>
#include <boost/utility.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace functor{

    // Models Generator
    //
    // Successively returns elements of a range
    template<typename R>
    class range_visitor{
        typedef typename range_iterator<R>::type it_t;
        typedef std::size_t size_type;

        public:
        range_visitor(){}
        range_visitor(const R& r):r_(r){ this-> reset(0); }
        range_visitor(const range_visitor& that)
            :r_(that.r_)
        {
            this->reset(
                std::distance(
                    boost::begin(that.r_),
                    that.it_
                )
            );
        }
        range_visitor& operator=(const range_visitor& that)
        {
            if(&that!=this){
                this->r_ = that.r_;
                this->reset(
                    std::distance(
                        boost::begin(that.r_),
                        that.it_
                    )
                );
            }
            return *this;
        }

        typedef typename range_value<R>::type result_type;

        result_type operator()(){
            typedef typename range_visitor<R>::result_type x_;
            return *(this->it_++);
        }

        void reset(size_type n)
        {
            this->it_ = boost::next(boost::begin(this->r_),n);
        }
        
        private:
        R r_;
        it_t it_;
    };

}// functor
}// detail
}// statistics
}// boost

#endif