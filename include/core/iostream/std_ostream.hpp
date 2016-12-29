#ifndef BIIPS_STD_OSTREAM_HPP_
#define BIIPS_STD_OSTREAM_HPP_

#include <vector>
#include <sstream>

namespace std
{

  /*!
   * Prints the elements of a std::vector.
   */
  template<typename T>
  std::ostream & operator << (std::ostream & os, const std::vector<T> & vec)
  {
    if (vec.size()>0) {
      os << vec[0];
      for (unsigned int i=1; i<vec.size(); ++i) {
        os << ", " << vec[i];
      }
    }
    return os;
  }

}

#endif /* BIIPS_STD_OSTREAM_HPP_ */
