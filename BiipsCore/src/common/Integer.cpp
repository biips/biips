//                                               -*- C++ -*-
/*! \file Integer.cpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#include "common/Integer.hpp"

#include <boost/numeric/conversion/converter.hpp>
#include <cmath>

namespace Biips
{

  Bool checkInteger(Scalar s, Scalar * pIntPart)
  {
    Scalar int_part;
    if (!pIntPart)
      pIntPart = &int_part;
    return std::modf(s, pIntPart) <= std::numeric_limits<Scalar>::round_error();
  }

  typedef boost::numeric::converter<Int, Scalar,
    boost::numeric::conversion_traits<Int, Scalar>,
    boost::numeric::def_overflow_handler,
    boost::numeric::RoundEven<boost::numeric::conversion_traits<Int, Scalar>::source_type>
    > Scalar2IntRounder;

  Int roundInteger(Scalar s)
  {
    return Scalar2IntRounder::convert(s);
  }


  Bool checkSize(Scalar s, Scalar * pIntPart)
  {
    return (s > 0.0) && checkInteger(s, pIntPart);
  }

  typedef boost::numeric::converter<Size, Scalar,
    boost::numeric::conversion_traits<Size, Scalar>,
    boost::numeric::def_overflow_handler,
    boost::numeric::RoundEven<boost::numeric::conversion_traits<Size, Scalar>::source_type>
    > Scalar2SizeRounder;

  Size roundSize(Scalar s)
  {
    return Scalar2SizeRounder::convert(s);
  }
}
