//                                               -*- C++ -*-
/*! \file Substract.cpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#include "functions/Substract.hpp"

namespace Biips
{
    DataType Substract::Eval(const DataType::Array & paramValues) const
    {
      // TODO check paramValues
      const DataType & left = paramValues[0];
      const DataType & right = paramValues[1];

      DataType ans;
      if ( left.IsScalar() )
        ans = DataType(right.DimPtr(), left.ScalarView() - right.Value());
      else if ( right.IsScalar() )
        ans = DataType(left.DimPtr(), left.Value() - right.ScalarView());
      else
        ans = DataType(left.DimPtr(), left.Value() - right.Value());
      return ans;
    }
}
