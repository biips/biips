//                                               -*- C++ -*-
/*! \file Divide.cpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#include "functions/Divide.hpp"

namespace Biips
{
    DataType Divide::Eval(const DataType::Array & paramValues) const
    {
      // TODO check paramValues
      const DataType & left = paramValues[0];
      const DataType & right = paramValues[1];

      DataType ans;
      if ( left.IsScalar() )
      {
        ans = DataType(right.DimPtr(), left.ScalarView() / right.Values());
      }
      else if ( right.IsScalar() )
        ans = DataType(left.DimPtr(), left.Values() / right.ScalarView());
      else
        ans = DataType(left.DimPtr(), left.Values() / right.Values());
      return ans;
    }
}
