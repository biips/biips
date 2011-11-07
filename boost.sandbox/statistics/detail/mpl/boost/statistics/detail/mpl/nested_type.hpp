////////////////////////////////////////////////////////////////////////////////
// nested_type.hpp                                                            //  
////////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_MPL_NESTED_TYPE_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_MPL_NESTED_TYPE_HPP_ER_2009
#include <boost/mpl/identity.hpp>
#include <boost/mpl/has_xxx.hpp>
#include <boost/mpl/eval_if.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace mpl{
    BOOST_MPL_HAS_XXX_TRAIT_DEF(type);

// Provided by Steven Watanabe in
// news://news.gmane.org:119/gma8hc$2a0$1@ger.gmane.org
template<class T>
struct nested_type :
 boost::mpl::eval_if<has_type<T>,
   T,
   boost::mpl::identity<T> >
{};

}// mpl
}// detail
}// statistics
}// boost

#endif
