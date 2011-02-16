//                                               -*- C++ -*-
/*! \file Equal.cpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#include "functions/Equal.hpp"

namespace Biips
{
    DataType Equal::Eval(const DataType::Array & paramValues) const
    {
      // TODO check paramValues
      const DataType & left = paramValues[0];
      const DataType & right = paramValues[1];

      DataType ans;
      if ( left.IsScalar() )
      {
        ans = DataType(right.DimPtr(), left.ScalarView() == right.Value());
      }
      else if ( right.IsScalar() )
        ans = DataType(left.DimPtr(), left.Value() == right.ScalarView());
      else
        ans = DataType(left.DimPtr(), left.Value() == right.Value());
      return ans;
    }
}
