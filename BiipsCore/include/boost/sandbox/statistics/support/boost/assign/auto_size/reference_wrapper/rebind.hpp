//////////////////////////////////////////////////////////////////////////////
// assign::detail::assign_reference_rebind.hpp                              //
//                                                                          //
//  (C) Copyright Thorsten Ottosen 2003-2004. Use, modification and         //
//  (C) Copyright 2010 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_ASSIGN_DETAIL_ASSIGN_REFERENCE_REBIND_ER_2010_HPP
#define BOOST_ASSIGN_DETAIL_ASSIGN_REFERENCE_REBNID_ER_2010_HPP

namespace boost{
namespace assign{
namespace detail{

    // same as assign_detail::assign_reference except swaps by address
    template< class T >
    struct assign_reference_rebind
    {
        assign_reference_rebind()
        { /* intentionally empty */ }

        assign_reference_rebind( T& r ) : ref_(&r)
        { }

        void operator=( T& r ) 
        {
            this->rebind(r);
        }

        operator T&() const
        {
            return *this->ref_;
        }

        void swap( assign_reference_rebind& r )
        {
            T* tmp = this->ref_;
            this->ref_ = r.ref_; // this->rebing(r.ref_)
            r.ref_ = tmp; // r.rebind(tmp)
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

    template<typename T>
    void rebind(assign_reference_rebind<T>& a,T& r){
    	a = r;
    }

    template< class T >
    inline bool operator<( const assign_reference_rebind<T>& l, 
                           const assign_reference_rebind<T>& r )
    {
        return l.get_ref() < r.get_ref();
    }

    template< class T >
    inline bool operator>( const assign_reference_rebind<T>& l,
                           const assign_reference_rebind<T>& r )
    {
        return l.get_ref() > r.get_ref();
    }

    template< class T >
    inline void swap( assign_reference_rebind<T>& l, 
                      assign_reference_rebind<T>& r )
    {
        l.swap( r );
    }

}// detail
}// assign
}// boost

#endif
