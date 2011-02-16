//////////////////////////////////////////////////////////////////////////////
// iterator::transform_iterator::deduce_sig_fp.hpp                          //
//                                                                          //
//  (C) Copyright 2010 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_ITERATOR_TRANSFORM_ITERATOR_DEDUCE_SIG_FP_ER_2010_HPP
#define BOOST_ITERATOR_TRANSFORM_ITERATOR_DEDUCE_SIG_FP_ER_2010_HPP
#include <boost/range.hpp>
#include <boost/iterator/transform_iterator.hpp>
#include <boost/iterator/iterator_traits.hpp>

namespace boost
{

	namespace detail 
  	{
		namespace transform_iterator{

    		template<typename Return,class Iterator>
  			struct deduce_sig_fp{
				typedef typename boost::iterator_reference<
                	Iterator>::type arg_;
    			typedef Return(*sig)(arg_);
				typedef boost::transform_iterator<
                	sig,Iterator,Return> type;	
                
                static type call(Iterator it,sig fp){
                	return boost::make_transform_iterator(it,fp);
                }
  			};

 	 	}// transform_iterator
	}// detail

	// Deduces the function pointer passed as second argument
	template <class Return, class Iterator>
  	typename detail::transform_iterator::deduce_sig_fp<
    	Return,Iterator>::type
   	make_transform_iterator2(
   		Iterator it, 
    	typename detail::transform_iterator::deduce_sig_fp<
        	Return,Iterator>::sig fp
  	)
  	{
		return detail::transform_iterator::deduce_sig_fp<
        	Return,Iterator>::call(it,fp);
  	}

	
}// boost

#endif