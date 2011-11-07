//////////////////////////////////////////////////////////////////////////////
// assign::detail::assign_reference_copy.hpp                                //
//                                                                          //
//  (C) Copyright 2010 Manuel Peinado Gallego                               //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_ASSIGN_DETAIL_ASSIGN_REFERENCE_COPY_MPG_2010_HPP
#define BOOST_ASSIGN_DETAIL_ASSIGN_REFERENCE_COPY_MPG_2010_HPP

namespace boost{
namespace assign{
namespace detail{

    // This is a reference wrapper whose assignment operator copies the value of
    // the rhs to the object pointed to.
    //
    // This is in contrast to assign_reference whose assignement operator 
    // rebinds the address of the internal pointer. For the same effect, here, 
    // use rebind().
    template< class T >
    struct assign_reference_copy
    {
        assign_reference_copy()
        { /* intentionally empty */ }

        explicit assign_reference_copy( T& r ) : ref_(&r)
        { }

        void operator=( const T& r )
        {
            *this->ref_ = r;
        }

        operator T&() const
        {
            return *this->ref_;
        }

        void swap( assign_reference_copy& r )
        {
            std::swap( *this->ref_, *r.ref_ );
        }

        T& get_ref() const
        {
            return *this->ref_;
        }

        void rebind( T & r )
    	{
            this->ref_ = &r;
    	}
        
    private:
        T* ref_;

    };

	// Added by ER March 10 : 

	template<typename T>
	void rebind(assign_reference_copy<T>& a,T& r){
    	a.rebind(r);
    }

    template< class T >
    inline bool operator<( const assign_reference_copy<T>& l, 
                           const assign_reference_copy<T>& r )
    {
        return l.get_ref() < r.get_ref();
    }

    template< class T >
    inline bool operator>( const assign_reference_copy<T>& l,
                           const assign_reference_copy<T>& r )
    {
        return l.get_ref() > r.get_ref();
    }

    template< class T >
    inline void swap( assign_reference_copy<T>& l, 
                      assign_reference_copy<T>& r )
    {
        l.swap( r );
    }

}// detail
}// assign
}// boost

#endif

