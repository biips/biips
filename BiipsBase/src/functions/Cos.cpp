//                                               -*- C++ -*-
/*! \file Cos.cpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#include "functions/Cos.hpp"

namespace Biips
{
    DataType Cos::Eval(const DataType::Array & paramValues) const
    {
      // TODO check paramValues
      const DataType & val = paramValues[0];

      DataType ans = DataType(val.DimPtr(), val.Value().Apply(&cosScalar));
      return ans;
    }
}
