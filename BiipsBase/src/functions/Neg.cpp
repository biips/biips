//                                               -*- C++ -*-
/*! \file Neg.cpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#include "functions/Neg.hpp"

namespace Biips
{
    DataType Neg::Eval(const DataType::Array & paramValues) const
    {
      // TODO check paramValues
      const DataType & val = paramValues[0];

      DataType ans = DataType(val.DimPtr(), -val.Values());
      return ans;
    }
}
