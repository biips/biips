//                                               -*- C++ -*-
/*! \file Utility.hpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_UTILITY_HPP_
#define BIIPS_UTILITY_HPP_

#include <utility>

#include "Types.hpp"

namespace Biips
{

  //  template<typename T>
  //  boost::shared_ptr<T> pointer(const T & obj)
  //  {
  //    return boost::shared_ptr<T>(new T(obj));
  //  }
//  template<typename T>
//  typename Types<T>::Ptr pointer(const T & obj)
//  {
//    return typename Types<T>::Ptr(new T(obj));
//  }


  //  template <typename Container>
  //  std::pair<typename Container::iterator, typename Container::iterator> iterRange(Container & cont)
  //  {
  //    return std::make_pair(cont.begin(), cont.end());
  //  }
  template <typename Container>
  typename Types<typename Container::iterator>::Pair iterRange(Container & cont)
  {
    return std::make_pair(cont.begin(), cont.end());
  }

  template <typename Container>
  typename Types<typename Container::const_iterator>::Pair iterRange(const Container & cont)
  {
    return std::make_pair(cont.begin(), cont.end());
  }


  template<typename T1, typename T2>
  std::ostream & operator << (std::ostream & os, const std::pair<T1,T2> & p)
  {
    return os << "(" << p.first << ", " << p.second << ")";
  }

}

#endif /* BIIPS_UTILITY_HPP_ */
