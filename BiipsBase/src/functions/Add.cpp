//                                               -*- C++ -*-
/*! \file Add.cpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#include "functions/Add.hpp"

namespace Biips
{
    DataType Add::Eval(const DataType::Array & paramValues) const
    {
      // TODO check paramValues
      const DataType & left = paramValues[0];
      const DataType & right = paramValues[1];

      DataType ans;
      if ( left.IsScalar() )
        ans = DataType(right.DimPtr(), left.ScalarView() + right.Value());
      else if ( right.IsScalar() )
        ans = DataType(left.DimPtr(), left.Value() + right.ScalarView());
      else
        ans = DataType(left.DimPtr(), left.Value() + right.Value());
      return ans;
    }
}
