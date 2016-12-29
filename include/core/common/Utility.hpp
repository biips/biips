#ifndef BIIPS_UTILITY_HPP_
#define BIIPS_UTILITY_HPP_

#include <utility>
#include <sstream>

#include "common/Types.hpp"
#include "common/IndexRange.hpp"

namespace Biips
{

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


  template<typename T>
  String print(const T & val)
  {
    std::ostringstream ostr;
    ostr << val;
    return ostr.str();
  }


  inline Bool allTrue(const Flags & mask)
  {
    return std::find(mask.begin(), mask.end(), false) == mask.end();
  }


  inline Bool anyTrue(const Flags & mask)
  {
    return std::find(mask.begin(), mask.end(), true) != mask.end();
  }


  String expandFileName(const char * s);

  class ValArray;

  void printRecursion(std::ostream & os, const ValArray & val, const IndexRange & range, IndexRange::SizeType dim, IndexRange::Indices & indices);
}

#endif /* BIIPS_UTILITY_HPP_ */
