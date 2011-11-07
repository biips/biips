//////////////////////////////////////////////////////////////////////////////
// iterator::transform_iterator::deduce_sig_uf.hpp                          //
//                                                                          //
//  (C) Copyright 2010 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_ITERATOR_TRANSFORM_ITERATOR_DEDUCE_SIG_UF_ER_2010_HPP
#define BOOST_ITERATOR_TRANSFORM_ITERATOR_DEDUCE_SIG_UF_ER_2010_HPP
#include <boost/iterator/iterator_traits.hpp>
#include <boost/iterator/transform_iterator.hpp>
#include <boost/utility/result_of.hpp>

namespace boost
{

	namespace detail 
  	{
		namespace transform_iterator{

   	 		template <class Iterator>
    		struct deduce_sig_uf
    		{
				typedef typename boost::iterator_reference<Iterator>::type arg_;

				template<typename UnaryFunc>
                struct apply{
					typedef typename boost::result_of<
                		UnaryFunc(arg_)>::type result_;

					struct wrapper{
        				wrapper(const UnaryFunc& f):g(f){}

                        typedef result_ result_type;

						template<typename T>
						result_type operator()(const T& x)const{ 
                        	return g(x); 
                        }
						private:
						mutable UnaryFunc g; // MUST be contained
					};                

                	typedef boost::transform_iterator<wrapper,Iterator> type;

					static type call(Iterator it,const UnaryFunc& f){
                		return boost::make_transform_iterator(it,wrapper(f));
                	}
                };
    		};
            
        }// transform_iterator
	}// detail

    // Activated if UnaryFun::result<F(Arg)>::type defined. See:
    // http://www.boost.org/doc/libs/1_42_0/libs/utility/utility.htm#result_of
    template <class UnaryFunc, class Iterator>
    typename detail::transform_iterator::deduce_sig_uf<Iterator>
        ::template apply<UnaryFunc>::type
    make_transform_iterator2(
        Iterator it, 
        UnaryFunc fun,
        typename UnaryFunc::template result<UnaryFunc(
            typename detail::transform_iterator::deduce_sig_uf<Iterator>::arg_
        )>::type* = 0
    )
    {	
    	typedef typename 
        	detail::transform_iterator::deduce_sig_uf<Iterator> meta_;
    	return meta_::template apply<UnaryFunc>::call(it,fun);
    }


    // Activated if UnaryFun::result_type defined. See:
    // http://www.boost.org/doc/libs/1_42_0/libs/utility/utility.htm#result_of
    template <class UnaryFunc, class Iterator>
    boost::transform_iterator<UnaryFunc,Iterator>
    make_transform_iterator2(
        Iterator it, 
        UnaryFunc fun,
        typename UnaryFunc::result_type* = 0
    )
    {	
    	return boost::make_transform_iterator(it,fun);
    }
	
}// boost

#endif