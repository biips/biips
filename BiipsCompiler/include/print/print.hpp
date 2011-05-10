//                                               -*- C++ -*-
/*! \file print.hpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#ifndef BIIPS_PRINT_HPP_
#define BIIPS_PRINT_HPP_

#include "common/Types.hpp"
#include <sstream>

namespace Biips
{

  class ValArray;

  /*!
   * Prints the elements of the ValArray in line and comma separated
   */
  std::ostream & operator << (std::ostream & os, const ValArray & val);


  class MultiArray;

  /*!
   * Prints the elements of the MultiArray according to its dimensions.
   */
  std::ostream & operator << (std::ostream & os, const MultiArray & dat);


  class IndexRange;

  /*!
   * Prints a BUGS language representation of the
   * given range: e.g. a range with lower limit (1,2,3) and upper limit
   * (3,3,3) will be printed as "[1:3,2:3,3]"
   */
  std::ostream & operator << (std::ostream & os, const IndexRange & range);

}

#endif /* BIIPS_PRINT_HPP_ */
