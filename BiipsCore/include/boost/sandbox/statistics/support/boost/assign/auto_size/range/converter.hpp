//////////////////////////////////////////////////////////////////////////////
// converter.hpp                                                            //
//                                                                          //
//  Copyright Thorsten Ottosen 2003-2004.                                   //
//  (C) Copyright 2010 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_ASSIGN_DETAIL_RANGE_CONVERTER_ER_2010_HPP
#define BOOST_ASSIGN_DETAIL_RANGE_CONVERTER_ER_2010_HPP
#include <boost/assign/list_of.hpp> // contains parts of impl
#include <boost/typeof/typeof.hpp>
#include <boost/range.hpp>
#include <boost/assign/auto_size/range/convert_range.hpp>

// This brings the code within assign_detail::converter<> into a macro to be 
// expanded in what would otherwise be a derived class. This is to solve 
// some compiler complaints in the presence of multiple inheritance.
// 
// Warning: some previous compiler workarounds were removed  (for now).


#define BOOST_ASSIGN_AS_CONVERTER(R)                                           \
    template< class Container>                                                 \
    Container convert_to_container() const{                                    \
        return range_convert::convert_to_container<Container>(*this); }        \
                                                                               \
    template< class Container>                                                 \
    Container to_container( Container& c) const{                               \
        return range_convert::to_container(c,*this); }                         \
                                                                               \
    range_convert::impl::adapter_converter<R>                                  \
    to_adapter() const{ return range_convert::to_adapter(*this); }             \
                                                                               \
    template<class Adapter>                                                    \
    Adapter to_adapter( Adapter& a)const{                                      \
        return range_convert::to_adapter(a,*this); }                           \
                                                                               \
    template<class Array> Array to_array( Array& a) const{                     \
        return to_array(a,*this); }                                            \
                                                                               \
    template< class Container >                                                \
    operator Container() const                                                 \
    {                                                                          \
        return  this->convert_to_container<Container>();                       \
    }                                                                          \
/**/
    
namespace boost{
namespace assign{
namespace detail{
namespace range_convert{
namespace impl{
                                                                               
    template< class Container, class R> 
    Container convert( const Container*, 
        assign_detail::default_type_tag, const R& r) 
    { 
        typedef typename boost::range_reference<Container>::type ref_;
        typedef typename boost::remove_reference<ref_>::type val_;
        typedef typename boost::assign::detail::result_of::convert_range<
            const R,val_>::type tmp_;
        tmp_ tmp = boost::assign::convert_range<
            val_,boost::use_default>( r );
        return Container( boost::begin( tmp ), boost::end( tmp ) );                    
    } 
                                                                                     
    template< class Array ,class R>
    Array convert( const Array*, assign_detail::array_type_tag, const R& r )
    {                                                                      
        typedef BOOST_DEDUCED_TYPENAME Array::value_type value_type;       
        typedef BOOST_DEDUCED_TYPENAME boost::range_iterator<const R>::type it_;       
                                                                               
        Array ar;                                                          
        const std::size_t sz = ar.size();                                  
        if( sz < boost::size( r ) )                                    
            throw assign::assignment_exception( 
             "array initialized with too many elements" );
        std::size_t n = 0;                                                 
        it_ i =  boost::begin( r ) ;
        it_ e =  boost::end( r ) ;
        for( ; i != e; ++i, ++n )
            ar[n] = *i;
        for( ; n < sz; ++n ) 
            ar[n] = value_type(); 
        return ar; 
    }
                                             
    template< class Adapter ,class R>
    Adapter convert_to_adapter(const R& r , const Adapter* = 0)
    {                                                                      
        typedef BOOST_DEDUCED_TYPENAME boost::range_iterator<const R>::type it_;       
        Adapter a;                                                         
        it_ i = boost::begin( r );                                   
        it_ e = boost::end( r );                                     
        for( ; i != e; ++i )
        	a.push( *i );                                                  
        return a;                                                          
    }                                                                      

    template<class R>
    struct adapter_converter                                               
    {                                                                      
        typedef typename boost::sub_range<const R>::type sub_;                                                   
        adapter_converter( const R& r ) : sub( r )                      
        {}                                                                 
                                                                               
        adapter_converter( const adapter_converter& a )                    
        : sub( a.sub )                                                       
        { }                                                                
                                                                               
        template< class Adapter >                                          
        operator Adapter() const                                           
        {                                                                  
            return convert_to_adapter<Adapter>(sub);                       
        }  
            
        private:
        sub_ sub;                                                                                                                                
    }; 
}// impl

    template< class Container,class R >
    Container convert_to_container(const R& r)
    {                                                                      
        static Container* c = 0;                                           
        BOOST_STATIC_CONSTANT( 
            bool, 
            is_array_flag = sizeof(
                assign_detail::assign_is_array( c ) 
            ) == sizeof( type_traits::yes_type ) 
        );       

        typedef BOOST_DEDUCED_TYPENAME boost::mpl::if_c< is_array_flag,           
        	assign_detail::array_type_tag,
                        assign_detail::default_type_tag >::type tag_type;

        return range_convert::impl::convert<Container>( c, tag_type(), r );                  
    }
    
    template< class Container,class R >
    Container to_container( Container& c,const R& r )
    {                                                                      
        return range_convert::impl::convert( &c, 
            assign_detail::default_type_tag(),r );           
    }
    
    template<class R>                                                                       
    impl::adapter_converter<R> to_adapter(const R& r)
    {                                                                      
        return range_convert::impl::adapter_converter<R>( r );                                 
    }
    
    template< class Adapter,class R >                                              
    Adapter to_adapter( Adapter& a ,const R& r)
    {                                                                      
        return range_convert::impl::convert_to_adapter(r, &a);                             
    }
    
    template< class Array ,class R>                                                
    Array to_array( Array& a ,const R& r)
    {                                                                      
        return range_convert::impl::convert( 
            &a, assign_detail::array_type_tag(),r );             
    }                                                                      

}// range_convert
}// detail
}// assign
}// boost

#endif
