//                                               -*- C++ -*-
/*! \file Pow.cpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#include "functions/Pow.hpp"

namespace Biips
{
    DataType Pow::Eval(const DataType::Array & paramValues) const
    {
      // TODO check paramValues
      const DataType & base = paramValues[0];
      const DataType & exponent = paramValues[1];

      static PowScalar pow_scalar;

      DataType ans;
      if ( base.IsScalar() )
      {
        ans = DataType(exponent.DimPtr(), exponent.Values().Apply(std::bind1st(pow_scalar, base.ScalarView())));
      }
      else if ( exponent.IsScalar() )
        ans = DataType(base.DimPtr(), base.Values().Apply(std::bind2nd(pow_scalar, exponent.ScalarView())));
      else
        ans = DataType(base.DimPtr(), base.Values().Apply(pow_scalar, exponent.Values()));
      return ans;
    }
}
