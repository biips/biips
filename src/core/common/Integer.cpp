#include "common/Integer.hpp"

#include <boost/math/special_functions/round.hpp>
#include <boost/numeric/conversion/converter.hpp>
#include <cmath>

namespace Biips
{

  Bool checkInteger(Scalar s, Scalar * pIntPart)
  {
    Scalar int_part;
    if (!pIntPart)
      pIntPart = &int_part;
    return std::abs(std::modf(s, pIntPart)) <= std::numeric_limits<Scalar>::epsilon();
  }


  Int roundInteger(Scalar s)
  {
    return boost::math::iround(s);
  }


  Bool checkSize(Scalar s, Scalar * pIntPart)
  {
    return (s >= 0.0) && checkInteger(s, pIntPart);
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
