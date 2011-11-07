///////////////////////////////////////////////////////////////////////////////
// flatten_range.hpp                              				 	 	 	 //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_ITERATOR_FLATTEN_RANGE_HPP_ER_2010
#define BOOST_ITERATOR_FLATTEN_RANGE_HPP_ER_2010
#include <boost/range.hpp>
#include <boost/iterator/flatten_iterator.hpp>

namespace boost{

	template<typename R>
    struct result_of_flatten_range{
    	typedef typename range_iterator<R>::type it_;
        typedef boost::flatten_iterator<it_> fit_;
        typedef boost::iterator_range<fit_> type;

		static type call(it_ b,it_ e){
        	return type(fit_(b,e),fit_(e,e));
        }
    
    };

	template<typename R>
    typename result_of_flatten_range<R>::type
    flatten_range(R& r){
    	typedef result_of_flatten_range<R> meta_;
    	return meta_::call(boost::begin(r),boost::end(r));
    }

	template<typename R>
    typename result_of_flatten_range<const R>::type
    flatten_range(const R& r){
    	typedef result_of_flatten_range<const R> meta_;
    	return meta_::call(boost::const_begin(r),boost::const_end(r));
    }

}

#endif
