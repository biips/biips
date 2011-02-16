//                                               -*- C++ -*-
/*! \file Sqrt.cpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#include "functions/Sqrt.hpp"

namespace Biips
{
    DataType Sqrt::Eval(const DataType::Array & paramValues) const
    {
      // TODO check paramValues
      const DataType & val = paramValues[0];

      DataType ans = DataType(val.DimPtr(), val.Value().Apply(&sqrtScalar));
      return ans;
    }
}
