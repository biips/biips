//////////////////////////////////////////////////////////////////////////////
// serialization::detail::inherit::detail::inherit2.hpp         			//
//                                                                          //
//                                                                          //
//  (C) Copyright 2010 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_SERIALIZATION_DETAIL_INHERIT_DETAIL_INHERIT2_HPP_ER_2010
#define BOOST_SERIALIZATION_DETAIL_INHERIT_DETAIL_INHERIT2_HPP_ER_2010
#include <boost/mpl/empty_base.hpp>
#include <boost/concept/assert.hpp>
#include <boost/accumulators/framework/accumulator_base.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/detail/inherit/detail/stream.hpp>
#include <boost/serialization/detail/concept/serializable.hpp>

namespace boost{
namespace serialization{
namespace detail{

	// inherit2<T1,T2> innherits from the elements of {T1,T2} that are not of
    // type mpl::empty_base, and for those, forwards an argument pack at
    // construction, and serialization.
    //
    // operator<<(ostream,) requires special formatting that is implemented in
    // detail/stream.hpp

	template<typename T1,typename T2>
	struct inherit2 : T1, T2
	{
    	typedef inherit2 type;
                
		inherit2(){}

    	template<typename Args>
    	inherit2(const Args& args):T1(args),T2(args){}

		private:
        
        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive & ar, const unsigned int version)
        {
			BOOST_CONCEPT_ASSERT((concept::Serializable<T1>));
			BOOST_CONCEPT_ASSERT((concept::Serializable<T2>));
            ar & boost::serialization::base_object<T1>(*this);
            ar & boost::serialization::base_object<T2>(*this);
        }
	};
	

	template<typename T1>
	struct inherit2<T1,boost::mpl::empty_base> : T1
	{
    	typedef inherit2 type;

		inherit2(){}

    	template<typename Args>
    	inherit2(const Args& args):T1(args){}

		private:
		typedef boost::mpl::empty_base empty_;

        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive & ar, const unsigned int version)
        {
			BOOST_CONCEPT_ASSERT((concept::Serializable<T1>));
            ar & boost::serialization::base_object<T1>(*this);
        }
        
	};

	template<typename T2>
	struct inherit2<boost::mpl::empty_base,T2> : T2
	{
    	typedef inherit2 type;

		inherit2(){}

    	template<typename Args>
    	inherit2(const Args& args):T2(args){}

		private:

		typedef boost::mpl::empty_base empty_;

        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive & ar, const unsigned int version)
        {
			BOOST_CONCEPT_ASSERT((concept::Serializable<T2>));
            ar & boost::serialization::base_object<T2>(*this);
        }
	};

	template<>
	struct inherit2<boost::mpl::empty_base,boost::mpl::empty_base>
	{
		private:
    	typedef boost::accumulators::dont_care dont_care_;
		public:

    	typedef inherit2 type;
    
		inherit2(){}

    	inherit2(dont_care_){}

		private:
		typedef boost::mpl::empty_base empty_;

        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive & ar, const unsigned int version)
        {
        }
        
	};
    
}// detail
}// serialization
}// boost

#endif
